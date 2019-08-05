// Game Main.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
// 제작 : 최인균
// Game Main 은 Game dll 외에 다른 4개 Project 의 DLL 을 참조로 하여
// 윈도우를 초기화 시키고 Game dll 을 이용하여 게임을 진행 시키는 역활을
// 하게 됩니다.
// 다른 게임을 만들어도 이 Base는 변하지 않을겁니다.

// g_cApplication -> Engine_Library(Application) Singleton
// g_cGame -> Game Singleton
// g_cStateManager -> Engine_Utility(StateManager) Singleton
// g_cMainEngine -> Engine_Main(Engine_Main) Singleton

//○느ㅎ├己ㄷ　　フ│부ス으 ○│□├∧Hフ│フ│ 
//┴己　　┬┬·····　　 └ ┴└ 己　└　○ ········ 己 
//....................??...........................?? 
//.................?......?.....................?▒▒? 
//..............?...??.?...................?.??▒? 
//...........?...???..?.................?..???▒? 
//.........?.....???...???????...???.▒? 
//.......?.....................................................▒▒? 
//.....?.........................................................▒▒? 
//...???????..............................??????? 
//.?.............................................................▒▒▒? 
//?......??................................................??▒▒? 
//?......??................................................??▒▒? 
//?..................................................................▒▒? 
//?..........???....?.........?.........?....???...▒▒? 
//?.......?????....???...???....?????▒▒? 
//...?......???....................................???▒▒? 
//......???.......................................▒▒▒???  

//...............???????????????? 
//........................?????????? 
//.....................??............▒▒???? 
//..................?▒?...............▒▒???? 
//...............?▒??...............▒▒????? 
//............?...?...?...............▒▒??...?▒? 
//.........?...?......?...............▒▒▒?......?▒? 
//......?...?.........?...............▒▒▒?.........?...? 
//.........?............?............▒▒▒▒?............?		

#include "stdafx.h"
#include "Game Main.h"

#define MAX_LOADSTRING 128
// 전역 변수입니다.
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

FrameSkip m_FS;

bool   b_ExplorerCheck;

// 게임이 가장 먼저 시작되는 부분이다.
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

	// 처음 프로그램을 실행시켰을때 explorer 창이 있는 형태로 실행시킬껀지 아닐껀지;
	b_ExplorerCheck = false;
	g_cGame.m_WebBrowser.m_bNowWebBrowser = b_ExplorerCheck;
	/////////// CURSOR
	// 커서를 생성한다.
	g_cGame.m_hCursor = LoadCursorFromFile(".\\Data\\Image\\game.ani");  
	g_cApplication.SetMouseCursor(g_cGame.m_hCursor);

	// exe 아이콘 등을 등록한다.
	HICON m_hIcon;
	m_hIcon = LoadIcon(_hInstance,"Game_Main.ico");
	SendMessage(hWndle, WM_SETICON, TRUE, (LPARAM)m_hIcon);
	SendMessage(hWndle, WM_SETICON, FALSE, (LPARAM)m_hIcon);

	// 메인 윈도우를 생성한다.
	if(!MainWindowCreate(_hInstance)) return false;

	// 윈도우를 생성하기위해 정보를 셋팅해준다.
	g_cApplication.WIndowCreate("게임","게임",WndProc,WINDOW_WIDTH, WINDOW_HEIGHT, true, false, false
								//ws_popup 를 넣으므로써 타이블바가 없어진다.
						      ,  WS_VISIBLE | WS_POPUP);

	// 초기화를 하고 윈도우를 생성한다
	g_cApplication.Run();

	SetParent(g_cApplication.gethWnd(), hWndle);

	g_cGame.InitMsg();

	MSG Msg;
	// Enter the message pump
	ZeroMemory( &Msg, sizeof(MSG) );

	m_FS.SetFramePerSec(40); // 60 fps 고정. 기본적으로.

	// 메세지 Loop 로 들어간다. 이게 DLL 이나 LIB 안에 있으면 여기서 처리할 수 있는 방법이 없기때문에,
	// 여기로 옮겨서 처리한다.
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
			//   프레임율 비의존적인 코드
			//      dt에 의해 진행되기때문에 가능한한 빨리도는 코드들.
			//      시스템이 좋을수록 더 자주 호출된다.
			//******************************************************************
			float dt = DXUtil_Timer( TIMER_GETELAPSEDTIME ) ; // 프레임을 처리하기 위해서 dxutil 의 타이머를 사용한다.

			// Do any processing other than rendering.
			if( m_FS.Update(dt) )
			{
				//******************************************************************
				//   프레임율 의존적인 코드
				//      이부분은 frame skip에 의해 고정 프레임율을 갖고 동작한다.
				//******************************************************************
				if( FAILED( Process() ) ) break;
				g_cApplication.ProcessUpdateStats();
			}	

			if( ! m_FS.IsFrameSkip() ) 
			{ 
				//****************************************************************** 
				//   경우에 따라 호출되지 않아도 되는 코드 
				//      이부분은 frame skip에 의해 skip될수 있다. 
				//      렌더링따위의 시간을 많이 잡아먹는 코드를 넣는다. 
				//      역시 시스템이 좋을수록 더 자주 호출된다. 
				//****************************************************************** 
				if( !g_cMainEngine.IsPaused() && SUCCEEDED( g_cMainEngine.RestoreMode() ) )
				{
					// Do per-frame processing, break on FALSE return value
					if( FAILED( Render() ) ) break;

					// 프레임 업데이트
					g_cApplication.RenderUpdateStats();
				}
			} 

			// fps 에 따른 프레임 조절.
			if(g_cApplication.render_get_fps() >= 75)
				m_FS.SetFramePerSec(70); // 70 fps 고정.
			else if(g_cApplication.render_get_fps() >= 65)
				m_FS.SetFramePerSec(60); // 70 fps 고정.
			else if(g_cApplication.render_get_fps() >= 55)
				m_FS.SetFramePerSec(50); // 60 fps 고정.
			else
				m_FS.SetFramePerSec(40); // 최하 40프레임은 보장된다.

		}
	}

	// 위에 루프에서 빠져나오게 되면 종료시킨다.
	g_cApplication.OnApplicationReleased();

	// uninitialize COM
	CoUninitialize();

	// Unregister the window class
	UnregisterClass( g_cApplication.getclassname() , g_cApplication.gethInstance() );
}

// 윈도우 프로시져. 메시지를 처리한다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 여기엔 게임 전체에 쓰일만한 메세지들을 넣는다.

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		// 이게 없으면 게임이 그려지지 않는다.
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

// 게임 에 대한 처리는 대부분 Game.cpp 에 있는 Progcess, Render 함수에서 한다.
UINT Process()
{
	// Main Engine 으로 보내면 StateManager 로 전송되기 때문에 그쪽을 실제 게임에서 처리해주면 된다.
	return g_cMainEngine.Process();
}

UINT Render()
{
	// Main Engine 으로 보내면 StateManager 로 전송되기 때문에 그쪽을 실제 게임에서 처리해주면 된다.
	return g_cMainEngine.Render();
}

//
// 함수: MyRegisterClass()
//
// 목적: 창 클래스를 등록합니다.
//
// 설명:
//
// Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
// 해당 코드가 Win32 시스템과 호환되도록
// 하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
// 해당 응용 프로그램에 연결된
// '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
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
// 함수: InitInstance(HANDLE, int)
//
// 목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
// 설명:
//
// 이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
// 주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	//explorer 창이 있냐 없냐에 따라 뜨는 위치나 크기가 다르다.
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
// 함수: WndProc(HWND, unsigned, WORD, LONG)
//
// 목적: 주 창의 메시지를 처리합니다.
//
// WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
// WM_PAINT	- 주 창을 그립니다.
// WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
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
		// TODO: 여기에 그리기 코드를 추가합니다.
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

// 메인 윈도우를 생성한다.
BOOL MainWindowCreate(HINSTANCE hInstance)
{
	strcpy(szWindowClass ,"mainwindow");
	strcpy(szTitle ,"게임");

	// 전역 문자열을 초기화합니다.
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, SW_NORMAL)) 
	{
		return FALSE;
	}
	return TRUE;
}