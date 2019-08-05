// DSEffectManager.cpp: implementation of the CDSEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSEffectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSEffectManager::CDSEffectManager()
{

	this->m_sound_off = false;
}

CDSEffectManager::~CDSEffectManager()
{

	for (int i=0; i<m_Effect_Count; i++)
	{
		delete m_effect[i];
		m_effect[i] = NULL;
	}
	free(m_effect);

	if(g_pSoundClip)
	{
		for(int i=0;i<m_Effect_Count;i++)
		{
			delete g_pSoundClip[i];
		}
		free(g_pSoundClip);
		g_pSoundClip = 0;
	}

	if(g_pSoundManager) 
		delete g_pSoundManager;
}

bool CDSEffectManager::Initialize()
{
	strcpy(this->m_LoadListFilename, ".\\Data\\Sound\\twogo_effect_loadlist.txt");	

	FILE *fp;
	fp = fopen(m_LoadListFilename, "r");

	char tmp1[256];
	char tmp2[256];

	fscanf(fp, "SOUND=EFFECT\n");
	fscanf(fp, "PCOUNT=%d\n", &this->m_Effect_Count);

	m_effect = (CDSEffect**)malloc(sizeof(CDSEffect*)*(m_Effect_Count));

	for (int i=0; i<m_Effect_Count; i++)
	{
		m_effect[i] = new CDSEffect();
		fscanf(fp, "%d=%s %s %d\n", &m_effect[i]->index, tmp1, tmp2, &m_effect[i]->repeat);
		strcpy(m_effect[i]->soundname,tmp1);
		strcpy(m_effect[i]->filename,tmp2);
	}

	fclose(fp);

	if (!InitializeDSound())
		return false;
	return true;
}

bool CDSEffectManager::Initialize(char *filename)
{
	strcpy(this->m_LoadListFilename, filename);	

	FILE *fp;
	fp = fopen(m_LoadListFilename, "r");

	char tmp1[256];
	char tmp2[256];

	fscanf(fp, "SOUND=EFFECT\n");
	fscanf(fp, "PCOUNT=%d\n", &this->m_Effect_Count);

	m_effect = (CDSEffect**)malloc(sizeof(CDSEffect*)*(m_Effect_Count));

	for (int i=0; i<m_Effect_Count; i++)
	{
		m_effect[i] = new CDSEffect();
		fscanf(fp, "%d=%s %s %d\n", &m_effect[i]->index, tmp1, tmp2, &m_effect[i]->repeat);
		strcpy(m_effect[i]->soundname,tmp1);
		strcpy(m_effect[i]->filename,tmp2);
	}

	fclose(fp);

	if (!InitializeDSound())
		return false;
	return true;
}


bool CDSEffectManager::InitializeDSound()
{
    HRESULT hr;
	g_pSoundManager = NULL;
	
	g_pSoundClip = (CSound**)malloc(sizeof(CSound*)*m_Effect_Count);
	
	for (int i=0; i<m_Effect_Count; i++)
		g_pSoundClip[i] = NULL;

    g_pSoundManager = new CSoundManager();	

    if( FAILED( hr = g_pSoundManager->Initialize( g_cApplication.gethWnd(), DSSCL_PRIORITY, 2, 22050, 16 ) ) )		// ADAMSOFT
    {
        DXTRACE_ERR( TEXT("Initialize"), hr );
//        AfxMessageBox("Error initializing DirectSound.  Sample will now exit.");
        return false;
    }

    g_bBufferPaused = FALSE;		

	return true;
     
}

HRESULT CDSEffectManager::OpenDirectSoundFile(char* strFileName, CSound** g_pSound)
{
   HRESULT hr;

    if( *g_pSound )
    {
        (*g_pSound)->Stop();	
        (*g_pSound)->Reset();	
    }

    // Free any previous sound, and make a new one
    SAFE_DELETE( (*g_pSound) );

    // Load the wave file into a DirectSound buffer
    if( FAILED( hr = g_pSoundManager->Create( g_pSound, strFileName,  DSBCAPS_CTRLVOLUME, GUID_NULL ) ) )	
    {
        // Not a critical failure, so just update the status
        DXTRACE_ERR_NOMSGBOX( TEXT("Create"), hr );
        return hr; 
    }

	return S_OK;
}

HRESULT CDSEffectManager::PlayDirectSound(CSound** g_pSound, bool bLooped)		// nanapark 2002.3.4
{
    HRESULT hr;
//AfxMessageBox("start of playdirectsound");
	if( g_bBufferPaused )	
    {
        // Play the buffer since it is currently paused
        // DWORD dwFlags = bLooped ? DSBPLAY_LOOPING : 0L;		

		DWORD dwFlags = bLooped ? DSBPLAY_LOOPING : 0L;		// nanapark 2002.3.4

        if( FAILED( hr = (*g_pSound)->Play( 0, dwFlags, m_volume ) ) )		// nanapark 2002.3.4 // daniel 2002.11.18 volume
            return DXTRACE_ERR( TEXT("Play"), hr );			

        g_bBufferPaused = FALSE;						

    }
    else
    {
        if( (*g_pSound)->IsSoundPlaying() )	
        {
            // To pause, just stop the buffer, but don't reset the position
            if( *g_pSound )		
               (*g_pSound)->Stop();		

            g_bBufferPaused = TRUE;		
        }
        else		
        {
            // The buffer is not playing, so play it again
            DWORD dwFlags = bLooped ? DSBPLAY_LOOPING : 0L;
	        if( FAILED( hr = (*g_pSound)->Play( 0, dwFlags, m_volume ) ) )		// nanapark 2002.3.4 // daniel 2002.11.18 volume
                //return DXTRACE_ERR( TEXT("Play"), hr );
				return false;

            g_bBufferPaused = FALSE;		
        }
    }

    return S_OK;

}


HRESULT CDSEffectManager::StopDirectSound(CSound** g_pSound)
{
	if (*g_pSound)
	{
		(*g_pSound)->Stop();
		(*g_pSound)->Reset();
	}
	
	return S_OK;
}


bool CDSEffectManager::OpenSound(int index)
{
	if (this->m_sound_off == true)
		return true;

	this->OpenDirectSoundFile(this->m_effect[index]->filename, &this->g_pSoundClip[index]);

	return true;
}

bool CDSEffectManager::OpenSound(char *soundname)
{
	// sound off 면 return
	if (this->m_sound_off == true)
		return true;

	int i = 0;
	// soudname에 맞는 index를 찾기 위해
	for (i=0; i<this->m_Effect_Count; i++)
	{
		if (!strcmpi(m_effect[i]->soundname, soundname))
			break;
	}

	// index 의 유효성 검사
	if (i < m_Effect_Count)
		this->OpenDirectSoundFile(this->m_effect[i]->filename, &this->g_pSoundClip[i]);
	else
	{
		g_cApplication.DisplayError( TRUE, "사운드 파일을 찾을수 없습니다.");
		return false;
	}

	return true;
}

bool CDSEffectManager::PlaySound(char *soundname)
{
	if (this->m_sound_off == true)
		return true;

	int i = 0;
	// soudname에 맞는 index를 찾기 위해
	for (i=0; i<this->m_Effect_Count; i++)
	{
		if (!strcmpi(m_effect[i]->soundname, soundname))			
			break;
	}

	// index 의 유효성 검사
	if (i < m_Effect_Count)
		this->PlayDirectSound(&this->g_pSoundClip[i], this->m_effect[i]->repeat);
	else
	{
		g_cApplication.DisplayError( TRUE, "사운드 파일을 찾을수 없습니다.");
		return false;
	}

	return true;
}

bool CDSEffectManager::PlaySound(int index)
{
	if (this->m_sound_off == true)
		return true;

	this->PlayDirectSound(&this->g_pSoundClip[index], this->m_effect[index]->repeat);

	return true;
}


bool CDSEffectManager::StopSound(char *soundname)
{
	if (this->m_sound_off == true)
		return true;

	int i = 0;
	// soudname에 맞는 index를 찾기 위해
	for (i=0; i<this->m_Effect_Count; i++)
	{
		if (!strcmpi(m_effect[i]->soundname, soundname))			
			break;
	}

	// index 의 유효성 검사
	if (i < m_Effect_Count)
		this->StopDirectSound(&this->g_pSoundClip[i]);
	else
	{
		g_cApplication.DisplayError( TRUE, "사운드 파일을 찾을수 없습니다.");
		return false;
	}


	return true;
}

bool CDSEffectManager::StopSound(int index)
{
	if (this->m_sound_off == true)
		return true;

	this->StopDirectSound(&this->g_pSoundClip[index]);
	return true;
}



bool CDSEffectManager::StopAllSound()
{

	for (int i=0; i< this->m_Effect_Count; i++)
	{
		this->StopDirectSound(&this->g_pSoundClip[i]);
	}
	return true;
}

void CDSEffectManager::SetVolume(float v)  // v -> 0.0(min) ~ 1.0(max)    daniel 2002.11.18
{
	if(v<0.0f)
		v = 0.0f;
	if(v>1.0f)
		v = 1.0f;

	m_volume = v;
}
