
#include "stdafx.h"
#include "DlgMain.h"
#include "..\Game.h"
//-----------------------------------------------------------------------------
// Global access sIngleton
//-----------------------------------------------------------------------------
_CDlgMain			cDlgMainBlock;
unsigned long	_init_CDlgMain::ms_dwLong;

// 소멸은 ~cUIDialog 알아서 된다.

// 다이얼로그들을 지워준다.
CDlgMain::~CDlgMain()
{
	m_nPopupMenu = 0;
}

// 생성
// 다이얼로그들을 생성한다.
void CDlgMain::Create()
{
	m_nPopupMenu = 0;
}

// 다이얼로드 메세지
void CALLBACK CDlgMain::UIDialogProc( UINT nDialogID, int nControlID, UINT nEvent, cUIControl* pControl )
{
	switch( nDialogID )
	{

		break;
	}
}

// 메세지 loop
LRESULT CALLBACK CDlgMain::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( m_nPopupMenu != 0 )
	{
		switch( m_nPopupMenu )
		{
		}
	}

	if(g_cGame.GetGameState() == GAME_LOBBY)
	{
	}
	if(g_cGame.GetGameState() == GAME_BOARD)
	{
	}
	return 0;
}
// 그려주기
void CDlgMain::Render()
{
	if(g_cGame.GetGameState() == GAME_LOBBY)
	{
	}
	else if(g_cGame.GetGameState() == GAME_BOARD)
	{
	}
}

// 방만들기 할때 불러줘야지.
void CDlgMain::MakeRoom()
{
}
