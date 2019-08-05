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
#include "StdAfx.h"
#include "Board.h"
#include "GameBoard.h"


cGameBoard::cGameBoard()
{
	jackpot_money            = 0;

	ground_card_positioner   = new cGroundCardPositioner;
	player_card_positioner   = new cAcquiredCardPositioner( TRUE );
	opponent_card_positioner = new cAcquiredCardPositioner( FALSE );
	playing_card_positioner  = new cPlayingCardPositioner( TRUE );
	counting_card_positioner = new cPlayingCardPositioner( FALSE );

//	memset( &room_info, 0x00, sizeof(ROOMINFOCATALOG) );

	m_bNewChatMessage        = FALSE;
	memset( m_wszChatMessage, 0x00, sizeof(m_wszChatMessage) );

	clear();
}

cGameBoard::~cGameBoard()
{
	delete ground_card_positioner;
	delete player_card_positioner;
	delete opponent_card_positioner;
	delete playing_card_positioner;
	delete counting_card_positioner;
}

void cGameBoard::clear()
{
	status					= GAMEBOARD_WAITING;

	user_slot				= INVALID_SLOT;
	player_slot				= INVALID_SLOT;
	first_slot				= INVALID_SLOT;
	turn_slot				= INVALID_SLOT;
	timer_slot				= INVALID_SLOT;

	playing_cards.clear();
	acquired_cards.clear();
	acquired_opponent.clear();
	ground_cards.clear();

	ground_card_positioner->Initialize();
	player_card_positioner->Initialize();
	opponent_card_positioner->Initialize();
	// 5장씩이나 10씩 쌓이게 하는 루틴 초기화를 위해. Initialize() 를 사용하면 어디선가 꼬여버려서 에러가 남.
	player_card_positioner->clear();
	opponent_card_positioner->clear();
	/////////////////////////////////
	playing_card_positioner->Initialize();
	counting_card_positioner->Initialize();

	bRequested_GameStart	= FALSE;
	bRequested_SelectCard	= FALSE;
	bRequested_GameQuestion	= FALSE;
	bSep_Ten_As_Hide		= FALSE;

	number_of_player_cards	= 0;						// 플레이어 카드 수(관전 모드 지원용)
	number_of_opponent_cards= 0;						// 상대방 카드 수(관전 모드 지원용)

	number_of_deck_cards	= 0;						// 뒤집혀진 카드 수
	turn_counter			= 0;						// 턴 수

	// 플레이어 상태 초기화
	for(int i = 0; i < 2; i++ ) playerStatus[i].clear();					

	game_animation			= GAMEANIMATION_INVALID;
	bAnimating				= FALSE;
	bMissionCompleted		= FALSE;

//	memset( &gamement_ads,		0x00, sizeof(GAMEMENT_ADS)		);
//	memset( &gamemission_ads,	0x00, sizeof(GAMEMISSION_ADS)	);
//	memset( &gamequestion_req,	0x00, sizeof(GAMEQUESTION_REQ)	);
//	memset( &gamebill_snd,		0x00, sizeof(GAMEBILL_SND)		);
}

void cGameBoard::set_userid( LPCWSTR userid1, LPCWSTR userid2 )
{
	playerStatus[0].set_userid( userid1 );
	playerStatus[1].set_userid( userid2 );
}

void cGameBoard::set_player_money( money_t money1, money_t money2 )
{
	playerStatus[0].set_player_money( money1 );
	playerStatus[1].set_player_money( money2 );
}

void cGameBoard::set_level( int level1, int level2 )
{
	playerStatus[0].set_level( level1 );
	playerStatus[1].set_level( level2 );
}

void cGameBoard::set_avatar( int avatar1, int avatar2 )
{
	playerStatus[0].set_avatar( avatar1 );
	playerStatus[1].set_avatar( avatar2 );
}

void cGameBoard::set_stars( int stars1, int stars2 )
{
	playerStatus[0].set_number_of_stars( stars1 );
	playerStatus[1].set_number_of_stars( stars2 );
}

void cGameBoard::set_game_balance( money_t balance1, money_t balance2 )
{
	playerStatus[0].set_game_balance( balance1 );
	playerStatus[1].set_game_balance( balance2 );
}

//---------------------------------------------------------------------------//

void cGameBoard::set_playing_cards( CARD *cards )			// 들고 있는 카드를 설정한다.
{
	playing_cards = cCard( cards );
}

void cGameBoard::add_playing_cards( CARD *cards )			// 들고 있는 카드에 추가한다.
{
	playing_cards.append( cards );
}

void cGameBoard::add_playing_cards( CARD card )				// 들고 있는 카드에 추가한다.
{
	playing_cards.append( card );
}

void cGameBoard::del_playing_cards( CARD *cards )			// 들고 있는 카드에서 삭제한다.
{
	cCard pickup_cards( cards );

	playing_cards = playing_cards - pickup_cards;
}

void cGameBoard::add_acquired_cards( CARD *cards )			// 먹은 카드에 추가한다.
{
	acquired_cards.append( cards );
}

void cGameBoard::del_acquired_cards( CARD *cards )			// 먹은 카드에서 삭제한다.
{
	cCard pickup_cards( cards );

	acquired_cards  = acquired_cards - pickup_cards;
}

void cGameBoard::add_acquired_opponent( CARD *cards )		// 상대방이 먹은 카드에 추가한다.
{
	acquired_opponent.append( cards );
}

void cGameBoard::del_acquired_opponent( CARD *cards )		// 상대방이 먹은 카드에서 삭제한다.
{
	cCard pickup_cards( cards );

	acquired_opponent  = acquired_opponent - pickup_cards;
}

void cGameBoard::set_ground_cards( CARD *cards )			// 바닥에 깔린 카드를 설정한다.
{
	ground_cards = cCard( cards );
}

void cGameBoard::add_ground_cards( CARD *cards )			// 바닥에 깔린 카드에 추가한다.
{
	ground_cards.append( cards );
}

void cGameBoard::add_ground_cards( CARD card )				// 바닥에 깔린 카드를 한장 추가한다.
{
	ground_cards.append( card );
}

void cGameBoard::del_ground_cards( CARD *cards )			// 바닥에 깔린 카드에서 삭제한다.
{
	cCard pickup_cards( cards );

	ground_cards = ground_cards - pickup_cards;
}
/*
//---------------------------------------------------------------------------//

void cGameBoard::add_chatmessage( char *szChatMessage, int iType )
{
	cUnicodeEncoding::MBS2WSTR( szChatMessage, m_wszChatMessage, 256 - 1 );
	m_wszChatMessage[256 - 1]	= '\0';
	m_iMessageType				= iType;
	m_bNewChatMessage			= TRUE;
}

int  cGameBoard::get_chatmessage( LPWSTR *ppwszMessage )
{
	if( !m_bNewChatMessage ) return 0;
	m_bNewChatMessage			= FALSE;
	*ppwszMessage				= m_wszChatMessage;
	return m_iMessageType;
}

//---------------------------------------------------------------------------//
	
void cGameBoard::clear_userlist()
{
	cInvitationListBox* pControl = (cInvitationListBox*)( GetBoardManager()->m_InvitationDlg.GetControl( IDC_INVITATION_LIST ) );
	if( !pControl ) return;

	pControl->RemoveAllItems();
}

void cGameBoard::update_userlist( USERINFOSHORT* uis )
{
	cInvitationListBox* pControl = (cInvitationListBox*)( GetBoardManager()->m_InvitationDlg.GetControl( IDC_INVITATION_LIST ) );
	if( !pControl ) return;

	WCHAR wszUserid[MAX_USERID];
	cUnicodeEncoding::MBS2WSTR( (LPCSTR)uis->userid, wszUserid, MAX_USERID - 1 );
	wszUserid[MAX_USERID - 1] = L'\0';

	WCHAR wszLevel[11];
	cUnicodeEncoding::MBS2WSTR( (LPCSTR)get_level_code_to_string( uis->level ), wszLevel, 11 - 1 );
	wszLevel[11 - 1] = '\0';

	WCHAR strText[256];
	swprintf( strText, L"%d\t%s\t%s", 
			uis->sex,								// 성별
			wszUserid,								// 아이디
			wszLevel								// 레벨
	);

	pControl->RemoveItemByText( wszUserid );		// 이미 등록된 사용자가 있다면 지운다.
	pControl->AddItem( strText, (void*)NULL );
}

void cGameBoard::delete_userlist( USERINFOSHORT* uis )
{
	cInvitationListBox* pControl = (cInvitationListBox*)( GetBoardManager()->m_InvitationDlg.GetControl( IDC_INVITATION_LIST ) );
	if( !pControl ) return;

	WCHAR wszUserid[MAX_USERID];
	cUnicodeEncoding::MBS2WSTR( (LPCSTR)uis->userid, wszUserid, MAX_USERID - 1 );
	wszUserid[MAX_USERID - 1] = L'\0';

	pControl->RemoveItemByText( wszUserid );
}
*/
//---------------------------------------------------------------------------//

