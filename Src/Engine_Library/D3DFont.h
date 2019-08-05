
#ifndef __ENGINE_D3DFONT_H__
#define __ENGINE_D3DFONT_H__

#include "D3DGraphics.h"

/*-----------------------------------------------------------------------------
 * cFont CLASS
 *---------------------------------------------------------------------------*/

class __declspec(dllexport) cFont
{
private:
	ID3DXFont		*m_Font;
	cGraphics		*m_pGraphics;

public:
	cFont();
	~cFont();

	ID3DXFont		*GetFontCOM();

	BOOL Create( cGraphics *Graphics, LPCSTR Name, long Size = 16, 
			BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE );
	BOOL Free();

	BOOL Begin();
	BOOL End();
	BOOL Print( LPCSTR Text, long XPos, long YPos, long Width = 0, long Height = 0, 
			D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0 );
};

#endif

