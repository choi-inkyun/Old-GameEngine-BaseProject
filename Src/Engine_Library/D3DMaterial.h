
#ifndef __ENGINE_D3DMATERIAL_H__
#define __ENGINE_D3DMATERIAL_H__

/*-----------------------------------------------------------------------------
 * cMaterial CLASS
 *---------------------------------------------------------------------------*/

class cMaterial
{
protected:
	D3DMATERIAL9	m_Material;

public:
	cMaterial();

	D3DMATERIAL9	*GetMaterial();

	BOOL SetDiffuseColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetDiffuseColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetAmbientColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetAmbientColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetSpecularColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetSpecularColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetEmissiveColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetEmissiveColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetPower( float Power );
	float GetPower();
};

#endif

