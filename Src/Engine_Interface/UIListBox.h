
#ifndef __UILISTBOX_H__
#define __UILISTBOX_H__

struct sListBoxItem
{
	char strText[255+1];
	void* pData;

	RECT rcActive;
	BOOL bSelected;
};

class __declspec(dllexport) cUIListBox : public cUIControl
{
public:
	cUIListBox( cUIDialog *pDialog = NULL );
	virtual ~cUIListBox();

	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
	virtual BOOL CanHaveFocus() { return ( m_bVisible && m_bEnabled ); }
	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void Render( IDirect3DDevice9* pd3dDevice );
	virtual void DrawListBoxItem( sListBoxItem *pItem, RECT *prcItem );
	virtual void UpdateRects();

	void SetScrollBarSize( int nSBWidth, int nSBHeight, int nSBThumbWidth, int nSBThumbHeight, int nSBOffset = 0 );	
	void SetScrollBarIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb );
	void SetItemSize( int nLineHeight, int nItemWidth, int nItemHeight, int nItemsPerPage, int nTopMargin, int nLeftMargin );

	int  GetSize() const { return m_Items.GetSize(); }
	void SetStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
	HRESULT AddItem( const char *wszText, void *pData );
	HRESULT InsertItem( int nIndex, const char *wszText, void *pData );
	void RemoveItem( int nIndex );
	void RemoveItemByText( char *wszText );
	void RemoveItemByData( void *pData );
	void RemoveAllItems();
	
	sListBoxItem *GetItem( int nIndex );
	int  GetSelectedIndex( int nPreviousSelected = -1 );
	sListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void SelectItem( int nNewIndex );

protected:
	RECT m_rcText;				// Text rendering bound
	RECT m_rcSelection;			// Selection box bound
	cUIScrollBar m_ScrollBar;
	int  m_nSBWidth;
	int  m_nBorder;
	int  m_nTopMargin;			// 위쪽 여백
	int  m_nLeftMargin;			// 왼쪽 여백
	int  m_nLineHeight;			// Height of a single line.
	int  m_nItemWidth;			// Item 너비
	int  m_nItemHeight;			// Height of a single record.
	int  m_nItemsPerPage;		// Number of records per page.
	DWORD m_dwStyle;			// List box style
	int  m_nSelected;			// Index of the selected item for single selection list box
	BOOL m_bDrag;				// Whether the user is dragging the mouse to select
	int  m_nMouseOverItem;		// Compute the index of the item below cursor
	BOOL m_bAutoScroll;			// Auto scroll after adding a new item.

	cGrowableArray< sListBoxItem* > m_Items;
};

#endif

