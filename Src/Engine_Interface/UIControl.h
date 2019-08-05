
#ifndef __UICONTROL_H__
#define __UICONTROL_H__

//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIControl
{
public:
	cUIControl( cUIDialog *pDialog = NULL );
	virtual ~cUIControl();

	virtual HRESULT OnInit() { return S_OK; }
	virtual void Refresh();
	virtual void Render( IDirect3DDevice9* pd3dDevice ) { };

	// Windows message handler
	virtual BOOL MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return FALSE; }

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return FALSE; }
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return FALSE; }

	virtual BOOL CanHaveFocus() { return FALSE; }
	virtual void OnFocusIn()  { m_bHasFocus = TRUE; }
	virtual void OnFocusOut() { m_bHasFocus = FALSE; }
	virtual void OnMouseEnter() { m_bMouseOver = TRUE; }
	virtual void OnMouseLeave() { m_bMouseOver = FALSE; }
	virtual void OnHotkey() {}

	virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }

	virtual void SetEnabled( BOOL bEnabled ) { m_bEnabled = bEnabled; }
	virtual BOOL GetEnabled() { return m_bEnabled; }
	virtual void SetVisible( BOOL bVisible ) { m_bVisible = bVisible; }
	virtual BOOL GetVisible() { return m_bVisible; }

	UINT GetType() const { return m_Type; }

	int  GetIDC() const { return m_IDC; }
	void SetIDC( int IDC ) { m_IDC = IDC; }

	int  GetIDE() const { return m_IDE; }
	void SetIDE( int IDE ) { m_IDE = IDE; }

	void SetLocation( int x, int y ) { m_x = x; m_y = y; UpdateRects(); }
	void SetSize( int width, int height ) { m_width = width; m_height = height; UpdateRects(); }

	void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }

	void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	BOOL				m_bVisible;			// Shown/hidden flag
	BOOL				m_bMouseOver;		// Mouse pointer is above control
	BOOL				m_bHasFocus;		// Control has input focus
	BOOL				m_bIsDefault;		// Is the default control

	// Size, scale, and positioning members
	int					m_x, m_y;
	int					m_width, m_height;

	// These members are set by the container
	cUIDialog*			m_pDialog;			// Parent container
	UINT				m_Index;			// Index within the control list

protected:
	virtual void UpdateRects();

	int					m_IDC;				// control ID number
	int					m_IDE;				// element ID number
	UI_CONTROL_TYPE		m_Type;				// Control type, set once in constructor
	UINT				m_nHotkey;			// Virtual key code for this control's hotkey
	void*				m_pUserData;		// Data associated with this control that is set by user.
	BOOL				m_bEnabled;			// Enable/disabled flag
	RECT				m_rcBoundingBox;	// Rectangle defining the active region of the control
};

#include "UIStatic.h"

#endif

