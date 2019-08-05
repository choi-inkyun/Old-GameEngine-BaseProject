
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIStatic.h"

//--------------------------------------------------------------------------------------
// cUIStatic class
//--------------------------------------------------------------------------------------

cUIStatic::cUIStatic( cUIDialog *pDialog )
{
	m_Type			= UI_CONTROL_STATIC;
	m_pDialog		= pDialog;

	m_StaticType	= STATICTYPE_TEXT;
	ZeroMemory( &m_strText, sizeof( m_strText) );
}

HRESULT cUIStatic::GetTextCopy( LPSTR strDest, UINT bufferCount )
{
	// Validate incoming parameters
	if( strDest == NULL || bufferCount == 0 ) return E_INVALIDARG;

	// Copy the window text
	strncpy( strDest, m_strText, bufferCount );
	strDest[bufferCount-1] = 0;

	return S_OK;
}

HRESULT cUIStatic::SetText( LPCSTR strText )
{
	if( strText == NULL )
	{
		m_strText[0] = L'\0';
		return S_OK;
	}

	strncpy( m_strText, strText, MAX_PATH-1 );
	return S_OK;
}

void cUIStatic::Render( IDirect3DDevice9* pd3dDevice )
{
	if( !m_bVisible ) return;

	UI_CONTROL_STATE iState = UI_STATE_NORMAL;

	if( !m_bEnabled ) iState = UI_STATE_DISABLED;

	cUIElement *pElement = m_pDialog->GetResourceElement( m_IDE, iState );
	if( !pElement ) return;

	if( m_StaticType == STATICTYPE_TEXT )
		m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox );
	else
		m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
}

