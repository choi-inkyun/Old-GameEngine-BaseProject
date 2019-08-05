
#ifndef __ENGINE_D3DLIGHT_H__
#define __ENGINE_D3DLIGHT_H__

/*-----------------------------------------------------------------------------
 * cLight CLASS
 *---------------------------------------------------------------------------*/

class cLight
{
protected:
	D3DLIGHT9			m_Light;

public:
	cLight();

	D3DLIGHT9			*GetLight();

	BOOL SetType( D3DLIGHTTYPE Type );

	BOOL Move( float XPos, float YPos, float ZPos );
	BOOL MoveRel( float XPos, float YPos, float ZPos );
	BOOL GetPos( float *XPos, float *YPos, float *ZPos );

	BOOL Point( float XFrom, float YFrom, float ZFrom,
				float XAt,   float YAt,   float ZAt );
	BOOL GetDirection( float *XDir, float *YDir, float *ZDir );

	BOOL SetDiffuseColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetDiffuseColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetAmbientColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetAmbientColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetSpecularColor( BYTE Red, BYTE Green, BYTE Blue );
	BOOL GetSpecularColor( BYTE *Red, BYTE *Green, BYTE *Blue );

	BOOL SetRange( float Range );
	float GetRange();

	BOOL SetFalloff( float Falloff );
	float GetFalloff();

	BOOL SetAttenuation0( float Attenuation );
	float GetAttenuation0();

	BOOL SetAttenuation1( float Attenuation );
	float GetAttenuation1();

	BOOL SetAttenuation2( float Attenuation );
	float GetAttenuation2();

	BOOL SetTheta( float Theta );
	float GetTheta();

	BOOL SetPhi( float Phi );
	float GetPhi();
};

#endif

