
#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIButton : public cUIStatic
{
public:
	cUIButton( cUIDialog *pDialog = NULL );

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey() { m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, TRUE, this ); }

	virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
	virtual BOOL CanHaveFocus() { return ( m_bVisible && m_bEnabled ); }

	virtual void Render( IDirect3DDevice9* pd3dDevice );

protected:
	BOOL m_bPressed;
};

#endif

