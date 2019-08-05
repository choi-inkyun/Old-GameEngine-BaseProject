

#include "stdafx.h"
#include "Game.h"

// 여러 버튼들을 그리고 처리해준다..
BOOL CGame::CommonRenderButton()
{
	//HDC hdc;
	//BYTE n_buttoncheck = 0;

	///// 윈도우 최소화 버튼 /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_MINIMIZE_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.

	//	// 풀모드라면 윈도우 모드라 바꿔주고 나서 최소화 해야한다.
	//	if(g_cApplication.Get_Windowed()==false)
	//		g_cMainEngine.ToggleMode();
	//	ShowWindow( m_MainWindowHwnd, SW_MINIMIZE );
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	/////////////////////////////////
	/////// 윈도우 종료 버튼 /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_EXIT_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.

	//	PostQuitMessage(0);
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	/////////////////////////////////
	/////// 옵션 버튼 //////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_OPTION_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	/////////////////////////////////
	/////// 캡쳐 버튼 /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_CAPTURE_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	ScreenCapture();
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	//////////////////////////////////////
	//// 윈도우 모드 일때와 풀모드일 때를 구분하여 출력 하여야 한다.
	//if(g_cApplication.Get_Windowed()==true)
	//{
	//	n_buttoncheck = g_cGame.m_ImageManager.Draw("WINDOW_FULLMODE_BUTTON");
	//	if(n_buttoncheck == 2)
	//	{
	//		SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.

	//		// T 를 누르면 fullscreen mode 와 window mode 로 변경 할 수 있다.	
	//		g_cMainEngine.ToggleMode();
	//		m_WebBrowser.ExplorerToggle(1);
	//	}
	//	else if(n_buttoncheck == 1)
	//		SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	//}
	//else
	//{
	//	n_buttoncheck = m_ImageManager.Draw("WINDOW_WINDOWMODE_BUTTON");
	//	if(n_buttoncheck == 2)
	//	{
	//		SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.
	//		// T 를 누르면 fullscreen mode 와 window mode 로 변경 할 수 있다.	
	//		g_cMainEngine.ToggleMode();
	//		m_WebBrowser.ExplorerToggle(1);
	//	}
	//	else if(n_buttoncheck == 1)
	//		SoundPlay("GAME_REFRESH"); // 버튼 오버 했을때 임시 소리.
	//}

	return true;
}