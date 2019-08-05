
// 제작 : 최인균
// Engine_Library, Engine_Interface, Engine_Utility dll 를 참조합니다.

#pragma once

#ifndef __ENGINE_MAIN_H__
#define __ENGINE_MAIN_H__

#include "stdafx.h"

// 싱글톤 객체 생성 클레스 선언
class _init_CEngine;

class __declspec(dllexport) CEngineMainApp
{
friend _init_CEngine;
public:
	CEngineMainApp();
	~CEngineMainApp();

	cApplication *cApp;

	void SetWindowParameter( DWORD x, DWORD y, DWORD width, DWORD height, DWORD style );
	void SetMode();
	void ToggleMode();

	// D3D Device 와 연결함
	UINT Cleanup3DEnvironment();
	UINT RestoreMode();
	BOOL IsPaused();

	UINT Process();
	UINT Render();

//	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	BOOL			m_Windowed;				// 윈도우 모드면 True 아니면 false
public:

public:
	RENDER_STATE	m_RenderState;			// 어떤것을 랜더링 할껀가 스테이트

protected:
	static CEngineMainApp*			__singleton();

public:
	
	void SetWindowMode(bool b_set) { m_Windowed = b_set; }
	bool GetWindowMode() { return m_Windowed; }
};

////////////////////////// Singleton ///////////////////////////////////////

	typedef unsigned char	_CEngineMain[sizeof(CEngineMainApp)];
	extern __declspec(dllimport) _CEngineMain		cEngineMainBlock;
	#define g_cMainEngine			(*(CEngineMainApp*) cEngineMainBlock)

	// 싱글톤을 생성해야함.
	#ifdef MAIN_ENGINE_EXPORTS

	// initializer dummy
	class _init_CEngine {
	protected:
		static unsigned long			ms_dwLong;

	public:
		_init_CEngine()
		{
			if (ms_dwLong == 0)
			{
				new (cEngineMainBlock) CEngineMainApp;
			}

			ms_dwLong++;
		}
		~_init_CEngine()
		{
			if (ms_dwLong == 1)
			{
				((CEngineMainApp*) cEngineMainBlock)->~CEngineMainApp();
			}

			ms_dwLong--;
		}
	};

	// declare CEngineMainApp singleton instance
	static _init_CEngine			_init_cEngine;

	#endif
/////////////////////////////////////////////////////////////////////////////////////////

#endif