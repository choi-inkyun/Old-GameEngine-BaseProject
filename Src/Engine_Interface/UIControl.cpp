
#include "StdAfx.h"
#include "UIDialog.h"
#include "UIControl.h"

//--------------------------------------------------------------------------------------
// cUIControl class
//--------------------------------------------------------------------------------------

cUIControl::cUIControl( cUIDialog *pDialog )
{
	m_Type			= UI_CONTROL_STATIC;
	m_pDialog		= pDialog;
	m_IDC			= 0;
	m_IDE			= 0;
	m_Index			= 0;
	m_pUserData		= NULL;

	m_bEnabled		= TRUE;
	m_bVisible		= TRUE;
	m_bMouseOver	= FALSE;
	m_bHasFocus		= FALSE;
	m_bIsDefault	= FALSE;

	m_pDialog		= NULL;

	m_x				= 0;
	m_y				= 0;
	m_width			= 0;
	m_height		= 0;

	ZeroMemory( &m_rcBoundingBox, sizeof( m_rcBoundingBox ) );
}

cUIControl::~cUIControl()
{
	// do nothing.
}

void cUIControl::Refresh()
{
	m_bMouseOver	= FALSE;
	m_bHasFocus		= FALSE;
}

void cUIControl::UpdateRects()
{
	SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}

