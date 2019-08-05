
#include "StdAfx.h"
#include "UIDialogManager.h"

//--------------------------------------------------------------------------------------
// Global/Static Members
//--------------------------------------------------------------------------------------
_CUIDialogM			cUIDialogMBlock;
unsigned long		_init_CUIDialogM::ms_dwLong;


//--------------------------------------------------------------------------------------
// cUIDialogManager class member functions
//--------------------------------------------------------------------------------------

cUIDialogManager::cUIDialogManager()
{
	m_hWnd			= NULL;
    m_pd3dDevice	= NULL;
    m_pStateBlock	= NULL;
    m_pSprite		= NULL;

	m_ColorKey		= D3DCOLOR_RGBA(255,0,255,255);		// Transparent color
}

cUIDialogManager::~cUIDialogManager()
{
    int i;
    for( i=0; i < m_FontCache.GetSize(); i++ )
    {
        sUIFontNode* pFontNode = m_FontCache.GetAt( i );
        SAFE_DELETE( pFontNode );
    }
    m_FontCache.RemoveAll();   

    for( i=0; i < m_TextureCache.GetSize(); i++ )
    {
        sUITextureNode* pTextureNode = m_TextureCache.GetAt( i );
        SAFE_DELETE( pTextureNode );
    }
    m_TextureCache.RemoveAll();   
}

HRESULT cUIDialogManager::OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr = S_OK;
	int i=0;

	m_hWnd		 = hWnd;
    m_pd3dDevice = pd3dDevice;

    for( i=0; i < m_FontCache.GetSize(); i++ )
    {
        hr = CreateFont( i );
        if( FAILED(hr) ) return hr;
    }
    
    for( i=0; i < m_TextureCache.GetSize(); i++ )
    {
        hr = CreateTexture( i, m_ColorKey );
        if( FAILED(hr) ) return hr;
    }

    hr = D3DXCreateSprite( pd3dDevice, &m_pSprite );
    if( FAILED( hr ) ) return hr;

    // Call cUIIMEEditBox's StaticOnCreateDevice()
    // to initialize certain window-dependent data.
//FIXME    cUIIMEEditBox::StaticOnCreateDevice();

    return S_OK;
}

HRESULT cUIDialogManager::OnResetDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
    for( int i=0; i < m_FontCache.GetSize(); i++ )
    {
        sUIFontNode* pFontNode = m_FontCache.GetAt( i );

        if( pFontNode->pFont ) pFontNode->pFont->OnResetDevice();
    }

    if( m_pSprite ) m_pSprite->OnResetDevice();

    pd3dDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock );

    return S_OK;
}

void cUIDialogManager::OnLostDevice()
{
    for( int i=0; i < m_FontCache.GetSize(); i++ )
    {
        sUIFontNode* pFontNode = m_FontCache.GetAt( i );

        if( pFontNode->pFont ) pFontNode->pFont->OnLostDevice();
    }

    if( m_pSprite ) m_pSprite->OnLostDevice();

    SAFE_RELEASE( m_pStateBlock  );
}

void cUIDialogManager::OnDestroyDevice()
{
    int  i; 

    m_pd3dDevice = NULL;

    // Release the resources but don't clear the cache, as these will need to be
    // recreated if the device is recreated
    for( i = 0; i < m_FontCache.GetSize(); i++ )
    {
        sUIFontNode* pFontNode = m_FontCache.GetAt( i );
        SAFE_RELEASE( pFontNode->pFont );
    }
    
    for( i = 0; i < m_TextureCache.GetSize(); i++ )
    {
        sUITextureNode* pTextureNode = m_TextureCache.GetAt( i );
        SAFE_RELEASE( pTextureNode->pTexture );
    }

    SAFE_RELEASE( m_pSprite );
}

int cUIDialogManager::AddFont( LPCSTR strFaceName, LONG height, LONG weight )
{
    // See if this font already exists
    for( int i = 0; i < m_FontCache.GetSize(); i++ )
    {
        sUIFontNode* pFontNode = m_FontCache.GetAt(i);
 //       if( 0 == _wcsnicmp( pFontNode->strFace, strFaceName, MAX_PATH-1 ) &&
 //           pFontNode->nHeight == height &&
 //           pFontNode->nWeight == weight )
		  if( 0 == strcmp( pFontNode->strFace, (LPCSTR)strFaceName) &&
			              pFontNode->nHeight == height &&
			              pFontNode->nWeight == weight )
        {
            return i;
        }
    }

    // Add a new font and try to create it
    sUIFontNode* pNewFontNode = new sUIFontNode();
    if( pNewFontNode == NULL ) return -1;

 //   ZeroMemory( pNewFontNode, sizeof(sUIFontNode) );
//    strncpy( pNewFontNode->strFace, strFaceName, MAX_PATH-1 );
	 pNewFontNode->strFace = (LPCSTR)strFaceName;
    pNewFontNode->nHeight = height;
    pNewFontNode->nWeight = weight;
    m_FontCache.Add( pNewFontNode );
    
    int iFont = m_FontCache.GetSize()-1;

    // If a device is available, try to create immediately
    if( m_pd3dDevice ) CreateFont( iFont );

    return iFont;
}

int cUIDialogManager::AddTexture( LPCSTR strFilename )
{
	// See if this texture already exists
	for( int i = 0; i < m_TextureCache.GetSize(); i++ )
	{
		sUITextureNode* pTextureNode = m_TextureCache.GetAt(i);
		// .tmp 확장자면 암호화 된거라 똑같은 파일 이름이 있어도 괜찮다.
		if( 0 == strcmp (pTextureNode->strFilename, (LPCSTR)strFilename) && !strstr(pTextureNode->strFilename,".tmp"))
	//	if( 0 == _wcsnicmp( pTextureNode->strFilename, strFilename, MAX_PATH-1 ) )
		{
			return i;
		}
	}

	// Add a new texture and try to create it
	sUITextureNode* pNewTextureNode = new sUITextureNode();
	if( pNewTextureNode == NULL ) return -1;

//	ZeroMemory( pNewTextureNode, sizeof(sUITextureNode) );
	pNewTextureNode->strFilename = (LPCSTR)strFilename;
//	strncpy( pNewTextureNode->strFilename, strFilename, MAX_PATH-1 );
	m_TextureCache.Add( pNewTextureNode );

	int iTexture = m_TextureCache.GetSize()-1;

	// If a device is available, try to create immediately.
	if( m_pd3dDevice ) CreateTexture( iTexture, m_ColorKey );

	return iTexture;
}

HRESULT cUIDialogManager::CreateFont( UINT iFont )
{
	sUIFontNode* pFontNode = m_FontCache.GetAt( iFont );

	SAFE_RELEASE( pFontNode->pFont );

	return D3DXCreateFont( m_pd3dDevice, pFontNode->nHeight, 0, pFontNode->nWeight, 1, FALSE, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
							pFontNode->strFace, &pFontNode->pFont );
}

HRESULT cUIDialogManager::CreateTexture( UINT iTexture, D3DCOLOR ColorKey )
{
    HRESULT hr = S_OK;

    sUITextureNode* pTextureNode = m_TextureCache.GetAt( iTexture );
    
    // Make sure there's a texture to create
    if( pTextureNode->strFilename[0] == 0 )
        return S_OK;
    
    // Find the texture on the hard drive
    LPCSTR strPath;
#ifdef _UNUSED_CODE_
    hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, pTextureNode->strFilename );
    if( FAILED(hr) )
    {
        return DXTRACE_ERR( "DXUTFindDXSDKMediaFileCch", hr );
    }
#endif
	strPath = pTextureNode->strFilename;

    // Create texture
    D3DXIMAGE_INFO info;
    hr =  D3DXCreateTextureFromFileEx( m_pd3dDevice, strPath, D3DX_DEFAULT, D3DX_DEFAULT, 
                                       D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
                                       D3DX_DEFAULT, D3DX_DEFAULT, ColorKey, 
                                       &info, NULL, &pTextureNode->pTexture );
    if( FAILED(hr) ) return hr;

    // Store dimensions
    pTextureNode->dwWidth = info.Width;
    pTextureNode->dwHeight = info.Height;

    return S_OK;
}

