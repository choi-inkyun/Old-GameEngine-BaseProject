
#include "StdAfx.h"
#include "D3DFont.h"

/*-----------------------------------------------------------------------------
 * cFont CLASS
 *---------------------------------------------------------------------------*/

cFont::cFont()
{
	m_Font = NULL;
}

cFont::~cFont()
{
	Free();
}

ID3DXFont *cFont::GetFontCOM()
{
	return m_Font;
}

BOOL cFont::Create( cGraphics *Graphics, LPCSTR Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout )
{
	if( ( m_pGraphics = Graphics ) == NULL || Name == NULL ) return FALSE;
	if( Graphics->GetDeviceCOM() == NULL ) return FALSE;

#ifdef _OLD_CODES_
	LOGFONT lf;
	
	// Clear out the font structure
	ZeroMemory( &lf, sizeof( LOGFONT ) );

	// Set the font name and height
	strcpy( lf.lfFaceName, Name );
	lf.lfHeight = -Size;
	lf.lfWeight = ( Bold == TRUE ) ? 700 : 0;
	lf.lfItalic = Italic;
	lf.lfUnderline = Underline;
	lf.lfStrikeOut = Strikeout;

	// Create the font object
	if( FAILED( D3DXCreateFontIndirect( Graphics->GetDeviceCOM(), &lf, &m_Font ) ) ) return FALSE;
#else
	D3DXFONT_DESC Desc;

	// Clear out the font desc
	ZeroMemory( &Desc, sizeof( D3DXFONT_DESC ) );

	// Set the font name and height

	Desc.Height				= -Size;
	Desc.Width				= 0;
	Desc.Weight				= ( Bold == TRUE ) ? FW_BOLD : 0;
	Desc.MipLevels			= 0;
	Desc.Italic				= Italic;
	Desc.CharSet			= DEFAULT_CHARSET;
	Desc.OutputPrecision	= OUT_DEFAULT_PRECIS;
	Desc.Quality			= DEFAULT_QUALITY;
	Desc.PitchAndFamily		= DEFAULT_PITCH | FF_DONTCARE;
	strcpy(Desc.FaceName, Name);
//				= Name;

	if( FAILED( D3DXCreateFontIndirect( Graphics->GetDeviceCOM(), &Desc, &m_Font ) ) ) return FALSE;
#endif

	return TRUE;
}

BOOL cFont::Free()
{
	ReleaseCOM( m_Font );
	return TRUE;
}

BOOL cFont::Begin()
{
#ifdef _OLD_CODES_
	if( m_Font == NULL ) return FALSE;
	if( FAILED( m_Font->Begin() ) ) return FALSE;
#else
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D( &mat, NULL,	0.0f, NULL, NULL, 0.0f, NULL );
	m_pGraphics->GetSpriteCOM()->SetTransform( &mat );
#endif
	return TRUE;
}

BOOL cFont::End()
{
#ifdef _OLD_CODES_
	if( m_Font == NULL ) return FALSE;
	if( FAILED( m_Font->End() ) ) return FALSE;
#endif
	return TRUE;
}
// D3DCOLOR = r,g,b, ¾ËÆÄ°ª
BOOL cFont::Print( LPCSTR Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format )
{
	RECT Rect;

	if( m_Font == NULL ) return FALSE;

	if( !Width  ) Width  = 65535;
	if( !Height ) Height = 65535;

	Rect.left		= XPos;
	Rect.top		= YPos;
	Rect.right		= Rect.left + Width;
	Rect.bottom		= Rect.top + Height;

#ifdef _OLD_CODES_
	if( FAILED( m_Font->DrawText( Text, -1, &Rect, Format, Color ) ) ) return FALSE;
#else
	if( FAILED( m_Font->DrawText( m_pGraphics->GetSpriteCOM(), Text, -1, &Rect, Format, Color ) ) ) return FALSE;
#endif

	return TRUE;
}

