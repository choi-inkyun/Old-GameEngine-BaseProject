// DirectMovie.cpp: implementation of the CDirectMovie class.
//
//////////////////////////////////////////////////////////////////////

#include <StdAfx.h>
#include "DirectMovie.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectMovie*  g_CurDirectMovie = NULL;

LRESULT CALLBACK MovieProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
			g_CurDirectMovie->CloseClip();
		break;
	case WM_GRAPHNOTIFY:
		g_CurDirectMovie->HandleGraphEvent();
		break;
 	case WM_CLOSE:
	case WM_QUIT:
		return 0;
  }
	return DefWindowProc(hWnd, message, wParam, lParam);
}

CDirectMovie::CDirectMovie()
{
	m_pGB = NULL;
	m_pMC = NULL;
	m_pME = NULL;
	m_pVW = NULL;
	m_pBA = NULL;
	m_pBV = NULL;
	m_pMS = NULL;
	m_pMP = NULL;

	m_hWnd = NULL;

	m_psCurrent=Stopped;

	m_iWidth = 0;
	m_iHeight = 0;

	i_PosX = 0;
	i_PosY = 0;

    CoInitialize(NULL);
}

CDirectMovie::~CDirectMovie()
{
	ChangeDisplaySettings(NULL,0);
    CoUninitialize();
}

VOID CDirectMovie::InitAll(HINSTANCE hInst, HWND hParent)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MovieProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)COLOR_BACKGROUND;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "MovieWindow";
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

    m_hWnd = CreateWindow("MovieWindow", "MovieWindow", WS_POPUP |WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
     0, 0, m_iWidth, m_iHeight, hParent, NULL, hInst, NULL);

	g_CurDirectMovie = this;

	DEVMODE dmScreenSettings; // Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth = m_iWidth;
	dmScreenSettings.dmPelsHeight = m_iHeight;
	dmScreenSettings.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT;	//DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;	
//	dmScreenSettings.dmBitsPerPel = 16;						// Selected Bits Per Pixel
//	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;				//|DM_PELSWIDTH|DM_PELSHEIGHT;	
	ChangeDisplaySettings(&dmScreenSettings, CDS_GLOBAL);

	ShowVideoWindow();
}

VOID CDirectMovie::ShowVideoWindow()
{
    // Let's get ready to rumble!
    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateWindow(m_hWnd);
    SetFocus(m_hWnd);
}

VOID CDirectMovie::HideVideoWindow()
{
    ShowWindow(m_hWnd, SW_HIDE);
}

VOID CDirectMovie::ReleaseAll()
{
	CloseClip();
	DestroyWindow(m_hWnd);
}

HRESULT CDirectMovie::InitVideoWindow(int nMultiplier, int nDivider)
{
    HRESULT hr = S_OK;
    RECT rect;

    if (!m_pBV)
        return S_OK;

    // Read the default video size
    hr = m_pBV->GetVideoSize(&m_iWidth, &m_iHeight);
    if (hr == E_NOINTERFACE)
        return S_OK;

    // Account for requests of normal, half, or double size
    LONG lWidth  = m_iWidth  * nMultiplier / nDivider;
    LONG lHeight = m_iHeight * nMultiplier / nDivider;
    SetWindowPos(m_hWnd, NULL, 0, 0, lWidth, lHeight,
                 SWP_NOMOVE | SWP_NOOWNERZORDER);

    int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
    int nBorderWidth  = GetSystemMetrics(SM_CXBORDER);
    int nBorderHeight = GetSystemMetrics(SM_CYBORDER);

    // Account for size of title bar and borders for exact match
    // of window client area to default video size
    SetWindowPos(m_hWnd, NULL, 0, 0, lWidth + 2*nBorderWidth,
            lHeight + nTitleHeight + 2*nBorderHeight,
            SWP_NOMOVE | SWP_NOOWNERZORDER);

    GetClientRect(m_hWnd, &rect);
    m_pVW->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom);

    return hr;
}

/*
void MoveVideoWindow(void)
{
    HRESULT hr;

    // Track the movement of the container window and resize as needed
    if(m_pVW)
    {
        RECT client;

        GetClientRect(ghApp, &client);
        hr = m_pVW->SetWindowPosition(client.left, client.top,
                                    client.right, client.bottom);
    }
}
*/

void CDirectMovie::PauseClip(void)
{
    if (!m_pMC)
        return;

    // Toggle play/pause behavior
    if((m_psCurrent == Paused) || (m_psCurrent == Stopped))
    {
        if (SUCCEEDED(m_pMC->Run()))
            m_psCurrent = Running;
    }
    else
    {
        if (SUCCEEDED(m_pMC->Pause()))
            m_psCurrent = Paused;
    }
}


void CDirectMovie::StopClip(void)
{
    HRESULT hr;

    if ((!m_pMC) || (!m_pMS))
        return;

    // Stop and reset postion to beginning
    if((m_psCurrent == Paused) || (m_psCurrent == Running))
    {
        LONGLONG pos = 0;
        hr = m_pMC->Stop();
        m_psCurrent = Stopped;

        // Seek to the beginning
        hr = m_pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
            NULL, AM_SEEKING_NoPositioning);

        // Display the first frame to indicate the reset condition
        hr = m_pMC->Pause();
    }
}

void CDirectMovie::CloseClip(void)
{
    HRESULT hr;

    if(m_pMC)
        hr = m_pMC->Stop();

    // Relinquish ownership (IMPORTANT!) after hiding video window
    if(m_pVW)
    {
        hr = m_pVW->put_Visible(OAFALSE);
        hr = m_pVW->put_Owner(NULL);
    }

    // Disable event callbacks
    if (m_pME)
        hr = m_pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

    // Release and zero DirectShow interfaces
    MOVE_RELEASE(m_pME);
    MOVE_RELEASE(m_pMS);
    MOVE_RELEASE(m_pMP);
    MOVE_RELEASE(m_pMC);
    MOVE_RELEASE(m_pBA);
    MOVE_RELEASE(m_pBV);
    MOVE_RELEASE(m_pVW);
    MOVE_RELEASE(m_pGB);

	//HideVideoWindow();
	m_bPlayMovie = FALSE;
	m_psCurrent=Stopped;

	::ShowCursor(TRUE);
}

BOOL CDirectMovie::MovieRun()
{
	MSG msg;
	// Main message loop:
	while (m_bPlayMovie != FALSE) 
	{
		if(GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return TRUE;
}

HRESULT CDirectMovie::PlayMovieInWindow(LPTSTR szFile)
{
	if(szFile == NULL)
		return E_FAIL;

    USES_CONVERSION;

	CloseClip();

	::ShowCursor(FALSE);
    WCHAR wFile[MAX_PATH];
    HRESULT hr;

    // Clear open dialog remnants before calling RenderFile()
    //UpdateWindow(m_hWnd);

    // Convert filename to wide character string
    wcscpy(wFile, T2W(szFile));

    // Get the interface for DirectShow's GraphBuilder
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                         IID_IGraphBuilder, (void **)&m_pGB);

    if(SUCCEEDED(hr))
    {
        // Have the graph builder construct its the appropriate graph automatically
        m_pGB->RenderFile(wFile, NULL);

        // QueryInterface for DirectShow interfaces
        m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
        m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);
        m_pGB->QueryInterface(IID_IMediaSeeking, (void **)&m_pMS);
        m_pGB->QueryInterface(IID_IMediaPosition, (void **)&m_pMP);

        // Query for video interfaces, which may not be relevant for audio files
        m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW);
        m_pGB->QueryInterface(IID_IBasicVideo, (void **)&m_pBV);

        // Query for audio interfaces, which may not be relevant for video-only files
        m_pGB->QueryInterface(IID_IBasicAudio, (void **)&m_pBA);

        // Have the graph signal event via window callbacks for performance
        m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);

        m_pVW->put_Owner((OAHWND)m_hWnd);
        m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_pVW->HideCursor(TRUE);

		// 비디오 사이즈 정리
        InitVideoWindow(1, 1);

		ShowVideoWindow();

	    // Run the graph to play the media file
        m_pMC->Run();
        m_psCurrent=Running;
		m_bPlayMovie = TRUE;

//		FullScreen();
    }

//    return MovieRun(); // 여기로 들어가면 동영상 재생이 끝날 때까지 다른 것 처리가 안된다.
	return TRUE;
}

HRESULT CDirectMovie::FullScreen(void)
{
    HRESULT hr=S_OK;
    LONG lMode;
//    static HWND hDrain=0;

    // Don't bother with full-screen for audio-only files
    if (!m_pVW)
        return S_OK;

    // Read current state
    m_pVW->get_FullScreenMode(&lMode);

    if (lMode == OAFALSE)
    {
        // Save current message drain
//        LIF(pVW->get_MessageDrain((OAHWND *) &hDrain));

        // Set message drain to application main window
        m_pVW->put_MessageDrain((OAHWND) m_hWnd);

        // Switch to full-screen mode
        lMode = OATRUE;
		m_pVW->put_FullScreenMode(lMode);
    }
/*    else
    {
        // Switch back to windowed mode
        lMode = OAFALSE;
        JIF(pVW->put_FullScreenMode(lMode));

        // Undo change of message drain
        LIF(pVW->put_MessageDrain((OAHWND) hDrain));

        // Reset video window
        LIF(pVW->SetWindowForeground(-1));

        // Reclaim keyboard focus for player application
        UpdateWindow(ghApp);
        SetForegroundWindow(ghApp);
        SetFocus(ghApp);
        g_bFullscreen = FALSE;
    }
*/
    return hr;
}


HRESULT CDirectMovie::HandleGraphEvent(void)
{
    LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    // Make sure that we don't access the media event interface
    // after it has already been released.
    if (!m_pME)
	    return S_OK;

    // Process all queued events
    while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                    (LONG_PTR *) &evParam2, 0)))
    {
        // Free memory associated with callback, since we're not using it
        hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);

        // If this is the end of the clip, reset to beginning
        if(EC_COMPLETE == evCode)
        {
			CloseClip();
			return S_OK;

            LONGLONG pos=0;

            // Reset to first frame of movie
            hr = m_pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
                                   NULL, AM_SEEKING_NoPositioning);
            if (FAILED(hr))
            {
                // Some custom filters (like the Windows CE MIDI filter)
                // may not implement seeking interfaces (IMediaSeeking)
                // to allow seeking to the start.  In that case, just stop
                // and restart for the same effect.  This should not be
                // necessary in most cases.
                if (FAILED(hr = m_pMC->Stop()))
                    break;

                if (FAILED(hr = m_pMC->Run()))
                    break;
            }
        }
    }

    return hr;
}
