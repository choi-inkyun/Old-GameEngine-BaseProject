// DSBGMManager.cpp: implementation of the CDSBGMManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSBGMManager.h"
#include "common\dxutil.h"
#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
                                                                
CDSBGMManager::CDSBGMManager()
{
	this->m_sound_off = false;
	m_bgm = NULL;
}

CDSBGMManager::~CDSBGMManager()
{
	// nanapark 2002.2.21  /////////////
    // Stop playback
    if (g_pMediaControl)
        g_pMediaControl->Stop();

    // Release all remaining pointers
    SAFE_RELEASE( g_pSourceNext);
    SAFE_RELEASE( g_pSourceCurrent);
    SAFE_RELEASE( g_pMediaSeeking);
    SAFE_RELEASE( g_pMediaControl);
    SAFE_RELEASE( g_pGraphBuilder);

	if(m_bgm)
	{
		for (int i=0; i<m_BGM_Count; i++)
		{
			delete m_bgm[i];
			m_bgm[i] = NULL;
		}
		free(m_bgm);
	}

}

bool CDSBGMManager::Initialize()
{
	this->m_sound_off = false;	
	strcpy(this->m_LoadListFilename, ".\\Data\\Sound\\twogo_bgm_loadlist.txt");	

	m_iCurrent_index = -1;
	///////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// nanapark 2002.4.14
	///////////////////////////////////////////////////////////////////////

	FILE *fp;
	fp = fopen(m_LoadListFilename, "r");
	if (!fp)
	{
		g_cApplication.DisplayError( TRUE, "BGM loadlist 파일이 없습니다.");
		return false;
	}

	char tmp1[256];
	char tmp2[256];

	fscanf(fp, "SOUND=BGM\n");
	fscanf(fp, "PCOUNT=%d\n", &this->m_BGM_Count);

	m_bgm = (CDSBGM**)malloc(sizeof(CDSBGM*)*(m_BGM_Count));


	for (int i=0; i<m_BGM_Count; i++)
	{
		m_bgm[i] = new CDSBGM();
		fscanf(fp, "%d=%s %s %d\n", &m_bgm[i]->index, tmp1, tmp2, &m_bgm[i]->repeat);
		strcpy(m_bgm[i]->soundname,tmp1);
		strcpy(m_bgm[i]->filename,tmp2);
	}


	fclose(fp);
	///////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// nanapark 2002.4.14
	///////////////////////////////////////////////////////////////////////

	InitializeDShow();

	return true;
}


bool CDSBGMManager::Initialize(char *filename)
{
	this->m_sound_off = false;	
	strcpy(this->m_LoadListFilename, filename);	

	m_iCurrent_index = -1;
	///////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// nanapark 2002.4.14
	///////////////////////////////////////////////////////////////////////

	FILE *fp;
	fp = fopen(m_LoadListFilename, "r");
	if (!fp)
	{
		g_cApplication.DisplayError( TRUE, "BGM loadlist 파일이 없습니다.");
		return false;
	}

	char tmp1[256];
	char tmp2[256];

	fscanf(fp, "SOUND=BGM\n");
	fscanf(fp, "PCOUNT=%d\n", &this->m_BGM_Count);

	m_bgm = (CDSBGM**)malloc(sizeof(CDSBGM*)*(m_BGM_Count));


	for (int i=0; i<m_BGM_Count; i++)
	{
		m_bgm[i] = new CDSBGM();
		fscanf(fp, "%d=%s %s %d\n", &m_bgm[i]->index, tmp1, tmp2, &m_bgm[i]->repeat);
		strcpy(m_bgm[i]->soundname,tmp1);
		strcpy(m_bgm[i]->filename,tmp2);
	}


	fclose(fp);
	///////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// nanapark 2002.4.14
	///////////////////////////////////////////////////////////////////////

	InitializeDShow();

	return true;
}

HRESULT CDSBGMManager::InitializeDShow(void)
{

	g_pGraphBuilder=NULL;		// nanapark 2002.2.21
	g_pMediaControl=NULL;		// nanapark 2002.2.21
	g_pMediaSeeking=NULL;		// nanapark 2002.2.21
	g_pSourceCurrent=NULL;		// nanapark 2002.2.21
	g_pSourceNext=NULL;			// nanapark 2002.2.21

    HRESULT hr;

    // Create DirectShow Graph
    if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,		// ADAMSOFT
                                      CLSCTX_INPROC, IID_IGraphBuilder,
                                      reinterpret_cast<void **>(&g_pGraphBuilder))) )
        return hr;

    // Get the IMediaControl Interface
    if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaControl,		// ADAMSOFT
                                 reinterpret_cast<void **>(&g_pMediaControl))))
        return hr;

    // Get the IMediaControl Interface
    if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,		// ADAMSOFT
                                 reinterpret_cast<void **>(&g_pMediaSeeking))))
        return hr;

	if (FAILED (g_pGraphBuilder->QueryInterface(IID_IBasicAudio,            // daniel 2002.11.13
								reinterpret_cast<void **>(&g_pBasicAudio))))
		return hr;

    // Create Source Filter for first file
//    g_iNextFile = 0;			// ADAMSOFT

    // Set a timer for switching the sources
    //SetTimer(g_hwndApp, 0, MEDIA_TIMEOUT, &MyTimerProc);

    return S_OK;
}


HRESULT CDSBGMManager::SwapSourceFilter(const  char *szFilename)
{
    HRESULT hr = S_OK;
    IPin *pPin = NULL;
//    TCHAR szFilename[MAX_PATH];
    WCHAR wFileName[MAX_PATH];

    // Determine the file to load based on DirectX Media path (from SDK)
//    _tcscpy( szFilename, DXUtil_GetDXSDKMediaPath() );
//    _tcscat( szFilename, pstrFiles[g_iNextFile % g_iNumFiles]);
//    _tcscpy( g_szCurrentFile, pstrFiles[g_iNextFile % g_iNumFiles]);
//    g_iNextFile++;

//	sprintf(szFilename, "sound\\bingtor.mp3");
//	g_szCurrentFile = "sound\\background.mp3";

    // Make sure that this file exists
    DWORD dwAttr = GetFileAttributes(szFilename);
    if (dwAttr == (DWORD) -1)
        return ERROR_FILE_NOT_FOUND;

    #ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, szFilename, -1, wFileName, MAX_PATH);		// ADAMSOFT
    #else
        lstrcpy(wFileName, szFilename);
    #endif
    // OPTIMIZATION OPPORTUNITY
	// This will open the file, which is expensive. To optimize, this
    // should be done earlier, ideally as soon as we knew this was the
    // next file to ensure that the file load doesn't add to the
    // filter swapping time & cause a hiccup.
    //
    // Add the new source filter to the graph. (Graph can still be running)
    hr = g_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &g_pSourceNext);		//ADAMSOFT
	
    // Get the first output pin of the new source filter. Audio sources 
    // typically have only one output pin, so for most audio cases finding 
    // any output pin is sufficient.
    if (SUCCEEDED(hr)) {								// ADAMSOFT
        hr = g_pSourceNext->FindPin(L"Output", &pPin);	// ADAMSOFT
    }
	else
	{
		g_cApplication.DisplayError( TRUE, "사운드를 재생할 수 없습니다. 드라이버를 확인하십시오.");
	}

    // Stop the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Stop();
    }
    // Break all connections on the filters. You can do this by adding 
    // and removing each filter in the graph
    if (SUCCEEDED(hr)) {
        IEnumFilters *pFilterEnum = NULL;
        IBaseFilter  *pFilterTemp = NULL;

        if (SUCCEEDED(hr = g_pGraphBuilder->EnumFilters(&pFilterEnum))) {
            int iFiltCount = 0;
            int iPos = 0;

            // Need to know how many filters. If we add/remove filters during the
            // enumeration we'll invalidate the enumerator
            while (S_OK == pFilterEnum->Skip(1)) {
                iFiltCount++;
            }

            // Allocate space, then pull out all of the 
            IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
                                      (_alloca(sizeof(IBaseFilter *) * iFiltCount));
            pFilterEnum->Reset();

            while (S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));
            SAFE_RELEASE(pFilterEnum);

            for (iPos = 0; iPos < iFiltCount; iPos++) {
                g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
				// Put the filter back, unless it is the old source
				if (ppFilters[iPos] != g_pSourceCurrent) {
					g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
                }
				SAFE_RELEASE(ppFilters[iPos]);
            }
        }
    }

    // We have the new ouput pin. Render it
    if (SUCCEEDED(hr)) {
        hr = g_pGraphBuilder->Render(pPin);
        g_pSourceCurrent = g_pSourceNext;
        g_pSourceNext = NULL;
    }

    SAFE_RELEASE(pPin);
    SAFE_RELEASE(g_pSourceNext); // In case of errors

    // Re-seek the graph to the beginning
    if (SUCCEEDED(hr)) {
        LONGLONG llPos = 0;
        hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                           &llPos, AM_SEEKING_NoPositioning);
    } 

    // Start the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Run();		
    }

	
    // Release the old source filter.
    SAFE_RELEASE(g_pSourceCurrent)
    return S_OK;   
}

bool CDSBGMManager::PlaySound(char *soundname)
{
	if (this->m_sound_off == true)
		return true;

	int i = 0;
	for (i=0; i<this->m_BGM_Count; i++)
	{
		if (!strcmpi(m_bgm[i]->soundname, soundname))			
			break;
	}

	if (i < m_BGM_Count)
	{
		m_iCurrent_index = i;
		this->SwapSourceFilter(this->m_bgm[i]->filename);
	}
	else
	{
		g_cApplication.DisplayError( TRUE, "사운드 파일을 찾을수 없습니다.");
		return false;
	}
	return true;
}


bool CDSBGMManager::PauseSound()
{
	if (m_sound_off == true)
		g_pMediaControl->Pause();
	else
		g_pMediaControl->Run();

	return true;
}

bool CDSBGMManager::PlaySound(int index)
{
	m_iCurrent_index = index;

	if (this->m_sound_off == true)
		return true;

	if (index != -1)
		this->SwapSourceFilter(this->m_bgm[index]->filename);

	return true;
}

bool CDSBGMManager::PlayNextSound()
{
	m_iCurrent_index = (m_iCurrent_index+1) % m_BGM_Count;

	if (this->m_sound_off == true)
		return true;

	PlaySound(m_iCurrent_index);
	return true;
}


bool CDSBGMManager::OpenSound(int index)
{

	// 일단은 필요없음

	return true;
}


bool CDSBGMManager::OpenSound(char *soundname)
{

	// 일단은 필요없음

	return true;
}

bool CDSBGMManager::StopSound(int index)
{

	// 다른 파일 Play 하면 저절로 멈춤.
    if (g_pMediaControl)
        g_pMediaControl->Stop();


	return true;
}

bool CDSBGMManager::StopSound(char *soundname)
{
	// 다른 파일 Play 하면 저절로 멈춤.

	return true;
}

bool CDSBGMManager::GetState()
{
	//OAFilterState pfs;
	//HRESULT hr = g_pMediaControl->GetState(10, &pfs);

	LONGLONG stopP;
	g_pMediaSeeking->GetStopPosition(&stopP);
	LONGLONG currP;
	g_pMediaSeeking->GetCurrentPosition(&currP);

	if ( stopP == currP )
	{
		return 0;
	}
	else
		return 1;

	/*
	if (hr != S_OK)
		Beep(900,1000);

	// pfs ==  0:stop , 1:pause, 2:running
	if (pfs <= 1)
		return 0;		// stop
	else
		return 1;		// running or pause
		*/
}

bool CDSBGMManager::SetVolume(float volume)  // daniel 2002.11.13  volume은 0 ~ 1.0 사이의 값으로.
{
	long a = (long)((1.0f-volume) * - 2500.0f);  // daniel 2002.11.18

	if(a > 0)
		a = 0;
	if(a < -2500)
		a = -2500;

	g_pBasicAudio->put_Volume(a);

	return true;
}
