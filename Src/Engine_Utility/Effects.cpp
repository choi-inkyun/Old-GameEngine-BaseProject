
#include "StdAfx.h"
#include "Effects.h"

cEffects::cEffects()
{
	m_nTotalImageCount = 0;
	m_nTotalEffectCount = 0;
}

cEffects::~cEffects()
{
	Release();
}

// i_number : 최대 임펙트 숫자.
void cEffects::Initialize( cGraphics* pGraphics, int i_ImageNumber, int i_GifNumber )
{
	m_pGraphics = pGraphics;

	m_nTotalEffectCount = i_ImageNumber + i_GifNumber;
	m_nTotalImageCount = i_ImageNumber;

	if(m_nTotalEffectCount != 0)
		m_Effects = new EFFECT_INFO[m_nTotalEffectCount];

	if(i_GifNumber != 0)
	{
#ifdef _DEBUG
		ASSERT( m_GIFEffects.Create( m_pGraphics, i_GifNumber ) );
#else
		m_GIFEffects.Create( g_cApplication.getgraphics(), i_GifNumber );
#endif
	}
	for( int i = 0; i < m_nTotalEffectCount; i++ ) m_Effects[i].bActive	= FALSE;

	// load BMP effects
	if(i_ImageNumber != 0)
	{
#ifdef _DEBUG
		ASSERT( m_BMPEffects.Create( m_pGraphics, i_ImageNumber ) );
#else
		m_BMPEffects.Create( m_pGraphics, i_ImageNumber );
#endif
	}
}

void cEffects::Release()
{	
	if(m_nTotalImageCount != 0)
		m_BMPEffects.Free();
	if(m_nTotalEffectCount != 0)
	{
		m_GIFEffects.Free();
		delete [] m_Effects;
	}
}

int  cEffects::Play( int type, int effect, long x, long y, EFFECT_PLAY_TYPE play_type, DWORD duration, int tileid )
{
	for( int i = 0; i < m_nTotalEffectCount; i++ )
	{
		if( !m_Effects[i].bActive )
		{
			m_Effects[i].bActive	= TRUE;
			m_Effects[i].type		= type;
			m_Effects[i].effect		= effect;
			m_Effects[i].x			= x;
			m_Effects[i].y			= y;
			m_Effects[i].play_type	= play_type;
			m_Effects[i].duration	= duration;
			m_Effects[i].tileid		= tileid;
			m_Effects[i].triggered	= GetTickCount();

			switch( m_Effects[i].type )
			{
				case EFFECTTYPE_GIF:
					// 처음 프레임으로 이동한다.
					m_GIFEffects.Stop( m_Effects[i].effect );

					switch( play_type )
					{
						case EFFECT_PLAY_ONCE:
						case EFFECT_PLAY_ONCE_STILL:
							m_GIFEffects.Repeat( m_Effects[i].effect, FALSE );
							break;
					}
					break;
			}

			return i;
		}
	}

	return (-1);	
}

int  cEffects::Stop( int type, int effect )
{
	for( int i = 0; i < m_nTotalEffectCount; i++ )
	{
		if( !m_Effects[i].bActive ) continue;
		
		if( m_Effects[i].type == type && m_Effects[i].effect == effect )
		{
			if( m_Effects[i].type == EFFECTTYPE_GIF )
			{
				m_GIFEffects.Stop( m_Effects[i].effect );		// 현재 프레임을 초기화한다.
			}

			m_Effects[i].bActive = FALSE;
			return i;
		}
	}

	return (-1);
}

int  cEffects::Repeat( int type, int effect, BOOL bRepeat )
{
	for( int i = 0; i < m_nTotalEffectCount; i++ )
	{
		if( !m_Effects[i].bActive ) continue;
		
		if( m_Effects[i].type == type && m_Effects[i].effect == effect )
		{
			if( m_Effects[i].type == EFFECTTYPE_GIF ) m_GIFEffects.Repeat( m_Effects[i].effect, bRepeat );

			return i;
		}
	}

	return (-1);
}

BOOL cEffects::Render()
{
	for( int i = 0; i < m_nTotalEffectCount; i++ )
	{
		if( !m_Effects[i].bActive ) continue;

		switch( m_Effects[i].type )
		{
			case EFFECTTYPE_BMP:
				m_BMPEffects.Draw( m_Effects[i].effect, m_Effects[i].tileid, m_Effects[i].x, m_Effects[i].y );
				break;

			case EFFECTTYPE_GIF:
				m_GIFEffects.Draw( m_Effects[i].effect, m_Effects[i].x, m_Effects[i].y );
				break;

		}

		if( m_Effects[i].type == EFFECTTYPE_BMP || m_Effects[i].type == EFFECTTYPE_GIF )
		{
			switch( m_Effects[i].play_type )
			{
				case EFFECT_PLAY_ONCE:
					if( m_GIFEffects.IsEndOfPlay( m_Effects[i].effect ) ) m_Effects[i].bActive = FALSE;
					break;
				case EFFECT_PLAY_ONCE_STILL:
					/*break;*/
				case EFFECT_PLAY_STILL:
				case EFFECT_PLAY_REPEAT:
						 if( m_Effects[i].duration == (-1) ) /* endless play */;
					else if( m_Effects[i].triggered + m_Effects[i].duration < GetTickCount() ) m_Effects[i].bActive = FALSE;
					break;
			}
		}
	}

	return TRUE;
}

