// DirectMovie.h: interface for the CDirectMovie class.
//
//////////////////////////////////////////////////////////////////////

// 이 클레스로 동영상을 재생할 수 있게 해준다.
// 동영상을 재생하려면 일단 DDApplication 에 있는 g_Movie 포인터 객채를 new 로 생성해주고
// g_Movie->InitAll(hInstance, g_hWnd); // 초기화
// g_Movie.PlayMovieInWindow(szFilename); // 재생할 파일 이름
// g_Movie.ReleaseAll(); // 종료.
// delete g_Movie // 메모리 해제.

#if !defined(AFX_DIRECTMOVIE_H__393BE4BC_9A58_4456_B3A0_4CC51F87DC9F__INCLUDED_)
#define AFX_DIRECTMOVIE_H__393BE4BC_9A58_4456_B3A0_4CC51F87DC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"quartz.lib")
#pragma comment(lib,"strmiids.lib")
#include <mmsystem.h>
#include <dshow.h>
#include <stdio.h>
#include <atlbase.h>


//quartz.lib msvcrtd.lib msacm32.lib olepro32.lib strmiids.lib 

#define WM_GRAPHNOTIFY  WM_USER+0x1300

#define MOVE_RELEASE(x) { if (x) x->Release(); x = NULL; }

enum PLAYSTATE {Stopped, Paused, Running, Init};

class __declspec(dllexport) CDirectMovie  
{
	HWND				m_hWnd;
	LONG				m_iWidth;
	LONG				m_iHeight;

	// DirectShow interfaces
	IGraphBuilder*		m_pGB;
	IMediaControl*		m_pMC;
	IMediaEventEx*		m_pME;
	IVideoWindow*		m_pVW;
	IBasicAudio*		m_pBA;
	IBasicVideo*		m_pBV;
	IMediaSeeking*		m_pMS;
	IMediaPosition*		m_pMP;

	PLAYSTATE			m_psCurrent;
	BOOL				m_bPlayMovie;

public:
	VOID	InitAll(HINSTANCE hInst, HWND hParent);
	VOID	ReleaseAll();
	
	void	PauseClip(void);
	void	StopClip(void);
	void	CloseClip(void);
	
	VOID	ShowVideoWindow();
	VOID	HideVideoWindow();

	BOOL	MovieRun();

	HRESULT FullScreen(void);

	HRESULT PlayMovieInWindow(LPTSTR szFile);
	HRESULT InitVideoWindow(int nMultiplier, int nDivider);
	HRESULT HandleGraphEvent(void);

	// 동영상 출력에 대한 위치 정보
	int					i_PosX;
	int					i_PosY;

	CDirectMovie();
	~CDirectMovie();

};

#endif // !defined(AFX_DIRECTMOVIE_H__393BE4BC_9A58_4456_B3A0_4CC51F87DC9F__INCLUDED_)
