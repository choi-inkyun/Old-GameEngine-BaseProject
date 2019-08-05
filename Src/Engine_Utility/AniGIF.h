
#ifndef __ANIGIF_H__
#define __ANIGIF_H__

#include "2DAnimation.h"

class __declspec(dllexport) cAniGIF
{
private:
	cGraphics*	m_pGraphics;		// parent cGraphics

	long		m_NumAnimations;	// # of animations
	c2DAnimation* m_Animations;		// 2D animation array
	cTexture*	m_Textures;			// CTexture array
	long*		m_Widths;			// Tile widths array
	long*		m_Heights;			// Tile heights array

public:
	cAniGIF();
	~cAniGIF();

	// Functions to create and free the tile interface
	BOOL Create( cGraphics *Graphics, long NumAnimations );
	BOOL Free();

	// Functions to load and unload a single texture
	BOOL Load( long AnimationNum, LPCSTR Filename, long Width, long Height,
			D3DCOLOR Transparent = 0, D3DFORMAT Format = D3DFMT_A1R5G5B5 );
	BOOL Unload( long AnimationNum );

	// Functions to retrieve tile dimensions and # of tiles in a texture.
	long GetWidth( long AnimationNum );
	long GetHeight( long AnimationNum );
	long GetNum( long AnimationNum );

	// Enable or disable transparent blitting
	BOOL SetTransparent( BOOL Enabled = TRUE );

	// Draw a single tile to location
	BOOL Draw( long AnimationNum, long ScreenX, long ScreenY,
				D3DCOLOR Color = 0xFFFFFFFF, float XScale = 1.0f, float YScale = 1.0f );
	BOOL Stop( long AnimationNum );
	BOOL Repeat( long AnimationNum, BOOL bRepeat );
	BOOL IsEndOfPlay( long AnimationNum );
};

#endif
