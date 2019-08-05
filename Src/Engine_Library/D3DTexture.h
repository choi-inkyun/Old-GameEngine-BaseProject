
#ifndef __ENGINE_D3DTEXTURE_H__
#define __ENGINE_D3DTEXTURE_H__

#include "D3DGraphics.h"

/*-----------------------------------------------------------------------------
 * cTexture CLASS
 *---------------------------------------------------------------------------*/

class __declspec(dllexport) cTexture
{
protected:
	cGraphics*			m_Graphics;				// Parent CGraphics
	IDirect3DTexture9*	m_Texture;				// Texture COM
	unsigned long		m_Width, m_Height;		// Dimensions of texture image.

public:
	cTexture();
	~cTexture();

	IDirect3DTexture9*	GetTextureCOM();		// Return texture COM

	// Load a texture from file
	BOOL Load( cGraphics *Graphics, LPCSTR Filename, DWORD Transparent = 0, D3DFORMAT Format = D3DFMT_UNKNOWN );
	BOOL Create( cGraphics *Graphics, IDirect3DTexture9 *Texture );
	BOOL Create( cGraphics *Graphics, unsigned long width, unsigned long height, D3DFORMAT Format = D3DFMT_UNKNOWN );

	BOOL Unload();
	BOOL IsLoaded();

	long		GetWidthFromDesc();
	long		GetHeightFromDesc();
	D3DFORMAT	GetFormatFromDesc();

	long		GetWidth();
	long		GetHeight();

	BOOL Blit( long DestX, long DestY,
			long SrcX = 0, long SrcY = 0,
			long Width = 0, long Height = 0,
			float XScale = 1.0f, float YScale = 1.0f,
			D3DCOLOR Color = 0xFFFFFFFF );
};

#endif

