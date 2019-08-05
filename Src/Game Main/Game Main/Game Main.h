

#ifndef __GAME_MAIN_H__
#define __GAME_MAIN_H__

#pragma once
/*
 제작 : 2005년 3월 10일
 제작자 : 최인균
 
 Game Engine -> Win 32 API
 Engine Main -> WIn 32 API DLL
 Engine Library -> Win 32 API LIB
 */

#include "resource.h"
#include "..\..\Game\Game.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// 프로시져
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

UINT Process();
UINT Render();

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc2(HWND, UINT, WPARAM, LPARAM);

BOOL MainWindowCreate(HINSTANCE hInstance);

BOOL ExplorerCreate();

static HWND hWndle;

// 프레임 스킵과 관련된 클레스.
class FrameSkip 
{ 
public: 
	void Clear() 
	{ 
		SetFramePerSec( 60.0f ); 
		m_Timer = 0.0f; 
	} 
	void SetFramePerSec( float fps ) 
	{ 
		m_SecPerFrame = 1.0f/fps; 
	} 

	// 원하는 프레임보다 너무 빠르면, 
	// false를 반환해서 코드를 동작시키지 않도록 한다. 
	// dt는 '초'단위 (밀리초 아님!!!) 
	bool Update( float dt ) 
	{ 
		m_Timer += dt;       
		if( m_Timer<0 ) return false;       
		// 한프레임에 해당하는 시간을 뺀다. 
		m_Timer -= m_SecPerFrame; 
		return true; 
	} 

	// Update후에 호출해서 frame skip을 수행해야 하는지 검사한다. 
	// frame skip이 일어나야 한다면 true를 반환한다. 
	bool IsFrameSkip() 
	{ 
		return m_Timer >= 0; 
	} 

	// 멤버변수와 생성/소멸자. 
	FrameSkip(){ Clear(); } 
	virtual ~FrameSkip(){} 
protected: 
	float m_Timer; 
	float m_SecPerFrame; 
}; 
/////////////////////////////////////

#endif