
#ifndef __ENGINE_D3DDEVICE_H__
#define __ENGINE_D3DDEVICE_H__

#define S_FAILED							(-1)

#define D3DDEVICEERROR_NODIRECT3D			0x80000001
#define D3DDEVICEERROR_NOWINDOW				0x80000002
#define D3DDEVICEERROR_NOCOMPATIBLEDEVICES	0x80000003

class __declspec(dllexport) cD3DDevice
{
protected:
	D3DPRESENT_PARAMETERS m_d3dpp;					// Parameters for CreateDevice/Reset
    D3DCAPS9			m_d3dCaps;					// Caps for the device
	IDirect3D9*			m_pd3d;						// the main D3D object
	IDirect3DDevice9*	m_pd3dDevice;				// the D3D rendering device
	IDirect3DSurface9*	m_pd3dSurface;
	ID3DXSprite*		m_pSprite;

	HWND				m_hWnd;						// The main app window
	DWORD				m_dwWindowStyle;			// Saved window style for mode switches
	RECT				m_rcWindowRect;				// Saved window bounds for mode switches
	RECT				m_rcClientRect;				// Saved client area size for mode switches

	long				m_Width;
	long				m_Height;
	int					m_BPP;
	BOOL				m_HAL;
	BOOL				m_ZBuffer;
	D3DFORMAT			m_Format;

	BOOL				m_bPaused;
	BOOL				m_Windowed;

public:
	cD3DDevice();
	~cD3DDevice();

	IDirect3D9*			GetDirect3DCOM() const;
	IDirect3DDevice9*	GetDeviceCOM() const;
	IDirect3DSurface9*	GetSurfaceCOM() const;
	ID3DXSprite*		GetSpriteCOM() const;

	BOOL				IsAutoDepthStencilEnabled() const;

	UINT SetMode( HWND hWnd, BOOL Windowed = TRUE, BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0, int BPP = 0 );

	UINT Initialize();
	UINT Release();

	BOOL CheckFormat( D3DFORMAT Format, BOOL Windowed, BOOL HAL );
	int  Format_To_BPP( D3DFORMAT Format );
	BOOL BPP_To_Format( int BPP, D3DFORMAT *fmtFormat, BOOL *fHAL );

	UINT BuildPresentParameters();

	void AdjustWindowForChange();
	void AdjustWindowAfterChange();

	UINT Initialize3DEnvironment();
	UINT Create3DEnvironment();
	UINT Delete3DEnvironment();
	UINT Reset3DEnvironment();
	UINT Cleanup3DEnvironment();

	UINT ToggleMode();
	UINT RestoreMode();
	UINT HandlePossibleSizeChange();

	void Pause( BOOL bPause );
	BOOL IsPaused();

	BOOL Get_Windowed() { return m_Windowed;}
};


#endif

