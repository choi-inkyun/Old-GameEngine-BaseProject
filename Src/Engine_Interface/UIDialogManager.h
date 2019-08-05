
#ifndef __UIDIALOGMANAGER_H__
#define __UIDIALOGMANAGER_H__

//--------------------------------------------------------------------------------------
// Structuress for shared resources.
//--------------------------------------------------------------------------------------
struct sUITextureNode
{
    LPCSTR strFilename;
    IDirect3DTexture9* pTexture;
    DWORD dwWidth;
    DWORD dwHeight;
};

struct sUIFontNode
{
    LPCSTR strFace;
    ID3DXFont* pFont;
    LONG  nHeight;
    LONG  nWeight;
};

class _init_CUIDialogM;


//-----------------------------------------------------------------------------
// Manages shared resources of dialogs
// Use UIGetDialogManager() to get access to the global instance
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIDialogManager
{
friend _init_CUIDialogM;
public:
    int AddFont( LPCSTR strFaceName, LONG height, LONG weight );
    int AddTexture( LPCSTR strFilename );

    sUIFontNode*      GetFontNode( int iIndex )     { return m_FontCache.GetAt( iIndex ); };
    sUITextureNode*   GetTextureNode( int iIndex )  { return m_TextureCache.GetAt( iIndex ); };
	HWND              GetHWND()						{ return m_hWnd; }
    IDirect3DDevice9* GetD3DDevice()                { return m_pd3dDevice; }
	double            GetTime()						{ return timeGetTime() / 1000.0f; }

    // Shared between all dialogs
    IDirect3DStateBlock9* m_pStateBlock;
    ID3DXSprite*          m_pSprite;			// Sprite used for drawing

	D3DCOLOR			  m_ColorKey;			// Transparent color for sprite

    ~cUIDialogManager();

	// The sample framework uses the global instance and automatically calls the device events
    HRESULT     OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT     OnResetDevice( LPDIRECT3DDEVICE9 pd3dDevice );
    void        OnLostDevice();
    void        OnDestroyDevice();

//	friend cUIDialogManager* UIGetDialogManager();
protected:
 
	// Use UIGetDialogManager() to get access to the global instance
    cUIDialogManager();

    cGrowableArray< sUITextureNode* > m_TextureCache;   // Shared textures
    cGrowableArray< sUIFontNode*    > m_FontCache;      // Shared fonts
    
    IDirect3DDevice9*	m_pd3dDevice;
	HWND				m_hWnd;

    // Resource creation helpers
    HRESULT CreateFont( UINT index );
    HRESULT CreateTexture( UINT index, D3DCOLOR ColorKey = 0 );
};



//cUIDialogManager* __declspec(dllexport) UIGetDialogManager();

////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CUIDialogM[sizeof(cUIDialogManager)];
extern __declspec(dllimport) _CUIDialogM		cUIDialogMBlock;
#define g_cUIDialogManager		(*(cUIDialogManager*) cUIDialogMBlock)

// 教臂沛阑 积己秦具窃.
#ifdef ENGINE_INTERFACE_EXPORTS

// initializer dummy
class _init_CUIDialogM {
protected:
	static unsigned long			ms_dwLong;

public:
	_init_CUIDialogM()
	{
		if (ms_dwLong == 0)
		{
			new (cUIDialogMBlock) cUIDialogManager;
		}

		ms_dwLong++;
	}
	~_init_CUIDialogM()
	{
		if (ms_dwLong == 1)
		{
			((cUIDialogManager*) cUIDialogMBlock)->~cUIDialogManager();
		}

		ms_dwLong--;
	}
};

// declare CEngineMainApp singleton instance
static _init_CUIDialogM			_init_cUIDialogM;

#endif



#endif

