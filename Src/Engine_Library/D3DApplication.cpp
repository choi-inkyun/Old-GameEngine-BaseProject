/*-----------------------------------------------------------------------------
 * FILENAME:
 *			Application.cpp
 *
 * DESCRIPTION:
 *			cApplication 클래스는 윈도우즈 클래스를 등록하고, 응용프로그램 윈도우를
 *			생성하는 외에도 응용프로그램 윈도우 메시지를 처리하고 내부 클래스 함수를
 *			호출하는 메시지 펌프에 들어간다.
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * 제작 변형 : 최인균(dingpong). 3월 10일 Start
 *---------------------------------------------------------------------------*/

#include "StdAfx.h"
#include "D3DApplication.h"

//-----------------------------------------------------------------------------
// Global access to the app ( needed for the global WndProc() )
//-----------------------------------------------------------------------------
_CApplicat			cAppMainBlock;
unsigned long		_init_CEngineLibrary::ms_dwLong;

//////////////////////////////////////////////////////////////////////////////////
cApplication* cApplication::__singleton()
// : script callback
//////////////////////////////////////////////////////////////////////////////////
{
	return &g_cApplication;
}
BOOL cApplication::Run()
{
	// 윈도우를 생성하기 위한 정보가 셋팅이 안되어 있다면 셋팅해준다.
	if(m_bWindowCreateSetting == false)
		WIndowCreate();

	// Register window class
	if( !RegisterClassEx( &m_wcex ) ) return FALSE;

	m_bRegisteredWndClass = true;

	// Create the main window
	m_hWnd = CreateWindow(	m_Class, m_Caption,
							m_Style,
							m_XPos, m_YPos,
							m_Width, m_Height,
							NULL, NULL, m_hInstance, NULL );

	if( !m_hWnd ) return FALSE;

	SetFocus( m_hWnd );
	if(m_iCmdShow == 100)
		ShowWindow( m_hWnd, SW_NORMAL );
	else
		ShowWindow( m_hWnd, m_iCmdShow );
	UpdateWindow( m_hWnd );

	// initialize COM
	CoInitialize( NULL );

	if( SUCCEEDED( OnApplicationInitialized() ) )
	{
		return false;
	} // end of if( SUCCEDDED( OnApplicationInitialized() ) )

//	OnApplicationReleased();

	// uninitialize COM
//	CoUninitialize();

	// Unregister the window class
//	UnregisterClass( m_Class, m_hInstance );

	DisplayError(TRUE, "윈도우 생성 에러입니다.");

	return TRUE;
}

// 윈도우를 생성하고 초기화 해주는 부분.
UINT cApplication::OnApplicationInitialized()
{
	HRESULT hr;

	if( FAILED( hr = Initialize() ) )
	{
		DisplayError( TRUE, "DirectX 9.0 초기화 오류가 발생하였습니다." );
		return S_OK;
	}

	return S_OK;
}


//---------------------------------------------------------------------------//

BOOL cApplication::ShowMouse( BOOL Show )
{
	ShowCursor( Show );
	return TRUE;
}

// 임의의 마우스 커서 설정
BOOL cApplication::SetMouseCursor( HCURSOR hCursor)
{
	SetCursor(hCursor);
	m_wcex.hCursor			= hCursor;
	return TRUE;
}

// 임의의 마우스 커서 설정
BOOL cApplication::SetMouseCursor( char* filename)
{
	HCURSOR hCursor;
	hCursor = LoadCursorFromFile(filename);  
	SetCursor(hCursor);
	ShowMouse(false);
	return TRUE;
}

BOOL cApplication::DisplayError( BOOL fatal, LPCSTR text, ... )
{
	LPCSTR CaptionText;
	LPCSTR ErrorText;
//	va_list valist;

	// build the message box caption based on fatal flag
	if( fatal == FALSE ) CaptionText  = "Error";
	else                 CaptionText = "Fatal Error";

	ErrorText = text;
	// build variable text buffer
//	va_start( valist, text );
//	vswprintf( ErrorText, text, valist );
//	va_end( valist );

	// Display the message box
	MessageBox( NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION );

	// Post a quit message if error is fatal.
	if( fatal == TRUE ) PostQuitMessage( 0 );

	return TRUE;
}

//---------------------------------------------------------------------------//

// The message handling procedure.
LRESULT cApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	sMsgParam msgParam = { hWnd, uMsg, wParam, lParam, FALSE };
	// 실제 게임에서 처리해야할 것을 메세지로 보내준다.
	g_cStateManager.Process( &msgParam, PURPOSE_MSGPROC );

	// 여기에서는 윈도우에 전형화된 그런 정보들만 넣는다. 변형할 필요 없는것들.
	switch( uMsg )
	{
		case WM_PAINT:
			if( m_pd3dDevice ) Render();
			break;

#ifdef _UNUSED_CODE_
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 100;
			break;
#endif

		case WM_ENTERSIZEMOVE:
			// halt frame movement while the app is sizing or moving
			Pause( TRUE );
			break;

		case WM_SIZE:
			// pick up possible changes to window style due to maximize, etc.
			if( m_Windowed && !m_hWnd ) m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );

			if( wParam == SIZE_MINIMIZED )
			{
				Pause( TRUE );
				m_bMinimized = TRUE;
				m_bMaximized = FALSE;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				if( m_bMinimized ) Pause( FALSE );
				m_bMinimized = FALSE;
				m_bMaximized = TRUE;
				HandlePossibleSizeChange();
			}
			else if( wParam == SIZE_RESTORED )
			{
				if( m_bMaximized )
				{
					m_bMaximized = FALSE;
					HandlePossibleSizeChange();
				}
				else if( m_bMinimized )
				{
					Pause( FALSE );
					m_bMinimized = FALSE;
					HandlePossibleSizeChange();
				}
				else
				{
					// the window size is changing by the user dragging the window edges.
					// we wait until the user stops dragging, and a WM_EXITSIZEMOVE message comes.
				}
			}
			break;

		case WM_EXITSIZEMOVE:
			Pause( FALSE );
			if( m_pd3dDevice ) HandlePossibleSizeChange();
			break;

		case WM_SYSCOMMAND:
			// Prevent moving/sizing and power loss in fullscreen mode
			switch( wParam )
			{
				case SC_MOVE:
				case SC_SIZE:
				case SC_MAXIMIZE:
				case SC_KEYMENU:
				case SC_MONITORPOWER:
					if( !m_Windowed ) return 1;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}

	if(m_lpfnWndProc == NULL)
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	else
		return m_lpfnWndProc( hWnd, uMsg, wParam, lParam );

	return FALSE;
}

//---------------------------------------------------------------------------//

// 별 기능은 없고 프레임을 체크하는 함수들
// Process 와 Render 를 나눠놓는다.
void cApplication::ProcessUpdateStats()
{
	static DWORD dwProcessLastTime = 0;
	static DWORD dwProcessFrames   = 0;
	static float dwProcessFrameCount = 0;
	static float dwProcessFrameSum = 0;

	DWORD dwTime = timeGetTime();
	dwProcessFrames++;

	if( dwTime - dwProcessLastTime >= 1000 )
	{
		// 일반 FPS 를 구한다.
		m_fProcessFPS		= (FLOAT)dwProcessFrames * 1000 / ( dwTime - dwProcessLastTime );
		dwProcessLastTime	= dwTime;
		dwProcessFrames	= 0;

		// 0 fps 가 아닐때만 count 를 더해준다.
		if(m_fProcessFPS > 1.0f && m_fProcessFPS < 1000.0f)
		{
			// 평균 FPS 를 구한다.
			dwProcessFrameSum+=m_fProcessFPS;
			dwProcessFrameCount++;
			m_fArageProcessFPS = (FLOAT)dwProcessFrameSum / dwProcessFrameCount;
		}
	}
}

void cApplication::RenderUpdateStats()
{
	static DWORD dwRenderLastTime = 0;
	static DWORD dwRenderFrames   = 0;
	static float dwRenderFrameCount = 0;
	static float dwRenderFrameSum = 0;

	DWORD dwTime = timeGetTime();
	dwRenderFrames++;

	if( dwTime - dwRenderLastTime >= 1000 )
	{
		// 일반 FPS 를 구한다.
		m_fRenderFPS		= (FLOAT)dwRenderFrames * 1000 / ( dwTime - dwRenderLastTime );
		dwRenderLastTime	= dwTime;
		dwRenderFrames	= 0;

		// 0 fps 가 아닐때만 count 를 더해준다.
		if(m_fRenderFPS > 1.0f && m_fRenderFPS < 1000.0f)
		{
			// 평균 FPS 를 구한다.
			dwRenderFrameSum+=m_fRenderFPS;
			dwRenderFrameCount++;
			m_fArageRenderFPS = (FLOAT)dwRenderFrameSum / dwRenderFrameCount;
		}
	}
}


//---------------------------------------------------------------------------//


// 이것은 밖으로 빼는게 더 유용성 있어 보이지만. 뺄수 있는 방법이 없어서 여기다 넣음
// 어차피 3개의 라이브러리들은 서로 공유가 되어야 할것 같기에..
// Device 가 생성될때 불려줌
UINT cApplication::OnDeviceCreated()
{
	g_cUIDialogManager.OnCreateDevice( gethWnd(), GetDeviceCOM() );

	return S_OK;
}

// Device 가 다시 리셋됨..
UINT cApplication::OnDeviceReset()
{
	g_cUIDialogManager.OnResetDevice( GetDeviceCOM() );

	return g_cStateManager.Process( NULL, PURPOSE_INITIALIZE ) ? S_OK : (-1);
}

//Device 를 잃어버려쓸떄..
UINT cApplication::OnDeviceLost()
{
	g_cUIDialogManager.OnLostDevice();

	return g_cStateManager.Process( NULL, PURPOSE_RELEASE ) ? S_OK : (-1);
}

// Device 를 지울떄.
UINT cApplication::OnDeviceDestroyed()
{
	g_cUIDialogManager.OnDestroyDevice();

	return S_OK;
}

// 어플리케이션을 종료함
UINT cApplication::OnApplicationReleased()
{
	g_cStateManager.PopAll();

	g_cApplication.Release();

	// 마우스를 보이도록 설정.
	ShowMouse();
	return S_OK;
}
