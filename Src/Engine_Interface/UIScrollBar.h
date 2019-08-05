
#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

//-----------------------------------------------------------------------------
// Scrollbar control
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIScrollBar : public cUIControl
{
public:
	cUIScrollBar( cUIDialog *pDialog = NULL );
	virtual ~cUIScrollBar();

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void Render( IDirect3DDevice9* pd3dDevice );
	virtual void UpdateRects();

	void SetTrackRange( int nStart, int nEnd );
	int  GetTrackPos() { return m_nPosition; }
	void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
	int  GetPageSize() { return m_nPageSize; }
	void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

	void Scroll( int nDelta );		// Scroll by nDelta items ( plus or minus )
	void ShowItem( int nIndex );	// Ensure that item nIndex is displayed, scroll if necessary.

	void SetHeight( int nButtonHeight, int nThumbHeight );
	void SetThumbWidth( int nOffset, int nWidth );
	void SetButtonIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb );

protected:
	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR			No arrow is down.
	// CLICKED_UP		Up arrow is clicked.
	// CLICKED_DOWN		Down arrow is clicked.
	// HELD_UP			Up arrow is held down for sustained period.
	// HELD_DOWN		Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

	virtual void UpdateThumbRect();
	void Cap();						// Clips positions at boundaries. Ensures it stays within legal range.

	BOOL m_bShowThumb;
	RECT m_rcUpButton;
	RECT m_rcDownButton;
	RECT m_rcTrack;
	RECT m_rcThumb;
	int  m_nPosition;				// Position of the first displayed item
	int  m_nPageSize;				// How many items are displayable in one page.
	int  m_nStart;					// First item
	int  m_nEnd;					// The index after the last item
	POINT m_LastMouse;				// Last mouse position
	ARROWSTATE m_Arrow;				// State of the arrows
	double m_dArrowTS;				// Timestamp of last arrow event.

	int  m_nButtonHeight;			// ��/�Ʒ� ���� ����
	int  m_nThumbOffset;			// ���� ���� ��ġ offset
	int  m_nThumbWidth;				// ���� ���� �ʺ�
	int  m_nThumbHeight;			// ���� ���� ����

	UINT m_IDEUpButton;				// ���� ���� �̹���
	UINT m_IDEDownButton;			// �Ʒ��� ���� �̹���
	UINT m_IDEThumbButton;			// ���� ���� �̹���

	BOOL m_bUpButtonMouseOver;		// ���� ���� ���콺����
	BOOL m_bDownButtonMouseOver;	// �Ʒ��� ���� ���콺����
	BOOL m_bThumbButtonMouseOver;	// ���� ���� ���콺����
};

#endif

