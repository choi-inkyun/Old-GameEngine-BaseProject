
#include "StdAfx.h"
#include "Tile.h"

/*-----------------------------------------------------------------------------
 * cTile CLASS
 *---------------------------------------------------------------------------*/

cTile::cTile()
{
	m_Graphics = NULL;
	m_NumTextures = 0;
	m_Textures = NULL;
	m_Widths = m_Heights = m_Columns = 0;
}

cTile::~cTile()
{
	Free();
}

BOOL cTile::Create( cGraphics *Graphics, long NumTextures )
{
	// Free in case of existing data
	Free();

	// error checking
	if( (m_Graphics = Graphics) == NULL ) return FALSE;
	if( (m_NumTextures = NumTextures) == 0 ) return FALSE;

	// Allocate texture objects
	if( (m_Textures = new cTexture[m_NumTextures]) == NULL ) return FALSE;

	// Allocate width, height, and column count arrays
	m_Widths	= new long[m_NumTextures];
	m_Heights	= new long[m_NumTextures];
	m_Columns	= new long[m_NumTextures];

	return TRUE;
}

BOOL cTile::Free()
{
	m_Graphics = NULL;

	if( m_NumTextures )
	{
		// Free all textures
		for( int i = 0; i < m_NumTextures; i++ ) m_Textures[i].Unload();
	}
	delete[] m_Textures;
	m_Textures = NULL;

	delete[] m_Widths;
	delete[] m_Heights;
	delete[] m_Columns;

	m_Widths = m_Heights = m_Columns = NULL;
	m_NumTextures = 0;

	return TRUE;
}

BOOL cTile::Load( long TextureNum, LPCSTR Filename, short TileWidth, short TileHeight, D3DCOLOR Transparent, D3DFORMAT Format )
{
	// error checking
	if( TextureNum >= m_NumTextures || m_Textures == NULL ) return FALSE;

	Unload( TextureNum );


	// Load the texture
	if( m_Textures[TextureNum].Load( m_Graphics, Filename, Transparent, Format ) == FALSE ) return FALSE;

	// Store width value (get width of texture if no TileWidth was specified).
	if( !TileWidth ) m_Widths[TextureNum] = m_Textures[TextureNum].GetWidth();
	else             m_Widths[TextureNum] = TileWidth;

	// Store height value (get height of texture if no TileHeight was specified).
	if( !TileHeight ) m_Heights[TextureNum] = m_Textures[TextureNum].GetHeight();
	else              m_Heights[TextureNum] = TileHeight;

	// Calculate how many columns of tiles there are in the texture.
	// This is used to speed up calculations when drawing titles.
	m_Columns[TextureNum] = m_Textures[TextureNum].GetWidth() / m_Widths[TextureNum];

	return TRUE;
}

BOOL cTile::Unload( long TextureNum )
{
	// error checking
	if( TextureNum >= m_NumTextures || m_Textures == NULL ) return FALSE;

	// Free a single texture resource
	m_Textures[TextureNum].Unload();

	return TRUE;
}

long cTile::GetWidth( long TextureNum )
{
	// error checking
	if( TextureNum >= m_NumTextures || m_Widths == NULL ) return 0;
	return m_Widths[TextureNum];
}

long cTile::GetHeight( long TextureNum )
{
	// error checking
	if( TextureNum >= m_NumTextures || m_Heights == NULL ) return 0;
	return m_Heights[TextureNum];
}

long cTile::GetNum( long TextureNum )
{
	// error checking
	if( TextureNum >= m_NumTextures || 
		m_Textures == NULL ||
		m_Columns == NULL ||
		m_Widths == NULL ||
		m_Heights == NULL ) return 0;

	return m_Columns[TextureNum] + m_Textures[TextureNum].GetHeight() / m_Heights[TextureNum];
}

BOOL cTile::SetTransparent( BOOL Enabled )
{
	// error checking
	if( m_Graphics == NULL ) return FALSE;

	return m_Graphics->EnableAlphaTesting( Enabled );
}

BOOL cTile::Draw( long TextureNum, long TileNum, long ScreenX, long ScreenY,
				  D3DCOLOR Color, float XScale, float YScale )
{
	long SrcX, SrcY;

	// Error checking
	if( m_Graphics == NULL ) return FALSE;
	if( TextureNum >= m_NumTextures || m_Textures == NULL ) return FALSE;

	// calculate the source tile coordinates from texture
	SrcX = ( TileNum % m_Columns[TextureNum] ) * m_Widths[TextureNum];
	SrcY = ( TileNum / m_Columns[TextureNum] ) * m_Heights[TextureNum];

	return m_Textures[TextureNum].Blit( ScreenX, ScreenY, SrcX, SrcY,
				m_Widths[TextureNum], m_Heights[TextureNum],
				XScale, YScale, Color );
}
