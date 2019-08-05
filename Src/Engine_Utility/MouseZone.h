
#ifndef __MOUSEZONE_H__
#define __MOUSEZONE_H__

typedef struct RECTANGLE
{
	LONG	x;
	LONG	y;
	LONG	width;
	LONG	height;
} RECTANGLE;

// ���콺 Ŭ�� Ÿ��
#define MOUSECLICK_LBUTTONDOWN	0x01		// ���콺 ���� ��ư Ȱ��ȭ
#define MOUSECLICK_RBUTTONDOWN	0x02		// ���콺 ������ ��ư Ȱ��ȭ
#define	MOUSECLICK_LRBUTTONDOWN	0x03		// ���콺 �����̳� ������ ��ư Ȱ��ȭ
#define MOUSECLICK_LRBUTTONUP	0x00		// ���콺 ��ư�� ������ ����

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

	// ���콺 ���� active ���¸� ���� �� �� �ִ�.
	bool ActiveChange(bool bActive, int zone_id);
};

#endif
