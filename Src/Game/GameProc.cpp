

#include "stdafx.h"
#include "Game.h"
/*
	Game.h, Game.cpp �� ��Ȱ�� ����
	���� cpp �Ѱ����ٰ� ���� ������ �� cpp �� ��Ȱ�� �����ϱ⵵ ����� ������ ���� ������
	�޼����� �ְ� �޴� �κ��� ����ٰ� ó���Ͽ���.
*/
/////////////////////////////////////////////////////////////
//////////////////// ������ ���ν���/////////////////////////
LRESULT CALLBACK CGame::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	if(m_nGameState == GAME_BOARD)
//		CBoard::MsgProc( hWnd, message, wParam, lParam );

	// ���⿣ ���� ��ü�� ���ϸ��� �޼������� �ִ´�.
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:

		hdc = BeginPaint(m_WebBrowser.m_hWnd_Explorer, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(m_WebBrowser.m_hWnd_Explorer, &ps);
		hdc = BeginPaint(m_WebBrowser.m_hWnd_Explorer2, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(m_WebBrowser.m_hWnd_Explorer2, &ps);

		break;

		//Client������ ĸ�ǹ� ���� ������ �ν� �ϰ� �ϴ� ���.
	case WM_LBUTTONDOWN:
		{    //��ư�� ������
			if(g_cApplication.Get_Windowed() == true)
			{
				RECT    rt;
				// ��Ʈ�� ���콺 ��ǥ�� ���;��Ѵ�.
				POINT    pt;
				// �� �������� ���콺�� �����츦 ������ �� �ְ� �Ѵ�.
				SetRect( &rt, 0,0,700,50 );
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);        
				if( PtInRect( &rt, pt ) )    //���콺 ��ǥ�� ������ �� �Ͽ� �Ǵ� �Ѵ�.
				{
					// � �������� �ڵ��� �ִ��Ŀ� ���� �� �����찡 �����̰� �ȴ�.
					SendMessage(m_MainWindowHwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
					// �̷���쿡 lbuttonup �� ���� �����Ƿ� �̷��� ȣ���� ���� ���ִ� �Ǽ��� ����Ѵ�.
					SendMessage(g_cApplication.gethWnd(),WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
				}
			}
			// Ǯ��ũ�� ����϶��� ������ ������ ��Ŀ���� ������
			else
				SetFocus(g_cApplication.gethWnd());
		}
		break;
	case WM_LBUTTONUP:
		{
			// ���� ĸ�ǹٰ� �ִٰ� �����ϰ� ��������� mainwindow �ڵ�� ��Ŀ���� ������ ������ ���⼭ �ٽ� ������ �����
			// ���ӿ� ��Ŀ���� ����� �����̴�. ��ǥ�� ��� �� �� �� ������ �׷��� ������ ó���� ���� ���ϱ�..
			SetFocus(g_cApplication.gethWnd());
		}
		break;
	case WM_SIZE:
		{
			SetFocus(g_cApplication.gethWnd());
		}
		break;
	case WM_KEYUP:
		switch( wParam )
		{
		case VK_ESCAPE:
			// Q �� ������ ���α׷��� ����ȴ�.
			g_cMainEngine.m_RenderState = RENDERSTATE_EXIT;
			PostQuitMessage( 0 );
			return 0;
		case VK_F3:
			// T �� ������ fullscreen mode �� window mode �� ���� �� �� �ִ�.
			g_cMainEngine.ToggleMode();
			m_WebBrowser.ExplorerToggle(1);
			return 1;
		case VK_F4:
			// ������ ����϶��� explorer â�� ���� �� ����.
			if(g_cApplication.Get_Windowed() == true)
			{
				// W �� ������ Explorer �� ���ų� �ݴ� ȭ����ȯ�� �� �� �ִ�..
				m_WebBrowser.ExplorerToggle(0);
			}
			return 1;
		case VK_F5:
			// �α��� ȭ������
			g_cMainEngine.m_RenderState = RENDERSTATE_LOGIN;
			return 1;
		case VK_F6:
			// ä�� ȭ��
			g_cMainEngine.m_RenderState = RENDERSTATE_CHANNEL;
			return 1;
		case VK_F7:
			// �κ� ȭ��
			g_cMainEngine.m_RenderState = RENDERSTATE_LOBBY;
			return 1;
		case VK_F8:
			// ���� ȭ��
			g_cMainEngine.m_RenderState = RENDERSTATE_BOARD;
			return 1;
		}
		break;
// �̰� �ּ� Ǯ�� �ѱ� �Է��Ҷ� 4���� ������ �̻��ϴ��󱸿�.. �ֱ׷����?!
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/////////////////////////////////////////////////////////////
//////////////////// ���� ���� �޼��� ///////////////////////

void CGame::GameProc( void *Ptr, long Purpose )
{
	switch( Purpose )
	{
	case PURPOSE_RENDER:
		// ȭ���� �׸���.
		ASSERT( __singleton() );
		__singleton()->Render();		
		break;
	case PURPOSE_PROCESS:
		// ������ ó���Ѵ�.
		ASSERT( __singleton() );
		__singleton()->Process();	
		break;
	case PURPOSE_MSGPROC:
		{
			// �޼������� ó���Ѵ�.
			ASSERT( __singleton() );

			sMsgParam *pMsgParam = (sMsgParam*)Ptr;
			pMsgParam->bHandled = __singleton()->MsgProc( pMsgParam->hWnd, pMsgParam->uMsg, pMsgParam->wParam, pMsgParam->lParam );
//			if( g_cDlgMain.MsgProc( hWnd, message, wParam, lParam ) ) return TRUE;
			g_cDlgMain.MsgProc(  pMsgParam->hWnd, pMsgParam->uMsg, pMsgParam->wParam, pMsgParam->lParam );

			break;
		}
	case PURPOSE_INITIALIZE:
		// ȭ�鿡 ���� ������ �����Ѵ�
		g_cGame.initialize();
		break;
	case PURPOSE_RELEASE:
		// ������ �ʱ�ȭ�Ѵ�.
		//		cGameMain->release();
		break;
	case PURPOSE_CREATE:
		// ���α׷��� �����Ѵ�.
		ASSERT( __singleton() );
		__singleton()->startup();
		break;
	case PURPOSE_DESTROY:
		// ���α׷��� �����Ѵ�.
		if( __singleton() )
		{
			__singleton()->release();
		}
		break;
	}
}