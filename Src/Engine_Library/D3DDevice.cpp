
#include "StdAfx.h"
#include "D3DDevice.h"

/*-----------------------------------------------------------------------------
 * cD3DDevice CLASS
 *---------------------------------------------------------------------------*/

cD3DDevice::cD3DDevice()
{
	m_pd3d			= NULL;
	m_pd3dDevice	= NULL;
	m_pd3dSurface	= NULL;
	m_pSprite		= NULL;

	m_hWnd			= NULL;

	m_Windowed		= TRUE;
	m_Width			= 0;
	m_Height		= 0;
	m_BPP			= 0;
	m_HAL			= FALSE;
	m_ZBuffer		= FALSE;
	m_Format		= D3DFMT_UNKNOWN;

	m_bPaused		= FALSE;
}

cD3DDevice::~cD3DDevice()
{
	Release();
}

UINT cD3DDevice::Initialize()
{
	Release();

	// Create the Direct3D object
	if( (m_pd3d = Direct3DCreate9( D3D_SDK_VERSION )) == NULL ) return D3DDEVICEERROR_NODIRECT3D;

	return S_OK;
}

UINT cD3DDevice::Release()
{
	Cleanup3DEnvironment();
	SAFE_RELEASE( m_pd3d );

	return S_OK;
}

//---------------------------------------------------------------------------//

BOOL cD3DDevice::CheckFormat( D3DFORMAT Format, BOOL Windowed, BOOL HAL )
{
	if( FAILED( m_pd3d->CheckDeviceType(
			D3DADAPTER_DEFAULT,
			(HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
			Format, 
			Format, 
			Windowed ) ) ) return FALSE;

	return TRUE;
}

int cD3DDevice::Format_To_BPP( D3DFORMAT Format )
{
	switch( Format )
	{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			return 32;						// 32 bit modes
		case D3DFMT_R8G8B8:
			return 24;						// 24 bit modes
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
			return 16;						// 16 bit modes
		case D3DFMT_A8P8:
		case D3DFMT_P8:
			return 8;						// 8 bit modes
		default:
			return 0;
	}
	return 0;
}

BOOL cD3DDevice::BPP_To_Format( int BPP, D3DFORMAT *fmtFormat, BOOL *fHAL )
{
	D3DFORMAT	Format, AltFormat;

	*fHAL = FALSE;

	// Figure display format to use
	switch( BPP )
	{
		case 32:
			Format		= D3DFMT_X8R8G8B8;
			AltFormat	= D3DFMT_X8R8G8B8;
			break;
		case 24:
			Format		= D3DFMT_R8G8B8;
			AltFormat	= D3DFMT_R8G8B8;
			break;
		case 16:
			Format		= D3DFMT_R5G6B5;
			AltFormat	= D3DFMT_X1R5G5B5;
			break;
		case  8:
			Format		= D3DFMT_P8;
			AltFormat	= D3DFMT_P8;
			break;
		default:
			return FALSE;
	}

	// check for HAL device
	if( CheckFormat( Format, FALSE, TRUE ) == TRUE )
	{
		*fHAL = TRUE;
	}
	else
	{
		// check for HAL device in alternate format
		if( CheckFormat( AltFormat, FALSE, TRUE ) == TRUE )
		{
			*fHAL = TRUE;
			Format = AltFormat;
		}
		else
		{
			// Check for Emulation device
			if( CheckFormat( Format, FALSE, FALSE ) == FALSE )
			{
				// Check for Emulation device in alternate format
				if( CheckFormat( AltFormat, FALSE, FALSE ) == FALSE ) return FALSE;
				else Format = AltFormat;
			}
		}
	}

	*fmtFormat = Format;
	return TRUE;
}

//---------------------------------------------------------------------------//

UINT cD3DDevice::BuildPresentParameters()
{
	// clear presentation structure
	ZeroMemory( &m_d3dpp, sizeof(D3DPRESENT_PARAMETERS) );

	m_d3dpp.Windowed						= m_Windowed;
	m_d3dpp.BackBufferCount					= 1;
	m_d3dpp.BackBufferFormat				= m_Format;
	m_d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality				= 0;
	m_d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	m_d3dpp.EnableAutoDepthStencil			= m_ZBuffer;
	m_d3dpp.hDeviceWindow					= m_hWnd;

	// Setup Zbuffer format - 16 bit
	if( m_ZBuffer == TRUE )
	{
		m_d3dpp.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_d3dpp.AutoDepthStencilFormat		= D3DFMT_D16;
	}
	else
	{
		m_d3dpp.Flags						= 0;
	}

	// Setup windowed or fullscreen usage
	if( m_Windowed == TRUE )
	{
		m_d3dpp.BackBufferWidth				= m_Width;
		m_d3dpp.BackBufferHeight			= m_Height;
		m_d3dpp.FullScreen_RefreshRateInHz	= 0;
		m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;	//D3DPRESENT_INTERVAL_IMMEDIATE
	}
	else
	{
		m_d3dpp.BackBufferWidth				= m_Width;
		m_d3dpp.BackBufferHeight			= m_Height;
		m_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
		m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;
	}

	return S_OK;
}

void cD3DDevice::AdjustWindowForChange()
{
	if( m_Windowed )
	{
		// Set windowed-mode style
		SetWindowLong( m_hWnd, GWL_STYLE, m_dwWindowStyle );
	}
	else
	{
		// Set fullscreen-mode style
		SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP /*| WS_CLIPCHILDREN*/ | WS_VISIBLE );
	}
}

void cD3DDevice::AdjustWindowAfterChange()
{
	// When moving from fullscreen to windowed mode, it is important to
	// adjust the window size after resetting the device.
	if( m_Windowed )
	{
		SetWindowPos( m_hWnd, HWND_NOTOPMOST,
					m_rcWindowRect.left, m_rcWindowRect.top, 
					m_rcWindowRect.right - m_rcWindowRect.left,
					m_rcWindowRect.bottom - m_rcWindowRect.top, 
					SWP_SHOWWINDOW );
	}
}

UINT cD3DDevice::Initialize3DEnvironment()
{
	HRESULT hr;

	// Prepare window for possible windowed/fullscreen change
	AdjustWindowForChange();

	// create the Direct3D device object
	hr = m_pd3d->CreateDevice( D3DADAPTER_DEFAULT, ( m_HAL == TRUE ) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
			m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, 
			&m_pd3dDevice );

	if( SUCCEEDED( hr ) )
	{
		// Adjust window size after resetting the device
		AdjustWindowAfterChange();

		return g_cApplication.OnDeviceCreated();
	}

	return hr;
}

UINT cD3DDevice::Create3DEnvironment()
{
	UINT hr;
    
	// Store render target surface desc
	if( FAILED( hr = m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pd3dSurface ) ) ) return hr;

	if( FAILED( hr = D3DXCreateSprite( m_pd3dDevice, &m_pSprite ) ) ) return hr;

	return g_cApplication.OnDeviceReset();
}

UINT cD3DDevice::Delete3DEnvironment()
{
	UINT hr;

	if( FAILED( hr = g_cApplication.OnDeviceLost() ) ) return hr;

	SAFE_RELEASE( m_pSprite );
	SAFE_RELEASE( m_pd3dSurface );

	return S_OK;
}

UINT cD3DDevice::Reset3DEnvironment()
{
	UINT hr;

	// Release all of objects
	Delete3DEnvironment();

	// Prepare window for possible windowed/fullscreen change
	AdjustWindowForChange();

	// Reset the device
	if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) ) return hr;

	// Adjust window size after resetting the device
	AdjustWindowAfterChange();
	
	// Create the objects again
	Create3DEnvironment();

	return S_OK;
}

UINT cD3DDevice::Cleanup3DEnvironment()
{
	UINT hr;

	if( FAILED( hr = Delete3DEnvironment() ) ) return hr;

	if( FAILED( hr = g_cApplication.OnDeviceDestroyed() ) ) return hr;

	SAFE_RELEASE( m_pd3dDevice );
	
	return S_OK;
}

//---------------------------------------------------------------------------//

UINT cD3DDevice::SetMode( HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, int BPP )
{
	if( m_pd3d == NULL ) return D3DDEVICEERROR_NODIRECT3D;
	if( (m_hWnd = hWnd) == NULL ) return D3DDEVICEERROR_NOWINDOW;

	m_Windowed	= Windowed;
	m_ZBuffer	= UseZBuffer;

	D3DDISPLAYMODE	d3ddm;
	UINT			hr;

	if( FAILED( hr = m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) ) return hr;

	m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	GetWindowRect( m_hWnd, &m_rcWindowRect );
	GetClientRect( m_hWnd, &m_rcClientRect );

	// configure width
	if( Width == 0 )
	{
		if( Windowed ) m_Width = m_rcClientRect.right;
		else		   m_Width = d3ddm.Width;
	}
	else m_Width = Width;

	// configure height
	if( Height == 0 )
	{
		if( Windowed ) m_Height = m_rcClientRect.bottom;
		else		   m_Height = d3ddm.Height;
	}
	else m_Height = Height;

	// Configure BPP
	if( BPP == 0 )
	{
		if( !( m_BPP = Format_To_BPP( d3ddm.Format ) ) ) return D3DDEVICEERROR_NOCOMPATIBLEDEVICES;
	}

	// Resize client window if using windowed mode
	if( Windowed == TRUE )
	{
		int WndWidth  = ( m_rcWindowRect.right  - ( m_rcClientRect.right  - m_Width  ) ) - m_rcWindowRect.left;
		int WndHeight = ( m_rcWindowRect.bottom - ( m_rcClientRect.bottom - m_Height ) ) - m_rcWindowRect.top;

		MoveWindow( m_hWnd, m_rcWindowRect.left, m_rcWindowRect.top, WndWidth, WndHeight, TRUE );
		
		GetWindowRect( m_hWnd, &m_rcWindowRect );
		GetClientRect( m_hWnd, &m_rcClientRect );		
	}

	if( !BPP_To_Format( m_BPP, &m_Format, &m_HAL ) ) return D3DDEVICEERROR_NOCOMPATIBLEDEVICES;

	// Setup the presentation parameters
	BuildPresentParameters();

	if( FAILED( hr = Initialize3DEnvironment() ) ) return hr;
	if( FAILED( hr = Create3DEnvironment()     ) ) return hr;

	return S_OK;
}

UINT cD3DDevice::ToggleMode()
{
	// Toggle the windowed state
	m_Windowed = !m_Windowed;

	UINT hr;
	BOOL fHAL;

	if( !BPP_To_Format( m_BPP, &m_Format, &fHAL ) ) return D3DDEVICEERROR_NOCOMPATIBLEDEVICES;

	Pause( TRUE );

	// Setup the presentation parameters
	BuildPresentParameters();

	// If AdapterOrdinal and DevType are the same, we can just do a Reset(),
	// If They've changed, we need to do a complete device teardown/rebuild.
	if( m_HAL == fHAL )
	{
		if( FAILED( hr = Reset3DEnvironment()      ) ) return hr;
	}
	else
	{
		m_HAL = fHAL;

		if( FAILED( hr = Cleanup3DEnvironment()    ) ) return hr;
		if( FAILED( hr = Initialize3DEnvironment() ) ) return hr;
		if( FAILED( hr = Create3DEnvironment()     ) ) return hr;
	}
	
	Pause( FALSE );
	
	return S_OK;
}

UINT cD3DDevice::RestoreMode()
{
	if( !m_pd3dDevice ) return S_FAILED;

	HRESULT hr = m_pd3dDevice->TestCooperativeLevel();
	if( hr == D3DERR_DEVICELOST ) return hr;
	if( hr == D3DERR_DEVICENOTRESET ) 
	{
		if( FAILED( hr = Reset3DEnvironment() ) ) return hr;
	}

	return S_OK;
}

UINT cD3DDevice::HandlePossibleSizeChange()
{
	if( m_Windowed == FALSE ) return S_OK;

	RECT rcClientOld = m_rcClientRect;

	// update window properties
	m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	GetWindowRect( m_hWnd, &m_rcWindowRect );
	GetClientRect( m_hWnd, &m_rcClientRect );
	
	if( m_rcClientRect.right - m_rcClientRect.left != rcClientOld.right - rcClientOld.left ||
		m_rcClientRect.bottom - m_rcClientRect.top != rcClientOld.bottom - rcClientOld.top )
	{
		Pause( TRUE );

		m_d3dpp.BackBufferWidth		= m_rcClientRect.right - m_rcClientRect.left;
		m_d3dpp.BackBufferHeight	= m_rcClientRect.bottom - m_rcClientRect.top;

		if( m_pd3dDevice != NULL )
		{
			UINT hr;
			if( FAILED( hr = Reset3DEnvironment() ) ) return hr;
		}

		Pause( FALSE );
	}

	return S_OK;
}


//---------------------------------------------------------------------------//

void cD3DDevice::Pause( BOOL bPause )
{
	static DWORD dwPausedCount = 0;

	dwPausedCount += ( bPause ? +1 : -1 );
	m_bPaused	   = ( dwPausedCount ? TRUE : FALSE );
}

BOOL cD3DDevice::IsPaused() { return m_bPaused; }

//---------------------------------------------------------------------------//

IDirect3D9*			cD3DDevice::GetDirect3DCOM() const { return m_pd3d; }
IDirect3DDevice9*	cD3DDevice::GetDeviceCOM()   const { return m_pd3dDevice; }
IDirect3DSurface9*	cD3DDevice::GetSurfaceCOM()  const { return m_pd3dSurface; }
ID3DXSprite*		cD3DDevice::GetSpriteCOM()   const { return m_pSprite; }

BOOL				cD3DDevice::IsAutoDepthStencilEnabled() const { return m_ZBuffer; }

//---------------------------------------------------------------------------//

