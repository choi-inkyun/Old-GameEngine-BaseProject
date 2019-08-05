/*------------------------------------------------------------------------------
 * COMMENTS:
 *			�°� ���� �� Ŭ����
 *
 * FILENAME:
 *			GameBoard.cpp
 *
 * DESCRIPTION:
 *			Ŭ���̾�Ʈ ȭ�� ��¿� ���õ� ������ ������ ���� Ŭ�����̴�.
 *
 * HISTORY:
 *			2004.07.22 - �ؽ�Ʈ ��� Ŭ���̾�Ʈ���� ���� �׽�Ʈ������ �ۼ���.
 *
 * THINGS TO DO:
 *			-
 *----------------------------------------------------------------------------*/
#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

#include "common\Card.h"
#include "common\CardPositioner.h"

#include "Network\PacketTypes.h"

// �÷��̾� ���� Ŭ����
class cPlayerStatus
{
private:
	WCHAR userid[MAX_USERID+1];						// ����� ID
	int  level;										// ���� ���
	money_t player_money;							// �÷��̾� �Ӵ�
	int  avatar;									// �ƹ�Ÿ
	money_t game_balance;							// ���� ���� �Ӵ�

	int  score;										// ����
	int  number_of_go;								// �� ȸ��
	int  number_of_overlapped;						// �� ȸ��
	int  number_of_triple;							// ��� ȸ��

	int  number_of_hide;							// ���� ����
	int  number_of_stars;							// �̼� ���� ȸ��(Ư�� ���)

	BOOL kwang_penalty;								// ���� ����
	BOOL hide_penalty;								// �ǹ� ����
	BOOL go_penalty;								// ��� ����

	BOOL exitrequested;								// ������ ���� ����

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
	GAMEBOARD_WAITING = 0,							// ���� �����
	GAMEBOARD_PLAYING,								// ���� ������
	GAMEBOARD_OBSERVING,							// ���� ������
} GAMEBOARD_STATUS;

typedef enum GAME_ANIMATION
{
	GAMEANIMATION_INVALID = 0,					// �ִϸ��̼� ����
	GAMEANIMATION_CLEARBOARD,					// ���� ���� ����� �ִϸ��̼�
	GAMEANIMATION_GAMESLOT,						// ���� ���� ���� ����(�ƹ�Ÿ �ε�)
	GAMEANIMATION_DISTRIBUTION,					// ī�� �����ִ� �ִϸ��̼�
	GAMEANIMATION_REQUESTCARD,					// ī�� �� �غ� ����
	GAMEANIMATION_REQUESTCARD_DONE,				// ī�� �� �غ� ����
	GAMEANIMATION_SHOWCARD,						// ��� �� �����ִ� �ִϸ��̼�
	GAMEANIMATION_DROPCARD,						// �� ���� �ִϸ��̼�
	GAMEANIMATION_TOSSCARD,						// �� ������ �ִϸ��̼�
	GAMEANIMATION_ACQUISITION,					// �� ȹ�� �ִϸ��̼�
	GAMEANIMATION_PICKUPCARD,					// �� ���� �ִϸ��̼�
	GAMEANIMATION_JOCKERCARD,					// ��Ŀ ���� �ִϸ��̼�
	GAMEANIMATION_GAMEMENT,						// ���� ��Ʈ �ִϸ��̼�
	GAMEANIMATION_MOVECARD,						// ��(����) ��� �ִϸ��̼�
	GAMEANIMATION_TAKECARD,						// ��(��) �̵� �ִϸ��̼�
	GAMEANIMATION_PLAYOVER,						// �÷��� ���� �ִϸ��̼�
	GAMEANIMATION_MISSION,						// �̼� �ִϸ��̼�
	GAMEANIMATION_GAMEBILL,						// ���� ���� �ִϸ��̼�
	GAMEANIMATION_GAMEEND,						// ���� ���� �ִϸ��̼�
	GAMEANIMATION_GAMESTATUS,					// ���� ���� ����
	GAMEANIMATION_TIMER_STOP,                   // ������ Ÿ�̸ӿ� ���� ��� ������ Ÿ�� ����
	GAMEANIMATION_IN,							// ���� ����
	GAMEANIMATION_OUT,							// ���� ����
} GAME_ANIMATION;

// ���� ���� Ŭ����
class cGameBoard
{
public:
	const static int  INVALID_SLOT = (-1);			// �ǹ̾��� ���� ��ȣ

private:
	GAMEBOARD_STATUS status;

//	ROOMINFOCATALOG room_info;						// �� ����
	money_t			jackpot_money;					// ���� �Ӵ� ����

	int  user_slot;									// ����� ����
	int  player_slot;								// �÷��̾�(�ϴ�) ����
	int  first_slot;								// ���� ����
	int  turn_slot;									// ���� ���� ����
	int  timer_slot;								// Ÿ�̸� �������� ����

	cGroundCardPositioner* ground_card_positioner;		// �ٴ� ī�� ��ġ
	cAcquiredCardPositioner* player_card_positioner;	// �ڽ��� ȹ�� ī�� ��ġ
	cAcquiredCardPositioner* opponent_card_positioner;	// ���� ȹ�� ī�� ��ġ
	cPlayingCardPositioner* playing_card_positioner;	// ��� �ִ� ī�� ��ġ
	cPlayingCardPositioner* counting_card_positioner;	// ���� ��� �ִ� ī�� ��ġ
	
	cCard playing_cards;							// ��� �ִ� ī��
	cCard acquired_cards;							// ���� ī��
	cCard acquired_opponent;						// ������ ���� ī��
	cCard ground_cards;								// �ٴڿ� �� ī��

	BOOL bRequested_GameStart;						// ���� ���� ��û �÷���
	BOOL bRequested_SelectCard;						// �� ī�� ���� ��û �÷���
	BOOL bRequested_GameQuestion;					// ���� ���� ���� ��û �÷���
	BOOL bSep_Ten_As_Hide;							// ���� �� ��� ���� �÷���

	cCard m_EatCard;								// ���� �Ͽ��� ���� ī�带 ����ٰ� ��� �ִ´�	
	cCard m_EatCardTemp;							// �� ������ �ӽ� ����.. ��Ŀ���� �����Ѵ�.

	cCard m_ComputerCard;							// ��ǻ�� ī��

	cCard m_PiChangeCard;							// �� ���� ī��

	int  number_of_player_cards;					// �÷��̾� ī�� ��(���� ��� ������)
	int  number_of_opponent_cards;					// ���� ī�� ��(���� ��� ������)

	int  number_of_deck_cards;						// �������� ī�� ��
	int  turn_counter;								// �� ��

	cPlayerStatus playerStatus[2];					// �÷��̾� ����

	GAME_ANIMATION		game_animation;				// ���� �ִϸ��̼�
	BOOL				bAnimating;					// �ִϸ��̼� ������ �÷���
	BOOL				bMissionCompleted;			// �̼� �ϼ� ���� �÷���

	int					dropevent;					// �� ���� �̺�Ʈ
	cCard				showcard;					// ��� ��

	GAMEMENT_ADS		gamement_ads;				// ���� ���� ��Ʈ
	GAMEMISSION_ADS		gamemission_ads;			// ���� �̼� 
	GAMEQUESTION_REQ	gamequestion_req;			// ���� ���� ����
	GAMEBILL_SND		gamebill_snd;				// ���� ���
//	CBase_Object<GAMEMENT_ADS>		gamement_ads;		// ���� ���� ��Ʈ
//	CBase_Object<GAMEMISSION_ADS>	gamemission_ads;	// ���� �̼�
//	CBase_Object<GAMEQUESTION_REQ>	gamequestion_req;	// ���� ���� ����
//	CBase_Object<GAMEBILL_SND>		gamebill_snd;		// ���� ���

	BOOL				m_bNewChatMessage;			// ���ο� ä�� �޽��� �÷���
	int					m_iMessageType;				// �Ϲ� ä��/�ý��� �޽��� ����
	WCHAR				m_wszChatMessage[256];		// ä�� �޼���

public:
	cGameBoard();
	~cGameBoard();

//	void set_room_info( ROOMINFOCATALOG *ric ) { memcpy( &room_info, ric, sizeof(ROOMINFOCATALOG) ); }
//	ROOMINFOCATALOG *get_room_info() { return &room_info; }

	void set_jackpot( money_t money ) { jackpot_money = money; }
	money_t get_jackpot() { return jackpot_money; }

	void clear();									// ��� ������ �ʱ�ȭ�Ѵ�.

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

	void set_playing_cards( CARD *cards );			// ��� �ִ� ī�带 �����Ѵ�.
	void add_playing_cards( CARD *cards );			// ��� �ִ� ī�忡 �߰��Ѵ�.
	void add_playing_cards( CARD card );			// ��� �ִ� ī�忡 �߰��Ѵ�(����).
	void del_playing_cards( CARD *cards );			// ��� �ִ� ī�忡�� �����Ѵ�.
	void add_acquired_cards( CARD *cards );			// ���� ī�忡 �߰��Ѵ�.
	void del_acquired_cards( CARD *cards );			// ���� ī�忡�� �����Ѵ�.
	void add_acquired_opponent( CARD *cards );		// ������ ���� ī�忡 �߰��Ѵ�.
	void del_acquired_opponent( CARD *cards );		// ������ ���� ī�忡�� �����Ѵ�.
	void set_ground_cards( CARD *cards );			// �ٴڿ� �� ī�带 �����Ѵ�.
	void add_ground_cards( CARD *cards );			// �ٴڿ� �� ī�忡 �߰��Ѵ�.
	void add_ground_cards( CARD card );
	void del_ground_cards( CARD *cards );			// �ٴڿ� �� ī�忡�� �����Ѵ�.

	void set_select_card_requested( BOOL flag ) { bRequested_SelectCard = flag; }
	BOOL get_select_card_requested() { return bRequested_SelectCard; }

	void set_gamestart_requested( BOOL flag ) { bRequested_GameStart = flag; }
	BOOL get_gamestart_requested() { return bRequested_GameStart; }

	void set_gamequestion_requested( BOOL flag ) { bRequested_GameQuestion = flag; }
	BOOL get_gamequestion_requested() { return bRequested_GameQuestion; }

	void set_sep_ten_as_hide( BOOL flag ) { bSep_Ten_As_Hide = flag; }
	BOOL get_sep_ten_as_hide() { return bSep_Ten_As_Hide; }

	void set_number_of_player_cards( int n ) { number_of_player_cards = n; }	// �÷��̾� ī�� �� ����(������ ������)
	int  get_number_of_player_cards() { return number_of_player_cards; }		// �÷��̾� ī�� �� ȹ��(������ ������)

	void set_number_of_opponent_cards( int n ) { number_of_opponent_cards = n; }// ���� ī�� �� ����(������ ������)
	int  get_number_of_opponent_cards() { return number_of_opponent_cards; }	// ���� ī�� �� ȹ��(������ ������)

	void set_number_of_deck_cards( int n ) { number_of_deck_cards = n; }// �������� ī�� ���� �����Ѵ�.
	void inc_number_of_deck_cards() { number_of_deck_cards++; }			// �������� ī�� ���� ������Ų��.
	void dec_number_of_deck_cards() { number_of_deck_cards--; }			// �������� ī�� ���� ���ҽ�Ų��.
	int  get_number_of_deck_cards() { return number_of_deck_cards; }	// �������� ī�� ���� ��´�.

	int  get_deck_height() { return (int)( ( number_of_deck_cards + 1 ) / 2 ); }

	void set_turn_counter( int turn ) { turn_counter = turn; }			// �� ī���͸� �����Ѵ�.
	void inc_turn_counter() { turn_counter++; }							// �� ī���͸� ������Ų��.
	int  get_turn_counter() { return turn_counter; }					// �� ī���͸� ��´�.

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

	// �ӽ� ���α׷��� //
	cCard card_dack;								// ī�� ��
};

#endif

