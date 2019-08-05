
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIEditBox.h"

//--------------------------------------------------------------------------------------
// Helper class that help us automatically initialize and uninitialize external API.
// Important: C++ does not guaranteed the order global and static objects are 
//			  initialized in. Therefor, do not use edit controls inside a constructor.
//--------------------------------------------------------------------------------------
// dingpong 3.15 주석처리. dll 로 sington 붙이고 난 후에 초기화 할때 이게 불린다. 왜 불리는지 모르겠다
// 초기화가 안되서 메모리 에러가 나므로 주석처리
class cExternalApiInitializer
{
public:
	cExternalApiInitializer()
	{
//		cUIEditBox::cUniBuffer::InitializeUniscribe();
		// FIXME:	cUIIMEEditBox::InitializeImm();
	}
	~cExternalApiInitializer()
	{
//		cUIEditBox::cUniBuffer::UninitializeUniscribe();
		// FIXME:	cUIIMEEditBox::UninitializeImm();
	}
} EXTERNAL_API_INITIALIZER;

// Static member initialization
HINSTANCE cUIEditBox::cUniBuffer::s_hDll = NULL;
HRESULT (WINAPI *cUIEditBox::cUniBuffer::_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, 
		 SCRIPT_CONTROL*, SCRIPT_STATE* ) = Dummy_ScriptApplyDigitSubstitution;
HRESULT (WINAPI *cUIEditBox::cUniBuffer::_ScriptStringAnalyse)( HDC, const void*, int, int, int, DWORD, int, 
		 SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) = 
		 Dummy_ScriptStringAnalyse;
HRESULT (WINAPI *cUIEditBox::cUniBuffer::_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) =
		 Dummy_ScriptStringCPtoX;
HRESULT (WINAPI *cUIEditBox::cUniBuffer::_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* ) =
		 Dummy_ScriptStringXtoCP;
HRESULT (WINAPI *cUIEditBox::cUniBuffer::_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* ) =
		 Dummy_ScriptStringFree;
const SCRIPT_LOGATTR* (WINAPI *cUIEditBox::cUniBuffer::_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS ) =
	     Dummy_ScriptString_pLogAttr;
const int* (WINAPI *cUIEditBox::cUniBuffer::_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS ) =
		 Dummy_ScriptString_pcOutChars;
BOOL cUIEditBox::s_bHideCaret;		// If true, we don't render the caret.


//--------------------------------------------------------------------------------------
// cUIEditBox class
//--------------------------------------------------------------------------------------

// When scrolling , EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
// If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
#define EDITBOX_SCROLLEXTENT	4

cUIEditBox::cUIEditBox( cUIDialog *pDialog )
{
	m_Type			= UI_CONTROL_EDITBOX;
	m_pDialog		= pDialog;

	m_nBorder		= 0;	// Default border width
	m_nSpacing		= 0;	// Default spacing

	m_bCaretOn		= TRUE;
	m_dfBlink		= GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink	= g_cUIDialogManager.GetTime();
	s_bHideCaret	= FALSE;
	m_nFirstVisible = 0;
	m_TextColor		= D3DCOLOR_ARGB( 255,   0,   0,   0 );
	m_SelTextColor	= D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor	= D3DCOLOR_ARGB( 255, 128, 128, 128 );
	m_CaretColor	= D3DCOLOR_ARGB( 255,   0,   0,   0 );
	m_nCaret		= 0;
	m_nSelStart		= 0;
	m_bInsertMode	= TRUE;

	m_bMouseDrag	= FALSE;
}

cUIEditBox::~cUIEditBox()
{
	// do nothing.
}

void cUIEditBox::UpdateRects()
{
	cUIControl::UpdateRects();

	// Update the text rectangle
	m_rcText = m_rcBoundingBox;

	// First inflate by m_nBorder to compute render rects.
	InflateRect( &m_rcText, -m_nBorder, -m_nBorder );

	// Update the render rectangles
	m_rcRender[0] = m_rcText;
	SetRect( &m_rcRender[1], m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top );
	SetRect( &m_rcRender[2], m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top );
	SetRect( &m_rcRender[3], m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top );
	SetRect( &m_rcRender[4], m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom );
	SetRect( &m_rcRender[5], m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom );
	SetRect( &m_rcRender[6], m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[8], m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom );

	// Inflate further by m_nSpacing
	InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );
}

// Set the caret to a character position, and adjust the scrolling if necessary.
void cUIEditBox::PlaceCaret( int nCP )
{
	ASSERT( nCP >= 0 && nCP <= m_Buffer.GetTextSize() );
	m_nCaret = nCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2, nXOffset;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );	// 1st visible char
	nXOffset = nX1st;
	m_Buffer.CPtoX( nCP, FALSE, &nX );					// LEAD
	// If nCP is the NULL terminator, get the leading edge instead of trailing.
	if( nCP == m_Buffer.GetTextSize() ) nX2 = nX;
	else m_Buffer.CPtoX( nCP, TRUE, &nX2 );				// TRAIL

	// If the left edge of the char is smaller than the left edge of the 1st visible char,
	// we need to scroll left until this char is visible.
	if( nX < nX1st )
	{
		// Simply make the first visible chacter the char at the new caret position.
		m_nFirstVisible = nCP;
	}
	else
	// If the right edge of the char is bigger than the offset of the control's
	// right edge, we need to scroll right to this character.
	if( nX2 > nX1st + RectWidth( m_rcText ) )
	{
		// Compute the X of the new left-most pixel.
		int nXNewLeft = nX2 - RectWidth( m_rcText );

		// Compute the char position of this character.
		int nCPNew1st, nNewTrail;
		m_Buffer.XtoCP( nXNewLeft, &nCPNew1st, &nNewTrail );

		// If this coordinate is not on a character border, start from the next character
		// so that the caret position does not fall outside the text rectangle.
		int nXNew1st;
		m_Buffer.CPtoX( nCPNew1st, FALSE, &nXNew1st );
		if( nXNew1st < nXNewLeft ) nCPNew1st++;

		m_nFirstVisible = nCPNew1st;
	}
}

// NOTE: m_FirstVisible이 0보다 큰 경우(즉, 긴 문자 입력으로 우측으로 스크롤된 경우), 
//		 m_Buffer.Clear()를 호출하고 PlaceCaret( 0 )를 실행시키면 메모리 침입 오류가
//		 발생한다. 그래서 Caret의 위치를 처음으로 옮기고 버퍼를 지우도록 라인 순서를
//		 변경하니 오류 발생은 없는듯하다.
void cUIEditBox::ClearText()
{
	PlaceCaret( 0 );
	m_Buffer.Clear();
	m_nSelStart = 0;
}

// NOTE: 위와 같은 문제가 예상되어 먼저 안전하게 Caret의 위치를 처음으로 옮기고 버퍼 
//		 을 변경한 다음 Caret를 문자열의 마지막으로 이동하게 수정하였다. 
void cUIEditBox::SetText( LPCSTR wszText, BOOL bSelected )
{
	ASSERT( wszText != NULL );

	PlaceCaret( 0 );
	m_Buffer.SetText( wszText );
	// Move the caret to the end of the text
	PlaceCaret( m_Buffer.GetTextSize() );
	m_nSelStart = bSelected ? 0 : m_nCaret;
}

HRESULT cUIEditBox::GetTextCopy( LPSTR strDest, UINT bufferCount )
{
	ASSERT( strDest );

	strncpy( strDest, m_Buffer.GetBuffer(), bufferCount );
	*(strDest + bufferCount - 1 ) = L'\0';

	return S_OK;
}

void cUIEditBox::DeleteSelectionText()
{
	int nFirst = min( m_nCaret, m_nSelStart );
	int nLast  = max( m_nCaret, m_nSelStart );

	// update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;

	// remove the characters
	for( int i = nFirst; i < nLast; i++ ) m_Buffer.RemoveChar( nFirst );
}

void cUIEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard.
	if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * ( m_Buffer.GetTextSize() + 1 ) );
		if( hBlock )
		{
			char *pwszText = (char*)GlobalLock( hBlock );
			if( pwszText )
			{
				int nFirst = min( m_nCaret, m_nSelStart );
				int nLast  = max( m_nCaret, m_nSelStart );
				if( nLast - nFirst > 0 )
					CopyMemory( pwszText, m_Buffer.GetBuffer() + nFirst, (nLast - nFirst ) * sizeof(char) );
				pwszText[nLast - nFirst] = L'\0';	// Terminate it
				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_UNICODETEXT, hBlock );
		}

		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock ) GlobalFree( hBlock );
	}
}

void cUIEditBox::PasteFromClipboard()
{
	DeleteSelectionText();

	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then insert to our buffer.
			char *pwszText = (char*)GlobalLock( handle );
			if( pwszText )
			{
				// Copy all characters up to null.
				if( m_Buffer.InsertString( m_nCaret, pwszText ) )
					PlaceCaret( m_nCaret + strlen( pwszText ) );
				m_nSelStart = m_nCaret;
				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
	}
}

//--------------------------------------------------------------------------------------

BOOL cUIEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	BOOL bHandled = FALSE;

	switch( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_HOME:
					PlaceCaret( 0 );
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						// Shift is not down. Update selection start along with the caret.
						m_nSelStart = m_nCaret;
					}
					ResetCaretBlink();
					bHandled = TRUE;
					break;

				case VK_END:
					PlaceCaret( m_Buffer.GetTextSize() );
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						// Shift is not down. Update selection start along with the caret.
						m_nSelStart = m_nCaret;
					}
					ResetCaretBlink();
					bHandled = TRUE;
					break;

				case VK_INSERT:
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control Insert. Copy to clipboard
						CopyToClipboard();
					} else if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Shift Insert. Paste from clipboard
						PasteFromClipboard();
					} else
					{
						// Toggle caret insert mode
						m_bInsertMode = !m_bInsertMode;
					}
					break;

				case VK_DELETE:
					// Check if there is a text selection.
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					}
					else
					{
						// Delete one character
						if( m_Buffer.RemoveChar( m_nCaret ) )
							m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					}
					ResetCaretBlink();
					bHandled = TRUE;
					break;

				case VK_LEFT:
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control is down. Move the caret to a new item instead of a character.
						m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
						PlaceCaret( m_nCaret );
					}
					else if( m_nCaret > 0 )
					{
						PlaceCaret( m_nCaret - 1 );
					}

					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						// Shift is not down. Update selection start along with the caret.
						m_nSelStart = m_nCaret;
					}
					ResetCaretBlink();
					bHandled = TRUE;
					break;

				case VK_RIGHT:
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control is down. Move the caret to a new item instead of a character.
						m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
						PlaceCaret( m_nCaret );
					}
					else if( m_nCaret < m_Buffer.GetTextSize() )
					{
						PlaceCaret( m_nCaret + 1 );
					}

					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						// Shift is not down. Update selection start along with the caret.
						m_nSelStart = m_nCaret;
					}
					ResetCaretBlink();
					bHandled = TRUE;					
					break;

				case VK_UP:
				case VK_DOWN:
					// Trap up and down arrows so that the dialog does not switch focus to 
					// another control.
					bHandled = TRUE;
					break;

				default:
					bHandled = ( wParam != VK_ESCAPE );		// Let the application handle ESC.
			}
		}
		break;
	}

	return bHandled;
}

BOOL cUIEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if( !m_bHasFocus ) m_pDialog->RequestFocus( this );
			if( !ContainsPoint( pt ) ) return FALSE;

			m_bMouseDrag = TRUE;
			SetCapture( g_cUIDialogManager.GetHWND() );
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );	// X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() ) PlaceCaret( nCP + 1 );
				else PlaceCaret( nCP );
				m_nSelStart = m_nCaret;
				ResetCaretBlink();
			}
			return TRUE;
		}

		case WM_LBUTTONUP:
			ReleaseCapture();
			m_bMouseDrag = FALSE;
			break;

		case WM_MOUSEMOVE:
		{
			if( m_bMouseDrag )
			{
				// Determine the character corresponding to the coordinates.
				int nCP, nTrail, nX1st;
				m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );	// X offset of the 1st visible char
				if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
				{
					// Cap at the NULL character.
					if( nTrail && nCP < m_Buffer.GetTextSize() ) PlaceCaret( nCP + 1 );
					else PlaceCaret( nCP );
				}
			}
			break;
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------

BOOL cUIEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	switch( uMsg )
	{
		case WM_CHAR:
		{
			switch( (char)wParam )
			{
				case VK_BACK:	// Backspace
				{
					// If there's a selection, treat this like a delete key.
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					}
					else if( m_nCaret > 0 )
					{
						// Move the caret, then delete the char.
						PlaceCaret( m_nCaret - 1 );
						m_nSelStart = m_nCaret;
						m_Buffer.RemoveChar( m_nCaret );
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					}
					ResetCaretBlink();
					break;
				}

				case 24:		// Ctrl-X Cut
				case VK_CANCEL:	// Ctrl-C Copy
				{
					CopyToClipboard();

					// If the key is Ctrl-X, delete the selection too.
					if( (char)wParam == 24 )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					}
					break;
				}

				case 22:		// Ctrl-V Paste
				{
					PasteFromClipboard();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
					break;
				}

				case 1:			// Ctrl-A Select All
				{
					if( m_nSelStart == m_nCaret )
					{
						m_nSelStart = 0;
						PlaceCaret( m_Buffer.GetTextSize() );
					}
					break;
				}

				case VK_RETURN:
					// Invoke the callback when the user presses Enter.
					m_pDialog->SendEvent( EVENT_EDITBOX_STRING, TRUE, this );
					break;

				// Junk characters we don't want in the string
                case 26:  // Ctrl Z
                case 2:   // Ctrl B
                case 14:  // Ctrl N
                case 19:  // Ctrl S
                case 4:   // Ctrl D
                case 6:   // Ctrl F
                case 7:   // Ctrl G
                case 10:  // Ctrl J
                case 11:  // Ctrl K
                case 12:  // Ctrl L
                case 17:  // Ctrl Q
                case 23:  // Ctrl W
                case 5:   // Ctrl E
                case 18:  // Ctrl R
                case 20:  // Ctrl T
                case 25:  // Ctrl Y
                case 21:  // Ctrl U
                case 9:   // Ctrl I
                case 15:  // Ctrl O
                case 16:  // Ctrl P
                case 27:  // Ctrl [
                case 29:  // Ctrl ]
                case 28:  // Ctrl \ 
					break;

				default:
				{
					// If there's a selection and the user starts to type,
					// the selection should be deleted.
					if( m_nCaret != m_nSelStart ) DeleteSelectionText();

					// If we are in overwrite mode and there is alreadya char
					// at the caret's position, simply replace it. Otherwise,
					// we insert the char as normal.
					if( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize() )
					{
						m_Buffer[m_nCaret] = (char)wParam;
						PlaceCaret( m_nCaret + 1 );
						m_nSelStart = m_nCaret;
					}
					else
					{
						// Insert the char
						if( m_Buffer.InsertChar( m_nCaret, (char)wParam ) )
						{
							PlaceCaret( m_nCaret + 1 );
							m_nSelStart = m_nCaret;
						}
					}
					ResetCaretBlink();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, TRUE, this );
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

void cUIEditBox::Render( IDirect3DDevice9* pd3dDevice )
{
	if( !m_bVisible ) return;

	HRESULT hr;
	int nSelStartX = 0, nCaretX = 0;	// Left and right X cordinates of the selection region.

	cUIElement* pElement = m_pDialog->GetResourceElement( m_IDE, UI_STATE_NORMAL );

	if( pElement )
	{
		m_Buffer.SetFontIndex( m_pDialog->GetFontIndex( pElement->iFont ) );

		// Call PlaceCaret now that we have the DC, so that scrolling can be handled.
		PlaceCaret( m_nCaret );
	}

	// Compute the X coordinates of the first visible character.
	int nXFirst;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	// Compute the X coordinates of the selection rectangle
	hr = m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
	if( m_nCaret != m_nSelStart )
		hr = m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
	else
		nSelStartX = nCaretX;

	// Render the selection rectangle
	RECT rcSelection;	// make this available for rendering selected text
	if( m_nCaret != m_nSelStart )
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left i bigger then right.
		if( nSelLeftX > nSelRightX )
		{
			int nTemp = nSelLeftX;
			nSelLeftX = nSelRightX;
			nSelRightX = nTemp;
		}

		SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
		OffsetRect( &rcSelection, m_rcText.left - nXFirst, 0 );
		IntersectRect( &rcSelection, &m_rcText, &rcSelection );
		m_pDialog->DrawRect( &rcSelection, m_SelBkColor );
	}

	//-------------------------------------------------------------------------
	// Render the text
	//-------------------------------------------------------------------------

	// Element 0 for text
	pElement->FontColor = m_TextColor;
	m_pDialog->DrawText( m_Buffer.GetBuffer() + m_nFirstVisible, pElement, &m_rcText );


	// Render the selected text
	if( m_nCaret != m_nSelStart )
	{
		int nFirstToRender   = max( m_nFirstVisible, min( m_nSelStart, m_nCaret ) );
		int nNumCharToRender = max( m_nSelStart, m_nCaret ) - nFirstToRender;
		pElement->FontColor  = m_SelTextColor;
		m_pDialog->DrawText( m_Buffer.GetBuffer() + nFirstToRender,
							 pElement, &rcSelection, nNumCharToRender );
	}

	// Blink the caret
	if( g_cUIDialogManager.GetTime() - m_dfLastBlink >= m_dfBlink )
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = g_cUIDialogManager.GetTime();
	}

	// Render the caret if this control has the focus.
	if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
						 m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

		// If we are in overwrite mode, adjust the caret rectangle to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character.
			int nRightEdgeX;
			m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}

		m_pDialog->DrawRect( &rcCaret, m_CaretColor );
	}
}

// MORE

void cUIEditBox::OnFocusIn()
{
	cUIControl::OnFocusIn();
	ResetCaretBlink();
}

void cUIEditBox::ResetCaretBlink()
{
	m_bCaretOn = TRUE;
	m_dfLastBlink = g_cUIDialogManager.GetTime();
}


//--------------------------------------------------------------------------------------
// cUIEditBox::cUniBuffer class
//--------------------------------------------------------------------------------------

void cUIEditBox::cUniBuffer::InitializeUniscribe()
{
	LPSTR wszPath;
	if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) ) return;
	strcat( wszPath, LPCSTR(UNISCRIBE_DLLNAME) );
	s_hDll = LoadLibrary( wszPath );
	if( s_hDll )
	{
		FARPROC Temp;

		GETPROCADDRESS( s_hDll, ScriptApplyDigitSubstitution, Temp );
		GETPROCADDRESS( s_hDll, ScriptStringAnalyse, Temp );
		GETPROCADDRESS( s_hDll, ScriptStringCPtoX, Temp );
		GETPROCADDRESS( s_hDll, ScriptStringXtoCP, Temp );
		GETPROCADDRESS( s_hDll, ScriptStringFree, Temp );
		GETPROCADDRESS( s_hDll, ScriptString_pLogAttr, Temp );
		GETPROCADDRESS( s_hDll, ScriptString_pcOutChars, Temp );
	}
}

void cUIEditBox::cUniBuffer::UninitializeUniscribe()
{
	if( s_hDll )
	{
		PLACEHOLDERPROC( ScriptApplyDigitSubstitution );
		PLACEHOLDERPROC( ScriptStringAnalyse );
		PLACEHOLDERPROC( ScriptStringCPtoX );
		PLACEHOLDERPROC( ScriptStringXtoCP );
		PLACEHOLDERPROC( ScriptStringFree );
		PLACEHOLDERPROC( ScriptString_pLogAttr );
		PLACEHOLDERPROC( ScriptString_pcOutChars );

		FreeLibrary( s_hDll );
		s_hDll = NULL;
	}
}

cUIEditBox::cUniBuffer::cUniBuffer( int nInitialSize )
{
	m_pwszBuffer		= new char[nInitialSize];
	*m_pwszBuffer		= L'\0';
	m_nBufferSize		= nInitialSize;
	m_nTextSize			= 0;
	m_bAnalyseRequired	= TRUE;
	m_Analysis			= NULL;
	m_iFont				= 0;
}

cUIEditBox::cUniBuffer::~cUniBuffer()
{
	delete [] m_pwszBuffer;
	if( m_Analysis ) _ScriptStringFree( &m_Analysis );
}

BOOL cUIEditBox::cUniBuffer::Grow( int nNewSize )
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if( m_nBufferSize == UI_MAX_EDITBOXLENGTH ) return FALSE;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize * 2 : 256 ) : nNewSize * 2;

	// Cap the buffer size at the maximum allowd.
	if( nAllocateSize > UI_MAX_EDITBOXLENGTH ) nAllocateSize = UI_MAX_EDITBOXLENGTH;

	char *pTempBuffer = new char[nAllocateSize];
	if( !pTempBuffer ) return FALSE;
	if( m_pwszBuffer ) CopyMemory( pTempBuffer, m_pwszBuffer, (m_nTextSize+1) * sizeof(char) );
	delete[] m_pwszBuffer;
	m_pwszBuffer  = pTempBuffer;
	m_nBufferSize = nAllocateSize;
	return TRUE;
}

// Uniscribe -- Analyse() analyses the string in the buffer
HRESULT cUIEditBox::cUniBuffer::Analyse()
{
	if( m_Analysis ) _ScriptStringFree( &m_Analysis );

	SCRIPT_CONTROL ScriptControl;		// for uniscribe
	SCRIPT_STATE   ScriptState;			// for uniscribe
	ZeroMemory( &ScriptControl, sizeof(ScriptControl) );
	ZeroMemory( &ScriptState,   sizeof(ScriptState)   );
	_ScriptApplyDigitSubstitution( NULL, &ScriptControl, &ScriptState );

	sUIFontNode* pFontNode = g_cUIDialogManager.GetFontNode( m_iFont );

	HRESULT hr = _ScriptStringAnalyse( pFontNode->pFont ? pFontNode->pFont->GetDC() : NULL,
										m_pwszBuffer,
										m_nTextSize + 1, // NULL is also analyzed.
										m_nTextSize * 3 / 2 + 16,
										-1,
										SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
										0,
										&ScriptControl,
										&ScriptState,
										NULL,
										NULL,
										NULL,
										&m_Analysis );

	if( SUCCEEDED( hr ) ) m_bAnalyseRequired = FALSE;	// Analysis is up-to-date.
	return hr;
}

BOOL cUIEditBox::cUniBuffer::SetBufferSize( int nSize )
{
	while( m_nBufferSize < nSize ) if( !Grow() ) return FALSE;
	return TRUE;
}

char& cUIEditBox::cUniBuffer::operator []( int n )		// No param checking
{
	// This version of operator[] is called only if we are asking for write access,
	// so re-analysis is required.
	m_bAnalyseRequired = TRUE;
	return m_pwszBuffer[n];
}

void cUIEditBox::cUniBuffer::Clear()
{
	*m_pwszBuffer		= L'\0';
	m_nTextSize			= 0;
	m_bAnalyseRequired	= TRUE;
}

// Insert the char at specified index. If nIndex == -1, insert to the end.
BOOL cUIEditBox::cUniBuffer::InsertChar( int nIndex, char wchar )
{
	ASSERT( nIndex >= 0 );
	if( nIndex > m_nTextSize ) return FALSE;

	// Check for maximum length allowed.
	if( GetTextSize() + 1 >= UI_MAX_EDITBOXLENGTH ) return FALSE;
	if( m_nTextSize + 1 >= m_nBufferSize ) if( !Grow() ) return FALSE;

	MoveMemory( m_pwszBuffer + nIndex + 1, m_pwszBuffer + nIndex, sizeof(char) * ( m_nTextSize - nIndex + 1 ) );
	m_pwszBuffer[ nIndex ] = wchar;
	m_nTextSize++;
	m_bAnalyseRequired = TRUE;
	return TRUE;
}

// Removes the char at specified index. If nIndex == -1, remove the last char.
BOOL cUIEditBox::cUniBuffer::RemoveChar( int nIndex )
{
	if( !m_nTextSize || nIndex < 0 || nIndex >= m_nTextSize ) return FALSE;

	MoveMemory( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(char) * ( m_nTextSize - nIndex ) );
	m_nTextSize--;
	m_bAnalyseRequired = TRUE;
	return TRUE;
}

BOOL cUIEditBox::cUniBuffer::InsertString( int nIndex, const char *pStr, int nCount )
{
	ASSERT( nIndex >= 0 );
	if( nIndex > m_nTextSize ) return FALSE;

	if( nCount == -1 ) nCount = strlen( pStr );

	// Check for maximum length allowed
	if( GetTextSize() + nCount >= UI_MAX_EDITBOXLENGTH ) return FALSE;

	if( m_nTextSize + nCount >= m_nBufferSize ) if( !Grow( m_nTextSize + nCount + 1 ) ) return FALSE;

	MoveMemory( m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof(char) * ( m_nTextSize - nIndex + 1 ) );
	CopyMemory( m_pwszBuffer + nIndex, pStr, nCount * sizeof(char) );
	m_nTextSize += nCount;
	m_bAnalyseRequired = TRUE;
	return TRUE;
}

BOOL cUIEditBox::cUniBuffer::SetText( LPCSTR wszText )
{
	ASSERT( wszText != NULL );

	int  nRequired = int( strlen( wszText ) + 1 );

	// Check for maximum length allowed
	if( nRequired >= UI_MAX_EDITBOXLENGTH ) return FALSE;

	while( GetBufferSize() < nRequired ) if( !Grow() ) break;

	// Check again in case out of memory occurred inside while loop.
	if( GetBufferSize() < nRequired ) return FALSE;

	strcpy( m_pwszBuffer, wszText );
	m_nTextSize = nRequired - 1;
	m_bAnalyseRequired = TRUE;
	return TRUE;
}

HRESULT cUIEditBox::cUniBuffer::CPtoX( int nCP, BOOL bTrail, int *pX )
{
	ASSERT( pX );
	*pX = 0;	// default value

	HRESULT hr = S_OK;

	if( m_bAnalyseRequired ) hr = Analyse();
	if( SUCCEEDED( hr ) ) hr = _ScriptStringCPtoX( m_Analysis, nCP, bTrail, pX );

	return hr;
}

HRESULT cUIEditBox::cUniBuffer::XtoCP( int nX, int *pCP, int *pnTrail )
{
	ASSERT( pCP && pnTrail );
	*pCP = 0; *pnTrail = FALSE;		// default value

	HRESULT hr = S_OK;

	if( m_bAnalyseRequired ) hr = Analyse();
	if( SUCCEEDED( hr ) ) hr = _ScriptStringXtoCP( m_Analysis, nX, pCP, pnTrail );

	// If the coordinate falls outside the text region, we can get character positions
	// that don't exist. We must filter them here and convert them to those that do exist.
	if( *pCP == -1 && *pnTrail == TRUE  )
	{
		*pCP = 0; *pnTrail = FALSE;
	}
	else if( *pCP > m_nTextSize && *pnTrail == FALSE )
	{
		*pCP = m_nTextSize; *pnTrail = TRUE;
	}

	return hr;
}

void cUIEditBox::cUniBuffer::GetPriorItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;	// Default is the char itself

	if( m_bAnalyseRequired ) if( FAILED( Analyse() ) ) return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr ) return;

	if( !_ScriptString_pcOutChars( m_Analysis ) ) return;

	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP - 1 < nInitial ) nInitial = nCP - 1;
	for( int i = nInitial; i > 0; i-- )
	{
		if( pLogAttr[i].fWordStop ||		// Either the fWordStop flag is set
			( !pLogAttr[i].fWhiteSpace &&	// Or the previous char is whitespace but this isn't.
			   pLogAttr[i-1].fWhiteSpace ) )
		{
			*pPrior = i;
			return;
		}
	}

	// We have reached index 0. 0 is always a break point, so simply return it.
	*pPrior = 0;
}

void cUIEditBox::cUniBuffer::GetNextItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;	// Default is the char itself

	if( m_bAnalyseRequired ) if( FAILED( Analyse() ) ) return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr ) return;

	if( !_ScriptString_pcOutChars( m_Analysis ) ) return;

	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP + 1 < nInitial ) nInitial = nCP + 1;
	for( int i = nInitial; i < *_ScriptString_pcOutChars( m_Analysis ) - 1; i++ )
	{
		if( pLogAttr[i].fWordStop )			// Either the fWordStop flag is set
		{
			*pPrior = i;
			return;
		}
		else if( pLogAttr[i].fWhiteSpace &&	// Or this whitespace but the next one isn't.
				!pLogAttr[i+1].fWhiteSpace )
		{
			*pPrior = i+1;	// the next char is a word stop.
			return;
		}
	}

	// We have reached the end. It's always a word stop, so simply return it.
	*pPrior = *_ScriptString_pcOutChars( m_Analysis ) - 1;
}


