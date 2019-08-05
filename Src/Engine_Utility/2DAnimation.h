/*-----------------------------------------------------------------------------
 * FILENAME:
 *			2DAnimation.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __2DANIMATION_H__
#define __2DANIMATION_H__

#include "AnimatedGIF.h"

#define INVALID_HANDLE			(-1)

class c2DAnimationFrame
{
public:
	SIZE			m_frameSize;			// 프레임 크기
	UINT			m_nDelay;				// delay (in 1/100s of a second)
	D3DFORMAT		m_nFormat;				// 색상 포맷(D3DFMT_A8R8G8B8)
	DWORD*			m_pBits;				// 이미지 데이터

public:
	c2DAnimationFrame()
	{
		m_frameSize.cx	= 0;
		m_frameSize.cy	= 0;
		m_nDelay		= 0;
		m_pBits			= NULL;
	}

	~c2DAnimationFrame()
	{
		SAFE_DELETEn( m_pBits );
	}
};

class c2DAnimationFrames
{
private:
	UINT			m_nHandle;				// 애니메이션 식별자
	SIZE			m_AnimationSize;		// 애니메이션 크기
	UINT			m_nLevel;				// 출력 레벨
	BOOL			m_bIsPlaying;			// 현재 재생중 플래그
	UINT			m_nCurrFrame;			// 현재 프레임
	DWORD			m_dwLastFrame;			// 최근 프레임 재생 시간
	LPBYTE			m_FrameBuffer;			// 프레임 이미지 생성용 버퍼
	BOOL			m_bDiscontinued;		// 중단 플래그
	BOOL			m_bEndOfPlay;			// 마지막 플래그
	BOOL			m_bRepeat;				// 반복 플래그(default: TRUE)

	cGrowableArray< c2DAnimationFrame* >	m_FrameList;

public:
	c2DAnimationFrames();
	~c2DAnimationFrames();

	// 애니메이션 크기를 지정한다.
	void SetAnimationSize( LONG cx, LONG cy );
	BOOL CreateFrameBuffer( int nBackgroundColorIndex = (-1) );
	void DeleteFrameBuffer();

	// 프레임을 추가한다.
	BOOL Add( TFrame *frame, D3DFORMAT nFormat = D3DFMT_UNKNOWN );

	// 현재 프레임을 재생한다.
	void Repeat( BOOL bRepeat = TRUE ) { m_bRepeat = bRepeat; }
	BOOL Draw( DWORD *lpBits );
	void Stop();

	void Discontinue() { m_bDiscontinued = TRUE; }
	void Continue() { m_bDiscontinued = FALSE; }
	BOOL IsEndOfPlay() { return m_bEndOfPlay; }
};

class c2DAnimation
{
private:
	BOOL					m_bIsLoaded;
	BOOL					m_bDiscontinued;
	UINT					m_Width;
	UINT					m_Height;
	LPDIRECT3DSURFACE9		m_D3DSurface;
	D3DFORMAT				m_nFormat;				// 색상 포맷(D3DFMT_A8R8G8B8)
	D3DCOLOR				m_ColorKey;

	cGrowableArray< c2DAnimationFrames* >	m_AnimationList;

public:
	c2DAnimation();
	~c2DAnimation();

	BOOL Init( LPDIRECT3DDEVICE9 pd3dDevice, UINT width, UINT height );
	BOOL Free();

	DWORD Create( LPCSTR lpwszFile );
	BOOL Play( DWORD Handle );
	BOOL Stop( DWORD Handle );
	BOOL Delete( DWORD Handle );

	BOOL Draw( LPDIRECT3DTEXTURE9 lpTexture );
	void Stop();
	void Repeat( BOOL bRepeat = TRUE );

	BOOL IsLoaded() { return m_bIsLoaded; }

	void Discontinue();
	void Continue();
	BOOL IsEndOfPlay();
};


#endif

