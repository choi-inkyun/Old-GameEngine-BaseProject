
#include "StdAfx.h"
#include "D3DMaterial.h"

/*-----------------------------------------------------------------------------
 * cMaterial CLASS
 *---------------------------------------------------------------------------*/

cMaterial::cMaterial()
{
	// Set a default material (white)
	ZeroMemory( &m_Material, sizeof(D3DMATERIAL9) );

	SetDiffuseColor( 255, 255, 255 );
	SetAmbientColor( 255, 255, 255 );
	SetSpecularColor( 255, 255, 255 );
	SetEmissiveColor( 0,   0,   0   );
	SetPower( 1.0f );
}

BOOL cMaterial::SetDiffuseColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Material.Diffuse.r = 1.0f / 255.0f * (float)Red;
	m_Material.Diffuse.g = 1.0f / 255.0f * (float)Green;
	m_Material.Diffuse.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetDiffuseColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Material.Diffuse.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Material.Diffuse.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Material.Diffuse.b );

	return TRUE;
}

BOOL cMaterial::SetAmbientColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Material.Ambient.r = 1.0f / 255.0f * (float)Red;
	m_Material.Ambient.g = 1.0f / 255.0f * (float)Green;
	m_Material.Ambient.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetAmbientColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Material.Ambient.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Material.Ambient.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Material.Ambient.b );

	return TRUE;
}

BOOL cMaterial::SetSpecularColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Material.Specular.r = 1.0f / 255.0f * (float)Red;
	m_Material.Specular.g = 1.0f / 255.0f * (float)Green;
	m_Material.Specular.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetSpecularColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Material.Specular.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Material.Specular.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Material.Specular.b );

	return TRUE;
}

BOOL cMaterial::SetEmissiveColor( BYTE Red, BYTE Green, BYTE Blue )
{
	m_Material.Emissive.r = 1.0f / 255.0f * (float)Red;
	m_Material.Emissive.g = 1.0f / 255.0f * (float)Green;
	m_Material.Emissive.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetEmissiveColor( BYTE *Red, BYTE *Green, BYTE *Blue )
{
	if( Red   != NULL ) *Red   = (BYTE)( 255.0f * m_Material.Emissive.r );
	if( Green != NULL ) *Green = (BYTE)( 255.0f * m_Material.Emissive.g );
	if( Blue  != NULL ) *Blue  = (BYTE)( 255.0f * m_Material.Emissive.b );

	return TRUE;
}

BOOL cMaterial::SetPower( float Power )
{
	m_Material.Power = Power;
	return TRUE;
}

float cMaterial::GetPower()
{
	return m_Material.Power;
}

D3DMATERIAL9 *cMaterial::GetMaterial()
{
	return &m_Material;
}

