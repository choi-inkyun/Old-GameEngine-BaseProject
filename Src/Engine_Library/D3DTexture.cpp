
#include "StdAfx.h"
#include "D3DTexture.h"

/*-----------------------------------------------------------------------------
 * cTexture CLASS
 *---------------------------------------------------------------------------*/

cTexture::cTexture()
{
	m_Graphics = NULL;
	m_Texture = NULL;
	m_Width = m_Height = 0;
}

cTexture::~cTexture()
{
	Unload();
}

BOOL cTexture::Load( cGraphics *Graphics, LPCSTR Filename, DWORD Transparent, D3DFORMAT Format )
{
	D3DXIMAGE_INFO image_info;

	Unload();

	if( (m_Graphics = Graphics) == NULL ) return FALSE;
	if( Graphics->GetDeviceCOM() == NULL ) return FALSE;
	if( Filename == NULL ) return FALSE;

	// Filter:
	//		D3DX_FILTER_NONE - ���ɸ��̳� ���͸��� ���� �ʴ´�.
	//		D3DX_FILTER_TRIANGLE - �� �̹����� �յ��ϰ� ����Ѵ�.
	if( FAILED( D3DXCreateTextureFromFileEx( Graphics->GetDeviceCOM(),
		Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, Format, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE,
		Transparent, &image_info, NULL, &m_Texture ) ) ) return FALSE;


	// ������ �ؽ����� ���̿� ����.
	// ���� ���� �̹����� ���� ū ���� ����� 2�� ��� ���� �����Ѵ�.
	//m_Width  = _GetWidthFromDesc();
	//m_Height = _GetHeightFromDesc();

	// �̹��� ���Ϸκ��� ��� ���� ���̿� ����
	m_Width  = image_info.Width;
	m_Height = image_info.Height;

	return TRUE;
}

BOOL cTexture::Create( cGraphics *Graphics, IDirect3DTexture9 *Texture )
{
	D3DLOCKED_RECT SrcRect, DestRect;
	D3DSURFACE_DESC Desc;

	Unload();

	if( (m_Graphics = Graphics) == NULL ) return FALSE;
	if( Texture == NULL ) return TRUE;

	// Copy texture over
	Texture->GetLevelDesc( 0, &Desc );
	m_Width		= Desc.Width;
	m_Height	= Desc.Height;
	m_Graphics->GetDeviceCOM()->CreateTexture( m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_Texture, NULL );

	Texture->LockRect( 0, &SrcRect, NULL, D3DLOCK_READONLY );
	m_Texture->LockRect( 0, &DestRect, NULL, 0 );
	memcpy( DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height );
	m_Texture->UnlockRect( 0 );
	Texture->UnlockRect( 0 );

	return TRUE;
}

BOOL cTexture::Create( cGraphics *Graphics, unsigned long width, unsigned long height, D3DFORMAT Format )
{
	Unload();

	if( (m_Graphics = Graphics) == NULL ) return FALSE;

	m_Width		= width;
	m_Height	= height;
	if( FAILED( m_Graphics->GetDeviceCOM()->CreateTexture( width, height, 1, 0, Format, D3DPOOL_MANAGED, &m_Texture, NULL ) ) ) return FALSE;
	
	return TRUE;
}

BOOL cTexture::Unload()
{
	ReleaseCOM( m_Texture );

	m_Graphics	= NULL;
	m_Width		= 0;
	m_Height	= 0;

	return TRUE;
}

BOOL cTexture::IsLoaded()
{
	if( m_Texture == NULL ) return FALSE;
	return TRUE;
}

IDirect3DTexture9 *cTexture::GetTextureCOM()
{
	return m_Texture;
}

// �����δ� ���� �̹����� ���� ū ���� ����� 2�� ��� ���� �����Ѵ�.
long cTexture::GetWidthFromDesc()
{
	D3DSURFACE_DESC d3dsd;

	if( m_Texture == NULL ) return 0;
	if( FAILED( m_Texture->GetLevelDesc( 0, &d3dsd ) ) ) return 0;
	return d3dsd.Width;
}

// �����δ� ���� �̹����� ���� ū ���� ����� 2�� ��� ���� �����Ѵ�.
long cTexture::GetHeightFromDesc()
{
	D3DSURFACE_DESC d3dsd;

	if( m_Texture == NULL ) return 0;
	if( FAILED( m_Texture->GetLevelDesc( 0, &d3dsd ) ) ) return 0;
	return d3dsd.Height;
}

D3DFORMAT cTexture::GetFormatFromDesc()
{
	D3DSURFACE_DESC d3dsd;

	if( m_Texture == NULL ) return D3DFMT_UNKNOWN;
	if( FAILED( m_Texture->GetLevelDesc( 0, &d3dsd ) ) ) return D3DFMT_UNKNOWN;
	return d3dsd.Format;
}

long cTexture::GetWidth()  { return m_Width; }
long cTexture::GetHeight() { return m_Height; }

BOOL cTexture::Blit( long DestX, long DestY,
					long SrcX, long SrcY,
					long Width, long Height,
					float XScale, float YScale,
					D3DCOLOR Color )
{
	RECT Rect;
	ID3DXSprite *pSprite;

	if( m_Texture == NULL ) return FALSE;
	if( m_Graphics == NULL ) return FALSE;
	if( (pSprite = m_Graphics->GetSpriteCOM()) == NULL ) return FALSE;

	if( !Width  ) Width  = m_Width;
	if( !Height ) Height = m_Height;

	Rect.left	= SrcX;
	Rect.top	= SrcY;
	Rect.right	= Rect.left + Width;
	Rect.bottom	= Rect.top + Height;

#ifdef _DIRECT_X_8_0_
	if( FAILED( pSprite->Draw( m_Texture, &Rect, &D3DXVECTOR2( XScale, YScale ),
					NULL, 0.0f,
					&D3DXVECTOR2( (float)DestX, (float)DestY ),
					Color ) ) ) return FALSE;
#else
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D( &mat, NULL,	0.0f,
					&D3DXVECTOR2( XScale, YScale ),
					NULL, 0.0f, 
					&D3DXVECTOR2( (float)DestX, (float)DestY ) );
	pSprite->SetTransform( &mat );

	if( FAILED( pSprite->Draw( m_Texture, &Rect, 
					NULL,
					NULL,
					Color ) ) ) return FALSE;
#endif

	return TRUE;
}

