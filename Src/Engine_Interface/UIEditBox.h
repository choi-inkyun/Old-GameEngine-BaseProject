
#ifndef __UIEDITBOX_H__
#define __UIEDITBOX_H__

#include <usp10.h>

// Uniscribe does not support strings having bigger-than-16-bits length.
// This means that the string must be less than 65536 characters long,
// including the NULL terminator.
#define UI_MAX_EDITBOXLENGTH			0xFFFF

//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
#define UNISCRIBE_DLLNAME	L"\\usp10.dll"

#define GETPROCADDRESS( Module, APIName, Temp )	\
	Temp = GetProcAddress( Module, #APIName );	\
	if( Temp ) *(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName )	\
	_##APIName = Dummy_##APIName

class __declspec(dllexport) cUIEditBox : public cUIControl
{
	friend class cExternalApiInitializer;

public:
	cUIEditBox( cUIDialog *pDialog = NULL );
	virtual ~cUIEditBox();

	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual BOOL MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual BOOL CanHaveFocus() { return ( m_bVisible && m_bEnabled ); }
	virtual void Render( IDirect3DDevice9* pd3dDevice );
	virtual void OnFocusIn();

	void SetText( LPCSTR wszText, BOOL bSelected = FALSE );
	LPCSTR GetText() { return m_Buffer.GetBuffer(); }
	int  GetTextLength() { return m_Buffer.GetTextSize(); }	// returns text length in chars excluding NULL.
	HRESULT GetTextCopy( LPSTR strDest, UINT bufferCount );
	void ClearText();
	virtual void SetTextColor( D3DCOLOR Color ) { m_TextColor = Color; }	// Text color
	void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }	// Selected text color
	void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }	// Selected background color
	void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }			// Caret color
	void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }	// Border of the window
	void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
	void ParseFloatArray( float *pNumbers, int nCount );
	void SetTextFloatArray( const float *pNumbers, int nCount );

public:
	//-----------------------------------------------------------------------------
	// cUniBuffer class for the edit control
	//-----------------------------------------------------------------------------
	class __declspec(dllexport) cUniBuffer
	{
		friend class cExternalApiInitializer;

		// static member

		// Empty implementation of the Uniscribe API
		static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
		static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void*, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
		static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
		static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
		static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
		static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
		static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

		// Function pointers
		static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
		static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void*, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
		static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
		static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
		static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
		static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
		static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

		static void InitializeUniscribe();
		static void UninitializeUniscribe();

		static HINSTANCE s_hDll;	// Uniscribe DLL handle

	// Instance data
	private:
		char *m_pwszBuffer;		// Buffer to hold text
		int    m_nBufferSize;		// Size of the buffer allocated, in characters
		int    m_nTextSize;			// Size of the text in buffer excluding NULL terminator, in characters

		// Uniscribe-specific
		int    m_iFont;				// Font index of this buffer
		BOOL   m_bAnalyseRequired;	// True if the string has changed since last analysis
		SCRIPT_STRING_ANALYSIS m_Analysis;	// Analysis for the current string

	private:
		BOOL Grow( int nNewSize = -1 );

		// Uniscribe -- Analyse() analyses the string in the buffer
		HRESULT Analyse();

	public:
		cUniBuffer( int nInitialSize = 1 );
		~cUniBuffer();
		int  GetBufferSize() { return m_nBufferSize; }
		BOOL SetBufferSize( int nSize );
		int  GetTextSize() { return m_nTextSize; }
		const char *GetBuffer() { return m_pwszBuffer; }
		const char &operator[]( int n ) const { return m_pwszBuffer[n]; }
		char &operator[]( int n );
		int  GetFontIndex() { return m_iFont; }
		void SetFontIndex( int iFont ) { m_iFont = iFont; }
		void Clear();

		// Inserts the char at specified index.
		// If nIndex == -1, insert to the end.
		BOOL InsertChar( int nIndex, char wchar );

		// Removes the char at specified index.
		// If nIndex == -1, remove the last char.
		BOOL RemoveChar( int nIndex );

		// Inserts the first nCount characters of the string pStr at specified index.
		// If nCount == -1, the entire string is inserted.
		// If nIndex == -1, insert to the end.
		BOOL InsertString( int nIndex, const char *pStr, int nCount = -1 );

		BOOL SetText( LPCSTR wszText );

		// Uniscribe
		HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
		HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
		void GetPriorItemPos( int nCP, int *pPrior );
		void GetNextItemPos( int nCP, int *pPrior );
	};

protected:
	void PlaceCaret( int nCP );
	void DeleteSelectionText();
	void ResetCaretBlink();
	void CopyToClipboard();
	void PasteFromClipboard();

	cUniBuffer	m_Buffer;			// Buffer to hold text
	int			m_nBorder;			// Border of the window
	int			m_nSpacing;			// Spacing between the text and the edge of border
	RECT		m_rcText;			// Bounding rectangle for the text
	RECT		m_rcRender[9];		// Convenient rectangles for rendering elements
	double		m_dfBlink;			// Caret blink time in milliseconds
	double		m_dfLastBlink;		// Last timestamp of caret blink
	BOOL		m_bCaretOn;			// Flag to indicate whether caret is currently visible
	int			m_nCaret;			// Caret position, in characters
	BOOL		m_bInsertMode;		// If true, control is in insert mode. Else, overwrite mode.
	int			m_nSelStart;		// Starting position of the selection. The caret marks the end.
	int			m_nFirstVisible;	// First visible character in the edit control
	D3DCOLOR	m_TextColor;		// Text color
	D3DCOLOR	m_SelTextColor;		// Selected text color
	D3DCOLOR	m_SelBkColor;		// Selected background color
	D3DCOLOR	m_CaretColor;		// Caret color

	// Mouse-specific
	BOOL		m_bMouseDrag;		// True to indicate drag in progress

	// Static
	static BOOL s_bHideCaret;		// If true, we don't render the caret.
};


#endif

