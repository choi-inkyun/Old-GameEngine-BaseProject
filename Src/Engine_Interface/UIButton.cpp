
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIButton.h"

//--------------------------------------------------------------------------------------
// cUIButton class
//--------------------------------------------------------------------------------------

cUIButton::cUIButton( cUIDialog *pDialog )
{
	m_Type			= UI_CONTROL_BUTTON;
	m_pDialog		= pDialog;

	m_StaticType	= STATICTYPE_IMAGE;
	m_bPressed		= FALSE;
	m_nHotkey		= 0;
}

BOOL cUIButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
						m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, TRUE, this );
					}
					return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

BOOL cUIButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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

				if( !m_bHasFocus ) m_pDialog->RequestFocus( this );
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

				if( !m_pDialog->m_bKeyboardInput ) m_pDialog->ClearFocus();

				// Button click
				if( ContainsPoint( pt ) ) m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, TRUE, this );
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

void cUIButton::Render( IDirect3DDevice9* pd3dDevice )
{
	if( !m_bVisible ) return;

	UI_CONTROL_STATE iState = UI_STATE_NORMAL;

		 if( !m_bEnabled  ) iState = UI_STATE_DISABLED;
	else if( m_bPressed   ) iState = UI_STATE_PRESSED;
	else if( m_bMouseOver ) iState = UI_STATE_MOUSEOVER;

	cUIElement *pElement = m_pDialog->GetResourceElement( m_IDE, iState );
	if( !pElement ) return;

	if( m_StaticType == STATICTYPE_TEXT )
		m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox );
	else
		m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
}

