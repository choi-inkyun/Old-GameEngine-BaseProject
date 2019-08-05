
#include "StdAfx.h"
#include "MouseZone.h"

cMouseZone::cMouseZone()
{
	m_iNumOfZones	= 0;
	m_HotSpots		= NULL;
}

cMouseZone::~cMouseZone()
{
	Free();
}

void cMouseZone::Initialize( int iNumOfZones )
{
	m_iNumOfZones	= iNumOfZones;

	m_HotSpots		= new HOTSPOT[ m_iNumOfZones ];

	for( int i = 0; i < m_iNumOfZones; i++ ) m_HotSpots[i].bActvie	= FALSE;
}

void cMouseZone::Free()
{
	SAFE_DELETEn( m_HotSpots );
}

int  cMouseZone::Add( LONG zone_id, LONG x, LONG y, LONG width, LONG height, WORD click_type)
{
	for( int i = 0; i < m_iNumOfZones; i++ )
	{
		if( !m_HotSpots[i].bActvie )
		{
			m_HotSpots[i].bActvie		= TRUE;
			m_HotSpots[i].zone_id		= zone_id;
			m_HotSpots[i].click_type	= click_type;
			m_HotSpots[i].rectangle.x	= x;
			m_HotSpots[i].rectangle.y	= y;
			m_HotSpots[i].rectangle.width = width;
			m_HotSpots[i].rectangle.height= height;
			return i;
		}
	}

	return (-1);
}

bool cMouseZone::ActiveChange(bool bActive, int zone_id)
{
	for( int i = 0; i < m_iNumOfZones; i++ )
	{
		if(m_HotSpots[i].zone_id == zone_id)
		{
			m_HotSpots[i].bActvie = bActive;
			return true;
		}
	}
	return false;
}

int  cMouseZone::Delete( LONG zone_id )
{
	for( int i = 0; i < m_iNumOfZones; i++ )
	{
		if( m_HotSpots[i].bActvie && ( m_HotSpots[i].zone_id == zone_id ) )
		{
			m_HotSpots[i].bActvie		= FALSE;
			return i;
		}
	}
	
	return (-1);
}

LONG cMouseZone::Check( LONG x, LONG y, BOOL bLButtonDown, BOOL bRButtonDown, int nID , int nID2 )
{
	WORD click_value = 0x00;
	
	if( bLButtonDown ) click_value |= MOUSECLICK_LBUTTONDOWN;
	if( bRButtonDown ) click_value |= MOUSECLICK_RBUTTONDOWN;
	
	for( int i = m_iNumOfZones - 1; i >= 0; i-- )
	{
		if( m_HotSpots[i].zone_id != nID && m_HotSpots[i].zone_id != nID2 ) continue; // dingpong 추가. 좌표가 겹칠 때는 이미지를 인식 모하기 때문에 어떤 이미지의 좌표를 체크하고 싶나 직접 넣는다
		if( !m_HotSpots[i].bActvie ) continue;
		if( m_HotSpots[i].click_type != click_value ) continue;
		if( ( m_HotSpots[i].rectangle.x <= x ) && 
			( m_HotSpots[i].rectangle.y <= y ) &&
			( m_HotSpots[i].rectangle.x + m_HotSpots[i].rectangle.width  >= x ) &&
			( m_HotSpots[i].rectangle.y + m_HotSpots[i].rectangle.height >= y ) )
		{
			return m_HotSpots[i].zone_id;
		}
	}

	return 0;
}

int  cMouseZone::Update( LONG zone_id, LONG x, LONG y, LONG width, LONG height, WORD click_type )
{
	for( int i = 0; i < m_iNumOfZones; i++ )
	{
		if( m_HotSpots[i].bActvie && ( m_HotSpots[i].zone_id == zone_id ) )
		{
			m_HotSpots[i].click_type	= click_type;
			m_HotSpots[i].rectangle.x	= x;
			m_HotSpots[i].rectangle.y	= y;
			m_HotSpots[i].rectangle.width = width;
			m_HotSpots[i].rectangle.height= height;
			return i;
		}
	}
	
	return Add( zone_id, x, y, width, height, click_type );
}

