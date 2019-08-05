
#include "stdafx.h"
#include "WebBrowser.h"
#include "..\Game.h"

CWebBrowser::CWebBrowser()
: m_bNowWebBrowser(false)
{

}
// ���ͳ� �ͽ��÷ξ �����.
BOOL CWebBrowser::ExplorerCreate()
{
	// ù��° explorer â
	if (FAILED(CoInitialize(NULL))) return FALSE;

	// Instantiate a browser
	if (FAILED(CoCreateInstance(CLSID_InternetExplorer,NULL,
		CLSCTX_LOCAL_SERVER, IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser))) return FALSE;

	// Capture Explorer HWND
	if((m_hWnd_Explorer = FindWindow("IEFrame", "Microsoft Internet Explorer")) == NULL) return FALSE;

	// ���� ������� �ͽ��÷ξ� â�̶� �����Ѵ�. ���������� �ȿ� ���ϰ� �ȴ�.
	SetParent(m_hWnd_Explorer, g_cGame.m_MainWindowHwnd);

	_bstr_t bs("http://www.daum.net");
	//	m_pWebBrowser->GetHostInfo(false);
	VARIANT vEmpty;
	VariantInit(&vEmpty);

	m_pWebBrowser->Navigate(bs, &vEmpty, &vEmpty, &vEmpty, &vEmpty); 

	SetWindowPos(m_hWnd_Explorer, NULL, -2, -21, 224, 789, NULL);

	m_pWebBrowser->put_ToolBar(VARIANT_FALSE);  // �ͽ��÷ξ� ���� ����
	m_pWebBrowser->put_MenuBar(VARIANT_FALSE);  // �޴��� ����
	m_pWebBrowser->put_AddressBar(VARIANT_FALSE);  // �ּ�â ����
	m_pWebBrowser->put_StatusBar(VARIANT_FALSE);     // ���¹� ����
	m_pWebBrowser->put_Offline(VARIANT_FALSE);
	m_pWebBrowser->put_Resizable(VARIANT_FALSE);
	m_pWebBrowser->put_FullScreen(VARIANT_FALSE);
	m_pWebBrowser->put_Visible(VARIANT_TRUE);

	HRGN hRgnTemp = CreateRectRgn(2, 21, 224, 789);
	SetWindowRgn(m_hWnd_Explorer2, hRgnTemp, TRUE);
	// �����츦 ����.
	ShowWindow(m_hWnd_Explorer, SW_NORMAL);
	UpdateWindow(m_hWnd_Explorer);


	// �ι�° explorer â
	if (FAILED(CoInitialize(NULL))) return FALSE;

	// Instantiate a browser
	if (FAILED(CoCreateInstance(CLSID_InternetExplorer,NULL,
		CLSCTX_LOCAL_SERVER, IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser2))) return FALSE;

	// Capture Explorer HWND
	if((m_hWnd_Explorer2 = FindWindow("IEFrame", "Microsoft Internet Explorer")) == NULL) return FALSE;

	// ���� ������� �ͽ��÷ξ� â�̶� �����Ѵ�. ���������� �ȿ� ���ϰ� �ȴ�.
	SetParent(m_hWnd_Explorer2, g_cGame.m_MainWindowHwnd);

	_bstr_t bs2("http://www.daum.net");
	//	_bstr_t bs2("http://www.etnews.co.kr/sokbo/xhome_sokbo.htm");
	VARIANT vEmpty2;
	VariantInit(&vEmpty2);

	m_pWebBrowser2->Navigate(bs2, &vEmpty2, &vEmpty2, &vEmpty2, &vEmpty2); 

	SetWindowPos(m_hWnd_Explorer2, NULL, 222, -21, 800, 189, NULL);

	m_pWebBrowser2->put_ToolBar(VARIANT_FALSE);  // �ͽ��÷ξ� ���� ����
	m_pWebBrowser2->put_MenuBar(VARIANT_FALSE);  // �޴��� ����
	m_pWebBrowser2->put_AddressBar(VARIANT_FALSE);  // �ּ�â ����
	m_pWebBrowser2->put_StatusBar(VARIANT_FALSE);     // ���¹� ����
	m_pWebBrowser2->put_Offline(VARIANT_FALSE);
	m_pWebBrowser2->put_Resizable(VARIANT_FALSE);
	m_pWebBrowser2->put_FullScreen(VARIANT_FALSE);
	m_pWebBrowser2->put_Visible(VARIANT_TRUE);

	HRGN hRgnTemp2 = CreateRectRgn(2, 21, 800, 189);
	SetWindowRgn(m_hWnd_Explorer2, hRgnTemp2, TRUE);

	ShowWindow(m_hWnd_Explorer2, SW_NORMAL);
	UpdateWindow(m_hWnd_Explorer2);

	// explorer �� ���� ����� �ֳ� �ȶ���� �ֳ� üũ�ϴ� ����
	m_bNowWebBrowser = true;
}

// Explorer �� �������� �������.
// Ư�� ��ư�� �����ų� Window Mode ���� FullScreen ���� �ٲ��� ���� ��� �����ؾ��Ѵ�.
void CWebBrowser::ExplorerToggle(int i_mode)
{
	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	// 0�̸� Explorer ǥ��, ���߱� ��ư�� �����ٰ� �����Ѵ�.
	if(i_mode == 0)
	{
		if(m_bNowWebBrowser == false)
		{
			// explorer ���� �����.
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - EXPLORER_WINDOW_WIDTH ) / 2, ( ScreenHeight  - EXPLORER_WINDOW_HEIGHT ) / 2, EXPLORER_WINDOW_WIDTH, EXPLORER_WINDOW_HEIGHT, NULL);
			ExplorerCreate();
			// ������ ��ġ�� �ٲ����Ѵ�.
			SetWindowPos(g_cApplication.gethWnd(), NULL,224, 168, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		}
		else
		{
			// ���� �����ػ󵵴� 800,600 �̱� ������..
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight  - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			ExplorerDestory();
			// ���� ������ ��ġ�� 0,0���� �����Ǿ� �Ѵ�.
			SetWindowPos(g_cApplication.gethWnd(), NULL,0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		}
	}
	
	// 1 �̸� explorer �� ����� �ִ� ���¿��� ��� ��ȯ�� �Ǿ��ٰ� �����Ѵ�.
	else if(i_mode == 1)
	{
		// ��ȯ�� �Ŀ� �̳��� �ҷ�����Ѵ�.
		if(g_cApplication.Get_Windowed() == true)
		{
			if(m_bNowWebBrowser == true)
			{
				// explorer ���� �����.
				SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - EXPLORER_WINDOW_WIDTH ) / 2, ( ScreenHeight  - EXPLORER_WINDOW_HEIGHT ) / 2, EXPLORER_WINDOW_WIDTH, EXPLORER_WINDOW_HEIGHT, NULL);
				ExplorerCreate();
				// ������ ��ġ�� �ٲ����Ѵ�.
				SetWindowPos(g_cApplication.gethWnd(), NULL,224, 168, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			}
			else
			{
				// ���� �����ػ󵵴� 800,600 �̱� ������..
				SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, ( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight  - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
				// ���� ������ ��ġ�� 0,0���� �����Ǿ� �Ѵ�.
				SetWindowPos(g_cApplication.gethWnd(), NULL,0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
			}
		}
		else
		{
			// ���� �����ػ󵵴� 800,600 �̱� ������..
//			SetWindowPos(m_MainWindowHwnd, NULL, ( ScreenWidth  - 800 ) / 2, ( ScreenHeight  - 600 ) / 2, 800, 600, NULL);
			SetWindowPos(g_cGame.m_MainWindowHwnd, NULL, 0,0, ScreenWidth, ScreenHeight, NULL);
			if(m_bNowWebBrowser == true)
				ExplorerDestory(true);
			// ���� ������ ��ġ�� 0,0���� �����Ǿ� �Ѵ�.
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
	// explorer �� ���� ����� �ֳ� �ȶ���� �ֳ� üũ�ϴ� ����
	if(b_check == false)
		m_bNowWebBrowser = false;
}