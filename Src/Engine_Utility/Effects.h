
#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "Tile.h"
#include "AniGIF.h"

// #define MAX_EFFECTS			10

typedef enum EFFECT_TYPE
{
	EFFECTTYPE_BMP = 1,
	EFFECTTYPE_GIF,
	EFFECTTYPE_PARTICLE,
};

typedef enum EFFECT_PLAY_TYPE
{
	EFFECT_PLAY_STILL = 0,					// ����
	EFFECT_PLAY_ONCE,						// 1ȸ ���
	EFFECT_PLAY_ONCE_STILL,					// 1ȸ ����� ����
	EFFECT_PLAY_REPEAT,						// �ݺ� ���
} EFFECT_PLAY_TYPE;

typedef struct EFFECT_INFO
{
	BOOL bActive;
	int  type;
	int  effect;
	long x;
	long y;
	EFFECT_PLAY_TYPE play_type;
	DWORD duration;	
	int  tileid;							// BMP Ÿ�� ��ȣ(default: 0)
	DWORD triggered;
} EFFECT_INFO;

class cEffects
{
public:
	cEffects();
	~cEffects();

	void Initialize( cGraphics* pGraphics, int i_ImageNumber, int i_GifNumber );
	void Release();
	BOOL Render();

	int  Play( int type, int effect, long x, long y, EFFECT_PLAY_TYPE play_type, DWORD duration = 0, int tileid = 0 );
	int  Stop( int type, int effect );
	int  Repeat( int type, int effect, BOOL bRepeat = TRUE );

	cTile *Get_BGMEffects(){return &m_BMPEffects;}
	cAniGIF* Get_GIFEffects(){return &m_GIFEffects;}

private:
	cGraphics*		m_pGraphics;

	cTile			m_BMPEffects;						// ������ ȿ����
	cAniGIF			m_GIFEffects;						// ������ ȿ����

	EFFECT_INFO		*m_Effects;				// ȿ��

	int				m_nTotalEffectCount;				// �� ����Ʈ ����
	int				m_nTotalImageCount;
};

#endif

