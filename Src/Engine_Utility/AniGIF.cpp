
#include "StdAfx.h"
#include "AniGIF.h"

/*-----------------------------------------------------------------------------
 * cAniGIF CLASS
 *---------------------------------------------------------------------------*/

cAniGIF::cAniGIF()
{
	m_pGraphics = NULL;
	m_NumAnimations = 0;
	m_Animations = NULL;
	m_Textures = NULL;
	m_Widths = m_Heights = NULL;
}

cAniGIF::~cAniGIF()
{
	Free();
}

BOOL cAniGIF::Create( cGraphics *pGraphics, long NumAnimations )
{
	// Free in case of existing data
	Free();

	// error checking
	if( (m_pGraphics = pGraphics) == NULL ) return FALSE;
	if( (m_NumAnimations = NumAnimations) == 0 ) return FALSE;

	// Allocate 2D animation objects
	if( (m_Animations = new c2DAnimation[m_NumAnimations]) == NULL ) return FALSE;

	// Allocate texture objects
	if( (m_Textures = new cTexture[m_NumAnimations]) == NULL ) return FALSE;

	// Allocate width, height, and column count arrays
	m_Widths	= new long[m_NumAnimations];
	m_Heights	= new long[m_NumAnimations];

	return TRUE;
}

BOOL cAniGIF::Free()
{
	m_pGraphics = NULL;

	if( m_NumAnimations )
	{
		// Free all animations and textures
		for( int i = 0; i < m_NumAnimations; i++ ) m_Animations[i].Free();
		for( int i = 0; i < m_NumAnimations; i++ ) m_Textures[i].Unload();
	}
	delete[] m_Animations;
	delete[] m_Textures;
	m_Animations = NULL;
	m_Textures = NULL;

	delete[] m_Widths;
	delete[] m_Heights;

	m_Widths = m_Heights = NULL;
	m_NumAnimations = 0;

	return TRUE;
}

BOOL cAniGIF::Load( long AnimationNum, LPCSTR Filename, long Width, long Height, D3DCOLOR Transparent, D3DFORMAT Format )
{
	// error checking
	if( AnimationNum >= m_NumAnimations || m_Textures == NULL ) return FALSE;

	Unload( AnimationNum );

	// Create the texture
	if( m_Textures[AnimationNum].Create( m_pGraphics, Width, Height, Format ) == FALSE ) return FALSE;

	// Store both width and height values
	m_Widths[AnimationNum] = Width;
	m_Heights[AnimationNum] = Height;

	// Create the animation
	m_Animations[AnimationNum].Init( m_pGraphics->GetDeviceCOM(), Width, Height );
	m_Animations[AnimationNum].Create( Filename);

	return TRUE;
}

BOOL cAniGIF::Unload( long AnimationNum )
{
	// error checking
	if( AnimationNum >= m_NumAnimations || m_Animations == NULL || m_Textures == NULL ) return FALSE;

	// Free a 2DAnimation resource
	m_Animations[AnimationNum].Free();

	// Free a single texture resource
	m_Textures[AnimationNum].Unload();

	return TRUE;
}

long cAniGIF::GetWidth( long AnimationNum )
{
	// error checking
	if( AnimationNum >= m_NumAnimations || m_Widths == NULL ) return 0;
	return m_Widths[AnimationNum];
}

long cAniGIF::GetHeight( long AnimationNum )
{
	// error checking
	if( AnimationNum >= m_NumAnimations || m_Heights == NULL ) return 0;
	return m_Heights[AnimationNum];
}

BOOL cAniGIF::SetTransparent( BOOL Enabled )
{
	// error checking
	if( m_pGraphics == NULL ) return FALSE;

	return m_pGraphics->EnableAlphaTesting( Enabled );
}

BOOL cAniGIF::Draw( long AnimationNum, long ScreenX, long ScreenY, D3DCOLOR Color, float XScale, float YScale )
{
	// Error checking
	if( m_pGraphics == NULL ) return FALSE;
	if( AnimationNum >= m_NumAnimations || m_Textures == NULL ) return FALSE;

	if( m_Animations[AnimationNum].IsLoaded() )
	{
		m_Animations[AnimationNum].Draw( m_Textures[AnimationNum].GetTextureCOM() );
		m_Textures[AnimationNum].Blit( ScreenX, ScreenY );
	}

	return TRUE;
}

BOOL cAniGIF::Stop( long AnimationNum )
{
	// Error checking
	if( m_pGraphics == NULL ) return FALSE;
	if( AnimationNum >= m_NumAnimations || m_Textures == NULL ) return FALSE;

	if( m_Animations[AnimationNum].IsLoaded() ) m_Animations[AnimationNum].Stop();
	return TRUE;
}

BOOL cAniGIF::Repeat( long AnimationNum, BOOL bRepeat )
{
	// Error checking
	if( m_pGraphics == NULL ) return FALSE;
	if( AnimationNum >= m_NumAnimations || m_Textures == NULL ) return FALSE;

	m_Animations[AnimationNum].Repeat( bRepeat );
	return TRUE;
}

BOOL cAniGIF::IsEndOfPlay( long AnimationNum )
{
	if( AnimationNum >= m_NumAnimations ) return FALSE;

	return m_Animations[AnimationNum].IsEndOfPlay();
}

