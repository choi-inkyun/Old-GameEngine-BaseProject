/*------------------------------------------------------------------------------
 * COMMENTS:
 *			맞고 게임 판 클래스
 *
 * FILENAME:
 *			GameBoard.cpp
 *
 * DESCRIPTION:
 *			클라이언트 화면 출력에 관련된 정보를 가지기 위한 클래스이다.
 *
 * HISTORY:
 *			2004.07.22 - 텍스트 기반 클라이언트에서 서버 테스트용으로 작성됨.
 *
 * THINGS TO DO:
 *			-
 *----------------------------------------------------------------------------*/
#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

#include "common\Card.h"
#include "common\CardPositioner.h"

#include "Network\PacketTypes.h"

// 플레이어 상태 클래스
class cPlayerStatus
{
private:
	WCHAR userid[MAX_USERID+1];						// 사용자 ID
	int  level;										// 레벨 등급
	money_t player_money;							// 플레이어 머니
	int  avatar;									// 아바타
	money_t game_balance;							// 게임 변경 머니

	int  score;										// 점수
	int  number_of_go;								// 고 회수
	int  number_of_overlapped;						// 뻑 회수
	int  number_of_triple;							// 흔듦 회수

	int  number_of_hide;							// 피의 갯수
	int  number_of_stars;							// 미션 성공 회수(특수 기능)

	BOOL kwang_penalty;								// 광박 상태
	BOOL hide_penalty;								// 피박 상태
	BOOL go_penalty;								// 고박 상태

	BOOL exitrequested;								// 나가기 예약 상태

public:
	cPlayerStatus()
	{
		clear();
	}

	~cPlayerStatus()
	{
		// Do nothing
	}

	void clear()
	{
		memset( userid, 0x00, sizeof(userid) );

		score					= 0;
		number_of_go			= 0;
		number_of_overlapped	= 0;
		number_of_triple		= 0;

		number_of_hide			= 0;
		number_of_stars			= 0;

		kwang_penalty			= FALSE;
		hide_penalty			= FALSE;
		go_penalty				= FALSE;

		exitrequested			= FALSE;
	}

	void set_userid( LPCWSTR userid )
	{
		memset( this->userid, 0x00, sizeof(this->userid) );
		wcsncpy( this->userid, userid, MAX_USERID );
	}
	LPWSTR get_userid() { return userid; }

	void set_level( int level ) { this->level = level; }
	int  get_level() { return level; }

	void set_player_money( money_t money ) { player_money = money; }
	money_t get_player_money() { return player_money; }

	void set_avatar( int avatar ) { this->avatar = avatar; }
	int  get_avatar() { return avatar; }

	void set_score( int score ) { this->score = score; }
	int  get_score() { return score; }

	void set_number_of_go( int times ) { number_of_go = times; }
	int  get_number_of_go() { return number_of_go; }

	void set_number_of_overlapped( int times ) { number_of_overlapped = times; }
	int  get_number_of_overlapped() { return number_of_overlapped; }

	void set_number_of_triple( int times ) { number_of_triple = times; }
	int  get_number_of_triple() { return number_of_triple; }

	void set_number_of_hide( int points ) { number_of_hide = points; }
	int  get_number_of_hide() { return number_of_hide; }

	void set_number_of_stars( int stars ) { number_of_stars = stars; }
	int  get_number_of_stars() { return number_of_stars; }

	void set_game_balance( money_t balance ) { game_balance = balance; }
	money_t get_game_balance() { return game_balance; }

	void set_kwang_penalty( BOOL flag ) { kwang_penalty = flag; }
	void set_hide_penalty( BOOL flag ) { hide_penalty = flag; }
	void set_go_penalty( BOOL flag ) { go_penalty = flag; }
	BOOL have_kwang_penalty() { return kwang_penalty; }
	BOOL have_hide_penalty() { return hide_penalty; }
	BOOL have_go_penalty() { return go_penalty; }

	void set_exitrequested( BOOL flag ) { exitrequested = flag; }
	BOOL get_exitrequested() { return exitrequested; }

};

typedef enum GAMEBOARD_STATUS
{
	GAMEBOARD_WAITING = 0,							// 게임 대기중
	GAMEBOARD_PLAYING,								// 게임 진행중
	GAMEBOARD_OBSERVING,							// 게임 관전중
} GAMEBOARD_STATUS;

typedef enum GAME_ANIMATION
{
	GAMEANIMATION_INVALID = 0,					// 애니메이션 없음
	GAMEANIMATION_CLEARBOARD,					// 게임 판을 지우는 애니메이션
	GAMEANIMATION_GAMESLOT,						// 게임 슬롯 정보 설정(아바타 로드)
	GAMEANIMATION_DISTRIBUTION,					// 카드 나눠주는 애니메이션
	GAMEANIMATION_REQUESTCARD,					// 카드 낼 준비 설정
	GAMEANIMATION_REQUESTCARD_DONE,				// 카드 낼 준비 해제
	GAMEANIMATION_SHOWCARD,						// 흔든 패 보여주는 애니메이션
	GAMEANIMATION_DROPCARD,						// 패 내는 애니메이션
	GAMEANIMATION_TOSSCARD,						// 패 뒤집는 애니메이션
	GAMEANIMATION_ACQUISITION,					// 패 획득 애니메이션
	GAMEANIMATION_PICKUPCARD,					// 패 집기 애니메이션
	GAMEANIMATION_JOCKERCARD,					// 조커 내는 애니메이션
	GAMEANIMATION_GAMEMENT,						// 게임 멘트 애니메이션
	GAMEANIMATION_MOVECARD,						// 패(국진) 사용 애니메이션
	GAMEANIMATION_TAKECARD,						// 패(피) 이동 애니메이션
	GAMEANIMATION_PLAYOVER,						// 플레이 종료 애니메이션
	GAMEANIMATION_MISSION,						// 미션 애니메이션
	GAMEANIMATION_GAMEBILL,						// 게임 정산 애니메이션
	GAMEANIMATION_GAMEEND,						// 게임 종료 애니메이션
	GAMEANIMATION_GAMESTATUS,					// 게임 진행 상태
	GAMEANIMATION_TIMER_STOP,                   // 게임이 타이머에 의해 잠시 딜레이 타임 상태
	GAMEANIMATION_IN,							// 게임 입장
	GAMEANIMATION_OUT,							// 게임 퇴장
} GAME_ANIMATION;

// 게임 보드 클래스
class cGameBoard
{
public:
	const static int  INVALID_SLOT = (-1);			// 의미없는 슬롯 번호

private:
	GAMEBOARD_STATUS status;

//	ROOMINFOCATALOG room_info;						// 방 정보
	money_t			jackpot_money;					// 잭팟 머니 상태

	int  user_slot;									// 사용자 슬롯
	int  player_slot;								// 플레이어(하단) 슬롯
	int  first_slot;								// 선번 슬롯
	int  turn_slot;									// 턴을 가진 슬롯
	int  timer_slot;								// 타이머 동작중인 슬롯

	cGroundCardPositioner* ground_card_positioner;		// 바닥 카드 위치
	cAcquiredCardPositioner* player_card_positioner;	// 자신의 획득 카드 위치
	cAcquiredCardPositioner* opponent_card_positioner;	// 상대방 획득 카드 위치
	cPlayingCardPositioner* playing_card_positioner;	// 들고 있는 카드 위치
	cPlayingCardPositioner* counting_card_positioner;	// 상대방 들고 있는 카드 위치
	
	cCard playing_cards;							// 들고 있는 카드
	cCard acquired_cards;							// 먹은 카드
	cCard acquired_opponent;						// 상대방이 먹은 카드
	cCard ground_cards;								// 바닥에 깔린 카드

	BOOL bRequested_GameStart;						// 게임 시작 요청 플래그
	BOOL bRequested_SelectCard;						// 낼 카드 선택 요청 플래그
	BOOL bRequested_GameQuestion;					// 게임 진행 선택 요청 플래그
	BOOL bSep_Ten_As_Hide;							// 국진 피 사용 여부 플래그

	cCard m_EatCard;								// 지금 턴에서 먹을 카드를 여기다가 모두 넣는다	
	cCard m_EatCardTemp;							// 위 변수의 임시 변수.. 조커등을 보관한다.

	cCard m_ComputerCard;							// 컴퓨터 카드

	cCard m_PiChangeCard;							// 피 뺏는 카드

	int  number_of_player_cards;					// 플레이어 카드 수(관전 모드 지원용)
	int  number_of_opponent_cards;					// 상대방 카드 수(관전 모드 지원용)

	int  number_of_deck_cards;						// 뒤집혀진 카드 수
	int  turn_counter;								// 턴 수

	cPlayerStatus playerStatus[2];					// 플레이어 상태

	GAME_ANIMATION		game_animation;				// 게임 애니메이션
	BOOL				bAnimating;					// 애니메이션 진행중 플래그
	BOOL				bMissionCompleted;			// 미션 완수 여부 플래그

	int					dropevent;					// 패 내기 이벤트
	cCard				showcard;					// 흔든 패

	GAMEMENT_ADS		gamement_ads;				// 게임 진행 멘트
	GAMEMISSION_ADS		gamemission_ads;			// 게임 미션 
	GAMEQUESTION_REQ	gamequestion_req;			// 게임 진행 선택
	GAMEBILL_SND		gamebill_snd;				// 게임 결과
//	CBase_Object<GAMEMENT_ADS>		gamement_ads;		// 게임 진행 멘트
//	CBase_Object<GAMEMISSION_ADS>	gamemission_ads;	// 게임 미션
//	CBase_Object<GAMEQUESTION_REQ>	gamequestion_req;	// 게임 진행 선택
//	CBase_Object<GAMEBILL_SND>		gamebill_snd;		// 게임 결과

	BOOL				m_bNewChatMessage;			// 새로운 채팅 메시지 플래그
	int					m_iMessageType;				// 일방 채팅/시스템 메시지 구분
	WCHAR				m_wszChatMessage[256];		// 채팅 메세지

public:
	cGameBoard();
	~cGameBoard();

//	void set_room_info( ROOMINFOCATALOG *ric ) { memcpy( &room_info, ric, sizeof(ROOMINFOCATALOG) ); }
//	ROOMINFOCATALOG *get_room_info() { return &room_info; }

	void set_jackpot( money_t money ) { jackpot_money = money; }
	money_t get_jackpot() { return jackpot_money; }

	void clear();									// 모든 변수를 초기화한다.

	void set_status( GAMEBOARD_STATUS status ) { this->status = status; }
	GAMEBOARD_STATUS get_status() { return status; }

	cPlayerStatus *get_player_status( int slot ) { return &playerStatus[ slot ]; }
	cPlayerStatus *get_opponent_status( int slot ) { return &playerStatus[ ( slot + 1 ) % 2 ]; }

	void set_userid( LPCWSTR userid1, LPCWSTR userid2 );
	void set_player_money( money_t money1, money_t money2 );
	void set_level( int level1, int level2 );
	void set_avatar( int avatar1, int avatar2 );
	void set_stars( int stars1, int stars2 );
	void set_game_balance( money_t balance1, money_t balance2 );

	void set_user_slot( int slot ) { user_slot = slot; }
	int  get_user_slot() { return user_slot; }
	void set_player_slot( int slot ) { player_slot = slot; }
	int  get_player_slot() { return player_slot; }
	void set_first_slot( int slot ) { first_slot = slot; }
	int  get_first_slot() { return first_slot; }
	void set_turn_slot( int slot ) { turn_slot = slot; }
	int  get_turn_slot() { return turn_slot; }
	void set_timer_slot( int slot ) { timer_slot = slot; }
	int  get_timer_slot() { return timer_slot; }
	
	BOOL is_my_slot( int slot ) { return user_slot == slot ? TRUE : FALSE; }
	BOOL is_my_turn() { return user_slot == turn_slot ? TRUE : FALSE; }

	cGroundCardPositioner *get_ground_card_positioner() { return ground_card_positioner; }
	cAcquiredCardPositioner *get_player_card_positioner() { return player_card_positioner; }
	cAcquiredCardPositioner *get_opponent_card_positioner() { return opponent_card_positioner; }
	cPlayingCardPositioner *get_playing_card_positioner() { return playing_card_positioner; }
	cPlayingCardPositioner *get_counting_card_positioner() { return counting_card_positioner; }

	cCard *get_playing_cards() { return &playing_cards; }
	cCard *get_acquired_cards() { return &acquired_cards; }
	cCard *get_acquired_opponent() { return &acquired_opponent; }
	cCard *get_ground_cards() { return &ground_cards; }

	cCard *get_eat_cards() { return &m_EatCard;	}
	cCard *get_eattemp_cards() { return &m_EatCardTemp;	}

	cCard *get_computer_cards() { return &m_ComputerCard;	}
	
	cCard *get_pichange_cards() { return &m_PiChangeCard;	}

	void set_playing_cards( CARD *cards );			// 들고 있는 카드를 설정한다.
	void add_playing_cards( CARD *cards );			// 들고 있는 카드에 추가한다.
	void add_playing_cards( CARD card );			// 들고 있는 카드에 추가한다(한장).
	void del_playing_cards( CARD *cards );			// 들고 있는 카드에서 삭제한다.
	void add_acquired_cards( CARD *cards );			// 먹은 카드에 추가한다.
	void del_acquired_cards( CARD *cards );			// 먹은 카드에서 삭제한다.
	void add_acquired_opponent( CARD *cards );		// 상대방이 먹은 카드에 추가한다.
	void del_acquired_opponent( CARD *cards );		// 상대방이 먹은 카드에서 삭제한다.
	void set_ground_cards( CARD *cards );			// 바닥에 깔린 카드를 설정한다.
	void add_ground_cards( CARD *cards );			// 바닥에 깔린 카드에 추가한다.
	void add_ground_cards( CARD card );
	void del_ground_cards( CARD *cards );			// 바닥에 깔린 카드에서 삭제한다.

	void set_select_card_requested( BOOL flag ) { bRequested_SelectCard = flag; }
	BOOL get_select_card_requested() { return bRequested_SelectCard; }

	void set_gamestart_requested( BOOL flag ) { bRequested_GameStart = flag; }
	BOOL get_gamestart_requested() { return bRequested_GameStart; }

	void set_gamequestion_requested( BOOL flag ) { bRequested_GameQuestion = flag; }
	BOOL get_gamequestion_requested() { return bRequested_GameQuestion; }

	void set_sep_ten_as_hide( BOOL flag ) { bSep_Ten_As_Hide = flag; }
	BOOL get_sep_ten_as_hide() { return bSep_Ten_As_Hide; }

	void set_number_of_player_cards( int n ) { number_of_player_cards = n; }	// 플레이어 카드 수 설정(관전자 지원용)
	int  get_number_of_player_cards() { return number_of_player_cards; }		// 플레이어 카드 수 획득(관전자 지원용)

	void set_number_of_opponent_cards( int n ) { number_of_opponent_cards = n; }// 상대방 카드 수 설정(관전자 지원용)
	int  get_number_of_opponent_cards() { return number_of_opponent_cards; }	// 상대방 카드 수 획득(관전자 지원용)

	void set_number_of_deck_cards( int n ) { number_of_deck_cards = n; }// 뒤집혀진 카드 수를 설정한다.
	void inc_number_of_deck_cards() { number_of_deck_cards++; }			// 뒤집혀진 카드 수를 증가시킨다.
	void dec_number_of_deck_cards() { number_of_deck_cards--; }			// 뒤집혀진 카드 수를 감소시킨다.
	int  get_number_of_deck_cards() { return number_of_deck_cards; }	// 뒤집혀진 카드 수를 얻는다.

	int  get_deck_height() { return (int)( ( number_of_deck_cards + 1 ) / 2 ); }

	void set_turn_counter( int turn ) { turn_counter = turn; }			// 턴 카운터를 설정한다.
	void inc_turn_counter() { turn_counter++; }							// 턴 카운터를 증가시킨다.
	int  get_turn_counter() { return turn_counter; }					// 턴 카운터를 얻는다.

	void set_animation( GAME_ANIMATION game_animation )
	{
		this->game_animation = game_animation;
		if( game_animation != GAMEANIMATION_INVALID ) bAnimating = TRUE;
	}
	GAME_ANIMATION get_animation() { return game_animation; }
	void set_animating( BOOL flag ) { bAnimating = flag; }
	BOOL is_animation_progressing() { return bAnimating; }

	void set_mission_completed( BOOL flag = TRUE ) { bMissionCompleted = flag; }
	BOOL get_mission_completed() { return bMissionCompleted; }

	void set_dropevent( int event ) { dropevent = event; }
	int  get_dropevent() { return dropevent; }

	void set_showcard( CARD *cards ) { showcard = cCard( cards ); }
	cCard *get_showcard() { return &showcard; }

	void set_gamement_ads( GAMEMENT_ADS *ment_ads ) { memcpy( &gamement_ads, ment_ads, sizeof(GAMEMENT_ADS) ); }
	GAMEMENT_ADS *get_gamement_ads() { return &gamement_ads; }

	void set_gamemission_ads( GAMEMISSION_ADS *mission_ads ) { memcpy( &gamemission_ads, mission_ads, sizeof(GAMEMISSION_ADS) ); }
	GAMEMISSION_ADS *get_gamemission_ads() { return &gamemission_ads; }

	void set_gamequestion_req( GAMEQUESTION_REQ *question_req ) { memcpy( &gamequestion_req, question_req, sizeof(GAMEQUESTION_REQ) ); }
	GAMEQUESTION_REQ *get_gamequestion_req() { return &gamequestion_req; }

	void set_gamebill_snd( GAMEBILL_SND *bill_snd ) { memcpy( &gamebill_snd, bill_snd, sizeof(GAMEBILL_SND) ); }
	GAMEBILL_SND *get_gamebill_snd() { return &gamebill_snd; }

	void add_chatmessage( char *szChatMessage, int iType );
	int  get_chatmessage( LPWSTR *ppwszMessage );

	void clear_userlist();
	void update_userlist( USERINFOSHORT* uis );
	void delete_userlist( USERINFOSHORT* uis );

	// 임시 프로그래밍 //
	cCard card_dack;								// 카드 덱
};

#endif

