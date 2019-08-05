/*
 * FILENAME:
 *			Application.h
 *
 * DESCRIPTION:
 *			CApplication 클래스는 윈도우즈 클래스를 등록하고, 응용프로그램 윈도우를
 *			생성하는 외에도 응용프로그램 윈도우 메시지를 처리하고 내부 클래스 함수를
 *			호출하는 메시지 펌프에 들어간다.
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * HISTORY:
 *			2004.03.26 책의 내용을 그대로 사용함.
 */

#ifndef __ENGINE_D3DAPPLICATION_H__
#define __ENGINE_D3DAPPLICATION_H__

#include "D3DDevice.h"
#include "D3DGraphics.h"
#include "DirectMovie.h"

class _init_CEngineLibrary;

class __declspec(dllexport) cApplication : public cD3DDevice
{
friend _init_CEngineLibrary;
private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

protected:
	LPCSTR		m_Class;
	LPCSTR		m_Caption;

	BOOL		m_bMinimized;
	BOOL		m_bMaximized;

	bool		m_bRegisteredWndClass;

	// 윈도우 클레스 등록.
	WNDCLASSEX	m_wcex;

	// 윈도우 프르시져. 사용자가 입력할 수 있다
	WNDPROC		m_lpfnWndProc;

	// fps 가 몇이 나오나.
	FLOAT		m_fProcessFPS;
	FLOAT		m_fArageProcessFPS;
	FLOAT		m_fRenderFPS;
	FLOAT		m_fArageRenderFPS;

	// 윈도우 띄울때 어떤 모양일지
	int			m_iCmdShow;

	// 엔진에 있어서 체크하는 부분들
	bool		m_bWindowCreateSetting;

	// 자기 자신이 싱글톤을 쓰고 싶다면 여기서 참조한다.
	static cApplication*			__singleton();

	cGraphics*		m_pGraphics;			// 그래픽스

public:
	cApplication();
	~cApplication();

	// 윈도우를 생성 할 수 있는 함수이다.0
	void WIndowCreate(LPCSTR c_classname, LPCSTR c_cation, int i_width, int i_height, bool b_center, bool b_max, bool b_min, 
			  		  UINT u_style = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE);
	HRESULT	WindowCreate(
		HINSTANCE _hInstance, 
		HINSTANCE _hPrevInstance,
		LPSTR _lpszCmdLine,
		WNDPROC _pfWndProc,
		unsigned long _dwIconResourceId,
		unsigned long _dwCursorResourceId,
		int _cmdShow);
	void WIndowCreate();
	void WIndowCreate(LPCSTR c_classname, LPCSTR c_cation, WNDPROC _pfWndProc, int i_width, int i_height, bool b_center, bool b_max, bool b_min,
					  UINT u_style = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE);

	// 실행이 된다.
	BOOL		Run();
	// State 가 업데이트 된다.
	void		ProcessUpdateStats();
	void		RenderUpdateStats();
	// 에러가 났을때 messagebox 를 띄울 수 있다.
	BOOL		DisplayError( BOOL fatal, LPCSTR Text, ... );

	// 마우스를 보여주나?
	BOOL		ShowMouse( BOOL Show = TRUE );
	BOOL		SetMouseCursor( HCURSOR hCursor);
	BOOL		SetMouseCursor( char* filename);

	HWND		gethWnd()      { return m_hWnd;      }
	HINSTANCE	gethInstance() { return m_hInstance; }
	LPCSTR		getclassname() { return m_Class;	 }

	cGraphics*	getgraphics() { return m_pGraphics; }

	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Overrideable functions
	UINT OnApplicationInitialized();
	UINT OnDeviceCreated();
	UINT OnDeviceReset();
	UINT Render()					{ return S_OK; }
	UINT Process()					{ return S_OK; }
	UINT OnDeviceLost();
	UINT OnDeviceDestroyed();
	UINT OnApplicationReleased();

	DWORD		m_XPos;
	DWORD		m_YPos;
	DWORD		m_Width;
	DWORD		m_Height;
	DWORD		m_Style;

	// fps 를 반환한다.
	FLOAT process_get_fps() { return m_fProcessFPS; }
	FLOAT process_arage_get_fps() { return m_fArageProcessFPS; }
	FLOAT render_get_fps() { return m_fRenderFPS; }
	FLOAT render_arage_get_fps() { return m_fArageRenderFPS; }

	CDirectMovie *g_Movie;	// 동영상 플레이 객체.
};

////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CApplicat[sizeof(cApplication)];
extern __declspec(dllimport) _CApplicat		cAppMainBlock;
#define g_cApplication			(*(cApplication*) cAppMainBlock)

// 싱글톤을 생성해야함.
#ifdef ENGINE_LIBRARY_EXPORTS

// initializer dummy
class _init_CEngineLibrary {
protected:
	static unsigned long			ms_dwLong;

public:
	_init_CEngineLibrary()
	{
		if (ms_dwLong == 0)
		{
			new (cAppMainBlock) cApplication;
		}

		ms_dwLong++;
	}
	~_init_CEngineLibrary()
	{
		if (ms_dwLong == 1)
		{
			((cApplication*) cAppMainBlock)->~cApplication();
		}

		ms_dwLong--;
	}
};

// declare CEngineMainApp singleton instance
static _init_CEngineLibrary			_init_cEngineLibrary;

#endif


#endif

