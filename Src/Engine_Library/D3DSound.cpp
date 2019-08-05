/*
 * FILENAME:
 *			ENGINE_Sound.cpp
 *
 * DESCRIPTION:
 *
 * AUTHOR:	
 *
 * HISTORY:
 *			
 */

#include "StdAfx.h"
#include "D3DSound.h"

/*-----------------------------------------------------------------------------
 * cSound CLASS
 *---------------------------------------------------------------------------*/

cSound::cSound()
{
	short i;

	// Initialize COM
	CoInitialize( NULL );

	m_hWnd				= NULL;
	m_Volume			= 0;

	m_hThread			= NULL;
	m_ThreadID			= NULL;
	m_ThreadActive		= FALSE;

	m_pDMPerformance	= NULL;
	m_pDMLoader			= NULL;

	m_pDS				= NULL;
	m_pDSBPrimary		= NULL;

	for( i = 0; i < MAX_EVENTS; i++ ) m_EventChannel[i] = NULL;
	for( i = 0; i < MAX_EVENTS+1; i++ ) m_Events[i] = NULL;
}

cSound::~cSound()
{
	Shutdown();

	// Uninitialize COM
	CoUninitialize();
}

BOOL cSound::Initialize( HWND hWnd, long Frequency, short Channels, short BitsPerSample, long CooperativeLevel )
{
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	wfex;
	short			i;

	// Shutdown system in case of prior install
	Shutdown();

	if( ( m_hWnd = hWnd ) == NULL ) return FALSE;

	///////////////////////////////////////////////////////////
	// Initialize DirectSound
	///////////////////////////////////////////////////////////

	// Save settings of sound setup
	if( CooperativeLevel == DSSCL_NORMAL ) CooperativeLevel = DSSCL_PRIORITY;
	m_CooperativeLevel = CooperativeLevel;
	m_Frequency        = Frequency;
	m_Channels         = Channels;
	m_BitsPerSample    = BitsPerSample;

	// Create an IDirectSound8 object
	if( FAILED( DirectSoundCreate8( NULL, &m_pDS, NULL ) ) ) return FALSE;

	// Set cooperative mode
	if( FAILED( m_pDS->SetCooperativeLevel( m_hWnd, m_CooperativeLevel ) ) ) return FALSE;

	// Get primary buffer control
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes	= 0;
	dsbd.lpwfxFormat	= NULL;
	if( FAILED( m_pDS->CreateSoundBuffer( &dsbd, &m_pDSBPrimary, NULL ) ) ) return FALSE;

	// Set the primary buffer format
	ZeroMemory( &wfex, sizeof(WAVEFORMATEX) );
	wfex.wFormatTag		= WAVE_FORMAT_PCM;
	wfex.nChannels		= (WORD)m_Channels;
	wfex.nSamplesPerSec	= m_Frequency;
	wfex.wBitsPerSample	= (WORD)m_BitsPerSample;
	wfex.nBlockAlign	= wfex.wBitsPerSample / 8 * wfex.nChannels;
	wfex.nAvgBytesPerSec= wfex.nSamplesPerSec * wfex.nBlockAlign;
	if( FAILED( m_pDSBPrimary->SetFormat( &wfex) ) ) return FALSE;

	// Create the events, plus an extra one for thread termination
	for( i = 0; i < MAX_EVENTS+1; i++ )
	{
		if( ( m_Events[i] = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL ) return FALSE;
	}
	
	// Create a thread for handling notifications
	if( ( m_hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &m_ThreadID ) ) == NULL ) return FALSE;

	// Start main buffer playing
	if( FAILED( m_pDSBPrimary->Play( 0, 0, DSBPLAY_LOOPING ) ) ) return FALSE;

	return TRUE;
}

BOOL cSound::Shutdown()
{
	short i;

	// Stop the music, and close down
	if( m_pDMPerformance != NULL )
	{
		m_pDMPerformance->Stop( NULL, NULL, 0, 0 );
		m_pDMPerformance->CloseDown();
	}

	// Release the DirectMusic objects
	ReleaseCOM( m_pDMPerformance );
	ReleaseCOM( m_pDMLoader );

	// Go through all used sound channels and free them
	for( i = 0; i < MAX_EVENTS; i++ )
	{
		if( m_EventChannel[i] != NULL )
		{
			m_EventChannel[i]->Free();
			m_EventChannel[i] = NULL;
		}

		// Clear the event status
		if( m_Events[i] != NULL ) ResetEvent( m_Events[i] );
	}

	// Stop the primary channel from playing
	if( m_pDSBPrimary != NULL ) m_pDSBPrimary->Stop();

	// Release the DirectSound objects
	ReleaseCOM( m_pDSBPrimary );
	ReleaseCOM( m_pDS );

	// Force a closure of the thread by triggering the last event
	// and waiting for it to terminate
	if( m_hThread != NULL ) 
	{
		if( m_Events[32] != NULL )
		{
			while( m_ThreadActive == TRUE ) SetEvent( m_Events[32] );
		}
		else
		{
			// getting here means no event assigned on thread and need to terminate it.
			TerminateThread( m_hThread, 0 );
		}
	}

	// Close all event handles
	for( i = 0; i < MAX_EVENTS; i++ )
	{
		if( m_Events[i] != NULL )
		{
			CloseHandle( m_Events[i] );
			m_Events[i] = NULL;
		}
	}

	// Free the thread handle
	if( m_hThread != NULL )
	{
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}
	m_ThreadID = 0;
	
	return TRUE;
}

IDirectSound8 *cSound::GetDirectSoundCOM() { return m_pDS; }
IDirectSoundBuffer *cSound::GetPrimaryBufferCOM() { return m_pDSBPrimary; }
IDirectMusicPerformance8 *cSound::GetPerformanceCOM() { return m_pDMPerformance; }
IDirectMusicLoader8 *cSound::GetLoaderCOM() { return m_pDMLoader; }

BOOL cSound::AssignEvent( cSoundChannel *Channel, short *EventNum, HANDLE *EventHandle )
{
	short i;

	for( i = 0; i < MAX_EVENTS; i++ )
	{
		if( m_Events[i] != NULL && m_EventChannel[i] == NULL )
		{
			ResetEvent( m_Events[i] );
			m_EventChannel[i] = Channel;
			*EventNum = i;
			*EventHandle = m_Events[i];
			return TRUE;
		}
	}

	return FALSE;
}

BOOL cSound::ReleaseEvent( cSoundChannel *Channel, short *EventNum )
{
	if( (unsigned short)*EventNum < MAX_EVENTS && m_EventChannel[*EventNum] == Channel )
	{
		ResetEvent( m_Events[*EventNum] );
		m_EventChannel[*EventNum] = NULL;
		*EventNum = -1;
		return TRUE;
	}

	return FALSE;
}

long cSound::GetVolume()
{
	return m_Volume;
}

BOOL cSound::SetVolume( long Percent )
{
	long Volume;

	// Set the sound main volume
	if( m_pDSBPrimary == NULL ) return FALSE;

	// Calculate a usable volume level
	if( !Percent ) Volume = DSBVOLUME_MIN;
	else Volume = -20 * ( 100 - ( Percent % 101 ) );

	if( FAILED( m_pDSBPrimary->SetVolume( Volume ) ) ) return FALSE;

	m_Volume = Percent % 101;

	return TRUE;
}

DWORD cSound::HandleNotifications( LPVOID lpvoid )
{
	DWORD	dwResult, Channel;
	cSound *SoundPtr = (cSound*)lpvoid;
	BOOL	Complete = FALSE;
	MSG		Msg;

	SoundPtr->m_ThreadActive = TRUE;

	while( Complete == FALSE )
	{
		// Wait for a message
		dwResult = MsgWaitForMultipleObjects( 33, SoundPtr->m_Events, FALSE, INFINITE, QS_ALLEVENTS );

		// Get channel # to update
		Channel = dwResult - WAIT_OBJECT_0;

		// Check for channel update
		if( Channel >= 0 && Channel < MAX_EVENTS )
		{
			if( SoundPtr->m_EventChannel[Channel] != NULL ) 
				SoundPtr->m_EventChannel[Channel]->Update();
		}
		// Check for thread closure
		else if( Channel == MAX_EVENTS )
		{
			Complete = TRUE;
		}
		// Check for waiting messages
		else if( Channel > MAX_EVENTS )
		{
			while( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE) )
			{
				if( Msg.message == WM_QUIT )
				{
					Complete = TRUE;
					break;
				}
			}
		}
	} // end of while( Complete == FALSE )

	SoundPtr->m_ThreadActive = FALSE;

	return 0L;
}

BOOL cSound::Restore()
{
	short i;

	// Handle primary
	if( m_pDSBPrimary != NULL ) m_pDSBPrimary->Restore();

	// Handle all used sound channels
	for( i = 0; i < MAX_EVENTS; i++ )
	{
		if( m_EventChannel[i] != NULL ) m_EventChannel[i]->m_pDSBuffer->Restore();
	}

	return TRUE;
}


/*-----------------------------------------------------------------------------
 * cSoundData CLASS
 *---------------------------------------------------------------------------*/

cSoundData::cSoundData()
{
	m_Frequency		= 22050;
	m_Channels		= 1;
	m_BitsPerSample	= 16;

	m_fp			= NULL;
	m_Ptr			= NULL;
	m_Buf			= NULL;
	m_StartPos		= 0;
	m_Pos			= 0;
	m_Size			= 0;
	m_Left			= 0;
}

cSoundData::~cSoundData()
{
	Free();
}

BOOL cSoundData::Create()
{
	return Create( m_Size );
}

BOOL cSoundData::Create( long Size )
{
	// Free prior allocated data
	Free();

	// Check for valid size
	if( ( m_Size = Size ) == 0 ) return FALSE;

	// Create a new buffer
	m_Buf = new char[ m_Size ];
	if( m_Buf == NULL ) return FALSE;

	// Point to new buffer
	m_Ptr = m_Buf;
	m_fp  = NULL;

	// Clear out new buffer
	ZeroMemory( m_Buf, m_Size );

	return TRUE;
}

BOOL cSoundData::Free()
{
	if( m_Buf != NULL )
	{
		delete [] m_Buf;
		m_Buf = NULL;
	}

	m_Ptr  = NULL;
	m_Size = 0;

	return TRUE;
}

char *cSoundData::GetPtr() { return m_Buf; }
long cSoundData::GetSize() { return m_Size; }

BOOL cSoundData::SetFormat( long Frequency, short Channels, short BitsPerSample )
{
	m_Frequency		= Frequency;
	m_Channels		= Channels;
	m_BitsPerSample = BitsPerSample;

	return TRUE;
}

BOOL cSoundData::SetSource( FILE *fp, long Pos, long Size )
{
	m_fp  = fp;
	m_Ptr = NULL;

	if( Pos  != -1 ) m_StartPos = m_Pos = Pos;
	if( Size != -1 ) m_Size = m_Left = Size;

	return TRUE;
}

BOOL cSoundData::SetSource( void *Ptr, long Pos, long Size )
{
	m_fp  = NULL;
	m_Ptr = (char*)Ptr;

	if( Pos  != -1 ) m_StartPos = m_Pos = Pos;
	if( Size != -1 ) m_Size = m_Left = Size;

	return TRUE;
}

BOOL cSoundData::LoadWAV( char *Filename, FILE *fp )
{
	if( LoadWAVHeader( Filename, fp ) == FALSE ) return FALSE;

	if( Create() == FALSE ) return FALSE;

	// open file, seek to position and read in data
	if( Filename != NULL )
	{
		if( ( fp = fopen( Filename, "rb" ) ) == NULL ) return FALSE;
	}
	fseek( fp, m_StartPos, SEEK_SET );
	fread( m_Buf, 1, m_Size, fp );
	m_StartPos = m_Pos = 0;

	// close up file
	if( Filename != NULL ) fclose( fp );

	return TRUE;
}

BOOL cSoundData::LoadWAVHeader( char *Filename, FILE *fp )
{
	sWaveHeader Hdr;
	BOOL ReturnVal;
	long Pos;

	if( Filename == NULL && fp == NULL ) return FALSE;
	if( Filename != NULL )
	{
		if( ( fp = fopen( Filename, "rb" ) ) == NULL ) return FALSE;
	}

	// Save position in file
	Pos = ftell( fp );

	// Read in header and parse
	ReturnVal = FALSE;
	fread( &Hdr, 1, sizeof(sWaveHeader), fp );
	if( !memcmp( Hdr.RiffSig, "RIFF", 4 ) && !memcmp( Hdr.WaveSig, "WAVE", 4 ) && 
		!memcmp( Hdr.FormatSig, "fmt ", 4 ) && !memcmp( Hdr.DataSig, "data", 4 )  )
	{
		m_Frequency		= Hdr.SampleRate;
		m_Channels		= Hdr.Channels;
		m_BitsPerSample	= Hdr.BitsPerSample;

		m_Size = m_Left = Hdr.DataSize;
		m_StartPos = m_Pos = ftell( fp );

		ReturnVal = TRUE;
	}

	// Close if we opened file otherwise return to original position
	if( Filename != NULL ) fclose( fp );
	else fseek( fp, Pos, SEEK_SET );

	return ReturnVal;
}

BOOL cSoundData::Copy( cSoundData *Source )
{
	if( Source == NULL ) return FALSE;

	m_Frequency			= Source->m_Frequency;
	m_Channels			= Source->m_Channels;
	m_BitsPerSample		= Source->m_BitsPerSample;

	m_fp				= Source->m_fp;
	m_Ptr				= Source->m_Ptr;
	m_Size				= Source->m_Size;
	m_Left				= Source->m_Left;
	m_Pos				= Source->m_Pos;
	m_StartPos			= Source->m_StartPos;

	return TRUE;
}


/*-----------------------------------------------------------------------------
 * cSoundChannel CLASS
 *---------------------------------------------------------------------------*/

cSoundChannel::cSoundChannel()
{
	m_Sound		= NULL;
	m_pDSBuffer = NULL;
	m_pDSNotify = NULL;

	m_Event		= -1;

	m_Volume	= 0;
	m_Pan		= 0;
	m_Frequency = 0;
	m_Playing	= FALSE;
}

cSoundChannel::~cSoundChannel()
{
	Free();
}

IDirectSoundBuffer8 *cSoundChannel::GetSoundBufferCOM() { return m_pDSBuffer; }
IDirectSoundNotify8 *cSoundChannel::GetNotifyCOM() { return m_pDSNotify; }

BOOL cSoundChannel::Create( cSound *Sound, long Frequency, short Channels, short BitsPerSample )
{
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfex;
	HANDLE		 hEvent;
	DSBPOSITIONNOTIFY	dspn[4];
	IDirectSoundBuffer *pDSBuffer;

	// Free a prior channel
	Free();

	if( ( m_Sound = Sound ) == NULL ) return FALSE;
	if( m_Sound->GetDirectSoundCOM() == NULL ) return FALSE;

	// Save playback format
	m_Frequency		= Frequency;
	m_BitsPerSample	= BitsPerSample;
	m_Channels		= Channels;

	// Create a new sound buffer for this channel Using specified format
	ZeroMemory( &wfex, sizeof(WAVEFORMATEX) );
	wfex.wFormatTag		= WAVE_FORMAT_PCM;
	wfex.nChannels		= (WORD)m_Channels;
	wfex.nSamplesPerSec = m_Frequency;
	wfex.wBitsPerSample = (WORD)m_BitsPerSample;
	wfex.nBlockAlign	= wfex.wBitsPerSample / 8 * wfex.nChannels;
	wfex.nAvgBytesPerSec= wfex.nSamplesPerSec * wfex.nBlockAlign;

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	dsbd.dwSize			= sizeof( DSBUFFERDESC );
	dsbd.dwFlags		= DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes	= SOUND_BUFFER_SIZE;
	dsbd.lpwfxFormat	= &wfex;
	if( FAILED( m_Sound->GetDirectSoundCOM()->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) ) return FALSE;

	// query for newer interface
	if( FAILED( pDSBuffer->QueryInterface( IID_IDirectSoundBuffer8, (void**)&m_pDSBuffer ) ) )
	{
		pDSBuffer->Release();
		return FALSE;
	}

	// Release old object - we have the newer one now
	pDSBuffer->Release();

	// Create the notification interface
	if( FAILED( m_pDSBuffer->QueryInterface( IID_IDirectSoundNotify8, (void**)&m_pDSNotify ) ) ) return FALSE;

	// Get an event for this
	if( m_Sound->AssignEvent( this, &m_Event, &hEvent ) == FALSE ) return FALSE;

	// Setup the 4 notification positions
	dspn[0].dwOffset = SOUND_BUFFER_CHUNK - 1;
	dspn[0].hEventNotify = hEvent;
	dspn[1].dwOffset = SOUND_BUFFER_CHUNK * 2 - 1;
	dspn[1].hEventNotify = hEvent;
	dspn[2].dwOffset = SOUND_BUFFER_CHUNK * 3 - 1;
	dspn[2].hEventNotify = hEvent;
	dspn[3].dwOffset = SOUND_BUFFER_SIZE - 1;
	dspn[3].hEventNotify = hEvent;
	if( FAILED( m_pDSNotify->SetNotificationPositions( 4, dspn ) ) ) return FALSE;

	// Set the pan and default volume
	SetVolume( 100 );
	SetPan( 0 );

	return TRUE;
}

BOOL cSoundChannel::Create( cSound *Sound, cSoundData *SoundDesc )
{
	return Create( Sound, SoundDesc->m_Frequency, SoundDesc->m_Channels, SoundDesc->m_BitsPerSample );
}

BOOL cSoundChannel::Free()
{
	// Stop any playback
	Stop();

	// Release the notification
	ReleaseCOM( m_pDSNotify );

	// Release event from parent cSound class
	m_Sound->ReleaseEvent( this, &m_Event );

	// Release the buffer
	ReleaseCOM( m_pDSBuffer );

	// Set to no parent sound
	m_Sound = NULL;

	return TRUE;
}

BOOL cSoundChannel::Play( cSoundData *Desc, long VolumePercent, long Loop )
{
	if( Desc == NULL ) return FALSE;
	if( m_pDSBuffer == NULL ) return FALSE;
	if( m_pDSNotify == NULL ) return FALSE;

	// Stop any playback
	Stop();

	// Restore a lost buffer just in case
	m_pDSBuffer->Restore();

	// Setup playing information
	m_Desc.Copy( Desc );

	// Set looping data
	m_Loop = Loop;

	// Calculate stop section position
	if( !m_Loop ) m_StopSection = -1;
	else m_StopSection = (short)( ( ( m_Desc.m_Size * m_Loop ) % SOUND_BUFFER_SIZE ) / SOUND_BUFFER_CHUNK );

	// Buffer in data
	m_LoadSection = 0;
	BufferData();
	BufferData();
	BufferData();
	BufferData();

	// Set the volume
	SetVolume( VolumePercent );

	// Set position and begin play
	m_NextNotify = 0;
	if( FAILED( m_pDSBuffer->SetCurrentPosition( 0 ) ) ) return FALSE;
	if( FAILED( m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING ) ) ) return FALSE;

	// Flag as playing
	m_Playing = TRUE;

	return TRUE;
}

BOOL cSoundChannel::Stop()
{
	if( m_pDSBuffer ) m_pDSBuffer->Stop();
	m_Playing = FALSE;

	return TRUE;
}

long cSoundChannel::GetVolume()
{
	return m_Volume;
}

BOOL cSoundChannel::SetVolume( long Percent )
{
	long Volume;

	if( m_pDSBuffer == NULL ) return FALSE;

	// calculate a usable volume level
	if( !Percent ) Volume = DSBVOLUME_MIN;
	else Volume = -20 * ( 100 - ( Percent % 101 ) );

	if( FAILED( m_pDSBuffer->SetVolume( Volume ) ) ) return FALSE;

	m_Volume = Percent % 101;

	return TRUE;
}

signed long cSoundChannel::GetPan()
{
	return m_Pan;
}

BOOL cSoundChannel::SetPan( signed long Level )
{
	signed long Pan;

	if( m_pDSBuffer == NULL ) return FALSE;

	// calculate a usable setting
	if( Level < 0 ) Pan = DSBPAN_LEFT  / 100 * ( ( -Level ) % 101 );
	else            Pan = DSBPAN_RIGHT / 100 * ( Level % 101 );

	if( FAILED( m_pDSBuffer->SetPan( Pan ) ) ) return FALSE;

	m_Pan = Level % 101;

	return TRUE;
}

long cSoundChannel::GetFrequency()
{
	return m_Frequency;
}

BOOL cSoundChannel::SetFrequency( long Level )
{
	if( m_pDSBuffer == NULL ) return FALSE;
	if( FAILED( m_pDSBuffer->SetFrequency( Level ) ) ) return FALSE;

	m_Frequency = Level;

	return TRUE;
}

BOOL cSoundChannel::IsPlaying()
{
	if( m_Sound == NULL || m_pDSBuffer == NULL || m_pDSNotify == NULL ) return FALSE;
	return m_Playing;
}

BOOL cSoundChannel::BufferData()
{
	long Pos;
	long ToLoad, LoadPos;
	char *Ptr = NULL;
	DWORD Size = 0;

	if( m_pDSBuffer == NULL ) return FALSE;

	// Setup position to load in
	Pos = ( m_LoadSection % 4 ) * SOUND_BUFFER_CHUNK;
	if( FAILED( m_pDSBuffer->Lock( Pos, SOUND_BUFFER_CHUNK, (void**)&Ptr, (DWORD*)&Size, NULL, NULL, 0 ) ) ) return FALSE;

	// Clear out buffer if nothing left to load
	if( !m_Desc.m_Left )
	{
		ZeroMemory( Ptr, Size );
	}
	else
	{
		// Load in the data - take looping into account
		ToLoad = Size;
		LoadPos = 0;
		for( ; ; )
		{
			if( m_Desc.m_Left > ToLoad ) 
			{
				if( m_Desc.m_fp != NULL )
				{
					fseek( m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET );
					fread( &Ptr[LoadPos], 1, ToLoad, m_Desc.m_fp );
				}
				else
				{
					memcpy( &Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], ToLoad );
				}
				m_Desc.m_Left -= ToLoad;
				m_Desc.m_Pos  += ToLoad;
				break;
			}
			else
			{
				if( m_Desc.m_fp != NULL )
				{
					fseek( m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET );
					fread( &Ptr[LoadPos], 1, m_Desc.m_Left, m_Desc.m_fp );
				}
				else
				{
					memcpy( &Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], m_Desc.m_Left );
				}
				ToLoad  -= m_Desc.m_Left;
				LoadPos += m_Desc.m_Left;

				// Check if we need to stop loop
				if( m_Loop >= 1 )
				{
					m_Loop--;
					if( !m_Loop )
					{
						// Clear out remaining buffer space
						if( ToLoad ) ZeroMemory( &Ptr[LoadPos], ToLoad );
						m_Desc.m_Left = 0L;
						break;
					}
				}

				m_Desc.m_Pos  = m_Desc.m_StartPos;
				m_Desc.m_Left = m_Desc.m_Size;

				// See if we need to stop loading data
				if( !ToLoad ) break;
			}
		}
	}

	// Unlock the buffer
	m_pDSBuffer->Unlock( Ptr, Size, NULL, 0 );

	// Mark next section to load
	m_LoadSection++;
	if( m_LoadSection > 3 ) m_LoadSection = 0;

	return TRUE;
}

BOOL cSoundChannel::Update()
{
	// Check for end of sound
	if( m_NextNotify == m_StopSection && !m_Desc.m_Left )
	{
		Stop();
	}
	else
	{
		// Buffer in more data
		BufferData();
		m_NextNotify++;
		if( m_NextNotify > 3 ) m_NextNotify = 0;
	}

	return TRUE;
}

