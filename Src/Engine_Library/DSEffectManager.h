// DSEffectManager.h: interface for the CDSEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSEFFECTMANAGER_H__D1909DB2_2995_4A71_AA11_28F29A5B7BD6__INCLUDED_)
#define AFX_DSEFFECTMANAGER_H__D1909DB2_2995_4A71_AA11_28F29A5B7BD6__INCLUDED_

#include "DSEffect.h"	// Added by ClassView

#include <mmsystem.h>		// DirectSound
#include <dxerr8.h>
#include <dsound.h>
#include "common\DSUtil.h"
#include "common\DXUtil.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class __declspec(dllexport) CDSEffectManager  
{
public:
	bool StopAllSound();
	char m_LoadListFilename[256];
	bool Initialize();
	bool Initialize(char *filename);
	bool OpenSound(char *soundname);
	bool OpenSound(int index);
	bool PlaySound(int index);
	bool PlaySound(char *soundname);
	bool StopSound(int index);
	bool StopSound(char *soundname);
	CDSEffect **m_effect;
	int m_Effect_Count;		// ÃÑ EFFECT °¹¼ö
	CDSEffectManager();
	~CDSEffectManager();


	// DirectSound
	bool InitializeDSound();		// nanapark 2002.2.21
	HRESULT PlayDirectSound(CSound **, bool);			// nanapark 2002.3.4
	HRESULT OpenDirectSoundFile(char* filename, CSound **);
	HRESULT StopDirectSound(CSound **);

	//DirectSound 
	CSoundManager* g_pSoundManager;
	CSound**       g_pSoundClip;
	BOOL           g_bBufferPaused;


	bool m_sound_off;	// SOUND OFF¸é true 


	float m_volume; // daniel 2002.11.18
	void SetVolume(float v);  // v -> 0.0(min) ~ 1.0(max)    daniel 2002.11.18
};

#endif // !defined(AFX_DSEFFECTMANAGER_H__D1909DB2_2995_4A71_AA11_28F29A5B7BD6__INCLUDED_)
