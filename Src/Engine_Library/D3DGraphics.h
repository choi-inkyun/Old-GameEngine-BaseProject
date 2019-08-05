
#ifndef __ENGINE_D3DGRAPHICS_H__
#define __ENGINE_D3DGRAPHICS_H__

#include "D3DDevice.h"

// forward declarations
class cVertexBuffer;
class cMaterial;
class cTexture;
class cLight;
class cFont;
class cWorldPosition;
class cCamera;

class __declspec(dllexport) cGraphics
{
private:
	cD3DDevice*			m_pD3DDevice;

public:
	cGraphics( cD3DDevice *pD3DDevice );
	~cGraphics();

	IDirect3DDevice9*	GetDeviceCOM() const;
	ID3DXSprite*		GetSpriteCOM() const;

	BOOL BeginScene();
	BOOL EndScene();

	BOOL BeginSprite();
	BOOL EndSprite();

	BOOL Clear( long Color = 0, float ZBuffer = 1.0f );
	BOOL ClearDisplay( long Color = 0 );
	BOOL ClearZBuffer( float ZBuffer = 1.0f );

	BOOL Display();

	BOOL SetPerspective( float FOV = D3DX_PI / 4.0f, float Aspect = 1.3333f, float Near = 1.0f, float Far = 10000.0f );

	BOOL SetWorldPosition( cWorldPosition *WorldPos );
	BOOL SetCamera( cCamera *Camera );
	BOOL SetLight(long Num, cLight *Light);
	BOOL SetMaterial( cMaterial *Material );
	BOOL SetTexture( short Num, cTexture *Texture );

	BOOL EnableLight( long Num, BOOL Enable = TRUE );
	BOOL EnableLighting( BOOL Enable = TRUE );
	BOOL EnableZBuffer( BOOL Enable = TRUE );
	BOOL EnableAlphaBlending( BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA );
	BOOL EnableAlphaTesting( BOOL Enable = TRUE );
};

#endif
