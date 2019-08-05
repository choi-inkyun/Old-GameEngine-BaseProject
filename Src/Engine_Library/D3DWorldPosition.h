
#ifndef __ENGINE_D3DWORLDPOSITION_H__
#define __ENGINE_D3DWORLDPOSITION_H__

#include "D3DGraphics.h"

/*-----------------------------------------------------------------------------
 * cWorldPosition CLASS
 *---------------------------------------------------------------------------*/

class cWorldPosition
{
protected:
	BOOL	m_Billboard;

	float	m_XPos,			m_YPos,			m_ZPos;
	float	m_XRotation,	m_YRotation,	m_ZRotation;
	float	m_XScale,		m_YScale,		m_ZScale;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matScale;
	D3DXMATRIX m_matRotation;
	D3DXMATRIX m_matTranslation;
	D3DXMATRIX *m_matCombine1;
	D3DXMATRIX *m_matCombine2;

public:
	cWorldPosition();

	D3DXMATRIX *GetMatrix( cGraphics *Graphics = NULL );
	BOOL SetCombineMatrix1( D3DXMATRIX *Matrix = NULL );
	BOOL SetCombineMatrix2( D3DXMATRIX *Matrix = NULL );

	BOOL Copy( cWorldPosition *DestPos );

	BOOL Move( float XPos, float YPos, float ZPos );
	BOOL MoveRel( float XAdd, float YAdd, float ZAdd );

	BOOL Rotate( float XRot, float YRot, float ZRot );
	BOOL RotateRel( float XAdd, float YAdd, float ZAdd );

	BOOL Scale( float XScale, float YScale, float ZScale );
	BOOL ScaleRel( float XAdd, float YAdd, float ZAdd );

	BOOL Update( cGraphics *Graphics = NULL );
	BOOL EnableBillboard( BOOL Enable = TRUE );

	float GetXPos() const;
	float GetYPos() const;
	float GetZPos() const;
	float GetXRotation() const;
	float GetYRotation() const;
	float GetZRotation() const;
	float GetXScale() const;
	float GetYScale() const;
	float GetZScale() const;
};

#endif

