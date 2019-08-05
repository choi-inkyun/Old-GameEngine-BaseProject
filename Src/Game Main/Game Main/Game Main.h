

#ifndef __GAME_MAIN_H__
#define __GAME_MAIN_H__

#pragma once
/*
 ���� : 2005�� 3�� 10��
 ������ : ���α�
 
 Game Engine -> Win 32 API
 Engine Main -> WIn 32 API DLL
 Engine Library -> Win 32 API LIB
 */

#include "resource.h"
#include "..\..\Game\Game.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

UINT Process();
UINT Render();

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc2(HWND, UINT, WPARAM, LPARAM);

BOOL MainWindowCreate(HINSTANCE hInstance);

BOOL ExplorerCreate();

static HWND hWndle;

// ������ ��ŵ�� ���õ� Ŭ����.
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

	// ���ϴ� �����Ӻ��� �ʹ� ������, 
	// false�� ��ȯ�ؼ� �ڵ带 ���۽�Ű�� �ʵ��� �Ѵ�. 
	// dt�� '��'���� (�и��� �ƴ�!!!) 
	bool Update( float dt ) 
	{ 
		m_Timer += dt;       
		if( m_Timer<0 ) return false;       
		// �������ӿ� �ش��ϴ� �ð��� ����. 
		m_Timer -= m_SecPerFrame; 
		return true; 
	} 

	// Update�Ŀ� ȣ���ؼ� frame skip�� �����ؾ� �ϴ��� �˻��Ѵ�. 
	// frame skip�� �Ͼ�� �Ѵٸ� true�� ��ȯ�Ѵ�. 
	bool IsFrameSkip() 
	{ 
		return m_Timer >= 0; 
	} 

	// ��������� ����/�Ҹ���. 
	FrameSkip(){ Clear(); } 
	virtual ~FrameSkip(){} 
protected: 
	float m_Timer; 
	float m_SecPerFrame; 
}; 
/////////////////////////////////////

#endif