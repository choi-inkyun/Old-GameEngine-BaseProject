// Engine Main.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Engine Main.h"

// 싱글톤을 다른 프로젝트에 알린다.
_CEngineMain		cEngineMainBlock;
unsigned long		_init_CEngine::ms_dwLong;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
// 생성자
CEngineMainApp::CEngineMainApp()
: m_Windowed(true)	// 풀스크린 모드냐 아니냥~
{
}

// 소멸자
CEngineMainApp::~CEngineMainApp()
{
}

/*
// 윈도우 프로시져
LRESULT CEngineMainApp::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef _UNUSED_CODE_
	switch( uMsg )
	{
	case WM_KEYUP:
		switch( wParam )
		{
		case 'T':
			ToggleMode();
			return 1;
		case 'Q':
			PostQuitMessage( 0 );
			return 1;
		}
		break;
	}
#endif

	// Let the default handler take care of it!
	return g_cApplication.MsgProc( hWnd, uMsg, wParam, lParam );
}
*/

//////////////////////////////////////////////////////////////////////////////////
CEngineMainApp* CEngineMainApp::__singleton()
// : script callback
//////////////////////////////////////////////////////////////////////////////////
{
	return &g_cMainEngine;
}

/*-----------------------------------------------------------------------------
* 파라메타 설정 관련 함수
*---------------------------------------------------------------------------*/
void CEngineMainApp::SetWindowParameter( DWORD x, DWORD y, DWORD width, DWORD height, DWORD style )
{
	// 화면 크기가 같다면 중앙으로 이동은 생략한다.
	if( g_cApplication.m_Width == width && g_cApplication.m_Height == height ) return;

	g_cApplication.m_XPos		= x;
	g_cApplication.m_YPos		= y;
	g_cApplication.m_Width	= width;
	g_cApplication.m_Height	= height;

	g_cApplication.m_Style		= style;

	MoveWindow( g_cApplication.gethWnd(), g_cApplication.m_XPos, g_cApplication.m_YPos, g_cApplication.m_Width, g_cApplication.m_Height, TRUE );
}

// DirectX 를 셋팅함. 꼭 해야하는것임.
void CEngineMainApp::SetMode()
{
	if( FAILED( g_cApplication.SetMode( g_cApplication.gethWnd(), m_Windowed, TRUE, g_cApplication.m_Width, g_cApplication.m_Height ) ) )
	{
		g_cApplication.DisplayError( TRUE, "DirectX 9.0 그래픽 설정 오류가 발생하였습니다." );
		return;
	}
}

// 풀스크린에서 윈도우 모드로, 윈도우 모드에서 풀스크린 모드로
void CEngineMainApp::ToggleMode()
{
	if( FAILED( g_cApplication.ToggleMode() ) )
	{
		g_cApplication.DisplayError( TRUE, "DirectX 9.0 그래픽 전환 오류가 발생하였습니다." );
		return;
	}

	m_Windowed = !m_Windowed;
}

/*-----------------------------------------------------------------------------
* 화면 렌더링 관련 함수
*---------------------------------------------------------------------------*/

UINT CEngineMainApp::Cleanup3DEnvironment()
{
	return g_cApplication.Cleanup3DEnvironment();
}

UINT CEngineMainApp::RestoreMode()
{
	return g_cApplication.RestoreMode();
}
BOOL CEngineMainApp::IsPaused()
{
	return g_cApplication.IsPaused();
}

// Loop 에서 실행시키는 Process
UINT CEngineMainApp::Process()
{
	// process 
	// 게임 dll 로 proc 를 보내준다. Process 를 하라고
	g_cStateManager.Process( NULL, PURPOSE_PROCESS );

	return S_OK;
}

UINT CEngineMainApp::Render()
{
	// render a scene
	// 게임 dll 로 proc 를 보내준다. Render 을 하라고
	return g_cStateManager.Process( NULL, PURPOSE_RENDER ) ? S_OK : (-1);
}