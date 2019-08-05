
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIScrollBar.h"

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE			8

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY		0.33
#define SCROLLBAR_ARROWCLICK_REPEAT		0.05

//--------------------------------------------------------------------------------------
// cUIScrollBar class
//--------------------------------------------------------------------------------------

cUIScrollBar::cUIScrollBar( cUIDialog *pDialog )
{
	m_Type			= UI_CONTROL_SCROLLBAR;
	m_pDialog		= pDialog;

	m_bShowThumb	= TRUE;

	SetRect( &m_rcUpButton,   0, 0, 0, 0 );
	SetRect( &m_rcDownButton, 0, 0, 0, 0 );
	SetRect( &m_rcTrack,      0, 0, 0, 0 );
	SetRect( &m_rcThumb,      0, 0, 0, 0 );

	m_nPosition		= 0;
	m_nPageSize		= 1;
	m_nStart		= 0;
	m_nEnd			= 1;
	m_Arrow			= CLEAR;
	m_dArrowTS		= 0.0f;

	m_nButtonHeight			= (-1);
	m_nThumbOffset			= 0;
	m_nThumbWidth			= (-1);
	m_nThumbHeight			= (-1);

	m_IDEUpButton			= (-1);
	m_IDEDownButton			= (-1);
	m_IDEThumbButton		= (-1);

	m_bUpButtonMouseOver	= FALSE;
	m_bDownButtonMouseOver	= FALSE;
	m_bThumbButtonMouseOver	= FALSE;
}

cUIScrollBar::~cUIScrollBar()
{
	// do nothing.
}

void cUIScrollBar::UpdateRects()
{
	cUIControl::UpdateRects();

	// Make the buttons square
	if( m_nButtonHeight < 0 ) m_nButtonHeight = RectWidth( m_rcBoundingBox );
	if( m_nThumbWidth < 0 ) m_nThumbWidth = RectWidth( m_rcBoundingBox );

	SetRect( &m_rcUpButton, m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcBoundingBox.right,
							m_rcBoundingBox.top + m_nButtonHeight );
	SetRect( &m_rcDownButton, m_rcBoundingBox.left, m_rcBoundingBox.bottom - m_nButtonHeight,
							m_rcBoundingBox.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcTrack, m_rcUpButton.left, m_rcUpButton.bottom, m_rcDownButton.right, m_rcDownButton.top );

	m_rcThumb.left  = m_rcUpButton.left + m_nThumbOffset;
	m_rcThumb.right = m_rcUpButton.left + m_nThumbOffset + m_nThumbWidth /*- 1*/;

	UpdateThumbRect();
}

// Compute the dimension of the scroll thumb
void cUIScrollBar::UpdateThumbRect()
{
	if( m_nEnd - m_nStart > m_nPageSize )
	{
		int nThumbHeight;
		if( m_nThumbHeight > 0 ) nThumbHeight = m_nThumbHeight;
		else nThumbHeight = max( RectHeight( m_rcTrack ) * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );

		int  nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
		m_rcThumb.top = m_rcTrack.top + ( m_nPosition - m_nStart ) * ( RectHeight( m_rcTrack ) - nThumbHeight ) / nMaxPosition;
		m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
		m_bShowThumb = TRUE;
	}
	else
	{
		// No content to scroll
		m_rcThumb.bottom = m_rcThumb.top;
		m_bShowThumb = FALSE;
	}
}

void cUIScrollBar::SetHeight( int nButtonHeight, int nThumbHeight )
{
	m_nButtonHeight	= nButtonHeight;
	m_nThumbHeight	= nThumbHeight;

	UpdateRects();
}

void cUIScrollBar::SetThumbWidth( int nOffset, int nWidth )
{
	m_nThumbOffset	= nOffset;
	m_nThumbWidth	= nWidth;

	UpdateRects();
}

void cUIScrollBar::SetButtonIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb )
{
	m_IDEUpButton		= IDEUp;
	m_IDEDownButton		= IDEDown;
	m_IDEThumbButton	= IDEThumb;
}

// Scroll() scrolls by nDelta items. A positive value scrolls down, while a negative
// value scrolls up.
void cUIScrollBar::Scroll( int nDelta )
{
	// Perform scroll
	m_nPosition += nDelta;

	// Cap position
	Cap();

	// Update thumb position
	UpdateThumbRect();
}

void cUIScrollBar::ShowItem( int nIndex )
{
	// Cap the index
	if( nIndex < 0 ) nIndex = 0;
	if( nIndex >= m_nEnd ) nIndex = m_nEnd - 1;

	// Adjust position
	     if( m_nPosition > nIndex ) m_nPosition = nIndex;
	else if( m_nPosition + m_nPageSize <= nIndex ) m_nPosition = nIndex - m_nPageSize + 1;

	UpdateThumbRect();
}

BOOL cUIScrollBar::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// do nothing.
	return FALSE;
}

BOOL cUIScrollBar::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	static int  ThumbOffsetY;
	static BOOL bDrag;

	m_LastMouse = pt;

	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			SetCapture( g_cUIDialogManager.GetHWND() );

			// Check for click on UP button
			if( PtInRect( &m_rcUpButton, pt ) )
			{
				if( m_nPosition > m_nStart ) m_nPosition--;
				UpdateThumbRect();
				m_Arrow = CLICKED_UP;
				m_dArrowTS = g_cUIDialogManager.GetTime();
				return TRUE;
			}

			// Check for click on DOWN button
			if( PtInRect( &m_rcDownButton, pt ) )
			{
				if( m_nPosition + m_nPageSize < m_nEnd ) m_nPosition++;
				UpdateThumbRect();
				m_Arrow = CLICKED_DOWN;
				m_dArrowTS = g_cUIDialogManager.GetTime();
				return TRUE;
			}

			// Check for click on thumb
			if( PtInRect( &m_rcThumb, pt ) )
			{
				bDrag = TRUE;
				ThumbOffsetY = pt.y - m_rcThumb.top;
				return TRUE;
			}

			// Check for click on track
			if( m_rcThumb.left <= pt.x && m_rcThumb.right > pt.x )
			{
				if( m_rcThumb.top > pt.y && m_rcTrack.top <= pt.y )
				{
					Scroll( -( m_nPageSize - 1 ) );
					return TRUE;
				}
				else if( m_rcThumb.bottom <= pt.y && m_rcTrack.bottom > pt.y )
				{
					Scroll( ( m_nPageSize - 1 ) );
					return TRUE;
				}
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			bDrag = FALSE;
			ReleaseCapture();
			UpdateThumbRect();
			m_Arrow = CLEAR;
			break;
		}

		case WM_MOUSEMOVE:
		{
			// Check for click on UP button
			if( PtInRect( &m_rcUpButton, pt ) )	  m_bUpButtonMouseOver   = TRUE;
			else                                  m_bUpButtonMouseOver   = FALSE;
			if( PtInRect( &m_rcDownButton, pt ) ) m_bDownButtonMouseOver = TRUE;
			else                                  m_bDownButtonMouseOver = FALSE;
			if( PtInRect( &m_rcThumb, pt ) )      m_bThumbButtonMouseOver = TRUE;
			else                                  m_bThumbButtonMouseOver = FALSE;

			if( bDrag )
			{
				m_rcThumb.bottom += pt.y - ThumbOffsetY - m_rcThumb.top;
				m_rcThumb.top = pt.y - ThumbOffsetY;
				if( m_rcThumb.top < m_rcTrack.top ) OffsetRect( &m_rcThumb, 0, m_rcTrack.top - m_rcThumb.top );
				else if( m_rcThumb.bottom > m_rcTrack.bottom ) OffsetRect( &m_rcThumb, 0, m_rcTrack.bottom - m_rcThumb.bottom );

				// Compute the first item index based on thumb position
				int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;				// Largest possible index for first item
				int nMaxThumb = RectHeight( m_rcTrack ) - RectHeight( m_rcThumb );	// Largest possible thumb position from the top

				// Shift by half a row to avoid last row covered by only one pixel.
				// 주) nMaxFirstItem이 0인 경우 나눗셈 오류가 발생하여 아래와 같이 if 문을 추가하였다.
				if( nMaxFirstItem == 0 ) m_nPosition = m_nStart;
				else m_nPosition = m_nStart + ( m_rcThumb.top - m_rcTrack.top + nMaxThumb / ( nMaxFirstItem * 2 ) ) * nMaxFirstItem / nMaxThumb;
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

void cUIScrollBar::Render( IDirect3DDevice9* pd3dDevice )
{
	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated scroll.
	if( m_Arrow != CLEAR )
	{
		double dCurrTime = g_cUIDialogManager.GetTime();

		if( PtInRect( &m_rcUpButton, m_LastMouse ) )
		{
			switch( m_Arrow )
			{
				case CLICKED_UP:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( -1 );
						m_Arrow = HELD_UP;
						m_dArrowTS = dCurrTime;
					}
					break;

				case HELD_UP:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( -1 );
						m_dArrowTS = dCurrTime;
					}
					break;
			}
		}
		else if( PtInRect( &m_rcDownButton, m_LastMouse ) )
		{
			switch( m_Arrow )
			{
				case CLICKED_DOWN:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_Arrow = HELD_DOWN;
						m_dArrowTS = dCurrTime;
					}
					break;

				case HELD_DOWN:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_dArrowTS = dCurrTime;
					}
					break;
			}
		}
	}

	if( !m_bVisible ) return;

	UI_CONTROL_STATE iState;
	cUIElement* pElement;

	// Draw Up Arrow
	iState = UI_STATE_NORMAL;
	if( m_bUpButtonMouseOver ) iState = UI_STATE_MOUSEOVER;
	if( m_Arrow == CLICKED_UP || m_Arrow == HELD_UP ) iState = UI_STATE_PRESSED;
	pElement = m_pDialog->GetResourceElement( m_IDEUpButton, iState );
	m_pDialog->DrawSprite( pElement, &m_rcUpButton );

	// Draw Down Arrow
	iState = UI_STATE_NORMAL;
	if( m_bDownButtonMouseOver ) iState = UI_STATE_MOUSEOVER;
	if( m_Arrow == CLICKED_DOWN || m_Arrow == HELD_DOWN ) iState = UI_STATE_PRESSED;
	pElement = m_pDialog->GetResourceElement( m_IDEDownButton, iState );
	m_pDialog->DrawSprite( pElement, &m_rcDownButton );

	// Thumb button
	iState = UI_STATE_NORMAL;
	if( m_bThumbButtonMouseOver ) iState = UI_STATE_MOUSEOVER;
	pElement = m_pDialog->GetResourceElement( m_IDEThumbButton, iState );
	m_pDialog->DrawSprite( pElement, &m_rcThumb );
}

void cUIScrollBar::SetTrackRange( int nStart, int nEnd )
{
	m_nStart = nStart;
	m_nEnd   = nEnd;

	Cap();
	UpdateThumbRect();
}

// Clips position at boundaries. Ensures it stays within legal range.
void cUIScrollBar::Cap()
{
	     if( m_nPosition < m_nStart || m_nEnd - m_nStart <= m_nPageSize ) m_nPosition = m_nStart;
	else if( m_nPosition + m_nPageSize > m_nEnd ) m_nPosition = m_nEnd - m_nPageSize;
}

