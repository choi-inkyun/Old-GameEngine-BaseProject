
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIListBox.h"

//--------------------------------------------------------------------------------------
// cUIListBox class
//--------------------------------------------------------------------------------------

cUIListBox::cUIListBox( cUIDialog *pDialog ) : m_ScrollBar( pDialog )
{
	m_Type				= UI_CONTROL_LISTBOX;
	m_pDialog			= pDialog;

	m_dwStyle			= 0;
	m_nSBWidth			= 16;
	m_nSelected			= -1;
	m_bDrag				= FALSE;
	m_nBorder			= 0;
	m_nLineHeight		= 0;
	m_nItemWidth		= 0;
	m_nItemHeight		= 0;
	m_nItemsPerPage		= 0;
	m_nTopMargin		= 0;
	m_nLeftMargin		= 0;
	m_bAutoScroll		= FALSE;
}

cUIListBox::~cUIListBox()
{
	RemoveAllItems();
}

void cUIListBox::UpdateRects()
{
	cUIControl::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder );
	m_rcText = m_rcSelection;
	
	//InflateRect( &m_rcText, -m_nMargin, 0 );
	m_rcText.top += m_nTopMargin;

	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top );
	m_ScrollBar.SetSize( m_nSBWidth, m_height );
	if( m_nLineHeight )
	{
		m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nLineHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_nSelected );
	}
}

void cUIListBox::SetScrollBarSize( int nSBWidth, int nSBHeight, int nSBThumbWidth, int nSBThumbHeight, int nSBOffset )
{
	m_nSBWidth = nSBWidth;
	m_ScrollBar.SetHeight( nSBHeight, nSBThumbHeight );
	m_ScrollBar.SetThumbWidth( nSBOffset, nSBThumbWidth );
	UpdateRects();
}

void cUIListBox::SetScrollBarIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb )
{
	m_ScrollBar.SetButtonIDE( IDEUp, IDEDown, IDEThumb );
}

void cUIListBox::SetItemSize( int nLineHeight, int nItemWidth, int nItemHeight, int nItemsPerPage, int nTopMargin, int nLeftMargin )
{
	m_nLineHeight		= nLineHeight;
	m_nItemWidth		= nItemWidth;
	m_nItemHeight		= nItemHeight;
	m_nItemsPerPage		= nItemsPerPage;
	m_nTopMargin		= nTopMargin;
	m_nLeftMargin		= nLeftMargin;

	UpdateRects();
}

HRESULT cUIListBox::AddItem( const char *wszText, void *pData )
{
	sListBoxItem *pNewItem = new sListBoxItem;
	if( !pNewItem ) return E_OUTOFMEMORY;

	strncpy( pNewItem->strText, wszText, 256 );
	pNewItem->strText[255] = L'\0';
	pNewItem->pData = pData;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = FALSE;

	HRESULT hr = m_Items.Add( pNewItem );
	if( SUCCEEDED( hr ) ) m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );

	if( m_bAutoScroll )
	{
		// make the added item appear on the control by scrolling. 
		m_ScrollBar.ShowItem( m_Items.GetSize() - 1 );
	}

	return hr;
}

HRESULT cUIListBox::InsertItem( int nIndex, const char *wszText, void *pData )
{
	sListBoxItem *pNewItem = new sListBoxItem;
	if( !pNewItem ) return E_OUTOFMEMORY;

	strncpy( pNewItem->strText, wszText, 256 );
	pNewItem->strText[255] = L'\0';
	pNewItem->pData = pData;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = FALSE;

	HRESULT hr = m_Items.Insert( nIndex, pNewItem );
	if( SUCCEEDED( hr ) ) m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );

	return hr;
}

void cUIListBox::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() ) return;

	sListBoxItem *pItem = m_Items.GetAt( nIndex );

	delete pItem;
	m_Items.Remove( nIndex );
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	if( m_nSelected >= (int)m_Items.GetSize() ) m_nSelected = m_Items.GetSize() - 1;

	m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
}

void cUIListBox::RemoveItemByText( char *wszText )
{
	// do nothing.
}

void cUIListBox::RemoveItemByData( void *pData )
{
	for( int i = 0; i < m_Items.GetSize(); i++ )
	{
		sListBoxItem *pItem = m_Items.GetAt( i );
		if( pItem->pData == pData )
		{
			RemoveItem( i );
			break;
		}
	}
}

void cUIListBox::RemoveAllItems()
{
	for( int i = 0; i < m_Items.GetSize(); i++ )
	{
		sListBoxItem *pItem = m_Items.GetAt( i );
		delete pItem;
	}

	m_Items.RemoveAll();
	m_nSelected = -1;
	m_ScrollBar.SetTrackRange( 0, 1 );
}

sListBoxItem *cUIListBox::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() ) return NULL;
	return m_Items[nIndex];
}

// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected. For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as
// nPreviousSelected. Returns -1 on error or if no item is selected.
int  cUIListBox::GetSelectedIndex( int nPreviousSelected )
{
	if( nPreviousSelected < -1 ) return -1;

	return m_nSelected;
}

void cUIListBox::SelectItem( int nNewIndex )
{
	// If no item exists, do nothing.
	if( m_Items.GetSize() == 0 ) return;

	int  nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 ) m_nSelected = 0;
	if( m_nSelected >= (int)m_Items.GetSize() ) m_nSelected = m_Items.GetSize() - 1;

	if( nOldSelected != m_nSelected )
	{
		// Adjust scroll bar
		m_ScrollBar.ShowItem( m_nSelected );
	}

	m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
}

BOOL cUIListBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	// Let the scroll bar have a chance to handle it first.
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) ) return TRUE;

	switch( uMsg )
	{
		case WM_KEYDOWN:
			switch( wParam )
			{
				case VK_UP:
				case VK_DOWN:
				case VK_NEXT:
				case VK_PRIOR:
				case VK_HOME:
				case VK_END:
				{
					// If no item exists, do nothing.
					if( m_Items.GetSize() == 0 ) return TRUE;

					int nOldSelected = m_nSelected;

					// Adjust m_nSelected
					switch( wParam )
					{
						case VK_UP:		m_nSelected--; break;
						case VK_DOWN:	m_nSelected++; break;
						case VK_NEXT:	m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
						case VK_PRIOR:	m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
						case VK_HOME:	m_nSelected = 0; break;
						case VK_END:	m_nSelected = m_Items.GetSize() - 1; break;
					}

					// Perform capping
					if( m_nSelected < 0 ) m_nSelected = 0;
					if( m_nSelected >= (int)m_Items.GetSize() ) m_nSelected = m_Items.GetSize() - 1;

					if( nOldSelected != m_nSelected )
					{
						// Adjust scroll bar
						m_ScrollBar.ShowItem( m_nSelected );

						// Send notification
						m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
					}
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}

BOOL cUIListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg && !m_bHasFocus ) m_pDialog->RequestFocus( this );

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) ) return TRUE;

	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// Check for clicks in the text area
			if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
			{
				// Compute the index of the clicked item
				int nClicked;

				if( m_nLineHeight ) nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nLineHeight;
				else                nClicked = -1;

				// Only proceed if the click falls on top of an item.
				if( nClicked >= m_ScrollBar.GetTrackPos() &&
					nClicked < (int)m_Items.GetSize() &&
					nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
				{
					SetCapture( g_cUIDialogManager.GetHWND() );
					m_bDrag = TRUE;

					// If this is a double click, fire off an event and exit
					// since the first click would have taken care of the selection
					// updating.
					if( uMsg == WM_LBUTTONDBLCLK )
					{
						m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, TRUE, this );
						return TRUE;
					}

					m_nSelected = nClicked;
					
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
				}

				return TRUE;
			}
			break;

		case WM_LBUTTONUP:
		{
			ReleaseCapture();
			m_bDrag = FALSE;

			if( m_nSelected != -1 )
			{
				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
			}
			break;
		}

		case WM_MOUSEMOVE:
			if( m_bDrag )
			{
				// Compute the index of the item below cursor.
				int nItem;

				if( m_nLineHeight ) nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nLineHeight;
				else                nItem = -1;

				// Only proceed if the cursor is on top of an item.
				if( nItem >= m_ScrollBar.GetTrackPos() &&
					nItem < (int)m_Items.GetSize() &&
					nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
				{
					m_nSelected = nItem;
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
				}
				else if( nItem < (int)m_ScrollBar.GetTrackPos() )
				{
					// User drags the mouse above window top
					m_ScrollBar.Scroll( -1 );
					m_nSelected = m_ScrollBar.GetTrackPos();
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
				}
				else if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
				{
					// User drags the mouse below window bottom
					m_ScrollBar.Scroll( 1 );
					m_nSelected = min( (int)m_Items.GetSize(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, TRUE, this );
				}
			}
			else
			{
				if( m_nLineHeight && pt.y >= m_rcText.top && pt.x >= m_rcText.left && pt.x <= m_rcText.right ) 
				{
					m_nMouseOverItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nLineHeight;
				}
				else
				{
					m_nMouseOverItem = -1;
				}
			}
			break;

		case WM_MOUSEWHEEL:
		{
			UINT uLines;
			//			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			SystemParametersInfo( 104, 0, &uLines, 0 );
			int nScrollAmount = int( (short)HIWORD(wParam) ) / WHEEL_DELTA * uLines;
			m_ScrollBar.Scroll( -nScrollAmount );
			return TRUE;
		}
	}

	return FALSE;
}

void cUIListBox::Render( IDirect3DDevice9 *pd3dDevice )
{
	if( !m_bVisible ) return;

	cUIElement* pElementMouseOver = m_pDialog->GetResourceElement( m_IDE, UI_STATE_MOUSEOVER );
	cUIElement* pElementPressed  = m_pDialog->GetResourceElement( m_IDE, UI_STATE_PRESSED );
	cUIElement* pElementSelected = m_pDialog->GetResourceElement( m_IDE, UI_STATE_SELECTED );

	// Render the text
	if( m_Items.GetSize() > 0 )
	{
		// Find out the height of a single line of text
		RECT rc = m_rcText;
		rc.bottom = rc.top + m_nItemHeight;

		static BOOL bSBInit = FALSE;
		if( !bSBInit )
		{
			m_ScrollBar.SetPageSize( m_nItemsPerPage );
			bSBInit = TRUE;
		}

		rc.right = m_rcText.right;
		for( int i = m_ScrollBar.GetTrackPos(); i < (int)m_Items.GetSize(); i++ )
		{
			if( rc.bottom > m_rcText.bottom ) break;

			sListBoxItem *pItem = m_Items.GetAt( i );

			RECT rcSelected = rc;
			rcSelected.left  = rc.left + m_nLeftMargin;
			rcSelected.right = rcSelected.left + m_nItemWidth - 1;

			if( m_bMouseOver && m_nSelected == i && m_bDrag && pElementPressed )
			{
				m_pDialog->DrawSprite( pElementPressed, &rcSelected );
			}
			else if( m_bMouseOver && m_nMouseOverItem == i && !m_bDrag && pElementMouseOver )
			{
				m_pDialog->DrawSprite( pElementMouseOver, &rcSelected );
			}
			else if( m_nSelected == i && pElementSelected )
			{
				m_pDialog->DrawSprite( pElementSelected, &rcSelected );
			}

			// Draw a record.
			DrawListBoxItem( pItem, &rc );

			OffsetRect( &rc, 0, m_nLineHeight );
		}
	}

	// Render the scroll bar
	m_ScrollBar.Render( pd3dDevice );
}

void cUIListBox::DrawListBoxItem( sListBoxItem *pItem, RECT *prcItem )
{
	cUIElement* pElement = m_pDialog->GetResourceElement( m_IDE, UI_STATE_NORMAL );

	m_pDialog->DrawText( pItem->strText, pElement, prcItem );
}

