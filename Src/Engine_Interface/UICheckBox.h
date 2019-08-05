
#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

//-----------------------------------------------------------------------------
// CheckBox control
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUICheckBox : public cUIButton
{
public:
	cUICheckBox( cUIDialog *pDialog = NULL );

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey() { SetCheckedInternal( !m_bChecked, TRUE ); }

	virtual BOOL ContainsPoint( POINT pt );
	virtual void UpdateRects();

	virtual void Render( IDirect3DDevice9* pd3dDevice );

	BOOL GetChecked() { return m_bChecked; }
	void SetChecked( BOOL bChecked ) { SetCheckedInternal( bChecked, FALSE ); }

protected:
	virtual void SetCheckedInternal( BOOL bChecked, BOOL bFromInput );

	BOOL m_bChecked;
	RECT m_rcButton;
	RECT m_rcText;
};

#endif

