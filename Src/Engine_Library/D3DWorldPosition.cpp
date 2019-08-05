
#include "StdAfx.h"
#include "D3DWorldPosition.h"

/*-----------------------------------------------------------------------------
 * cWorldPosition CLASS
 *---------------------------------------------------------------------------*/

cWorldPosition::cWorldPosition()
{
	m_Billboard = FALSE;
	m_matCombine1 = m_matCombine2 = NULL;

	Move  ( 0.0f, 0.0f, 0.0f );
	Rotate( 0.0f, 0.0f, 0.0f );
	Scale ( 1.0f, 1.0f, 1.0f );

	Update();
}

BOOL cWorldPosition::Copy( cWorldPosition *DestPos )
{
	DestPos->Move( m_XPos, m_YPos, m_ZPos );
	DestPos->Rotate( m_XRotation, m_YRotation, m_ZRotation );
	DestPos->Scale( m_XScale, m_YScale, m_ZScale );
	DestPos->EnableBillboard( m_Billboard );

	return TRUE;
}

BOOL cWorldPosition::Move( float XPos, float YPos, float ZPos )
{
	m_XPos = XPos;
	m_YPos = YPos;
	m_ZPos = ZPos;

	D3DXMatrixTranslation( &m_matTranslation, m_XPos, m_YPos, m_ZPos );

	return TRUE;
}

BOOL cWorldPosition::MoveRel( float XAdd, float YAdd, float ZAdd )
{
	return Move( m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd );
}

BOOL cWorldPosition::Rotate( float XRot, float YRot, float ZRot )
{
	m_XRotation = XRot;
	m_YRotation = YRot;
	m_ZRotation = ZRot;

	D3DXMatrixRotationYawPitchRoll( &m_matRotation, m_YRotation, m_XRotation, m_ZRotation );

	return TRUE;
}

BOOL cWorldPosition::RotateRel( float XAdd, float YAdd, float ZAdd )
{
	return Rotate( m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd );
}

BOOL cWorldPosition::Scale( float XScale, float YScale, float ZScale )
{
	m_XScale = XScale;
	m_YScale = YScale;
	m_ZScale = ZScale;

	D3DXMatrixScaling( &m_matScale, XScale, YScale, ZScale );

	return TRUE;
}

BOOL cWorldPosition::ScaleRel( float XAdd, float YAdd, float ZAdd )
{
	return Scale( m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd );
}

BOOL cWorldPosition::Update( cGraphics *Graphics )
{
	D3DXMATRIX matView, matTransposed;

	// Setup billboarding matrix
	if( m_Billboard == TRUE )
	{
		if( Graphics != NULL && Graphics->GetDeviceCOM() != NULL ) 
		{
			Graphics->GetDeviceCOM()->GetTransform( D3DTS_VIEW, &matView );
			D3DXMatrixTranspose( &matTransposed, &matView );
			matTransposed._41 = matTransposed._42 = matTransposed._43 = 
				matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f;
		}
		else
		{
			D3DXMatrixIdentity( &matTransposed );
		}
	}

	// Combine scaling and rotation matrices first
	D3DXMatrixMultiply( &m_matWorld, &m_matScale, &m_matRotation );

	// Apply billboard matrix
	if( m_Billboard == TRUE ) D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matTransposed );

	// Combine with translation matrix
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matTranslation );

	// Combine with combined matrices (if any)
	if( m_matCombine1 != NULL ) D3DXMatrixMultiply( &m_matWorld, &m_matWorld, m_matCombine1 );
	if( m_matCombine2 != NULL ) D3DXMatrixMultiply( &m_matWorld, &m_matWorld, m_matCombine2 );

	return TRUE;
}

BOOL cWorldPosition::EnableBillboard( BOOL Enable )
{
	m_Billboard = Enable;
	return TRUE;
}

D3DXMATRIX *cWorldPosition::GetMatrix( cGraphics *Graphics )
{
	Update( Graphics );
	return &m_matWorld;
}

BOOL cWorldPosition::SetCombineMatrix1( D3DXMATRIX *Matrix )
{
	m_matCombine1 = Matrix;
	return TRUE;
}

BOOL cWorldPosition::SetCombineMatrix2( D3DXMATRIX *Matrix )
{
	m_matCombine2 = Matrix;
	return TRUE;
}

float cWorldPosition::GetXPos() const { return m_XPos; }
float cWorldPosition::GetYPos() const { return m_YPos; }
float cWorldPosition::GetZPos() const {	return m_ZPos; }

float cWorldPosition::GetXRotation() const { return m_XRotation; }
float cWorldPosition::GetYRotation() const { return m_YRotation; }
float cWorldPosition::GetZRotation() const { return m_ZRotation; }

float cWorldPosition::GetXScale() const { return m_XScale; }
float cWorldPosition::GetYScale() const { return m_YScale; }
float cWorldPosition::GetZScale() const { return m_ZScale; }

