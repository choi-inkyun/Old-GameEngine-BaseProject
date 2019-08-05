
#ifndef __MOUSEZONE_H__
#define __MOUSEZONE_H__

typedef struct RECTANGLE
{
	LONG	x;
	LONG	y;
	LONG	width;
	LONG	height;
} RECTANGLE;

// 마우스 클릭 타입
#define MOUSECLICK_LBUTTONDOWN	0x01		// 마우스 왼쪽 버튼 활성화
#define MOUSECLICK_RBUTTONDOWN	0x02		// 마우스 오른쪽 버튼 활성화
#define	MOUSECLICK_LRBUTTONDOWN	0x03		// 마우스 왼쪽이나 오른쪽 버튼 활성화
#define MOUSECLICK_LRBUTTONUP	0x00		// 마우스 버튼이 눌리지 않음

typedef struct HOTSPOT
{
	BOOL		bActvie;
	LONG		zone_id;
	WORD		click_type;
	RECTANGLE	rectangle;
} HOTSPOT;

class __declspec(dllexport) cMouseZone
{
private:
	int		m_iNumOfZones;
	HOTSPOT *m_HotSpots;

public:
	cMouseZone();
	~cMouseZone();

	void Initialize( int iNumOfZones );
	void Free();
	int  Add( LONG zone_id, LONG x, LONG y, LONG width, LONG height, WORD click_type );
	int  Delete( LONG zone_id );
	LONG Check( LONG x, LONG y, BOOL bLButtonDown, BOOL bRButtonDown , int nID ,int nID2 );
	int  Update( LONG zone_id, LONG x, LONG y, LONG width, LONG height, WORD click_type );

	// 마우스 존의 active 상태를 변경 할 수 있다.
	bool ActiveChange(bool bActive, int zone_id);
};

#endif
