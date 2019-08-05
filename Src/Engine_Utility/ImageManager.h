
#ifndef __IMAGEMANAGER_H__
#define __IMAGEMANAGER_H__

#include "MouseZone.h"
#include "Tile.h"
#include "Effects.h"

// ��ư ���콺�� over �ǰų� down �Ȱ� üũ�ϴ� ���..
// 	m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown );
//  char ImageName[100];
//  strcpy(imageName, Get_ImageName(m_ZoneId))
//  if(strcmp(imageName, "aa")

typedef struct
{
	char ParentName[256]; // ���ϳ���
	char Name[256]; // �� Ÿ���� ����
	int tilenum; // ���° Ÿ������
	int cx; // �� Ÿ���� �κ�
	int cy; // �� Ÿ���� ����
	int destX; // ��� ��ġ
	int destY; // ��� ��ġ
	float scaleX; // ������. ���� 1.0
	float scaleY; // ������. ���� 1.0
	int type; // 0 : �Ϲ� �̹���. 1 : ��ư. 2. �Ϲ� �̹��� �ִϸ��̼�. 3. gif �ִϸ��̼�
	int flag1; // ��ư�̶�� ���콺 ���� �� �������� �����Ѵ�. �̰� ��ư down �׿ܿ� ��������..
	int flag2; // �̰� ��ư over
	int format; // 20->D3DFMT_R8G8B8, 21->D3DFMT_A8R8G8B8. 20 �� ���ȭ�� �����ſ�.. 21�� ������ ��������Ʈ.. gif ����Ʈ��.
} tsImageInfo;

class __declspec(dllexport) CImageManager
{
public:
	CImageManager();
	~CImageManager();
	// �̹����� �ε��ϴ� �Լ�
	bool ImageLoad(char *FileName);

	// �Ϲ� �׸��� �ѷ��ִ� �Լ�. color �� ���û���. ��ǥ�Ŷ�.
	BYTE CImageManager::Draw(char *ImageName, int x = 0, int y = 0, D3DCOLOR color = 0xFFFFFFFF);
	// ���� ������ ���� �������� �ɼ����� �Ѹ� �� �ִ�.
	BYTE Draw(char *ImageName, int tilenum, int destX, int destY, D3DCOLOR color = 0xFFFFFFFF);

	// ��ư �׸��� �ѷ��ִ� �Լ�. Mouse Zone �� �����ϵ��� ����
	void ButtonDraw(char *ButtonName);

	// list �� ���� ��� ������.. 
	int				Get_ImageIndex(char *imagename);
	// list �� ��� ������
	tsImageInfo*	Get_ImageList(char *imagename);


	// �⺻ 3��, 1�� �÷���
	void EffectPlay(char *ImageName, EFFECT_PLAY_TYPE n_flag = EFFECT_PLAY_ONCE, DWORD n_time = 0, int position_x = -1, int position_y = -1);
	void EffectDraw();

	// �� Ÿ�� �̸��� �׸����� ����Ʈ�� ��������.
	bool			IsEndEffect(char *imagename);

	// �� �ν��Ͻ��� ��ü���� �������ش�.
	cMouseZone*		Get_MouseZone()					{ return &m_MouseZone; }
	cTile*			Get_Tile()						{ return &m_Tiles; }
	tsImageInfo*	Get_ImageList(int i_index)		{ return m_ImageList[i_index]; }
	UINT			Get_ImageCount()				{ return m_ImageCount; }
	LONG			Get_ZoneldDown()				{ return m_ZoneIdDown; }
	BOOL			Get_ButtonUp()					{ return m_bButtonUp;  }
protected:
	cMouseZone		m_MouseZone;
	cTile			m_Tiles;
	cEffects        m_Effects;

	tsImageInfo **m_ImageList;

	UINT m_ImageCount;

	LONG			m_ZoneId;

	LONG			m_ZoneIdDown;
	BOOL			m_bButtonUp;

	////////// input ���� /////////
	LONG			m_lMouseXPos;
	LONG			m_lMouseYPos;
	BOOL			m_bLButtonDown;
	BOOL			m_bRButtonDown;

	cInput			m_Input;
	cInputDevice	m_Keyboard;
	cInputDevice	m_Mouse;
	///////////////////////////////
	bool			*m_MouseDownCheck;
	bool			*m_MouseOverCheck;
private:
	char m_LoadPath[256];

	bool AddSurface( tsImageInfo* ImageInfo );
	// ��ü�� ������
	bool CleanUp();

	int *m_nImageIndex;
	int *m_nImageEffectIndex;
	int *m_nGifEffectIndex;
};


#endif
