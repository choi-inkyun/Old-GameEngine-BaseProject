// Engine Main.cpp : DLL ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Engine Main.h"

// �̱����� �ٸ� ������Ʈ�� �˸���.
_CEngineMain		cEngineMainBlock;
unsigned long		_init_CEngine::ms_dwLong;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
// ������
CEngineMainApp::CEngineMainApp()
: m_Windowed(true)	// Ǯ��ũ�� ���� �ƴϳ�~
{
}

// �Ҹ���
CEngineMainApp::~CEngineMainApp()
{
}

/*
// ������ ���ν���
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
* �Ķ��Ÿ ���� ���� �Լ�
*---------------------------------------------------------------------------*/
void CEngineMainApp::SetWindowParameter( DWORD x, DWORD y, DWORD width, DWORD height, DWORD style )
{
	// ȭ�� ũ�Ⱑ ���ٸ� �߾����� �̵��� �����Ѵ�.
	if( g_cApplication.m_Width == width && g_cApplication.m_Height == height ) return;

	g_cApplication.m_XPos		= x;
	g_cApplication.m_YPos		= y;
	g_cApplication.m_Width	= width;
	g_cApplication.m_Height	= height;

	g_cApplication.m_Style		= style;

	MoveWindow( g_cApplication.gethWnd(), g_cApplication.m_XPos, g_cApplication.m_YPos, g_cApplication.m_Width, g_cApplication.m_Height, TRUE );
}

// DirectX �� ������. �� �ؾ��ϴ°���.
void CEngineMainApp::SetMode()
{
	if( FAILED( g_cApplication.SetMode( g_cApplication.gethWnd(), m_Windowed, TRUE, g_cApplication.m_Width, g_cApplication.m_Height ) ) )
	{
		g_cApplication.DisplayError( TRUE, "DirectX 9.0 �׷��� ���� ������ �߻��Ͽ����ϴ�." );
		return;
	}
}

// Ǯ��ũ������ ������ ����, ������ ��忡�� Ǯ��ũ�� ����
void CEngineMainApp::ToggleMode()
{
	if( FAILED( g_cApplication.ToggleMode() ) )
	{
		g_cApplication.DisplayError( TRUE, "DirectX 9.0 �׷��� ��ȯ ������ �߻��Ͽ����ϴ�." );
		return;
	}

	m_Windowed = !m_Windowed;
}

/*-----------------------------------------------------------------------------
* ȭ�� ������ ���� �Լ�
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

// Loop ���� �����Ű�� Process
UINT CEngineMainApp::Process()
{
	// process 
	// ���� dll �� proc �� �����ش�. Process �� �϶��
	g_cStateManager.Process( NULL, PURPOSE_PROCESS );

	return S_OK;
}

UINT CEngineMainApp::Render()
{
	// render a scene
	// ���� dll �� proc �� �����ش�. Render �� �϶��
	return g_cStateManager.Process( NULL, PURPOSE_RENDER ) ? S_OK : (-1);
}