
#ifndef __ENGINE_TILE_H__
#define __ENGINE_TILE_H__

#include "..\Engine_Library\D3DGraphics.h"
#include "..\Engine_Library\D3DTexture.h"

/*-----------------------------------------------------------------------------
 * cTile CLASS
 *---------------------------------------------------------------------------*/

class __declspec(dllexport) cTile
{
private:
	cGraphics*	m_Graphics;			// parent cGraphics

	long		m_NumTextures;		// # of textures
	cTexture*	m_Textures;			// CTexture array
	long*		m_Widths;			// Tile widths array
	long*		m_Heights;			// Tile heights array
	long*		m_Columns;			// # columns in texture

public:
	cTile();
	~cTile();

	// Functions to create and free the tile interface
	BOOL Create( cGraphics *Graphics, long NumTextures );
	BOOL Free();

	// Functions to load and unload a single texture
	BOOL Load( long TextureNum, LPCSTR Filename, 
			short TileWidth = 0, short TileHeight = 0,
			D3DCOLOR Transparent = 0, D3DFORMAT Format = D3DFMT_A1R5G5B5 );
	BOOL Unload( long TextureNum );

	// Functions to retrieve tile dimensions and # of tiles in a texture.
	long GetWidth( long TextureNum );
	long GetHeight( long TextureNum );
	long GetNum( long TextureNum );

	// Enable or disable transparent blitting
	BOOL SetTransparent( BOOL Enabled = TRUE );

	// Draw a single tile to location
	BOOL Draw( long TextureNum, long TileNum, long ScreenX, long ScreenY,
				D3DCOLOR Color = 0xFFFFFFFF, float XScale = 1.0f, float YScale = 1.0f );

};

#endif

