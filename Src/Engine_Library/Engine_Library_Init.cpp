
// ���� : ���α�
// ���۳��� : 2005�� 3�� 15��
// ���� : Window �����ڿ� �Ҹ��ڰ� ����. ���ν����� �Ѱ��ִ� �Լ���..

#include "StdAfx.h"
#include "D3DApplication.h"

// static message handler which passes messages to the application class.
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return g_cApplication.MsgProc( hWnd, uMsg, wParam, lParam );
}

//---------------------------------------------------------------------------//

cApplication::cApplication()
: 	
// window class
m_bRegisteredWndClass(false),
m_bWindowCreateSetting(false),
m_fProcessFPS(0.0f),
m_fArageProcessFPS(0.0f),
m_fRenderFPS(0.0f),
m_fArageRenderFPS(0.0f),
m_iCmdShow(100),
m_lpfnWndProc(NULL)
{
	//	cApplication(*cAppMainBlock) = this;

	// Get the instance handle
	m_hInstance = GetModuleHandle( NULL );

	// set default WNDCLASSEX structure
	m_wcex.cbSize			= sizeof(WNDCLASSEX);
	m_wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	m_wcex.lpfnWndProc		= WndProc;
	m_wcex.cbClsExtra		= 0;
	m_wcex.cbWndExtra		= 0;
	m_wcex.hInstance		= m_hInstance;
	m_wcex.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	m_wcex.hCursor			= LoadCursor( NULL, IDC_ARROW );
	m_wcex.hbrBackground	= NULL;
	m_wcex.lpszMenuName		= NULL;
	m_wcex.lpszClassName	= "ClassName";
	m_wcex.hIconSm			= LoadIcon( NULL, IDI_APPLICATION );

	// �׷��Ƚ� ����̽� ����
	m_pGraphics	= new cGraphics( (cD3DDevice*) this );
}

cApplication::~cApplication()
{
	SAFE_DELETE( m_pGraphics );

	// unregister window class
	if (m_bRegisteredWndClass)
	{
		UnregisterClass(m_Class, m_hInstance);
	}
}