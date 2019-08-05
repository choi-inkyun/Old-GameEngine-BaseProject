

#include "stdafx.h"
#include "Game.h"
/*
	Game.h, Game.cpp 와 역활은 같다
	단지 cpp 한곳에다가 몰아 넣으면 각 cpp 의 역활을 구분하기도 힘들고 복잡해 지기 때문에
	메세지를 주고 받는 부분은 여기다가 처리하였다.
*/
/////////////////////////////////////////////////////////////
//////////////////// 윈도우 프로시져/////////////////////////
LRESULT CALLBACK CGame::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	if(m_nGameState == GAME_BOARD)
//		CBoard::MsgProc( hWnd, message, wParam, lParam );

	// 여기엔 게임 전체에 쓰일만한 메세지들을 넣는다.
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:

		hdc = BeginPaint(m_WebBrowser.m_hWnd_Explorer, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(m_WebBrowser.m_hWnd_Explorer, &ps);
		hdc = BeginPaint(m_WebBrowser.m_hWnd_Explorer2, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(m_WebBrowser.m_hWnd_Explorer2, &ps);

		break;

		//Client영역을 캡션바 눌린 것으로 인식 하게 하는 방법.
	case WM_LBUTTONDOWN:
		{    //버튼이 눌리면
			if(g_cApplication.Get_Windowed() == true)
			{
				RECT    rt;
				// 렉트와 마우스 좌표를 얻어와야한다.
				POINT    pt;
				// 이 영영에선 마우스로 윈도우를 움직일 수 있게 한다.
				SetRect( &rt, 0,0,700,50 );
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);        
				if( PtInRect( &rt, pt ) )    //마우스 좌표와 영역을 비교 하여 판단 한다.
				{
					// 어떤 윈도우의 핸들을 넣느냐에 따라 그 윈도우가 움직이게 된다.
					SendMessage(m_MainWindowHwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
					// 이럴경우에 lbuttonup 이 오지 않으므로 이렇게 호출을 따로 해주는 꽁수를 써야한다.
					SendMessage(g_cApplication.gethWnd(),WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
				}
			}
			// 풀스크린 모드일때는 무저건 게임이 포커스를 갖도록
			else
				SetFocus(g_cApplication.gethWnd());
		}
		break;
	case WM_LBUTTONUP:
		{
			// 위애 캡션바가 있다고 생각하고 끌어왔을떄 mainwindow 핸들로 포커스를 보내기 떄문에 여기서 다시 셋팅을 해줘야
			// 게임에 포커스가 생기기 떄문이다. 좌표를 계산 할 수 도 있지만 그러면 오히려 처리가 많아 지니까..
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
			// Q 를 누르면 프로그램이 종료된다.
			g_cMainEngine.m_RenderState = RENDERSTATE_EXIT;
			PostQuitMessage( 0 );
			return 0;
		case VK_F3:
			// T 를 누르면 fullscreen mode 와 window mode 로 변경 할 수 있다.
			g_cMainEngine.ToggleMode();
			m_WebBrowser.ExplorerToggle(1);
			return 1;
		case VK_F4:
			// 윈도우 모드일때만 explorer 창이 나올 수 있음.
			if(g_cApplication.Get_Windowed() == true)
			{
				// W 를 누르면 Explorer 을 띄우거나 닫는 화면전환을 할 수 있다..
				m_WebBrowser.ExplorerToggle(0);
			}
			return 1;
		case VK_F5:
			// 로그인 화면으로
			g_cMainEngine.m_RenderState = RENDERSTATE_LOGIN;
			return 1;
		case VK_F6:
			// 채널 화면
			g_cMainEngine.m_RenderState = RENDERSTATE_CHANNEL;
			return 1;
		case VK_F7:
			// 로비 화면
			g_cMainEngine.m_RenderState = RENDERSTATE_LOBBY;
			return 1;
		case VK_F8:
			// 게임 화면
			g_cMainEngine.m_RenderState = RENDERSTATE_BOARD;
			return 1;
		}
		break;
// 이거 주석 풀면 한글 입력할때 4개씩 나오고 이상하더라구요.. 왜그럴까요?!
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/////////////////////////////////////////////////////////////
//////////////////// 게임 진행 메세지 ///////////////////////

void CGame::GameProc( void *Ptr, long Purpose )
{
	switch( Purpose )
	{
	case PURPOSE_RENDER:
		// 화면을 그린다.
		ASSERT( __singleton() );
		__singleton()->Render();		
		break;
	case PURPOSE_PROCESS:
		// 정보를 처리한다.
		ASSERT( __singleton() );
		__singleton()->Process();	
		break;
	case PURPOSE_MSGPROC:
		{
			// 메세지들을 처리한다.
			ASSERT( __singleton() );

			sMsgParam *pMsgParam = (sMsgParam*)Ptr;
			pMsgParam->bHandled = __singleton()->MsgProc( pMsgParam->hWnd, pMsgParam->uMsg, pMsgParam->wParam, pMsgParam->lParam );
//			if( g_cDlgMain.MsgProc( hWnd, message, wParam, lParam ) ) return TRUE;
			g_cDlgMain.MsgProc(  pMsgParam->hWnd, pMsgParam->uMsg, pMsgParam->wParam, pMsgParam->lParam );

			break;
		}
	case PURPOSE_INITIALIZE:
		// 화면에 대한 정보를 생성한다
		g_cGame.initialize();
		break;
	case PURPOSE_RELEASE:
		// 게임을 초기화한다.
		//		cGameMain->release();
		break;
	case PURPOSE_CREATE:
		// 프로그램을 생성한다.
		ASSERT( __singleton() );
		__singleton()->startup();
		break;
	case PURPOSE_DESTROY:
		// 프로그램을 종료한다.
		if( __singleton() )
		{
			__singleton()->release();
		}
		break;
	}
}