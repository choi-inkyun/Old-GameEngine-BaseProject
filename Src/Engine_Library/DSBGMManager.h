// DSBGMManager.h: interface for the CDSBGMManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSBGMMANAGER_H__F983D7DF_21BA_4F68_8CA3_53FDE53D3ADB__INCLUDED_)
#define AFX_DSBGMMANAGER_H__F983D7DF_21BA_4F68_8CA3_53FDE53D3ADB__INCLUDED_

#include "DSBGM.h"	// Added by ClassView
#include <dshow.h>		// nanapark 2002.2.21

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class __declspec(dllexport) CDSBGMManager  
{
public:
	char m_LoadListFilename[256];
	bool StopSound(char *soundname);
	bool StopSound(int index);
	bool OpenSound(char *soundname);
	bool OpenSound(int index);
	bool PlaySound(char *soundname);
	bool PlaySound(int index);
	bool PlayNextSound();
	bool PauseSound();
	bool Initialize();
	bool Initialize(char *filename);
	bool GetState();


	int m_BGM_Count;
	int m_iCurrent_index;
	CDSBGM **m_bgm;
	CDSBGMManager();
	~CDSBGMManager();

	int m_volume;		// 추후구현!!


	// DirectShow
	HRESULT InitializeDShow(void);		// nanapark 2002.2.21
	HRESULT SwapSourceFilter(const char*); // nanapark 2002.2.21

	IGraphBuilder *g_pGraphBuilder;		// nanapark 2002.2.21
	IMediaControl *g_pMediaControl;		// nanapark 2002.2.21
	IMediaSeeking *g_pMediaSeeking;		// nanapark 2002.2.21
	IBaseFilter   *g_pSourceCurrent;	// nanapark 2002.2.21
	IBaseFilter   *g_pSourceNext;		// nanapark 2002.2.21
	IBasicAudio   *g_pBasicAudio;       // daniel 2002.11.13
	////////////////////////////

	bool SetVolume(float volume);  // daniel 2002.11.13  volume은 0 ~ 1.0 사이의 값으로.

	bool m_sound_off;	// SOUND OFF면 true 
};

#endif // !defined(AFX_DSBGMMANAGER_H__F983D7DF_21BA_4F68_8CA3_53FDE53D3ADB__INCLUDED_)
