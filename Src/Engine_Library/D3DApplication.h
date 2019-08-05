/*
 * FILENAME:
 *			Application.h
 *
 * DESCRIPTION:
 *			CApplication Ŭ������ �������� Ŭ������ ����ϰ�, �������α׷� �����츦
 *			�����ϴ� �ܿ��� �������α׷� ������ �޽����� ó���ϰ� ���� Ŭ���� �Լ���
 *			ȣ���ϴ� �޽��� ������ ����.
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * HISTORY:
 *			2004.03.26 å�� ������ �״�� �����.
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

	// ������ Ŭ���� ���.
	WNDCLASSEX	m_wcex;

	// ������ ��������. ����ڰ� �Է��� �� �ִ�
	WNDPROC		m_lpfnWndProc;

	// fps �� ���� ������.
	FLOAT		m_fProcessFPS;
	FLOAT		m_fArageProcessFPS;
	FLOAT		m_fRenderFPS;
	FLOAT		m_fArageRenderFPS;

	// ������ ��ﶧ � �������
	int			m_iCmdShow;

	// ������ �־ üũ�ϴ� �κе�
	bool		m_bWindowCreateSetting;

	// �ڱ� �ڽ��� �̱����� ���� �ʹٸ� ���⼭ �����Ѵ�.
	static cApplication*			__singleton();

	cGraphics*		m_pGraphics;			// �׷��Ƚ�

public:
	cApplication();
	~cApplication();

	// �����츦 ���� �� �� �ִ� �Լ��̴�.0
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

	// ������ �ȴ�.
	BOOL		Run();
	// State �� ������Ʈ �ȴ�.
	void		ProcessUpdateStats();
	void		RenderUpdateStats();
	// ������ ������ messagebox �� ��� �� �ִ�.
	BOOL		DisplayError( BOOL fatal, LPCSTR Text, ... );

	// ���콺�� �����ֳ�?
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

	// fps �� ��ȯ�Ѵ�.
	FLOAT process_get_fps() { return m_fProcessFPS; }
	FLOAT process_arage_get_fps() { return m_fArageProcessFPS; }
	FLOAT render_get_fps() { return m_fRenderFPS; }
	FLOAT render_arage_get_fps() { return m_fArageRenderFPS; }

	CDirectMovie *g_Movie;	// ������ �÷��� ��ü.
};

////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CApplicat[sizeof(cApplication)];
extern __declspec(dllimport) _CApplicat		cAppMainBlock;
#define g_cApplication			(*(cApplication*) cAppMainBlock)

// �̱����� �����ؾ���.
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

