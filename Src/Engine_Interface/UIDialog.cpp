
#include "StdAfx.h"
#include "UIDialog.h"

struct UI_SCREEN_VERTEX
{
    float x, y, z, h;
    D3DCOLOR color;
    float tu, tv;

    static DWORD FVF;
};
DWORD UI_SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

//--------------------------------------------------------------------------------------

void cUIElement::SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor )
{
    this->iTexture = iTexture;
    
    if( prcTexture )
        rcTexture = *prcTexture;
    else
        SetRectEmpty( &rcTexture );
    
    TextureColor = defaultTextureColor;
}

void cUIElement::SetFont( UINT iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat )
{
    this->iFont = iFont;
    this->dwTextFormat = dwTextFormat;

    FontColor = defaultFontColor;
}


//--------------------------------------------------------------------------------------
// cUIDialog class member functions
//--------------------------------------------------------------------------------------

double      cUIDialog::s_fTimeRefresh    = 0.0f;
cUIControl* cUIDialog::s_pControlFocus   = NULL;		// The control which has focus
cUIControl* cUIDialog::s_pControlPressed = NULL;		// The control currently pressed


cUIDialog::cUIDialog()
{
    m_x					= 0;
    m_y					= 0;
    m_width				= 0;
    m_height			= 0;

	m_bCaption			= FALSE;
    m_bMinimized		= FALSE;
    m_wszCaption[0]		= L'\0';
    m_nCaptionHeight	= 18;

    m_colorTopLeft		= 0;
    m_colorTopRight		= 0;
    m_colorBottomLeft	= 0;
    m_colorBottomRight	= 0;

	m_fTimeLastRefresh	= 0.0f;

	m_pCallbackEvent	= NULL;
	m_pControlMouseOver = NULL;
	
	m_pNextDialog		= this;
	m_pPrevDialog		= this;

	m_nDefaultControlID	= 0xFFFF;
	m_bNonUserEvents	= FALSE;
	m_bKeyboardInput	= FALSE;
	m_bMouseInput		= TRUE;
}

cUIDialog::~cUIDialog()
{
	RemoveAllControls();

	m_Fonts.RemoveAll();
	m_Textures.RemoveAll();

	for( int i = 0; i < m_ResourceElements.GetSize(); i++ )
	{
		sUIResourceHolder* pImageHolder = m_ResourceElements.GetAt( i );
		SAFE_DELETE( pImageHolder );
	}
	m_ResourceElements.RemoveAll();

	for( int i = 0; i < m_ImageElements.GetSize(); i++ )
	{
		sUIImageHolder* pImageHolder = m_ImageElements.GetAt( i );
		SAFE_DELETE( pImageHolder );
	}
	m_ImageElements.RemoveAll();
}

void cUIDialog::RemoveControl( int IDC )
{
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		if( pControl->GetIDC() == IDC )
		{
			// Clean focus first
			ClearFocus();

			// Clear references to this control
			if( s_pControlFocus     == pControl ) s_pControlFocus     = NULL;
			if( s_pControlPressed   == pControl ) s_pControlPressed   = NULL;
			if( m_pControlMouseOver == pControl ) m_pControlMouseOver = NULL;

			SAFE_DELETE( pControl );
			m_Controls.Remove( i );

			return;
		}
	}
}

void cUIDialog::RemoveAllControls()
{
	if( s_pControlFocus   && s_pControlFocus->m_pDialog   == this ) s_pControlFocus = NULL;
	if( s_pControlPressed && s_pControlPressed->m_pDialog == this ) s_pControlPressed = NULL;
	
	m_pControlMouseOver = NULL;

	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		SAFE_DELETE( pControl );
	}

	m_Controls.RemoveAll();
}

//--------------------------------------------------------------------------------------

void cUIDialog::Refresh()
{
	if( s_pControlFocus ) s_pControlFocus->OnFocusOut();
	if( m_pControlMouseOver ) m_pControlMouseOver->OnMouseLeave();

	s_pControlFocus     = NULL;
	s_pControlPressed   = NULL;
	m_pControlMouseOver = NULL;

	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		pControl->Refresh();
	}

	if( m_bKeyboardInput ) FocusDefaultControl();
}

HRESULT cUIDialog::OnRender()
{
	// See if the dialog needs to be refreshed.
	if( m_fTimeLastRefresh < s_fTimeRefresh )
	{
		m_fTimeLastRefresh = g_cUIDialogManager.GetTime();
		Refresh();
	}

    UI_SCREEN_VERTEX vertices[4] =
    {
        (float)m_x,           (float)m_y,            0.5f, 1.0f, m_colorTopLeft,		0.0f, 0.5f, 
        (float)m_x + m_width, (float)m_y,            0.5f, 1.0f, m_colorTopRight,		1.0f, 0.5f,
        (float)m_x + m_width, (float)m_y + m_height, 0.5f, 1.0f, m_colorBottomRight,	1.0f, 1.0f, 
        (float)m_x,           (float)m_y + m_height, 0.5f, 1.0f, m_colorBottomLeft,		0.0f, 1.0f, 
    };

    IDirect3DDevice9* pd3dDevice = g_cUIDialogManager.GetD3DDevice();     

    // Set up a state block here and restore it when finished drawing all the controls
	g_cUIDialogManager.m_pStateBlock->Capture();

    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );

    //pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

    if( !m_bMinimized )
    {
        pd3dDevice->SetFVF( UI_SCREEN_VERTEX::FVF );
        pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(UI_SCREEN_VERTEX) );
    }

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

    sUITextureNode* pTextureNode = GetTexture( 0 );
    pd3dDevice->SetTexture( 0, pTextureNode->pTexture );

    g_cUIDialogManager.m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );

    // Render the caption if it's enabled.
    if( m_bCaption )
    {
        // DrawSprite will offset the rect down by
        // m_nCaptionHeight, so adjust the rect higher
        // here to negate the effect.
        RECT rc = { 0, -m_nCaptionHeight, m_width, 0 };
        DrawSprite( &m_CapElement, &rc );
        rc.left += 5; // Make a left margin
        char wszOutput[256];
        strncpy( wszOutput, m_wszCaption, 256 );
        wszOutput[255] = L'\0';
		//if( m_bMinimized ) wcsncat( wszOutput, L" (Minimized)", 256 - lstrlenW( wszOutput ) );
        if( strlen( wszOutput ) > 0 ) DrawText( wszOutput, &m_CapElement, &rc );
    }

    // If the dialog is minimized, skip rendering its controls.
    if( !m_bMinimized )
    {
        for( int i = 0; i < m_Controls.GetSize(); i++ )
        {
            cUIControl* pControl = m_Controls.GetAt( i );   

            // Focused control is drawn last
            if( pControl == s_pControlFocus ) continue;

            pControl->Render( pd3dDevice );
        }

        if( s_pControlFocus != NULL && s_pControlFocus->m_pDialog == this )
            s_pControlFocus->Render( pd3dDevice );
    }

    g_cUIDialogManager.m_pSprite->End();

    g_cUIDialogManager.m_pStateBlock->Apply();

	return S_OK;
}


//--------------------------------------------------------------------------------------

HRESULT cUIDialog::SetResourceElement( UINT nResourceId, UINT nResourceState, cUIElement *pElement )
{
	// If this Resource already exists in the list, simply update the stored Resource.
	for( int i = 0; i < m_ResourceElements.GetSize(); i++ )
	{
		sUIResourceHolder* pResourceHolder = m_ResourceElements.GetAt( i );

		if( pResourceHolder->nResourceId    == nResourceId &&
			pResourceHolder->nResourceState == nResourceState )
		{
			pResourceHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	sUIResourceHolder *pNewHolder = new sUIResourceHolder;
	if( pNewHolder == NULL ) return E_OUTOFMEMORY;

	pNewHolder->nResourceId    = nResourceId;
	pNewHolder->nResourceState = nResourceState;
	pNewHolder->Element		   = *pElement;

	m_ResourceElements.Add( pNewHolder );
	return S_OK;
}

cUIElement* cUIDialog::GetResourceElement( UINT nResourceId, UINT nResourceState )
{
	for( int i = 0; i < m_ResourceElements.GetSize(); i++ )
	{
		sUIResourceHolder* pResourceHolder = m_ResourceElements.GetAt( i );

		if( pResourceHolder->nResourceId    == nResourceId &&
			pResourceHolder->nResourceState == nResourceState ) return &pResourceHolder->Element;
	}

	return NULL;
}

//--------------------------------------------------------------------------------------

HRESULT cUIDialog::SetImageElement( UINT nImageId, cUIElement *pElement )
{
	sUIImageHolder *pNewHolder = new sUIImageHolder;
	if( pNewHolder == NULL ) return E_OUTOFMEMORY;

	pNewHolder->nImageId     = nImageId;
	pNewHolder->Element		 = *pElement;

	m_ImageElements.Add( pNewHolder );
	return S_OK;
}

cUIElement* cUIDialog::GetImageElement( UINT nImageId )
{
	for( int i = 0; i < m_ImageElements.GetSize(); i++ )
	{
		sUIImageHolder* pImageHolder = m_ImageElements.GetAt( i );

		if( pImageHolder->nImageId == nImageId ) return &pImageHolder->Element;
	}

	return NULL;
}

//--------------------------------------------------------------------------------------

BOOL cUIDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// If caption is enabled, check for clicks in the caption area.
	if( m_bCaption )
	{
		static BOOL bDrag = FALSE;
		static POINT ptOffset;
		POINT mousePoint = { short( LOWORD( lParam ) ), short( HIWORD( lParam ) ) };

		switch( uMsg )
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
					mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
				{
					bDrag = TRUE;
					ptOffset.x = mousePoint.x - m_x;
					ptOffset.y = mousePoint.y - m_y;
					SetCapture( g_cUIDialogManager.GetHWND() );
					return TRUE;
				}
				break;

			case WM_LBUTTONUP:
				if( bDrag )
				{
					if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
						mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
					{
						ReleaseCapture();
						bDrag = FALSE;
						//m_bMinimized = !m_bMinimized;
						return TRUE;
					}
				}
				break;

			case WM_MOUSEMOVE:
				if( bDrag )
				{
					m_x = mousePoint.x - ptOffset.x;
					m_y = mousePoint.y - ptOffset.y;
					return TRUE;
				}
				break;
		}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if( m_bMinimized ) return FALSE;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then
	// give it the first chance to handle the message.
	if( s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled() )
	{
		// If the control MsgProc handled it, then we don't.
		if( s_pControlFocus->MsgProc( uMsg, wParam, lParam ) ) return TRUE;
	}

	switch( uMsg )
	{
		case WM_ACTIVATEAPP:
			// Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
			// as the application is activated/deactivated. This matches the Windows
			// behavior.
			if( s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled() )
			{
				if( wParam ) s_pControlFocus->OnFocusIn();
				else         s_pControlFocus->OnFocusOut();
			}
			break;

		// Keyboard messages
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{	
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance to handle the message.
			if( s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) ) return TRUE;
			}

			// Not yet handled, see if this matches a control's hotkey
			// Activate the hotkey if the focus doesn't belong to an edit box.
			if( uMsg == WM_KEYUP && ( !s_pControlFocus ||
									  ( s_pControlFocus->GetType() != UI_CONTROL_EDITBOX &&
									    s_pControlFocus->GetType() != UI_CONTROL_IMEEDITBOX ) ) )
			{
				for( int i = 0; i < m_Controls.GetSize(); i++ )
				{
					cUIControl* pControl = m_Controls.GetAt( i );
					if( pControl->GetHotkey() == wParam )
					{
						pControl->OnHotkey();
						return TRUE;
					}
				}
			}

			// Not yet handled, check for focus messages.
			if( uMsg == WM_KEYDOWN )
			{
				// If keyboard input is not enabled, this message should be ignored.
				if( !m_bKeyboardInput ) return FALSE;

				switch( wParam )
				{
					case VK_RIGHT:
					case VK_DOWN:
						if( s_pControlFocus != NULL )
						{
							OnCycleFocus( TRUE );
							return TRUE;
						}
						break;

					case VK_LEFT:
					case VK_UP:
						if( s_pControlFocus != NULL )
						{
							OnCycleFocus( FALSE );
							return TRUE;
						}
						break;

					case VK_TAB:
						if( s_pControlFocus == NULL )
						{
							FocusDefaultControl();
						}
						else
						{
							BOOL bShiftDown = ( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0 );
							OnCycleFocus( !bShiftDown );
						}
						return TRUE;
				}
			}

			break;
		}

		// Mouse messages
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
//		case WM_XBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		{
			// If not accepting mouse input, return FALSE to indicate the message should still
			// be handled by the application ( usually to move the camera ).
			if( !m_bMouseInput ) return FALSE;

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			mousePoint.x -= m_x;
			mousePoint.y -= m_y;

			// If caption is enabled, offset the Y coordinate by the negative of its height.
			if( m_bCaption ) mousePoint.y -= m_nCaptionHeight;

			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance to handle the message.
			if( s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleMouse( uMsg, mousePoint, wParam, lParam ) ) return TRUE;
			}

			// Not yet handled, see if the mouse is over any controls.
			cUIControl* pControl = GetControlAtPoint( mousePoint );
			if( pControl != NULL && pControl->GetEnabled() )
			{
				if( pControl->HandleMouse( uMsg, mousePoint, wParam, lParam ) ) return TRUE;
			}
			else
			{
				// Mouse not over any controls in this dialog, if there was a control
				// which had focus it just lost it.
				if( uMsg == WM_LBUTTONDOWN && s_pControlFocus && s_pControlFocus->m_pDialog == this )
				{
					s_pControlFocus->OnFocusOut();
					s_pControlFocus = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return FALSE to indicate the
			// message should still be handled by the application ( usually to move the camera ).
			switch( uMsg )
			{
				case WM_MOUSEMOVE:
					OnMouseMove( mousePoint );
					return FALSE;
			}

			break;
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------

void cUIDialog::SendEvent( UINT nEvent, BOOL bTriggeredByUser, cUIControl* pControl )
{
	// If no callback has been registered there's nowhere to send the event to
	if( m_pCallbackEvent == NULL ) return;

	// Discard events triggered by programatcially if these types of events haven't
	// been enabled.
	if( !bTriggeredByUser && !m_bNonUserEvents ) return;

	m_pCallbackEvent( pControl->GetIDC(), nEvent, pControl );
}

//--------------------------------------------------------------------------------------

HRESULT cUIDialog::AddStatic( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault, cUIStatic** ppCreated )
{
	HRESULT hr = S_OK;

	cUIStatic* pStatic = new cUIStatic( this );
	if( ppCreated != NULL ) *ppCreated = pStatic;
	if( pStatic == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pStatic ) ) ) return hr;

	// Set the IDC and list index
	pStatic->SetIDC( IDC );
	pStatic->SetIDE( IDE );
	pStatic->SetText( strText );
	pStatic->SetLocation( x, y );
	pStatic->SetSize( width, height );
	pStatic->m_bIsDefault = bIsDefault;

	return S_OK;
}

HRESULT cUIDialog::AddButton( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, UINT nHotkey, BOOL bIsDefault, cUIButton** ppCreated )
{
	HRESULT hr = S_OK;

	cUIButton* pButton = new cUIButton( this );
	if( ppCreated != NULL ) *ppCreated = pButton;
	if( pButton == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pButton ) ) ) return hr;

	// Set the IDC and list index
	pButton->SetIDC( IDC );
	pButton->SetIDE( IDE );
	pButton->SetText( strText );
	pButton->SetLocation( x, y );
	pButton->SetSize( width, height );
	pButton->SetHotkey( nHotkey );
	pButton->m_bIsDefault = bIsDefault;

	return S_OK;
}

HRESULT cUIDialog::AddCheckBox( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bChecked, UINT nHotkey, BOOL bIsDefault, cUICheckBox** ppCreated )
{
	HRESULT hr = S_OK;

	cUICheckBox* pCheckBox = new cUICheckBox( this );
	if( ppCreated != NULL ) *ppCreated = pCheckBox;
	if( pCheckBox == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pCheckBox ) ) ) return hr;

	// Set the IDC and list index
	pCheckBox->SetIDC( IDC );
	pCheckBox->SetIDE( IDE );
	pCheckBox->SetText( strText );
	pCheckBox->SetLocation( x, y );
	pCheckBox->SetSize( width, height );
	pCheckBox->SetHotkey( nHotkey );
	pCheckBox->m_bIsDefault = bIsDefault;
	pCheckBox->SetChecked( bChecked );

	return S_OK;
}

#ifdef _T_
HRESULT cUIDialog::AddRadioButton( int IDC, UINT nButtonGroup, LPCSTR strText, int x, int y, int width, int height, BOOL bChecked, UINT nHotkey, BOOL bIsDefault, cUIRadioButton** ppCreated )
{
	HRESULT hr = S_OK;

	cUIRadioButton* pRadioButton = new cUIRadioButton( this );
	if( ppCreated != NULL ) *ppCreated = pRadioButton;
	if( pRadioButton == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pRadioButton ) ) ) return hr;

	// Set the IDC and list index
	pRadioButton->SetIDC( IDC );
	pRadioButton->SetText( strText );
	pRadioButton->SetButtonGroup( nButtonGroup );
	pRadioButton->SetLocation( x, y );
	pRadioButton->SetSize( width, height );
	pRadioButton->SetHotkey( nHotkey );
	pRadioButton->m_bIsDefault = bIsDefault;
	pRadioButton->SetChecked( bChecked );

	return S_OK;
}
#endif

HRESULT cUIDialog::AddListBox( int IDC, int IDE, int x, int y, int width, int height, DWORD dwStyle, cUIListBox** ppCreated )
{
	HRESULT hr = S_OK;

	cUIListBox* pListBox = new cUIListBox( this );
	if( ppCreated != NULL ) *ppCreated = pListBox;
	if( pListBox == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pListBox ) ) ) return hr;

	// Set the IDC and list index
	pListBox->SetIDC( IDC );
	pListBox->SetIDE( IDE );
	pListBox->SetLocation( x, y );
	pListBox->SetSize( width, height );
	pListBox->SetStyle( dwStyle );

	return S_OK;
}

HRESULT cUIDialog::AddComboBox( int IDC, int IDE, int x, int y, int width, int height, UINT nHotkey, BOOL bIsDefault, cUIComboBox** ppCreated )
{
	HRESULT hr = S_OK;

	cUIComboBox* pComboBox = new cUIComboBox( this );
	if( ppCreated != NULL ) *ppCreated = pComboBox;
	if( pComboBox == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pComboBox ) ) ) return hr;

	// Set the IDC and list index
	pComboBox->SetIDC( IDC );
	pComboBox->SetIDE( IDE );
	pComboBox->SetComboBoxIDE( IDE, IDE+1, IDE+2, IDE+3 );
	pComboBox->SetLocation( x, y );
	pComboBox->SetSize( width, height );
	pComboBox->SetHotkey( nHotkey );
	pComboBox->m_bIsDefault = bIsDefault;

	return S_OK;
}

#ifdef _T_
HRESULT cUIDialog::AddSlider( int IDC, int x, int y, int width, int height, int min, int max, int value, BOOL bIsDefault, cUISlider** ppCreated )
{
	HRESULT hr = S_OK;

	cUISlider* pSlider = new cUISlider( this );
	if( ppCreated != NULL ) *ppCreated = pSlider;
	if( pSlider == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pSlider ) ) ) return hr;

	// Set the IDC and list index
	pSlider->SetIDC( IDC );
	pSlider->SetLocation( x, y );
	pSlider->SetSize( width, height );
	pSlider->m_bIsDefault = bIsDefault;
	pSlider->SetRange( min, max );
	pSlider->SetValue( value );

	return S_OK;
}
#endif

HRESULT cUIDialog::AddEditBox( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault, cUIEditBox** ppCreated )
{
	HRESULT hr = S_OK;

	cUIEditBox* pEditBox = new cUIEditBox( this );
	if( ppCreated != NULL ) *ppCreated = pEditBox;
	if( pEditBox == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pEditBox ) ) ) return hr;

	// Set the IDC and list index
	pEditBox->SetIDC( IDC );
	pEditBox->SetIDE( IDE );
	pEditBox->SetLocation( x, y );
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;
	if( strText ) pEditBox->SetText( strText );

	return S_OK;
}

#ifdef _T_
HRESULT cUIDialog::AddIMEEditBox( int IDC, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault, cUIIMEEditBox** ppCreated )
{
	HRESULT hr = S_OK;

	cUIIMEEditBox* pEditBox = new cUIIMEEditBox( this );
	if( ppCreated != NULL ) *ppCreated = pEditBox;
	if( pEditBox == NULL ) return E_OUTOFMEMORY;

	if( FAILED( hr = AddControl( pEditBox ) ) ) return hr;

	// Set the IDC and list index
	pEditBox->SetIDC( IDC );
	pEditBox->SetLocation( x, y );
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;
	if( strText ) pEditBox->SetText( strText );

	return S_OK;
}
#endif

HRESULT cUIDialog::AddUIControl( cUIControl* pControl, int IDC, int IDE, int x, int y, int width, int height, BOOL bIsDefault )
{
	HRESULT hr = S_OK;

	ASSERT( pControl != NULL );

	if( FAILED( hr = AddControl( pControl ) ) ) return hr;

	// Set the IDC and list index
	pControl->SetIDC( IDC );
	pControl->SetIDE( IDE );
	pControl->SetLocation( x, y );
	pControl->SetSize( width, height );
	pControl->m_bIsDefault = bIsDefault;

	return S_OK;
}

//--------------------------------------------------------------------------------------

HRESULT cUIDialog::InitControl( cUIControl* pControl )
{
	if( pControl == NULL ) return E_INVALIDARG;

	pControl->m_Index = m_Controls.GetSize();

	return pControl->OnInit();
}

HRESULT cUIDialog::AddControl( cUIControl* pControl )
{
	HRESULT hr = S_OK;

	if( FAILED( hr = InitControl( pControl ) ) ) return hr;

	// Add to the list
	if( FAILED( hr = m_Controls.Add( pControl ) ) ) return hr;

	return S_OK;
}

cUIControl* cUIDialog::GetControl( int IDC )
{
	// Try to find the control with the given IDC
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		if( pControl->GetIDC() == IDC ) return pControl;
	}

	// Not found
	return NULL;
}

cUIControl* cUIDialog::GetControl( int IDC, UINT nControlType )
{
	// Try to find the control with the given IDC
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		if( pControl->GetIDC() == IDC && pControl->GetType() == nControlType ) return pControl;
	}

	// Not found
	return NULL;
}

cUIControl* cUIDialog::GetControlAtPoint( POINT pt )
{
	// Search through all child controls for the first one which contains the mouse point
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		if( pControl == NULL ) continue;

		// We only return the current control if it is visible and enabled.
		// Because GetControlAtPoint() is used to do mouse hittest, it makes sense
		// to perform this filtering.
		if( pControl->ContainsPoint( pt ) && pControl->GetEnabled() && pControl->GetVisible() )
		{
			return pControl;
		}
	}

	return NULL;
}

 void cUIDialog::SetRefreshTime( float fTime ) 
{
	s_fTimeRefresh = fTime;
}


cUIControl* cUIDialog::GetNextControl( cUIControl* pControl )
{
	int index = pControl->m_Index + 1;

	cUIDialog* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note that
	// if only one control exists in all looped dialogs it will be the returned
	// 'next' control.
	while( index >= (int) pDialog->m_Controls.GetSize() )
	{
		pDialog = pDialog->m_pNextDialog;
		index = 0;
	}

	return pDialog->m_Controls.GetAt( index );
}

cUIControl* cUIDialog::GetPrevControl( cUIControl* pControl )
{
	int index = pControl->m_Index - 1;

	cUIDialog* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note that
	// if only one control exists in all looped dialogs it will be the returned
	// 'previous' control.
	while( index < 0 )
	{
		pDialog = pDialog->m_pPrevDialog;
		if( pDialog == NULL ) pDialog = pControl->m_pDialog;
		index = pDialog->m_Controls.GetSize() - 1;
	}

	return pDialog->m_Controls.GetAt( index );
}

//--------------------------------------------------------------------------------------
#ifdef _T_
void cUIDialog::ClearRadioButtonGroup( UINT nButtonGroup )
{
	// Find all radio buttons with the given group number.
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetType() == UI_CONTROL_RADIOBUTTON )
		{
			cUIRadioButton* pRadioButton = (cUIRadioButton*)pControl;

			if( pRadioButton->GetButtonGroup() == nButtonGroup )
				pRadioButton->SetChecked( FALSE, FALSE );
		}
	}
}
#endif

void cUIDialog::ClearComboBox( int IDC )
{
	cUIComboBox* pComboBox = GetComboBox( IDC );
	if( pComboBox == NULL ) return;

	pComboBox->RemoveAllItems();
}


//--------------------------------------------------------------------------------------

void cUIDialog::SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight )
{
	m_colorTopLeft     = colorTopLeft;
	m_colorTopRight    = colorTopRight;
	m_colorBottomLeft  = colorBottomLeft;
	m_colorBottomRight = colorBottomRight;
}

void cUIDialog::SetNextDialog( cUIDialog* pNextDialog )
{
	if( pNextDialog == NULL ) pNextDialog = this;

	m_pNextDialog = pNextDialog;
	m_pNextDialog->m_pPrevDialog = this;
}

//--------------------------------------------------------------------------------------

void cUIDialog::ClearFocus()
{
	if( s_pControlFocus )
	{
		s_pControlFocus->OnFocusOut();
		s_pControlFocus = NULL;
	}
}

void cUIDialog::FocusDefaultControl()
{
	// Check for default control in this dialog.
	for( int i = 0; i < m_Controls.GetSize(); i++ )
	{
		cUIControl* pControl = m_Controls.GetAt( i );
		if( pControl->m_bIsDefault )
		{
			// Remove focus from the current control
			ClearFocus();

			// Give focus to the default control.
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}
}

void cUIDialog::RequestFocus( cUIControl* pControl )
{
	if( s_pControlFocus == pControl ) return;
	if( !pControl->CanHaveFocus() ) return;
	if( s_pControlFocus ) s_pControlFocus->OnFocusOut();

	pControl->OnFocusIn();
	s_pControlFocus = pControl;
}

void cUIDialog::OnCycleFocus( BOOL bForward )
{
	// This should only be handled by the dialog which owns the focused control, and
	// only if a control currently has focus.
	if( s_pControlFocus == NULL || s_pControlFocus->m_pDialog != this ) return;

	cUIControl* pControl = s_pControlFocus;
	for( int i = 0; i < 0xFFFF; i++ )
	{
		pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );

		// If we've gone in a full circle then focus doesn't change.
		if( pControl == s_pControlFocus ) return;

		// If the dialog accepts keyboard input and the control can have focus then
		// move focus.
		if( pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveFocus() )
		{
			s_pControlFocus->OnFocusOut();
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}

	// If we reached this point, the chain of dialogs didn't form a complete loop.
	ASSERT( !"Multiple dialogs are improperly chained together." );
}

//--------------------------------------------------------------------------------------

BOOL cUIDialog::GetControlEnabled( int IDC )
{
	cUIControl *pControl = GetControl( IDC );
	if( pControl == NULL ) return FALSE;
	return pControl->GetEnabled();
}

void cUIDialog::SetControlEnabled( int IDC, BOOL bEnabled )
{
	cUIControl *pControl = GetControl( IDC );
	if( pControl == NULL ) return;
	pControl->SetEnabled( bEnabled );	
}

void cUIDialog::OnMouseUp( POINT pt )
{
	s_pControlPressed   = NULL;
	m_pControlMouseOver = NULL;
}

void cUIDialog::OnMouseMove( POINT pt )
{
	// Figure out which control the mouse is over now.
	cUIControl* pControl = GetControlAtPoint( pt );

	// If the mouse is still over the same control, nothing needs to be done.
	if( pControl == m_pControlMouseOver ) return;

	// Handle mouse leaving the old control.
	if( m_pControlMouseOver ) m_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control.
	m_pControlMouseOver = pControl;
	if( pControl != NULL ) m_pControlMouseOver->OnMouseEnter();
}

void cUIDialog::OnMouseEnter( cUIControl* pControl )
{
	if( pControl == NULL ) return;

	//pControl->m_bMouseOver = TRUE;
}

void cUIDialog::OnMouseLeave( cUIControl* pControl )
{
	if( pControl == NULL ) return;

	//pControl->m_bMouseOver = FALSE;
}

//--------------------------------------------------------------------------------------

HRESULT cUIDialog::DrawRect( RECT* pRect, D3DCOLOR color )
{
    RECT rcScreen = *pRect;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption ) OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    UI_SCREEN_VERTEX vertices[4] =
    {
        (float) rcScreen.left  -0.5f, (float) rcScreen.top    -0.5f, 0.5f, 1.0f, color, 0, 0,
        (float) rcScreen.right -0.5f, (float) rcScreen.top    -0.5f, 0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.left  -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0,
    };

    IDirect3DDevice9* pd3dDevice = g_cUIDialogManager.GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    g_cUIDialogManager.m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( UI_SCREEN_VERTEX::FVF );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(UI_SCREEN_VERTEX) );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();

    return S_OK;
}

HRESULT cUIDialog::DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color )
{
    UI_SCREEN_VERTEX* vertices = new UI_SCREEN_VERTEX[ nNumPoints ];
    if( vertices == NULL ) return E_OUTOFMEMORY;

    UI_SCREEN_VERTEX* pVertex = vertices;
    POINT* pt = apPoints;
    for( UINT i=0; i < nNumPoints; i++ )
    {
        pVertex->x = m_x + (float) pt->x;
        pVertex->y = m_y + (float) pt->y;
        pVertex->z = 0.5f;
        pVertex->h = 1.0f;
        pVertex->color = color;
        pVertex->tu = 0.0f;
        pVertex->tv = 0.0f;

        pVertex++;
        pt++;
    }

    IDirect3DDevice9* pd3dDevice = g_cUIDialogManager.GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    g_cUIDialogManager.m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( UI_SCREEN_VERTEX::FVF );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, nNumPoints - 1, vertices, sizeof(UI_SCREEN_VERTEX) );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();

    SAFE_DELETEn( vertices );
    return S_OK;
}

HRESULT cUIDialog::DrawSprite( cUIElement* pElement, RECT* prcDest )
{
    RECT rcTexture = pElement->rcTexture;

    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption ) OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    sUITextureNode* pTextureNode = GetTexture( pElement->iTexture );
    
    float fScaleX = (float) RectWidth( rcScreen ) / RectWidth( rcTexture );
    float fScaleY = (float) RectHeight( rcScreen ) / RectHeight( rcTexture );

    D3DXMATRIXA16 matTransform;
    D3DXMatrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );

    g_cUIDialogManager.m_pSprite->SetTransform( &matTransform );
    
    D3DXVECTOR3 vPos( (float)rcScreen.left, (float)rcScreen.top, 0.0f );

    vPos.x /= fScaleX;
    vPos.y /= fScaleY;

    return g_cUIDialogManager.m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, pElement->TextureColor );
}

HRESULT cUIDialog::DrawImage( cUIElement* pElement, int x, int y )
{
    RECT rcTexture = pElement->rcTexture;
    
    sUITextureNode* pTextureNode = GetTexture( pElement->iTexture );
    
    D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling( &matTransform, 1.0f, 1.0f, 1.0f );
    g_cUIDialogManager.m_pSprite->SetTransform( &matTransform );
    
	D3DXVECTOR3 vPos( (float)m_x + x, (float)m_y + y + ( m_bCaption ? m_nCaptionHeight : 0 ), 0.0f );

    return g_cUIDialogManager.m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, pElement->TextureColor );
}

HRESULT cUIDialog::CalcTextRect( LPCSTR strText, cUIElement* pElement, RECT* prcDest, int nCount )
{
    HRESULT hr = S_OK;

    sUIFontNode* pFontNode = GetFont( pElement->iFont );
    DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
    // Since we are only computing the rectangle, we don't need a sprite.
    hr = pFontNode->pFont->DrawText( NULL,  (LPCSTR)strText, nCount, prcDest, dwTextFormat, pElement->FontColor );
    if( FAILED(hr) ) return hr;

    return S_OK;
}

HRESULT cUIDialog::DrawText( LPCSTR strText, cUIElement* pElement, RECT* prcDest, int nCount )
{
    HRESULT hr = S_OK;

    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption ) OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    //debug
    //DrawRect( &rcScreen, D3DCOLOR_ARGB(100, 255, 0, 0) );

    D3DXMATRIXA16 matTransform;
    D3DXMatrixIdentity( &matTransform );
    g_cUIDialogManager.m_pSprite->SetTransform( &matTransform );

    sUIFontNode* pFontNode = GetFont( pElement->iFont );
    
    hr = pFontNode->pFont->DrawText( g_cUIDialogManager.m_pSprite, (LPCSTR)strText, nCount, &rcScreen, pElement->dwTextFormat, pElement->FontColor );
    if( FAILED( hr ) ) return hr;

    return S_OK;
}


//--------------------------------------------------------------------------------------

HRESULT cUIDialog::SetFont( UINT index, LPCSTR strFaceName, LONG height, LONG weight )
{
    // Make sure the list is at least as large as the index being set
    UINT i;
    for( i=m_Fonts.GetSize(); i <= index; i++ )
    {
        m_Fonts.Add( -1 );
    }

    int iFont = g_cUIDialogManager.AddFont( strFaceName, height, weight );
    m_Fonts.SetAt( index, iFont );

    return S_OK;
}

sUIFontNode* cUIDialog::GetFont( UINT index )
{
    if( NULL == cUIDialogMBlock ) return NULL;
    return g_cUIDialogManager.GetFontNode( m_Fonts.GetAt( index ) );
}

UINT cUIDialog::GetFontIndex( UINT index )
{
	return m_Fonts.GetAt( index ) ;
}

HRESULT cUIDialog::SetTexture( UINT index, LPCSTR strFilename )
{
    // Make sure the list is at least as large as the index being set
    for( UINT i=m_Textures.GetSize(); i <= index; i++ )
    {
        m_Textures.Add( -1 );
    }

    int iTexture = g_cUIDialogManager.AddTexture( strFilename );

    m_Textures.SetAt( index, iTexture );
    return S_OK;
}

sUITextureNode* cUIDialog::GetTexture( UINT index )
{
    if( NULL == cUIDialogMBlock ) return NULL;
    return g_cUIDialogManager.GetTextureNode( m_Textures.GetAt( index ) );
}

UINT cUIDialog::GetTextureIndex( UINT index )
{
	return m_Textures.GetAt( index ) ;
}

//--------------------------------------------------------------------------------------

