
#include "stdafx.h"
#include "DlgMain.h"
#include "..\Game.h"
//-----------------------------------------------------------------------------
// Global access sIngleton
//-----------------------------------------------------------------------------
_CDlgMain			cDlgMainBlock;
unsigned long	_init_CDlgMain::ms_dwLong;

// �Ҹ��� ~cUIDialog �˾Ƽ� �ȴ�.

// ���̾�α׵��� �����ش�.
CDlgMain::~CDlgMain()
{
	m_nPopupMenu = 0;
}

// ����
// ���̾�α׵��� �����Ѵ�.
void CDlgMain::Create()
{
	m_nPopupMenu = 0;
}

// ���̾�ε� �޼���
void CALLBACK CDlgMain::UIDialogProc( UINT nDialogID, int nControlID, UINT nEvent, cUIControl* pControl )
{
	switch( nDialogID )
	{

		break;
	}
}

// �޼��� loop
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
// �׷��ֱ�
void CDlgMain::Render()
{
	if(g_cGame.GetGameState() == GAME_LOBBY)
	{
	}
	else if(g_cGame.GetGameState() == GAME_BOARD)
	{
	}
}

// �游��� �Ҷ� �ҷ������.
void CDlgMain::MakeRoom()
{
}
