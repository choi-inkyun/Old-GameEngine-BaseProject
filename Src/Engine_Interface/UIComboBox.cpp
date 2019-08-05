
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIComboBox.h"

//--------------------------------------------------------------------------------------
// cUIComboBox class
//--------------------------------------------------------------------------------------

cUIComboBox::cUIComboBox( cUIDialog *pDialog ) : m_ScrollBar( pDialog )
{
	m_Type		= UI_CONTROL_COMBOBOX;
	m_pDialog	= pDialog;

	m_nDropHeight	= 100;
	m_nSBWidth		= 16;
	m_bOpened		= FALSE;
	m_iSelected		= -1;
	m_iFocused		= -1;

	m_nLeftMargin	= 0;
	m_nTopMargin	= 0;

	m_IDEText		= 0;
	m_IDEButton		= 0;
	m_IDEDropdown	= 0;
	m_IDESelection	= 0;
}

cUIComboBox::~cUIComboBox()
{
	RemoveAllItems();
}

void cUIComboBox::SetTextColor( D3DCOLOR Color )
{
	cUIElement* pElement = m_pDialog->GetResourceElement( m_IDEText, UI_STATE_NORMAL );
	if( pElement ) pElement->FontColor = Color;

	cUIElement* pSelectionElement = m_pDialog->GetResourceElement( m_IDEDropdown, UI_STATE_NORMAL );
	if( pElement ) pElement->FontColor = Color;
}

void cUIComboBox::UpdateRects()
{
	cUIButton::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.left = m_rcButton.right - RectHeight( m_rcButton );

	m_rcMain = m_rcBoundingBox;
	m_rcMain.right = m_rcButton.left;

	m_rcMainText = m_rcMain;
	m_rcMainText.left   += m_nLeftMargin;
	m_rcMainText.right  -= m_nLeftMargin;

	m_rcDropdown = m_rcMain;
	OffsetRect( &m_rcDropdown, 0, RectHeight( m_rcMain ) );
	m_rcDropdown.bottom += m_nDropHeight;
	m_rcDropdown.right  -= m_nSBWidth;

	m_rcDropdownText = m_rcDropdown;
	m_rcDropdownText.left   += m_nLeftMargin;
	m_rcDropdownText.right  -= m_nLeftMargin;
	m_rcDropdownText.top    += m_nTopMargin;
	m_rcDropdownText.bottom -= 0;

	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcDropdown.right, m_rcDropdown.top+2 );
	m_ScrollBar.SetSize( m_nSBWidth, RectHeight( m_rcDropdown )-2 );
	cUIElement* pElement = m_pDialog->GetResourceElement( m_IDEDropdown, UI_STATE_NORMAL );
	sUIFontNode* pFontNode = g_cUIDialogManager.GetFontNode( pElement->iFont );
	if( pFontNode && pFontNode->nHeight )
	{
		m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / pFontNode->nHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_iSelected );
	}
}

void cUIComboBox::OnFocusOut()
{
	cUIButton::OnFocusOut();

	m_bOpened = FALSE;
}

void cUIComboBox::SetScrollBarSize( int nSBWidth, int nSBHeight, int nSBThumbWidth, int nSBThumbHeight, int nSBOffset )
{
	m_nSBWidth = nSBWidth;
	m_ScrollBar.SetHeight( nSBHeight, nSBThumbHeight );
	m_ScrollBar.SetThumbWidth( nSBOffset, nSBThumbWidth );
	UpdateRects();
}

void cUIComboBox::SetScrollBarIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb )
{
	m_ScrollBar.SetButtonIDE( IDEUp, IDEDown, IDEThumb );
}

void cUIComboBox::SetComboBoxIDE( UINT IDEText, UINT IDEButton, UINT IDEDropdown, UINT IDESelection )
{
	m_IDEText		= IDEText;
	m_IDEButton		= IDEButton;
	m_IDEDropdown	= IDEDropdown;
	m_IDESelection	= IDESelection;
}

void cUIComboBox::SetMargin( UINT nLeftMargin, UINT nTopMargin )
{
	m_nLeftMargin	= nLeftMargin;
	m_nTopMargin	= nTopMargin;
	UpdateRects();
}

BOOL cUIComboBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	const DWORD REPEAT_MASK = (0x40000000);

	if( !m_bEnabled || !m_bVisible ) return FALSE;

	// Let the scroll bar have a chance to handle it first.
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) ) return TRUE;

	switch( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_RETURN:
					if( m_bOpened )
					{
						if( m_iSelected != m_iFocused )
						{
							m_iSelected = m_iFocused;
							m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
						}
						m_bOpened = FALSE;

						if( !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();

						return TRUE;
					}
					break;

				case VK_F4:
					// Filter out auto-repeats.
					if( lParam & REPEAT_MASK ) return TRUE;

					m_bOpened = !m_bOpened;

					if( !m_bOpened )
					{
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );

						if( !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();
					}

					return TRUE;

				case VK_LEFT:
				case VK_UP:
					if( m_iFocused > 0 )
					{
						m_iFocused--;
						m_iSelected = m_iFocused;

						// Adjust scroll bar
						m_ScrollBar.ShowItem( m_iSelected );

						if( !m_bOpened ) m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
					}
					return TRUE;

				case VK_RIGHT:
				case VK_DOWN:
					if( m_iFocused + 1 < (int)GetNumItems() )
					{
						m_iFocused++;
						m_iSelected = m_iFocused;
						
						// Adjust scroll bar
						m_ScrollBar.ShowItem( m_iSelected );

						if( !m_bOpened ) m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
					}
					return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

BOOL cUIComboBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) ) return TRUE;

	switch( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected.
				for( int i = 0; i < m_Items.GetSize(); i++ )
				{
					sUIComboBoxItem* pItem = m_Items.GetAt( i );
					if( pItem->bVisible && PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = i;
					}
				}
				return TRUE;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if( ContainsPoint( pt ) )
			{
				// Pressed while inside the control.
				m_bPressed = TRUE;
				SetCapture( g_cUIDialogManager.GetHWND() );

				if( !m_bHasFocus ) m_pDialog->RequestFocus( this );

				// Toggle dropdown
				if( m_bHasFocus )
				{
					m_bOpened = !m_bOpened;

					if( !m_bOpened && !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();
				}

				return TRUE;
			}

			// Perhaps this click is within the dropdown.
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected.
				for( int i = m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
				{
					sUIComboBoxItem* pItem = m_Items.GetAt( i );
					if( pItem->bVisible && PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = m_iSelected = i;
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
						m_bOpened = FALSE;

						if( !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();

						break;
					}
				}

				return TRUE;
			}

			// Mouse click not on main control or in dropdown, fire an event if needed.
			if( m_bOpened )
			{
				m_iFocused = m_iSelected;

				m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
				m_bOpened = FALSE;
			}

			// Make sure the control is no longer in a pressed state
			m_bPressed = FALSE;

			// Release focus if appropriate.
			if( !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();

			break;
		}

		case WM_LBUTTONUP:
		{
			if( m_bPressed && ContainsPoint( pt ) )
			{
				// Button click
				m_bPressed = FALSE;
				ReleaseCapture();
				return TRUE;
			}

			break;
		}

		case WM_MOUSEWHEEL:
		{
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if( m_bOpened )
			{
				UINT uLines;
//				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				SystemParametersInfo( 104, 0, &uLines, 0 );
				m_ScrollBar.Scroll( -zDelta * uLines );
			}
			else
			{
				if( zDelta > 0 )
				{
					if( m_iFocused > 0 )
					{
						m_iFocused--;
						m_iSelected = m_iFocused;

						if( !m_bOpened ) m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
					}
				}
				else
				{
					if( m_iFocused+1 < (int)GetNumItems() )
					{
						m_iFocused++;
						m_iSelected = m_iFocused;

						if( !m_bOpened ) m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

void cUIComboBox::OnHotkey()
{
	if( m_bOpened ) return;
	if( m_iSelected == -1 ) return;

	m_iSelected++;
	if( m_iSelected >= (int)m_Items.GetSize() ) m_iSelected = 0;

	m_iFocused = m_iSelected;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, TRUE, this );
}

void cUIComboBox::Render( IDirect3DDevice9* pd3dDevice )
{
	UI_CONTROL_STATE iState = UI_STATE_NORMAL;

	// Dropdown box
	cUIElement* pElement = m_pDialog->GetResourceElement( m_IDEDropdown, iState );

	// If we have not initialized the scroll bar page size, do that now.
	static BOOL bSBInit;
	if( !bSBInit )
	{
		// Update the page size of the scroll bar
		if( g_cUIDialogManager.GetFontNode( pElement->iFont )->nHeight )
			m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / g_cUIDialogManager.GetFontNode( pElement->iFont )->nHeight );
		else
			m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) );
		bSBInit = TRUE;
	}

	// Scroll bar
	if( m_bOpened )
	{
		if( m_nSBWidth > 0 ) m_ScrollBar.Render( pd3dDevice );

		m_pDialog->DrawSprite( pElement, &m_rcDropdown );

		// Selection outline
		cUIElement* pSelectionElement = m_pDialog->GetResourceElement( m_IDESelection, iState );

		sUIFontNode* pFont = m_pDialog->GetFont( pElement->iFont );
		int curY = m_rcDropdownText.top;
		int nRemainingHeight = RectHeight( m_rcDropdownText );

		for( int i = m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
		{
			sUIComboBoxItem* pItem = m_Items.GetAt( i );

			// Make sure there's room left in the dropdown.
			nRemainingHeight -= pFont->nHeight;
			if( nRemainingHeight < 0 )
			{
				pItem->bVisible = FALSE;
				continue;
			}

			SetRect( &pItem->rcActive, m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + pFont->nHeight );
			curY += pFont->nHeight;

			pItem->bVisible = TRUE;

			if( m_bOpened )
			{
				if( (int)i == m_iFocused )
				{
					RECT rc;
					SetRect( &rc, m_rcDropdown.left, pItem->rcActive.top - 1, m_rcDropdown.right, pItem->rcActive.bottom + 1 );
					m_pDialog->DrawSprite( pSelectionElement, &rc );
					m_pDialog->DrawText( pItem->strText, pSelectionElement, &pItem->rcActive );
				}
				else
				{
					m_pDialog->DrawText( pItem->strText, pElement, &pItem->rcActive );
				}
			}
		}
	}

	iState = UI_STATE_NORMAL;

		 if( !m_bVisible ) iState = UI_STATE_HIDDEN;
	else if( !m_bEnabled ) iState = UI_STATE_DISABLED;
	else if( m_bPressed )  iState = UI_STATE_PRESSED;
	else if( m_bMouseOver ) iState = UI_STATE_MOUSEOVER;
	/*else if( m_bHasFocus ) iState = UI_STATE_FOCUS;*/

	// Button
	pElement = m_pDialog->GetResourceElement( m_IDEButton, iState );

	m_pDialog->DrawSprite( pElement, &m_rcButton );

	if( m_bOpened ) iState = UI_STATE_PRESSED;

	// Main text box
	pElement = m_pDialog->GetResourceElement( m_IDEText, iState );

	m_pDialog->DrawSprite( pElement, &m_rcMain );

	if( m_iSelected >= 0 && m_iSelected < (int) m_Items.GetSize() )
	{
		sUIComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
		if( pItem != NULL ) m_pDialog->DrawText( pItem->strText, pElement, &m_rcMainText );
	}
}

//--------------------------------------------------------------------------------------

HRESULT cUIComboBox::AddItem( const char* strText, void* pData )
{
	// Validate parameters
	if( strText == NULL ) return E_INVALIDARG;

	// Create a new item and set the data.
	sUIComboBoxItem* pItem = new sUIComboBoxItem;
	if( !pItem ) return E_OUTOFMEMORY;

	ZeroMemory( pItem, sizeof(sUIComboBoxItem) );
	strncpy( pItem->strText, strText, 255 );
	pItem->pData = pData;

	m_Items.Add( pItem );

	// Update the scroll bar with new range
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );

	// If this is the only item in the list, it's selected.
	if( GetNumItems() == 1 )
	{
		m_iSelected = 0;
		m_iFocused = 0;
		m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, FALSE, this );
	}

	return S_OK;
}

void cUIComboBox::RemoveItem( UINT index )
{
	sUIComboBoxItem* pItem = m_Items.GetAt( index );
	SAFE_DELETE( pItem );
	m_Items.Remove( index );
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	if( m_iSelected >= m_Items.GetSize() ) m_iSelected = m_Items.GetSize() - 1;
}

void cUIComboBox::RemoveAllItems()
{
	for( int i = 0; i < m_Items.GetSize(); i++ )
	{
		sUIComboBoxItem* pItem = m_Items.GetAt( i );
		SAFE_DELETE( pItem );
	}

	m_Items.RemoveAll();
	m_ScrollBar.SetTrackRange( 0, 1 );
	m_iFocused = m_iSelected = -1;
}

BOOL cUIComboBox::ContainsItem( const char* strText, UINT iStart )
{
	return ( FindItem( strText, iStart ) != -1 );
}

int  cUIComboBox::FindItem( const char* strText, UINT iStart )
{
	if( strText == NULL ) return -1;

	for( int i = iStart; i < m_Items.GetSize(); i++ )
	{
		sUIComboBoxItem* pItem = m_Items.GetAt( i );

		if( strcmp( pItem->strText, strText ) == 0 ) return i;
	}

	return -1;
}

void* cUIComboBox::GetSelectedData()
{
	if( m_iSelected < 0 ) return NULL;

	sUIComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
	return pItem->pData;
}

sUIComboBoxItem* cUIComboBox::GetSelectedItem()
{
	if( m_iSelected < 0 ) return NULL;

	return m_Items.GetAt( m_iSelected );
}

void* cUIComboBox::GetItemData( const char* strText )
{
	int index = FindItem( strText );
	if( index == -1 ) return NULL;

	sUIComboBoxItem* pItem = m_Items.GetAt( index );
	if( pItem == NULL ) return NULL;

	return pItem->pData;
}

HRESULT cUIComboBox::SetSelectedByIndex( UINT index )
{
	if( index >= GetNumItems() ) return E_INVALIDARG;

	m_iFocused = m_iSelected = index;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, FALSE, this );

	return S_OK;
}

HRESULT cUIComboBox::SetSelectedByText( const char* strText )
{
	if( strText == NULL ) return E_INVALIDARG;

	int index = FindItem( strText );
	if( index == -1 ) return E_FAIL;

	m_iFocused = m_iSelected = index;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, FALSE, this );

	return S_OK;
}

HRESULT cUIComboBox::SetSelectedByData( void* pData )
{
	for( int i = 0; i < m_Items.GetSize(); i++ )
	{
		sUIComboBoxItem *pItem = m_Items.GetAt( i );

		if( pItem->pData == pData )
		{
			m_iFocused = m_iSelected = i;
			m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, FALSE, this );
			return S_OK;
		}
	}

	return E_FAIL;
}

