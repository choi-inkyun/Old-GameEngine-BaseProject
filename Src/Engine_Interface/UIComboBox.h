
#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

//-----------------------------------------------------------------------------
// cUIComboBox control
//-----------------------------------------------------------------------------
struct sUIComboBoxItem
{
	char strText[256];
	void* pData;

	RECT rcActive;
	BOOL bVisible;
};

class __declspec(dllexport) cUIComboBox : public cUIButton
{
public:
	cUIComboBox( cUIDialog *pDialog = NULL );
	virtual ~cUIComboBox();

	virtual void SetTextColor( D3DCOLOR Color );
	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();

	virtual BOOL CanHaveFocus() { return ( m_bVisible && m_bEnabled ); }
	virtual void OnFocusOut();
	virtual void Render( IDirect3DDevice9* pd3dDevice );

	virtual void UpdateRects();

	void	SetScrollBarSize( int nSBWidth, int nSBHeight, int nSBThumbWidth, int nSBThumbHeight, int nSBOffset = 0 );	
	void	SetScrollBarIDE( UINT IDEUp, UINT IDEDown, UINT IDEThumb );

	HRESULT	AddItem( const char* strText, void* pData );
	void	RemoveAllItems();
	void	RemoveItem( UINT index );
	BOOL	ContainsItem( const char* strText, UINT iStart = 0 );
	int		FindItem( const char* strText, UINT iStart = 0 );
	void*	GetItemData( const char* strText );
	void	SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
	void	SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

	void*	GetSelectedData();
	sUIComboBoxItem* GetSelectedItem();

	UINT	GetNumItems() { return m_Items.GetSize(); }
	sUIComboBoxItem* GetItem( UINT index ) { return m_Items.GetAt( index ); }

	HRESULT SetSelectedByIndex( UINT index );
	HRESULT SetSelectedByText( const char* strText );
	HRESULT SetSelectedByData( void* pData );

	void	SetComboBoxIDE( UINT IDEText, UINT IDEButton, UINT IDEDropdown, UINT IDESelection );
	void	SetMargin( UINT nLeftMargin, UINT nTopMargin );

protected:
	int		m_iSelected;
	int		m_iFocused;
	int		m_nDropHeight;
	cUIScrollBar m_ScrollBar;
	int		m_nSBWidth;

	BOOL	m_bOpened;

	int		m_nLeftMargin;			// 왼쪽 여백
	int		m_nTopMargin;			// 위쪽 여백(드롭다운 영역에서)

	RECT	m_rcMain;				// 메인 영역
	RECT	m_rcMainText;			// 메인 텍스트 영역
	RECT	m_rcButton;				// 보턴 영역
	RECT	m_rcDropdown;			// 드롭다운 영역
	RECT	m_rcDropdownText;		// 드롭다운 텍스트 영역

	UINT	m_IDEText;				// 메인 텍스트 이미지와 폰트
	UINT	m_IDEButton;			// 보턴 이미지
	UINT	m_IDEDropdown;			// 드롭다운 이미지와 폰트
	UINT	m_IDESelection;			// 선택된 항목 이미지과 폰트

	cGrowableArray< sUIComboBoxItem* > m_Items;
};

#endif

