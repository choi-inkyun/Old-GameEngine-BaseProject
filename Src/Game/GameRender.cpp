

#include "stdafx.h"
#include "Game.h"

// ���� ��ư���� �׸��� ó�����ش�..
BOOL CGame::CommonRenderButton()
{
	//HDC hdc;
	//BYTE n_buttoncheck = 0;

	///// ������ �ּ�ȭ ��ư /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_MINIMIZE_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.

	//	// Ǯ����� ������ ���� �ٲ��ְ� ���� �ּ�ȭ �ؾ��Ѵ�.
	//	if(g_cApplication.Get_Windowed()==false)
	//		g_cMainEngine.ToggleMode();
	//	ShowWindow( m_MainWindowHwnd, SW_MINIMIZE );
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	/////////////////////////////////
	/////// ������ ���� ��ư /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_EXIT_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.

	//	PostQuitMessage(0);
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	/////////////////////////////////
	/////// �ɼ� ��ư //////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_OPTION_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	/////////////////////////////////
	/////// ĸ�� ��ư /////
	//n_buttoncheck = m_ImageManager.Draw("WINDOW_CAPTURE_BUTTON");
	//if(n_buttoncheck == 2)
	//{
	//	ScreenCapture();
	//}
	//else if(n_buttoncheck == 1)
	//	SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	//////////////////////////////////////
	//// ������ ��� �϶��� Ǯ����� ���� �����Ͽ� ��� �Ͽ��� �Ѵ�.
	//if(g_cApplication.Get_Windowed()==true)
	//{
	//	n_buttoncheck = g_cGame.m_ImageManager.Draw("WINDOW_FULLMODE_BUTTON");
	//	if(n_buttoncheck == 2)
	//	{
	//		SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.

	//		// T �� ������ fullscreen mode �� window mode �� ���� �� �� �ִ�.	
	//		g_cMainEngine.ToggleMode();
	//		m_WebBrowser.ExplorerToggle(1);
	//	}
	//	else if(n_buttoncheck == 1)
	//		SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	//}
	//else
	//{
	//	n_buttoncheck = m_ImageManager.Draw("WINDOW_WINDOWMODE_BUTTON");
	//	if(n_buttoncheck == 2)
	//	{
	//		SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.
	//		// T �� ������ fullscreen mode �� window mode �� ���� �� �� �ִ�.	
	//		g_cMainEngine.ToggleMode();
	//		m_WebBrowser.ExplorerToggle(1);
	//	}
	//	else if(n_buttoncheck == 1)
	//		SoundPlay("GAME_REFRESH"); // ��ư ���� ������ �ӽ� �Ҹ�.
	//}

	return true;
}