
// 제작 : 최인균
// 제작날자 : 2005년 3월 15일
// 내용 : Window를 Create 하는데 관여하는 함수들을 이 cpp 로 따로 빼었다.

#include "StdAfx.h"
#include "D3DApplication.h"

void cApplication::WIndowCreate(LPCSTR c_classname, LPCSTR c_cation, int i_width, int i_height, bool b_center, bool b_max, bool b_min, UINT u_style)
{
	// set a default window class and caption
	m_Class = c_classname;
	m_Caption = c_cation;

	m_wcex.lpszClassName	= c_classname;

	m_Style = u_style;

	m_bMinimized			= b_max;
	m_bMaximized			= b_min;

	m_bWindowCreateSetting = true;
}

void cApplication::WIndowCreate()
{
	// set a default window class and caption
	//	wcscpy( m_Class, L"ClassName" );
	m_Class = "ClassName";
	m_Caption = "Application Caption";

	m_Style = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME |
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;

	m_bMinimized			= FALSE;
	m_bMaximized			= FALSE;

	m_bWindowCreateSetting = true;
}

//////////////////////////////////////////////////////////////////////////////////
HRESULT cApplication::WindowCreate(
								   HINSTANCE _hInstance,			// in)		application instance
								   HINSTANCE _hPrevInstance,		// in)		previous application instance
								   LPSTR _lpszCmdLine,				// in)		command line
								   WNDPROC _pfWndProc,				// in)		window procedure
								   unsigned long _dwIconResourceId,			// in)		icon resource id
								   unsigned long _dwCursorResourceId,		// in)		icon resource id
								   int _cmdShow)					// in)		window show command
								   // : initializes pleione
								   //////////////////////////////////////////////////////////////////////////////////
{
	m_wcex.lpfnWndProc		= _pfWndProc;
	// instance handle
	m_hInstance		= _hInstance;
	m_wcex.hInstance = _hInstance;

	// load icon
	if (_dwIconResourceId != 0)
		m_wcex.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(_dwIconResourceId));
	else
		m_wcex.hIcon = 0;

	m_iCmdShow = _cmdShow;

	return S_OK;
}

void cApplication::WIndowCreate(LPCSTR c_classname, LPCSTR c_cation, WNDPROC _pfWndProc, int i_width, int i_height, bool b_center, bool b_max, bool b_min, UINT u_style)
{
	// set a default window class and caption
	m_Class = c_classname;
	m_Caption = c_cation;

	m_wcex.lpszClassName	= c_classname;

	m_Style = u_style;

	m_bMinimized			= b_max;
	m_bMaximized			= b_min;

	m_bWindowCreateSetting = true;

	// proc 등록
//	m_wcex.lpfnWndProc		= _pfWndProc;
	m_lpfnWndProc = _pfWndProc; // 바로 등록하는게 아니라 일단 넣어놓고 application 의 proc 에서 리턴되는값으로 해준다.
}