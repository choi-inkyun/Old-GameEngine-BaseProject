
#include "stdafx.h"
#include "WebBrowser.h"
#include "..\Game.h"

CWebBrowser::CWebBrowser()
: m_bNowWebBrowser(false)
{

}
// 인터넷 익스플로어를 만든다.
BOOL CWebBrowser::ExplorerCreate()
{
	// 첫번째 explorer 창
	if (FAILED(CoInitialize(NULL))) return FALSE;

	// Instantiate a browser
	if (FAILED(CoCreateInstance(CLSID_InternetExplorer,NULL,
		CLSCTX_LOCAL_SERVER, IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser))) return FALSE;

	// Capture Explorer HWND
	if((m_hWnd_Explorer = FindWindow("IEFrame", "Microsoft Internet Explorer")) == NULL) return FALSE;

	// 메인 윈도우랑 익스플로어 창이랑 연결한다. 메인윈도우 안에 속하게 된다.
	SetParent(m_hWnd_Explorer, g_cGame.m_MainWindowHwnd);

	_bstr_t bs("http://www.daum.net");
	//	m_pWebBrowser->GetHostInfo(false);
	VARIANT vEmpty;
	VariantInit(&vEmpty);

	m_pWebBrowser->Navigate(bs, &vEmpty, &vEmpty, &vEmpty, &vEmpty); 

	SetWindowPos(m_hWnd_Explorer, NULL, -2, -21, 224, 789, NULL);

	m_pWebBrowser->put_ToolBar(VARIANT_FALSE);  // 익스플로어 툴바 없앰
	m_pWebBrowser->put_MenuBar(VARIANT_FALSE);  // 메뉴바 없앰
	m_pWebBrowser->put_AddressBar(VARIANT_FALSE);  // 주소창 없앰
	m_pWebBrowser->put_StatusBar(VARIANT_FALSE);     // 상태바 없앰
	m_pWebBrowser->put_Offline(VARIANT_FALSE);
	m_pWebBrowser->put_Resizable(VARIANT_FALSE);
	m_pWebBrowser->put_FullScreen(VARIANT_FALSE);
	m_pWebBrowser->put_Visible(VARIANT_TRUE);

	HRGN hRgnTemp = CreateRectRgn(2, 21, 224, 789);
	SetWindowRgn(m_hWnd_Explorer2, hRgnTemp, TRUE);
	// 윈도우를 띄운다.
	ShowWindow(m_hWnd_Explorer, SW_NORMAL);
	UpdateWindow(m_hWnd_Explorer);


	// 두번째 explorer 창
	if (FAILED(CoInitialize(NULL))) return FALSE;

	// Instantiate a browser
	if (FAILED(CoCreateInstance(CLSID_InternetExplorer,NULL,
		CLSCTX_LOCAL_SERVER, IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser2))) return FALSE;

	// Capture Explorer HWND
	if((m_hWnd_Explorer2 = FindWindow("IEFrame", "Microsoft Internet Explorer")) == NULL) return FALSE;

	// 메인 윈도우랑 익스플로어 창이랑 연결한다. 메인윈도우 안에 속하게 된다.
	SetParent(m_hWnd_Explorer2, g_cGame.m_MainWindowHwnd);

	_bstr_t bs2("http://www.daum.net");
	//	_bstr_t bs2("http://www.etnews.co.kr/sokbo/xhome_sokbo.htm");
	VARIANT vEmpty2;
	VariantInit(&vEmpty2);

	m_pWebBrowser2->Navigate(bs2, &vEmpty2, &vEmpty2, &vEmpty2, &vEmpty2); 

	SetWindowPos(m_hWnd_Explorer2, NULL, 222, -21, 800, 189, NULL);

	m_pWebBrowser2->put_ToolBar(VARIANT_FALSE);  // 익스플로어 툴바 없앰
	m_pWebBrowser2->put_MenuBar(VARIANT_FALSE);  // 메뉴바 없앰
	m_pWebBrowser2->put_AddressBar(VARIANT_FALSE);  // 주소창 없앰
	m_pWebBrowser2->put_StatusBar(VARIANT_FALSE);     // 상태바 없앰
	m_pWebBrowser2->put_Offline(VARIANT_FALSE);
	m_pWebBrowser2->put_Resizable(VARIANT_FALSE);
	m_pWebBrowser2->put_FullScreen(VARIANT_FALSE);
	m_pWebBrowser2->put_Visible(VARIANT_TRUE);

	HRGN hRgnTemp2 = CreateRectRgn(2, 21, 800, 189);
	SetWindowRgn(m_hWnd_Explorer2, hRgnTemp2, TRUE);

	ShowWindow(m_hWnd_Explorer2, SW_NORMAL);
	UpdateWindow(m_hWnd_Explorer2);

	// explorer 이 지금 띄어져 있나 안띄어져 있나 체크하는 변수
	m_bNowWebBrowser = true;
}

// Explorer 이 없어졌다 사라졌다.
// 특정 버튼을 누르거나 Window Mode 에서 FullScreen 으로 바뀔떄 같은 경우 적용해야한다.
void CWebBrowser::ExplorerToggle(int i_mode)
{
	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	// 0이면 Explorer 표시, 감추기 버튼을 눌렀다고 생각한다.
	if(i_mode == 0)
	{
		if(m_bNowWebBrowser == false)
		{
			// explorer 들을 만든다.
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - EXPLORER_WINDOW_WIDTH ) / 2, ( ScreenHeight  - EXPLORER_WINDOW_HEIGHT ) / 2, EXPLORER_WINDOW_WIDTH, EXPLORER_WINDOW_HEIGHT, NULL);
			ExplorerCreate();
			// 게임의 위치도 바뀌어야한다.
			SetWindowPos(g_cApplication.gethWnd(), NULL,224, 168, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		}
		else
		{
			// 원래 게임해상도는 800,600 이기 때문에..
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight  - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			ExplorerDestory();
			// 실제 게임의 위치가 0,0으로 지정되야 한다.
			SetWindowPos(g_cApplication.gethWnd(), NULL,0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		}
	}
	
	// 1 이면 explorer 이 띄어져 있는 상태에서 모드 전환이 되었다고 생각한다.
	else if(i_mode == 1)
	{
		// 전환한 후에 이놈을 불러줘야한다.
		if(g_cApplication.Get_Windowed() == true)
		{
			if(m_bNowWebBrowser == true)
			{
				// explorer 들을 만든다.
				SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - EXPLORER_WINDOW_WIDTH ) / 2, ( ScreenHeight  - EXPLORER_WINDOW_HEIGHT ) / 2, EXPLORER_WINDOW_WIDTH, EXPLORER_WINDOW_HEIGHT, NULL);
				ExplorerCreate();
				// 게임의 위치도 바뀌어야한다.
				SetWindowPos(g_cApplication.gethWnd(), NULL,224, 168, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			}
			else
			{
				// 원래 게임해상도는 800,600 이기 때문에..
				SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight  - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
				// 실제 게임의 위치가 0,0으로 지정되야 한다.
				SetWindowPos(g_cApplication.gethWnd(), NULL,0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			}
		}
		else
		{
			// 원래 게임해상도는 800,600 이기 때문에..
//			SetWindowPos(m_MainWindowHwnd, NULL, ( ScreenWidth  - 800 ) / 2, ( ScreenHeight  - 600 ) / 2, 800, 600, NULL);
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, 0,0, ScreenWidth, ScreenHeight, NULL);
			if(m_bNowWebBrowser == true)
				ExplorerDestory(true);
			// 실제 게임의 위치가 0,0으로 지정되야 한다.
			SetWindowPos(g_cApplication.gethWnd(), NULL,0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		}
	}
}

void CWebBrowser::ExplorerDestory(bool b_check)
{
	if(m_hWnd_Explorer == NULL)
		return;

	m_pWebBrowser->Quit();
	m_pWebBrowser->Release();
	m_pWebBrowser2->Quit();
	m_pWebBrowser2->Release();

	m_hWnd_Explorer  = NULL;
	m_hWnd_Explorer2 = NULL;
	// explorer 이 지금 띄어져 있나 안띄어져 있나 체크하는 변수
	if(b_check == false)
		m_bNowWebBrowser = false;
}