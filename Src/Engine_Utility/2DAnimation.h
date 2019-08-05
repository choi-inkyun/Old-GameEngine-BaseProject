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
	SIZE			m_frameSize;			// ������ ũ��
	UINT			m_nDelay;				// delay (in 1/100s of a second)
	D3DFORMAT		m_nFormat;				// ���� ����(D3DFMT_A8R8G8B8)
	DWORD*			m_pBits;				// �̹��� ������

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
	UINT			m_nHandle;				// �ִϸ��̼� �ĺ���
	SIZE			m_AnimationSize;		// �ִϸ��̼� ũ��
	UINT			m_nLevel;				// ��� ����
	BOOL			m_bIsPlaying;			// ���� ����� �÷���
	UINT			m_nCurrFrame;			// ���� ������
	DWORD			m_dwLastFrame;			// �ֱ� ������ ��� �ð�
	LPBYTE			m_FrameBuffer;			// ������ �̹��� ������ ����
	BOOL			m_bDiscontinued;		// �ߴ� �÷���
	BOOL			m_bEndOfPlay;			// ������ �÷���
	BOOL			m_bRepeat;				// �ݺ� �÷���(default: TRUE)

	cGrowableArray< c2DAnimationFrame* >	m_FrameList;

public:
	c2DAnimationFrames();
	~c2DAnimationFrames();

	// �ִϸ��̼� ũ�⸦ �����Ѵ�.
	void SetAnimationSize( LONG cx, LONG cy );
	BOOL CreateFrameBuffer( int nBackgroundColorIndex = (-1) );
	void DeleteFrameBuffer();

	// �������� �߰��Ѵ�.
	BOOL Add( TFrame *frame, D3DFORMAT nFormat = D3DFMT_UNKNOWN );

	// ���� �������� ����Ѵ�.
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
	D3DFORMAT				m_nFormat;				// ���� ����(D3DFMT_A8R8G8B8)
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

