
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

	int		m_nLeftMargin;			// ���� ����
	int		m_nTopMargin;			// ���� ����(��Ӵٿ� ��������)

	RECT	m_rcMain;				// ���� ����
	RECT	m_rcMainText;			// ���� �ؽ�Ʈ ����
	RECT	m_rcButton;				// ���� ����
	RECT	m_rcDropdown;			// ��Ӵٿ� ����
	RECT	m_rcDropdownText;		// ��Ӵٿ� �ؽ�Ʈ ����

	UINT	m_IDEText;				// ���� �ؽ�Ʈ �̹����� ��Ʈ
	UINT	m_IDEButton;			// ���� �̹���
	UINT	m_IDEDropdown;			// ��Ӵٿ� �̹����� ��Ʈ
	UINT	m_IDESelection;			// ���õ� �׸� �̹����� ��Ʈ

	cGrowableArray< sUIComboBoxItem* > m_Items;
};

#endif

