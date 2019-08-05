
#ifndef __D3DSOUND_H__
#define __D3DSOUND_H__

#include <dmusici.h>
#include <dsound.h>

// Macro to release a COM object
#define ReleaseCOM( x ) if( x ) { x->Release(); x = NULL; }

// Sizes for sound channel buffers
#define SOUND_BUFFER_SIZE			65536
#define SOUND_BUFFER_CHUNK			SOUND_BUFFER_SIZE / 4

// Forward class declarations
class cSound;
class cSoundData;
class cSoundChannel;

/*-----------------------------------------------------------------------------
 * cSound CLASS
 *---------------------------------------------------------------------------*/

#define MAX_EVENTS				32
class cSound
{
protected:
	// Sound system related
	HWND					m_hWnd;
	long					m_Volume;
	HANDLE					m_Events[MAX_EVENTS+1];
	cSoundChannel			*m_EventChannel[MAX_EVENTS];

	HANDLE					m_hThread;
	DWORD					m_ThreadID;
	BOOL					m_ThreadActive;
	static DWORD			HandleNotifications( LPVOID lpvoid );

	// Sound related
	IDirectSound8			*m_pDS;
	IDirectSoundBuffer		*m_pDSBPrimary;

	long					m_CooperativeLevel;
	long					m_Frequency;
	short					m_Channels;
	short					m_BitsPerSample;

	// Music related - MIDI
	IDirectMusicPerformance8	*m_pDMPerformance;
	IDirectMusicLoader8			*m_pDMLoader;

public:
	cSound();
	~cSound();

	// Assign and release events
	BOOL AssignEvent( cSoundChannel *Channel, short *EventNum, HANDLE *EventHandle );
	BOOL ReleaseEvent( cSoundChannel *Channel, short *EventNum );

	// Functions to retrieve COM interfaces
	IDirectSound8				*GetDirectSoundCOM();
	IDirectSoundBuffer			*GetPrimaryBufferCOM();
	IDirectMusicPerformance8	*GetPerformanceCOM();
	IDirectMusicLoader8			*GetLoaderCOM();

	// Init and shutdown functions
	BOOL Initialize( HWND hWnd, long Frequency = 22050, short Channels = 1, short BitsPerSample = 16, long CooperativeLevel = DSSCL_PRIORITY );
	BOOL Shutdown();

	// Volume get/set
	long GetVolume();
	BOOL SetVolume( long Percent );

	// Restore system to known state
	BOOL Restore();
};


/*-----------------------------------------------------------------------------
 * cSoundData CLASS
 *---------------------------------------------------------------------------*/

class cSoundData
{
	friend class cSoundChannel;

protected:
	long	m_Frequency;
	short	m_Channels;
	short	m_BitsPerSample;

	FILE	*m_fp;
	char	*m_Ptr;
	char	*m_Buf;

	long	m_Size;
	long	m_Left;

	long	m_StartPos;
	long	m_Pos;

public:
	cSoundData();
	~cSoundData();

	char *GetPtr();
	long GetSize();

	BOOL Create();
	BOOL Create( long Size );
	BOOL Free();

	BOOL SetFormat( long Frequence, short Channels, short BitsPerSample );
	BOOL SetSource( FILE *fp, long Pos = -1, long Size = -1 );
	BOOL SetSource( void *Ptr, long Pos = -1, long Size = -1 );

	BOOL LoadWAV( char *Filename, FILE *fp = NULL );
	BOOL LoadWAVHeader( char *Filename, FILE *fp = NULL );

	BOOL Copy( cSoundData *Source );
};

/*-----------------------------------------------------------------------------
 * cSoundChannel CLASS
 *---------------------------------------------------------------------------*/

class cSoundChannel
{
	friend class cSound;

protected:
	cSound				*m_Sound;
	IDirectSoundBuffer8	*m_pDSBuffer;
	IDirectSoundNotify8	*m_pDSNotify;
	short				m_Event;

	long				m_Volume;
	signed long			m_Pan;
	BOOL				m_Playing;
	long				m_Loop;

	long				m_Frequency;
	short				m_BitsPerSample;
	short				m_Channels;

	cSoundData			m_Desc;

	short				m_LoadSection;
	short				m_StopSection;
	short				m_NextNotify;

	BOOL BufferData();
	BOOL Update();

public:
	cSoundChannel();
	~cSoundChannel();

	IDirectSoundBuffer8	*GetSoundBufferCOM();
	IDirectSoundNotify8	*GetNotifyCOM();

	BOOL Create( cSound *Sound, long Frequency = 22050, short Channels = 1, short BitsPerSample = 16 );
	BOOL Create( cSound *Sound, cSoundData *SoundDesc );
	BOOL Free();

	BOOL Play( cSoundData *Desc, long VolumePercent = 100, long Loop = 1 );
	BOOL Stop();

	long GetVolume();
	BOOL SetVolume( long Percent );

	signed long GetPan();
	BOOL SetPan( signed long Level );

	long GetFrequency();
	BOOL SetFrequency( long Level );

	BOOL IsPlaying();
};


#pragma pack(1)
typedef struct sWaveHeader
{
	char RiffSig[4];
	long WaveformChunkSize;
	char WaveSig[4];
	char FormatSig[4];
	long FormatChunkSize;
	short FormatTag;
	short Channels;
	long SampleRate;
	long BytesPerSec;
	short BlockAlign;
	short BitsPerSample;
	char DataSig[4];
	long DataSize;
} sWaveHeader;
#pragma pack()


#endif

