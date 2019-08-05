
#include "StdAfx.h"
#include "D3DLight.h"

/*-----------------------------------------------------------------------------
 * cLight CLASS
 *---------------------------------------------------------------------------*/

cLight::cLight()
{
	// Set a default light to point
	ZeroMemory( &m_Light, sizeof( D3DLIGHT9 ) );
	SetType( D3DLIGHT_POINT );
	Move( 0.0f, 0.0f, 0.0f );
	SetDiffuseColor( 255, 255, 255 );
	SetAmbientColor( 255, 255, 255 );
	SetRange( 1000.0f );
}

BOOL cLight::SetType( D3DLIGHTTYPE Type )
{
	m_Light.Type = Type;
	return TRUE;
}

BOOL cLight::Move( float XPos, float YPos, float ZPos )
{
	m_Light.Position.x = XPos;
	m_Light.Position.y = YPos;
	m_Light.Position.z = ZPos;
	return TRUE;
}

BOOL cLight::MoveRel( float XPos, float YPos, float ZPos )
{
	m_Light.Position.x += XPos;
	m_Light.Position.y += YPos;
	m_Light.Position.z += ZPos;
	return TRUE;
}

BOOL cLight::GetPos( float *XPos, float *YPos, float *ZPos )
{
	if( XPos != NULL ) *XPos = m_Light.Position.x;
	if( YPos != NULL ) *YPos = m_Light.Position.y;
	if( ZPos != NULL ) *ZPos = m_Light.Position.z;
	return TRUE;
}

BOOL cLight::Point( float XFrom, float YFrom, float ZFrom,
					float XAt,   float YAt,   float ZAt )
{
	D3DXVECTOR3 vecSrc;
	D3DXVECTOR3 vecDest;

	// Move the light
	Move( XFrom, YFrom, ZFrom );

	// Calculate vector between angles
	m_Light.Direction.x = XAt - XFrom;
	m_Light.Direction.y = YAt - YFrom;
	m_Light.Direction.z = ZAt - ZFrom;

	return TRUE;
}

BOOL cLight::GetDirection( float *XDir, float *YDir, float *ZDir )
{
	if( XDir != NULL ) *XDir = m_Light.Direction.x;
	if( YDir != NULL ) *YDir = m_Light.Direction.y;
	if( ZDir != NULL ) *ZDir = m_Light.Direction.z;

	return TRUE;
}

BOOL cLight::SetDiffuseColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Light.Diffuse.r = 1.0f / 255.0f * (float)Red;
	m_Light.Diffuse.g = 1.0f / 255.0f * (float)Green;
	m_Light.Diffuse.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cLight::GetDiffuseColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Light.Diffuse.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Light.Diffuse.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Light.Diffuse.b );

	return TRUE;
}

BOOL cLight::SetSpecularColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Light.Specular.r = 1.0f / 255.0f * (float)Red;
	m_Light.Specular.g = 1.0f / 255.0f * (float)Green;
	m_Light.Specular.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cLight::GetSpecularColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Light.Specular.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Light.Specular.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Light.Specular.b );

	return TRUE;
}

BOOL cLight::SetAmbientColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Light.Ambient.r = 1.0f / 255.0f * (float)Red;
	m_Light.Ambient.g = 1.0f / 255.0f * (float)Green;
	m_Light.Ambient.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cLight::GetAmbientColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Light.Ambient.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Light.Ambient.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Light.Ambient.b );

	return TRUE;
}

BOOL cLight::SetRange( float Range )
{
	m_Light.Range = Range;
	return TRUE;
}

float cLight::GetRange()
{
	return m_Light.Range;
}

BOOL cLight::SetFalloff( float Falloff )
{
	m_Light.Falloff = Falloff;
	return TRUE;
}

float cLight::GetFalloff()
{
	return m_Light.Falloff;
}

BOOL cLight::SetAttenuation0( float Attenuation )
{
	m_Light.Attenuation0 = Attenuation;
	return TRUE;
}

float cLight::GetAttenuation0()
{
	return m_Light.Attenuation0;
}

BOOL cLight::SetAttenuation1( float Attenuation )
{
	m_Light.Attenuation1 = Attenuation;
	return TRUE;
}

float cLight::GetAttenuation1()
{
	return m_Light.Attenuation1;
}

BOOL cLight::SetAttenuation2( float Attenuation )
{
	m_Light.Attenuation2 = Attenuation;
	return TRUE;
}

float cLight::GetAttenuation2()
{
	return m_Light.Attenuation2;
}

BOOL cLight::SetTheta( float Theta )
{
	m_Light.Theta = Theta;
	return TRUE;
}

float cLight::GetTheta()
{
	return m_Light.Theta;
}

BOOL cLight::SetPhi( float Phi )
{
	m_Light.Phi = Phi;
	return TRUE;
}

float cLight::GetPhi()
{
	return m_Light.Phi;
}

D3DLIGHT9 *cLight::GetLight()
{
	return &m_Light;
}

