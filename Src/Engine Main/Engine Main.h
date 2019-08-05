
// ���� : ���α�
// Engine_Library, Engine_Interface, Engine_Utility dll �� �����մϴ�.

#pragma once

#ifndef __ENGINE_MAIN_H__
#define __ENGINE_MAIN_H__

#include "stdafx.h"

// �̱��� ��ü ���� Ŭ���� ����
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

	// D3D Device �� ������
	UINT Cleanup3DEnvironment();
	UINT RestoreMode();
	BOOL IsPaused();

	UINT Process();
	UINT Render();

//	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	BOOL			m_Windowed;				// ������ ���� True �ƴϸ� false
public:

public:
	RENDER_STATE	m_RenderState;			// ����� ������ �Ҳ��� ������Ʈ

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

	// �̱����� �����ؾ���.
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