
#include "StdAfx.h"
#include "D3DGraphics.h"

//---------------------------------------------------------------------------//

cGraphics::cGraphics( cD3DDevice *pDevice )
{
	m_pD3DDevice	= pDevice;
}

cGraphics::~cGraphics()
{
	m_pD3DDevice	= NULL;
}

//---------------------------------------------------------------------------//

IDirect3DDevice9*	cGraphics::GetDeviceCOM() const { return m_pD3DDevice->GetDeviceCOM(); }
ID3DXSprite*		cGraphics::GetSpriteCOM() const { return m_pD3DDevice->GetSpriteCOM(); }

//---------------------------------------------------------------------------//

BOOL cGraphics::BeginScene()
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;

	if( FAILED( m_pD3DDevice->GetDeviceCOM()->BeginScene() ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::EndScene()
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;

	// release all textures
	for( int stage = 0; stage < 8; stage++ ) m_pD3DDevice->GetDeviceCOM()->SetTexture( stage, NULL );

	// End the scene
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->EndScene() ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::BeginSprite()
{
	if( m_pD3DDevice->GetSpriteCOM() == NULL ) return FALSE;

#ifdef _DIRECTX_8_0_
	if( FAILED( m_pD3DDevice->GetSpriteCOM()->Begin() ) ) return FALSE;
#endif
	if( FAILED( m_pD3DDevice->GetSpriteCOM()->Begin( D3DXSPRITE_ALPHABLEND ) ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::EndSprite()
{
	if( m_pD3DDevice->GetSpriteCOM() == NULL ) return FALSE;

	if( FAILED( m_pD3DDevice->GetSpriteCOM()->End() ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::Clear( long Color, float ZBuffer )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;

	// clear screen only if no zbuffer
	if( m_pD3DDevice->IsAutoDepthStencilEnabled() == FALSE ) return ClearDisplay( Color );

	// clear display and zbuffer
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, ZBuffer, 0 ) ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::ClearDisplay( long Color )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->Clear( 0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0 ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::ClearZBuffer( float ZBuffer )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL || !m_pD3DDevice->IsAutoDepthStencilEnabled() ) return FALSE;

	if( FAILED( m_pD3DDevice->GetDeviceCOM()->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0 ) ) ) return FALSE;

	return TRUE;
}

BOOL cGraphics::Display()
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->Present( NULL, NULL, NULL, NULL ) ) ) return FALSE;
	return TRUE;
}

//---------------------------------------------------------------------------//

BOOL cGraphics::SetPerspective( float FOV, float Aspect, float Near, float Far )
{
	D3DXMATRIX matProjection;

	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	D3DXMatrixPerspectiveFovLH( &matProjection, FOV, Aspect, Near, Far );
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetTransform( D3DTS_PROJECTION, &matProjection ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableLight( long Num, BOOL Enable )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->LightEnable( Num, Enable ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableLighting( BOOL Enable )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_LIGHTING, Enable ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableZBuffer( BOOL Enable )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL || !m_pD3DDevice->IsAutoDepthStencilEnabled() ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_ZENABLE, ( Enable == TRUE ) ? D3DZB_TRUE : D3DZB_FALSE ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableAlphaBlending( BOOL Enable, DWORD Src, DWORD Dest )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable ) ) ) return FALSE;
	if( Enable == TRUE )
	{
		m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_SRCBLEND,  Src );
		m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_DESTBLEND, Dest );
	}
	return TRUE;
}

BOOL cGraphics::EnableAlphaTesting( BOOL Enable )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_ALPHATESTENABLE, Enable ) ) ) return FALSE;

	// set test type
	if( Enable == TRUE )
	{
		m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_ALPHAREF, 0x08 );
		m_pD3DDevice->GetDeviceCOM()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	}

	return TRUE;
}

//---------------------------------------------------------------------------//

BOOL cGraphics::SetWorldPosition( cWorldPosition *WorldPos )
{
	if( WorldPos == NULL || m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetTransform( D3DTS_WORLD, WorldPos->GetMatrix(this) ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::SetCamera( cCamera *Camera )
{
	if( Camera == NULL || m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;
	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetTransform( D3DTS_VIEW, Camera->GetMatrix() ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::SetLight(long Num, cLight *Light)
{
	if( Light == NULL ) return FALSE;
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;

	if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetLight( Num, Light->GetLight() ) ) ) return FALSE;
	return TRUE;
}

BOOL cGraphics::SetMaterial( cMaterial *Material )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL ) return FALSE;

	if( Material != NULL )
	{
		if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetMaterial( Material->GetMaterial() ) ) ) return FALSE;
	}

	return TRUE;
}

BOOL cGraphics::SetTexture( short Num, cTexture *Texture )
{
	if( m_pD3DDevice->GetDeviceCOM() == NULL || Num < 0 || Num > 7 ) return FALSE;

	if( Texture == NULL )
	{
		// Clear the texture
		if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetTexture( Num, NULL ) ) ) return FALSE;
	}
	else
	{
		// Set the texture
		if( FAILED( m_pD3DDevice->GetDeviceCOM()->SetTexture( Num, Texture->GetTextureCOM() ) ) ) return FALSE;
	}

	return TRUE;
}

