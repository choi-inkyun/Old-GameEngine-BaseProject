// Game Main.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//
// ���� : ���α�
// Game Main �� Game dll �ܿ� �ٸ� 4�� Project �� DLL �� ������ �Ͽ�
// �����츦 �ʱ�ȭ ��Ű�� Game dll �� �̿��Ͽ� ������ ���� ��Ű�� ��Ȱ��
// �ϰ� �˴ϴ�.
// �ٸ� ������ ���� �� Base�� ������ �����̴ϴ�.

// g_cApplication -> Engine_Library(Application) Singleton
// g_cGame -> Game Singleton
// g_cStateManager -> Engine_Utility(StateManager) Singleton
// g_cMainEngine -> Engine_Main(Engine_Main) Singleton

//�۴��������������զ��Ϋ��� �ۦ��ধ��H�զ��զ� 
//�������������������������� �� ���� ���������� ���������������� �� 
//....................??...........................?? 
//.................?......?.....................?�Ƣ�? 
//..............?...??.?...................?.??��? 
//...........?...???..?.................?..???��? 
//.........?.....???...???????...???.��? 
//.......?.....................................................�Ƣ�? 
//.....?.........................................................�Ƣ�? 
//...???????..............................??????? 
//.?.............................................................�ƢƢ�? 
//?......??................................................??�Ƣ�? 
//?......??................................................??�Ƣ�? 
//?..................................................................�Ƣ�? 
//?..........???....?.........?.........?....???...�Ƣ�? 
//?.......?????....???...???....?????�Ƣ�? 
//...?......???....................................???�Ƣ�? 
//......???.......................................�ƢƢ�???  

//...............???????????????? 
//........................?????????? 
//.....................??............�Ƣ�???? 
//..................?��?...............�Ƣ�???? 
//...............?��??...............�Ƣ�????? 
//............?...?...?...............�Ƣ�??...?��? 
//.........?...?......?...............�ƢƢ�?......?��? 
//......?...?.........?...............�ƢƢ�?.........?...? 
//.........?............?............�ƢƢƢ�?............?		

#include "stdafx.h"
#include "Game Main.h"

#define MAX_LOADSTRING 128
// ���� �����Դϴ�.
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

FrameSkip m_FS;

bool   b_ExplorerCheck;

// ������ ���� ���� ���۵Ǵ� �κ��̴�.
//////////////////////////////////////////////////////////////////////////////////
INT WINAPI WinMain(
				   HINSTANCE _hInstance,				// in)		application instance
				   HINSTANCE _hPrevInstance,			// in)		previous instance
				   LPSTR _lpszCmdLine,					// in)		command line option
				   INT _nCmdShow)						// in)		window show option
				   // : the application's windows entry point
  //////////////////////////////////////////////////////////////////////////////////
{
	g_cMainEngine.SetWindowMode(true);

	// ó�� ���α׷��� ����������� explorer â�� �ִ� ���·� �����ų���� �ƴҲ���;
	b_ExplorerCheck = false;
	g_cGame.m_WebBrowser.m_bNowWebBrowser = b_ExplorerCheck;
	/////////// CURSOR
	// Ŀ���� �����Ѵ�.
	g_cGame.m_hCursor = LoadCursorFromFile(".\\Data\\Image\\game.ani");  
	g_cApplication.SetMouseCursor(g_cGame.m_hCursor);

	// exe ������ ���� ����Ѵ�.
	HICON m_hIcon;
	m_hIcon = LoadIcon(_hInstance,"Game_Main.ico");
	SendMessage(hWndle, WM_SETICON, TRUE, (LPARAM)m_hIcon);
	SendMessage(hWndle, WM_SETICON, FALSE, (LPARAM)m_hIcon);

	// ���� �����츦 �����Ѵ�.
	if(!MainWindowCreate(_hInstance)) return false;

	// �����츦 �����ϱ����� ������ �������ش�.
	g_cApplication.WIndowCreate("����","����",WndProc,WINDOW_WIDTH, WINDOW_HEIGHT, true, false, false
								//ws_popup �� �����Ƿν� Ÿ�̺�ٰ� ��������.
						      ,  WS_VISIBLE | WS_POPUP);

	// �ʱ�ȭ�� �ϰ� �����츦 �����Ѵ�
	g_cApplication.Run();

	SetParent(g_cApplication.gethWnd(), hWndle);

	g_cGame.InitMsg();

	MSG Msg;
	// Enter the message pump
	ZeroMemory( &Msg, sizeof(MSG) );

	m_FS.SetFramePerSec(40); // 60 fps ����. �⺻������.

	// �޼��� Loop �� ����. �̰� DLL �̳� LIB �ȿ� ������ ���⼭ ó���� �� �ִ� ����� ���⶧����,
	// ����� �Űܼ� ó���Ѵ�.
	while( Msg.message != WM_QUIT )
	{
		// Handle windows messages (if any)LPCTSTRB
		if( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &Msg );
			DispatchMessage( &Msg );
		}
		else
		{
			//******************************************************************
			//   �������� ���������� �ڵ�
			//      dt�� ���� ����Ǳ⶧���� �������� �������� �ڵ��.
			//      �ý����� �������� �� ���� ȣ��ȴ�.
			//******************************************************************
			float dt = DXUtil_Timer( TIMER_GETELAPSEDTIME ) ; // �������� ó���ϱ� ���ؼ� dxutil �� Ÿ�̸Ӹ� ����Ѵ�.

			// Do any processing other than rendering.
			if( m_FS.Update(dt) )
			{
				//******************************************************************
				//   �������� �������� �ڵ�
				//      �̺κ��� frame skip�� ���� ���� ���������� ���� �����Ѵ�.
				//******************************************************************
				if( FAILED( Process() ) ) break;
				g_cApplication.ProcessUpdateStats();
			}	

			if( ! m_FS.IsFrameSkip() ) 
			{ 
				//****************************************************************** 
				//   ��쿡 ���� ȣ����� �ʾƵ� �Ǵ� �ڵ� 
				//      �̺κ��� frame skip�� ���� skip�ɼ� �ִ�. 
				//      ������������ �ð��� ���� ��ƸԴ� �ڵ带 �ִ´�. 
				//      ���� �ý����� �������� �� ���� ȣ��ȴ�. 
				//****************************************************************** 
				if( !g_cMainEngine.IsPaused() && SUCCEEDED( g_cMainEngine.RestoreMode() ) )
				{
					// Do per-frame processing, break on FALSE return value
					if( FAILED( Render() ) ) break;

					// ������ ������Ʈ
					g_cApplication.RenderUpdateStats();
				}
			} 

			// fps �� ���� ������ ����.
			if(g_cApplication.render_get_fps() >= 75)
				m_FS.SetFramePerSec(70); // 70 fps ����.
			else if(g_cApplication.render_get_fps() >= 65)
				m_FS.SetFramePerSec(60); // 70 fps ����.
			else if(g_cApplication.render_get_fps() >= 55)
				m_FS.SetFramePerSec(50); // 60 fps ����.
			else
				m_FS.SetFramePerSec(40); // ���� 40�������� ����ȴ�.

		}
	}

	// ���� �������� ���������� �Ǹ� �����Ų��.
	g_cApplication.OnApplicationReleased();

	// uninitialize COM
	CoUninitialize();

	// Unregister the window class
	UnregisterClass( g_cApplication.getclassname() , g_cApplication.gethInstance() );
}

// ������ ���ν���. �޽����� ó���Ѵ�.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ���⿣ ���� ��ü�� ���ϸ��� �޼������� �ִ´�.

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		// �̰� ������ ������ �׷����� �ʴ´�.
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� �� ���� ó���� ��κ� Game.cpp �� �ִ� Progcess, Render �Լ����� �Ѵ�.
UINT Process()
{
	// Main Engine ���� ������ StateManager �� ���۵Ǳ� ������ ������ ���� ���ӿ��� ó�����ָ� �ȴ�.
	return g_cMainEngine.Process();
}

UINT Render()
{
	// Main Engine ���� ������ StateManager �� ���۵Ǳ� ������ ������ ���� ���ӿ��� ó�����ָ� �ȴ�.
	return g_cMainEngine.Render();
}

//
// �Լ�: MyRegisterClass()
//
// ����: â Ŭ������ ����մϴ�.
//
// ����:
//
// Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
// �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
// �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
// �ش� ���� ���α׷��� �����
// '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc2;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, IDC_ARROW);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
// �Լ�: InitInstance(HANDLE, int)
//
// ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
// ����:
//
// �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
// �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	//explorer â�� �ֳ� ���Ŀ� ���� �ߴ� ��ġ�� ũ�Ⱑ �ٸ���.
	if(b_ExplorerCheck == true)
	{
		hWndle = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_POPUP,
			( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	}
	else
	{
		hWndle = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_POPUP,
			( ScreenWidth  - WINDOW_WIDTH ) / 2, ( ScreenHeight - WINDOW_HEIGHT ) / 2, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	}

	if (!hWndle)
	{
		return FALSE;
	}

	g_cGame.m_MainWindowHwnd = hWndle;

	ShowWindow(hWndle, nCmdShow);
	UpdateWindow(hWndle);

	return TRUE;
}

//
// �Լ�: WndProc(HWND, unsigned, WORD, LONG)
//
// ����: �� â�� �޽����� ó���մϴ�.
//
// WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
// WM_PAINT	- �� â�� �׸��ϴ�.
// WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� �����츦 �����Ѵ�.
BOOL MainWindowCreate(HINSTANCE hInstance)
{
	strcpy(szWindowClass ,"mainwindow");
	strcpy(szTitle ,"����");

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, SW_NORMAL)) 
	{
		return FALSE;
	}
	return TRUE;
}