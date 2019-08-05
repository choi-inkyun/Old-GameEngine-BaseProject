
#include "StdAfx.h"
#include "UIDialog.h"
#include "UICheckBox.h"

//--------------------------------------------------------------------------------------
// cUICheckBox class
//--------------------------------------------------------------------------------------

cUICheckBox::cUICheckBox( cUIDialog *pDialog )
{
	m_Type		= UI_CONTROL_CHECKBOX;
	m_pDialog	= pDialog;

	m_bChecked	= FALSE;
}

BOOL cUICheckBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	switch( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_SPACE:
					m_bPressed = TRUE;
					return TRUE;
			}
			break;
		}

		case WM_KEYUP:
		{
			switch( wParam )
			{
				case VK_SPACE:
					if( m_bPressed )
					{
						m_bPressed = FALSE;
						SetCheckedInternal( !m_bChecked, TRUE );
					}
					return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

BOOL cUICheckBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible ) return FALSE;

	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if( ContainsPoint( pt ) )
			{
				// Pressed while inside the control
				m_bPressed = TRUE;
                SetCapture( g_cUIDialogManager.GetHWND() );

				if( !m_bHasFocus && m_pDialog->m_bKeyboardInput ) m_pDialog->RequestFocus( this );
				return TRUE;
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = FALSE;
				ReleaseCapture();

				// Button click
				if( ContainsPoint( pt ) ) SetCheckedInternal( !m_bChecked, TRUE );
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

void cUICheckBox::SetCheckedInternal( BOOL bChecked, BOOL bFromInput )
{
	m_bChecked = bChecked;

	m_pDialog->SendEvent( EVENT_CHECKBOX_CHANGED, bFromInput, this );
}

BOOL cUICheckBox::ContainsPoint( POINT pt )
{
	return ( PtInRect( &m_rcBoundingBox, pt ) || PtInRect( &m_rcButton, pt ) );
}

void cUICheckBox::UpdateRects()
{
	cUIButton::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );

	m_rcText = m_rcBoundingBox;
	m_rcText.left += (int) ( 1.25f * RectWidth( m_rcButton ) );
}

void cUICheckBox::Render( IDirect3DDevice9* pd3dDevice )
{
	UI_CONTROL_STATE iState = UI_STATE_NORMAL;

		 if( !m_bEnabled ) iState = UI_STATE_DISABLED;
	else if(  m_bPressed ) iState = UI_STATE_PRESSED;
	else if( m_bMouseOver) iState = UI_STATE_MOUSEOVER;

	cUIElement *pElement = m_pDialog->GetResourceElement( m_IDE, iState );
	if( !pElement ) return;

	m_pDialog->DrawSprite( pElement, &m_rcButton );
	m_pDialog->DrawText( m_strText, pElement, &m_rcText, TRUE );

	if( m_bChecked )
	{
		pElement = m_pDialog->GetResourceElement( m_IDE, UI_STATE_SELECTED );
		m_pDialog->DrawSprite( pElement, &m_rcButton );
	}
}

