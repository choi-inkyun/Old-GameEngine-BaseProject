
#ifndef __IMAGEMANAGER_H__
#define __IMAGEMANAGER_H__

#include "MouseZone.h"
#include "Tile.h"
#include "Effects.h"

// 버튼 마우스에 over 되거나 down 된거 체크하는 방법..
// 	m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown );
//  char ImageName[100];
//  strcpy(imageName, Get_ImageName(m_ZoneId))
//  if(strcmp(imageName, "aa")

typedef struct
{
	char ParentName[256]; // 파일네임
	char Name[256]; // 그 타일의 네임
	int tilenum; // 몇번째 타일인지
	int cx; // 한 타일의 널비
	int cy; // 한 타일의 높이
	int destX; // 출력 위치
	int destY; // 출력 위치
	float scaleX; // 스케일. 원본 1.0
	float scaleY; // 스케일. 원본 1.0
	int type; // 0 : 일반 이미지. 1 : 버튼. 2. 일반 이미지 애니메이션. 3. gif 애니메이션
	int flag1; // 버튼이라면 마우스 존에 들어갈 변수들을 셋팅한다. 이건 버튼 down 그외에 여러가지..
	int flag2; // 이건 버튼 over
	int format; // 20->D3DFMT_R8G8B8, 21->D3DFMT_A8R8G8B8. 20 은 배경화면 같은거에.. 21은 나머지 스프라이트.. gif 임팩트들.
} tsImageInfo;

class __declspec(dllexport) CImageManager
{
public:
	CImageManager();
	~CImageManager();
	// 이미지를 로드하는 함수
	bool ImageLoad(char *FileName);

	// 일반 그림을 뿌려주는 함수. color 은 선택사항. 좌표ㅕ랑.
	BYTE CImageManager::Draw(char *ImageName, int x = 0, int y = 0, D3DCOLOR color = 0xFFFFFFFF);
	// 위와 같으나 좀더 세부적인 옵션으로 뿌릴 수 있다.
	BYTE Draw(char *ImageName, int tilenum, int destX, int destY, D3DCOLOR color = 0xFFFFFFFF);

	// 버튼 그림을 뿌려주는 함수. Mouse Zone 와 연계하도록 하자
	void ButtonDraw(char *ButtonName);

	// list 의 값을 얻고 싶을때.. 
	int				Get_ImageIndex(char *imagename);
	// list 를 얻고 싶을때
	tsImageInfo*	Get_ImageList(char *imagename);


	// 기본 3초, 1번 플레이
	void EffectPlay(char *ImageName, EFFECT_PLAY_TYPE n_flag = EFFECT_PLAY_ONCE, DWORD n_time = 0, int position_x = -1, int position_y = -1);
	void EffectDraw();

	// 이 타일 이름의 그림파일 임펙트가 끝났느냐.
	bool			IsEndEffect(char *imagename);

	// 각 인스턴스의 객체들을 리턴해준다.
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

	////////// input 관련 /////////
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
	// 객체를 지워줌
	bool CleanUp();

	int *m_nImageIndex;
	int *m_nImageEffectIndex;
	int *m_nGifEffectIndex;
};


#endif
