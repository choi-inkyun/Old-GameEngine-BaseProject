
#ifndef __UIDIALOG_H__
#define __UIDIALOG_H__

#include "UIDialogManager.h"

//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED				0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED	0x0201
#define EVENT_RADIOBUTTON_CHANGED			0x0301
#define EVENT_CHECKBOX_CHANGED				0x0401
#define EVENT_SLIDER_VALUE_CHANGED			0x0501
#define EVENT_EDITBOX_STRING				0x0601
#define EVENT_EDITBOX_CHANGE				0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK			0x0701
#define EVENT_LISTBOX_SELECTION				0x0702


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class cUIElement;
class cUIControl;
class cUIStatic;
class cUIButton;
class cUICheckBox;
class cUIRadioButton;
class cUIScrollBar;
class cUIListBox;
class cUIComboBox;
class cUISlider;
class cUIEditBox;
class cUIIMEEditBox;

struct sUIElementHolder;


//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum UI_CONTROL_TYPE 
{ 
    UI_CONTROL_STATIC,
	UI_CONTROL_BUTTON, 
    UI_CONTROL_CHECKBOX,
    UI_CONTROL_RADIOBUTTON,
    UI_CONTROL_SCROLLBAR,
    UI_CONTROL_LISTBOX,
	UI_CONTROL_COMBOBOX,
    UI_CONTROL_SLIDER,
    UI_CONTROL_EDITBOX,
    UI_CONTROL_IMEEDITBOX,
};

#define MAX_CONTROL_STATES     7

enum UI_CONTROL_STATE
{
    UI_STATE_NORMAL,
    UI_STATE_HIDDEN,
    UI_STATE_DISABLED,
    UI_STATE_FOCUS,
    UI_STATE_MOUSEOVER,
    UI_STATE_PRESSED,
	UI_STATE_SELECTED,
};

typedef void (CALLBACK *PCALLBACKUIDIALOGPROC)( UINT nDialogID, int nControlID, UINT nEvent, cUIControl *pControl );
typedef void (CALLBACK *PCALLBACKUICONTROLEVENT)( int nControlID, UINT nEvent, cUIControl *pControl );

inline int RectWidth ( RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN	0x020B
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP	0x020C
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA		120
#endif


//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIElement
{
public:
    void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
    void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void Refresh();
    
    UINT iTexture;          // Index of the texture for this Element 
    UINT iFont;             // Index of the font for this Element
    DWORD dwTextFormat;     // The format argument to DrawText 

    RECT rcTexture;         // Bounding rect of this element on the composite texture
    
	D3DCOLOR TextureColor;
    D3DCOLOR FontColor;
};


//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct sUIResourceHolder
{
	UINT nResourceId;		// 자원 식별자
	UINT nResourceState;	// 자원 상태

	cUIElement Element;
};

struct sUIImageHolder
{
	UINT nImageId;

	cUIElement Element;
};


//-----------------------------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIDialog
{
private:
    BOOL m_bCaption;
    BOOL m_bMinimized;
    char m_wszCaption[256];

	int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_nCaptionHeight;

    D3DCOLOR m_colorTopLeft;
    D3DCOLOR m_colorTopRight;
    D3DCOLOR m_colorBottomLeft;
    D3DCOLOR m_colorBottomRight;

	cGrowableArray< int > m_Textures;   // Index into m_TextureCache;
    cGrowableArray< int > m_Fonts;      // Index into m_FontCache;

	cGrowableArray< cUIControl* >		m_Controls;
	cGrowableArray< sUIResourceHolder* > m_ResourceElements;
	cGrowableArray< sUIImageHolder* >	m_ImageElements;

	cUIDialog* m_pNextDialog;
	cUIDialog* m_pPrevDialog;

	int  m_nDefaultControlID;

	static cUIControl* s_pControlFocus;		// The control which has focus
	static cUIControl* s_pControlPressed;	// The control currently pressed

	cUIControl* m_pControlMouseOver;		// The control which is hovered over

	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	PCALLBACKUICONTROLEVENT m_pCallbackEvent;

	// Windows message hanlders
	void OnMouseMove( POINT pt );
	void OnMouseUp( POINT pt );

	// Control events
	void OnCycleFocus( BOOL bForward );
	void OnMouseEnter( cUIControl* pControl );
	void OnMouseLeave( cUIControl* pControl );

protected:
	cUIElement m_CapElement;			// Element for the caption

	// Initialize default Elements
    virtual void InitDefaultElements() = 0;

public:
	cUIDialog();
	~cUIDialog();

	void RemoveControl( int IDC );
	void RemoveAllControls();

    // Access the display Elements used when adding new controls
	HRESULT       SetResourceElement( UINT nResourceId, UINT nResourceState, cUIElement* pElement );
	cUIElement*   GetResourceElement( UINT nResourceId, UINT nResourceState );

	HRESULT       SetImageElement( UINT nImageId, cUIElement* pElement );
	cUIElement*   GetImageElement( UINT nImageId );

	// Windows message handler
	BOOL MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Methods called by controls
	void SendEvent( UINT hEvent, BOOL bTriggeredByUser, cUIControl* pControl );
	void RequestFocus( cUIControl* pControl );

	static void ClearFocus();
	void FocusDefaultControl();

	// Sets the callback used to notify the app of control events
	BOOL m_bNonUserEvents;
	BOOL m_bKeyboardInput;
	BOOL m_bMouseInput;

	// Sets the callback used to notify the app of control events
	void SetCallback( PCALLBACKUICONTROLEVENT pCallback ) { m_pCallbackEvent = pCallback; }
	void EnableNonUserEvents( BOOL bEnable ) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput( BOOL bEnable ) { m_bKeyboardInput = bEnable; }
	void EnableMouseInput   ( BOOL bEnable ) { m_bMouseInput    = bEnable; }

	// Control creation
	HRESULT AddStatic( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault = FALSE, cUIStatic** ppCreated = NULL );
	HRESULT AddButton( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, UINT nHotkey = 0, BOOL bIsDefault = FALSE, cUIButton** ppCreated = NULL );
	HRESULT AddCheckBox( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bChecked = FALSE, UINT nHotkey = 0, BOOL bIsDefault = FALSE, cUICheckBox** ppCreated = NULL );
//	HRESULT AddRadioButton( int IDC, UINT nButtonGroup, LPCSTR strText, int x, int y, int width, int height, BOOL bChecked = FALSE, UINT nHotkey = 0, BOOL bIsDefault = FALSE, cUIRadioButton** ppCreated = NULL );
	HRESULT AddListBox( int IDC, int IDE, int x, int y, int width, int height, DWORD dwStyle = 0, cUIListBox** ppCreated = NULL);
	HRESULT AddComboBox( int IDC, int IDE, int x, int y, int width, int height, UINT nHotkey = 0, BOOL bIsDefault = FALSE, cUIComboBox** ppCreated = NULL );
//	HRESULT AddSlider( int IDC, int x, int y, int width, int height, int min = 0, int max = 100, int value = 50, BOOL bIsDefault = FALSE, cUISlider** ppCreated = NULL );
	HRESULT AddEditBox( int IDC, int IDE, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault = FALSE, cUIEditBox** ppCreated = NULL );
//	HRESULT AddIMEEditBox( int IDC, LPCSTR strText, int x, int y, int width, int height, BOOL bIsDefault = FALSE, cUIIMEEditBox** ppCreated = NULL );
	HRESULT AddUIControl( cUIControl* pControl, int IDC, int IDE, int x, int y, int width, int height, BOOL bIsDefault );
	HRESULT InitControl( cUIControl* pControl );
	HRESULT AddControl ( cUIControl* pControl );

	// Control retrieval
	cUIStatic*		GetStatic( int IDC ) { return (cUIStatic*)GetControl( IDC, UI_CONTROL_STATIC ); }
	cUIButton*		GetButton( int IDC ) { return (cUIButton*)GetControl( IDC, UI_CONTROL_BUTTON ); }
	cUICheckBox*	GetCheckBox( int IDC ) { return (cUICheckBox*)GetControl( IDC, UI_CONTROL_CHECKBOX ); }
	cUIRadioButton* GetRadioButton( int IDC ) { return (cUIRadioButton*)GetControl( IDC, UI_CONTROL_RADIOBUTTON ); }
	cUIListBox*		GetListBox( int IDC ) { return (cUIListBox*)GetControl( IDC, UI_CONTROL_LISTBOX ); }
	cUIComboBox*	GetComboBox( int IDC ) { return (cUIComboBox*)GetControl( IDC, UI_CONTROL_COMBOBOX ); }
	cUISlider*		GetSlider( int IDC ) { return (cUISlider*)GetControl( IDC, UI_CONTROL_SLIDER ); }
	cUIEditBox*		GetEditBox( int IDC ) { return (cUIEditBox*)GetControl( IDC, UI_CONTROL_EDITBOX ); }
	cUIIMEEditBox*	GetIMEEditBox( int IDC ) { return (cUIIMEEditBox*)GetControl( IDC, UI_CONTROL_IMEEDITBOX ); }

	cUIControl* GetControl( int IDC );
	cUIControl* GetControl( int IDC, UINT nControlType );
	cUIControl* GetControlAtPoint( POINT pt );

	BOOL GetControlEnabled( int IDC );
	void SetControlEnabled( int IDC, BOOL bEnabled );

	static cUIControl* GetNextControl( cUIControl* pControl );
	static cUIControl* GetPrevControl( cUIControl* pControl );

	void ClearRadioButtonGroup( UINT nButtonGroup );
	void ClearComboBox( int IDC );

    // Render helpers
    HRESULT DrawRect( RECT* pRect, D3DCOLOR color );
    HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color );
    HRESULT DrawSprite( cUIElement* pElement, RECT* prcDest );
	HRESULT DrawImage( cUIElement* pElement, int x, int y );
    HRESULT CalcTextRect( LPCSTR strText, cUIElement* pElement, RECT* prcDest, int nCount = -1 );
    HRESULT DrawText( LPCSTR strText, cUIElement* pElement, RECT* prcDest, int nCount = -1 );

	// Attributes
	BOOL GetMinimized() { return m_bMinimized; }
	void SetMinimized( BOOL bMinimized ) { m_bMinimized = bMinimized; }
	void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
	void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
	void EnableCaption( BOOL bEnable ) { m_bCaption = bEnable; }
	int  GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
	void SetCaptionText( const char *pwszText ) { strncpy( m_wszCaption, pwszText, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]) ); m_wszCaption[sizeof(m_wszCaption)/sizeof(m_wszCaption[0])-1] = 0; }
	void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
	void SetLocation( int x, int y ) { m_x = x; m_y = y; }
	void SetSize( int width, int height ) { m_width = width, m_height = height; }
	int  GetWidth() { return m_width; }
	int  GetHeight() { return m_height; }

	void SetNextDialog( cUIDialog* pNextDialog );

	static void SetRefreshTime( float fTime );

	// Device state notification
    void Refresh();
    HRESULT OnRender();

	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.
	HRESULT          SetFont( UINT index, LPCSTR strFaceName, LONG height, LONG weight );
	sUIFontNode*     GetFont( UINT index );
	UINT			 GetFontIndex( UINT index );

	HRESULT          SetTexture( UINT index, LPCSTR strFilename );
	sUITextureNode*  GetTexture( UINT index );
	UINT			 GetTextureIndex( UINT index );
};

#include "UIControl.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIScrollBar.h"
#include "UIListBox.h"
#include "UIComboBox.h"
#include "UIEditBox.h"

#endif

