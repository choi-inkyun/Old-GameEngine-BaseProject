
#include "StdAfx.h"
#include "ImageManager.h"
#include "BlowFishData.h"

CImageManager::CImageManager()
: m_ImageCount(0),
  m_ImageList(0),
  m_ZoneId(0),
  m_ZoneIdDown(0),
  m_bButtonUp(0)
{
}
CImageManager::~CImageManager()
{
	CleanUp();
}

// 이미지를 로드한다.
bool CImageManager::ImageLoad(char *FileName)
{
	UINT i,j;
	char temp[256];
	char tmpParentName[256] = {0};
	char tmpParentName2[256] = {0};
	int tmpParentX, tmpParentY;
	int tmpParentNameCount;
	int TotalImage = 0, TotalButton = 0;
	int temcount = 0;
	m_ImageCount = 0;
	// 어차피 gif 를 로드하려면 따로 추가해 줘야 하는데 effect 를 그냥 따로 둔다.
	int n_TileCount = 0, n_tempTileCount = 0;
	int n_ImageEffectCount = 0, n_tempImageEffectCount = 0;
	int n_GifEffectCount = 0, n_tempGifEffectCount = 0;

	int color_r, color_g, color_b;

	// input 객체를 생성한다. input, mouse, keyboard 객체 생성
#ifdef _DEBUG
	ASSERT( m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() ) );
#else
	m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() );
#endif
	m_Keyboard.Create( &m_Input, KEYBOARD, g_cApplication.Get_Windowed() );
	m_Mouse.Create( &m_Input, MOUSE, g_cApplication.Get_Windowed() );

	FILE *fp;
	// 파일을 오픈함.
	fp = fopen(FileName, "rt");
	if (!fp)
	{
		g_cApplication.DisplayError( TRUE, "Image loadlist 파일이 없습니다.");
		return false;
	}

	fgets(temp,256,fp); // 한줄. 최대 256 Byte 만큼 Read 함
	sscanf(temp,"IMAGECOUNT=%d %d\n", &TotalImage, &TotalButton);

	fgets(temp,256,fp); // 한줄. 최대 256 Byte 만큼 Read 함
	sscanf(temp,"CLOLORKEY=%d %d %d\n", &color_r, &color_g, &color_b);

	// 파일 끝까지 처리한다.
	while(!feof(fp))
	{
		char tmpparentname[512];

		fgets(temp,256,fp); // 한줄. 최대 256 Byte 만큼 Read 함
		if(!strnicmp(temp,"//",2))
		{
			// 이건 주석 줄이므로 따로 처리를 해주지 않는것을 원칙으로 해준다.
		}
		// 각 파일 로드의 시작
		else if(!strnicmp(temp,"FILENAME=",9))
		{
			sscanf(temp,"FILENAME=%s %d %d\n", tmpParentName2, &tmpParentX, &tmpParentY);

			/////////////////////////////////////////////////////////     / -> \\ 바꿔주는 루틴 
			tmpParentNameCount = 0;
			for(i=0;i<strlen(tmpParentName2);i++)
			{
				if(tmpParentName2[i]=='/')
				{
					tmpParentName[tmpParentNameCount++] = '\\';
				}
				else
					tmpParentName[tmpParentNameCount++] = tmpParentName2[i];
			}
			tmpParentName[tmpParentNameCount] = 0;
			/////////////////////////////////////////////////////////
			strcpy(tmpparentname, m_LoadPath);
			strcat(tmpparentname, tmpParentName);
		}

		// 두번째 줄엔 그 파일에서 몇개의 타일을 읽어 들일지 설정한걸 불러온다.
		else if(!strnicmp(temp,"PCOUNT=",7))
		{
			sscanf(temp,"PCOUNT=%d\n", &temcount);

			for(i=0;i<temcount;i++)
			{
				tsImageInfo *tmpImageInfo;

				tmpImageInfo = new tsImageInfo;

				// 읽어 들인다. 파일정보를...
				fgets(temp,256,fp);
				sscanf(temp, "%d=%s %d %d %d %d %d %f %f %d %d %d %d\n", 
					&j, tmpImageInfo->Name,
					&tmpImageInfo->tilenum,
					&tmpImageInfo->cx, &tmpImageInfo->cy, &tmpImageInfo->destX, &tmpImageInfo->destY,
					&tmpImageInfo->scaleX, &tmpImageInfo->scaleY, &tmpImageInfo->type, &tmpImageInfo->flag1, &tmpImageInfo->flag2, &tmpImageInfo->format);

				strcpy(tmpImageInfo->ParentName,tmpparentname);  // daniel 2002.9.19

				// 카운트를 검사해줘야한다. 동적할당을 위해서도 이 코드가 필요하다.
				if(tmpImageInfo->type == 0 || tmpImageInfo->type == 1 || tmpImageInfo->type == 4)
					n_tempTileCount++;
				else if(tmpImageInfo->type == 2)
					n_tempImageEffectCount++;
				else
					n_tempGifEffectCount++;

				// List 에 추가해준다고 보면 된다.
				AddSurface(tmpImageInfo);
			}
		}
	}

	fclose(fp);

	// 여기서 동적할당을 해준다.
	// 인덱스를 꼭 저장하고 있어야한다.
	m_nImageIndex = new int[m_ImageCount];
	m_nImageEffectIndex = new int[m_ImageCount];
	m_nGifEffectIndex = new int[m_ImageCount];

	// 동적 할당을 해준다. 이미지 크기만큼. 이것은 마우스 down 과 over 시에 각종 이벤트를 체크하는데 쓰이게 될 것이다.
	m_MouseDownCheck = new bool[m_ImageCount];
	m_MouseOverCheck = new bool[m_ImageCount];

	// 임펙트를 생성하자
	m_Effects.Initialize(g_cApplication.getgraphics(), n_tempImageEffectCount, n_tempGifEffectCount);

	// 예외 상황 체크.
	if( TotalImage != m_ImageCount)
		g_cApplication.DisplayError( FALSE, "이미지 파일의 숫자와 불러들인 숫자가 맞지 않습니다.");
	
	// 타일을 생성함.
	// 전체 타일 카운터 숫자를 넣어준다.
#ifdef _DEBUG
	ASSERT(m_Tiles.Create(g_cApplication.getgraphics(), m_ImageCount));
#else if
	m_Tiles.Create(g_cApplication.getgraphics(), m_ImageCount);
#endif

	// 마우스 존을 초기화 시킨다.
	m_MouseZone.Initialize( TotalButton );

	// 암호화 클레스
	BlowFishData BlowFish;

	// 이제 타일을 로드 해야한다.
	for(i = 0; i < m_ImageCount; i++)
	{
#ifdef _DEBUG
		// 디버그 모드일때는 리소스들을 cik 로 무저건 바꿔준다. 로드리스트와 마찬가지의 개념
		// 그러면 릴리즈 모드일때는 디버그모드로 한번 실행해주고 나서 릴리즈로 하면 깔끔하다.
		char LoadFileName[200];
		char SaveFileName[200];
		memset(LoadFileName, NULL, 200);
		memset(SaveFileName, NULL, 200);
		int n_strlen = strlen(m_ImageList[i]->ParentName);
		strcpy(SaveFileName, m_ImageList[i]->ParentName);
		strcpy(LoadFileName, m_ImageList[i]->ParentName);
		// 확장자를 cik 로 바꾼다
		SaveFileName[n_strlen-1] = 'k';
		SaveFileName[n_strlen-2] = 'i';
		SaveFileName[n_strlen-3] = 'c';
		// cik 파일로 암호화한다.
		BlowFish.FileSave(LoadFileName, SaveFileName);
		// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
		BlowFish.FileLoad(SaveFileName);
#else
		// 릴리즈 모드일때는 gif 로 들어온 확장자를 cik 로 바꾼다. 그리고 그냥 로드하면 된다.
		char LoadFileName[200];
		memset(LoadFileName, NULL, 200);
		int n_strlen = strlen(m_ImageList[i]->ParentName);
		strcpy(LoadFileName, m_ImageList[i]->ParentName);
		// 확장자를 cik 로 바꾼다
		LoadFileName[n_strlen-1] = 'k';
		LoadFileName[n_strlen-2] = 'i';
		LoadFileName[n_strlen-3] = 'c';
		// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
		BlowFish.FileLoad(LoadFileName);
#endif

		// 초기화
		m_MouseDownCheck[i] = false;
		m_MouseOverCheck[i] = false;
		// 일반 이미지 이거나 버튼일때만
		if(m_ImageList[i]->type == 0 || m_ImageList[i]->type == 1 || m_ImageList[i]->type == 4)
		{
			if(m_ImageList[i]->type != 4)
			{
				// 실제 이미지를 로드한다.
				if(m_Tiles.Load( n_TileCount, BlowFish.generateTempFile(), m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,255), D3DFORMAT(m_ImageList[i]->format) ) == false)
				{
					char c_error[200];
					sprintf(c_error, "%s 이미지 파일을 불러들일 수 없습니다.", m_ImageList[i]->ParentName);
					g_cApplication.DisplayError( true, c_error);
				}
				else
				{
					// 인덱스를 저장해둔다. Draw 할때 필요하다.
					m_nImageIndex[i] = n_TileCount;
					n_TileCount++;
				}
			}
			else
			{
				// 인덱스를 저장해둔다. Draw 할때 필요하다.
				m_nImageIndex[i] = n_TileCount;
				n_TileCount++;
			}
			// 만약 버튼이면 or 마우스 좌표이면
			if(m_ImageList[i]->type == 1 || m_ImageList[i]->type == 4)
			{
				// 마우스 존을 설정해 줘야 한다.
				m_MouseZone.Add( m_ImageList[i]->flag1,	m_ImageList[i]->destX,   m_ImageList[i]->destY,  m_ImageList[i]->cx, m_ImageList[i]->cy, MOUSECLICK_LBUTTONDOWN );
				m_MouseZone.Add( m_ImageList[i]->flag2,	m_ImageList[i]->destX,   m_ImageList[i]->destY,  m_ImageList[i]->cx, m_ImageList[i]->cy, MOUSECLICK_LRBUTTONUP );
			}
		}
		// gif 아닌 그림파일 애니메이션
		else if(m_ImageList[i]->type == 2)
		{
			if(m_Effects.Get_BGMEffects()->Load(  n_ImageEffectCount,BlowFish.generateTempFile(),	m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,0),D3DFORMAT(m_ImageList[i]->format) ) == false)
			{
				char c_error[200];
				sprintf(c_error, "%s 이미지 파일을 불러들일 수 없습니다.", m_ImageList[i]->ParentName);
				g_cApplication.DisplayError( true, c_error);
			}
			else
			{
				// 인덱스를 저장해둔다. Draw 할때 필요하다.
				m_nImageEffectIndex[i] = n_ImageEffectCount;
				n_ImageEffectCount++;
			}
		}
		// gif 애니메이션
		else if(m_ImageList[i]->type == 3)
		{
			if(m_Effects.Get_GIFEffects()->Load(  n_GifEffectCount, BlowFish.generateTempFile(),	m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,0),D3DFORMAT(m_ImageList[i]->format) ) == false)
			{
				char c_error[200];
				sprintf(c_error, "%s 이미지 파일을 불러들일 수 없습니다.", m_ImageList[i]->ParentName);
				g_cApplication.DisplayError( true, c_error);
			}
			else
			{
				// 인덱스를 저장해둔다. Draw 할때 필요하다.
				m_nGifEffectIndex[i] = n_GifEffectCount;
				n_GifEffectCount++;
			}
		}
	}

	return true;
}

// 이미지네임을 입력하번 그 정보를 바탕으로 Draw 를 해준다
BYTE CImageManager::Draw(char *ImageName, int x, int y, D3DCOLOR color)
{
	// 마우스 & 키보드 정보를 얻어온다.
	m_Keyboard.Acquire( TRUE );
	m_Keyboard.Read();
	m_Mouse.Acquire( TRUE );
	m_Mouse.Read();

	m_lMouseXPos	= m_Mouse.GetXPos();
	m_lMouseYPos	= m_Mouse.GetYPos();
	m_bLButtonDown	= m_Mouse.GetButtonState( MOUSE_LBUTTON );
	m_bRButtonDown	= m_Mouse.GetButtonState( MOUSE_RBUTTON );
	//////////////////////////////////
	m_bButtonUp = !( m_bLButtonDown || m_bRButtonDown );
		
	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{
			m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown ,m_ImageList[i]->flag1, m_ImageList[i]->flag2 );

			// 마우스 다운 입력을 다시 받게 해준다.
			if( m_bButtonUp )  // 마우스 오버일때는 여기서 초기화 시켜주면 안된다.
				m_MouseDownCheck[i] = false;

			// 버튼이 아닐떄
			if(m_ImageList[i]->type == 0)
			{
				// 일반 이미지이기 떄문에 그냥 0이다
				if(x == 0 && y == 0) // x,y 좌표가 입력이 안되었거나 0이라면
					m_Tiles.Draw(m_nImageIndex[i], 0, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
				else
					m_Tiles.Draw(m_nImageIndex[i], 0, x, y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

				m_ZoneId= 0;
//				return 0;
			}
			// 버튼 일때
			else if(m_ImageList[i]->type == 1) // 마우스 over
			{
				// 버튼일떄. 마우스 상태에 따라서 따로 그려줘야함.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					if(x == 0 && y == 0) // x,y 좌표가 입력이 안되었거나 0이라면
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+1, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+1, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					// 마우스 오버도 1번이 되어야 한다. 마우스 오버를 체크해준다.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // 마우스 오버 했다.
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId) // 마우스 down
				{
					// 이것이 0일때만 down 입력을 받아야 한다. 그래야 중복 down 입력이 안된다.
					if(x == 0 && y == 0) // x,y 좌표가 입력이 안되었거나 0이라면
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+2, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+2, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					// 마우스 다운의 새로운 원리. 한번만 누르게 되되 누르고 있는 애니메이션 계속 출력되게한다.
					if(m_MouseDownCheck[i] != 1)
					{
						m_MouseDownCheck[i] = 1;
						return 2;
					}
					else 
						return 0;
				}
				else
				{
					if(x == 0 && y == 0) // x,y 좌표가 입력이 안되었거나 0이라면
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					m_MouseOverCheck[i] = false;
					return 0;
				}
			}
			// 그림은 안그래도 마우스 좌표만 체크함
			else if(m_ImageList[i]->type == 4)
			{
				// 버튼일떄. 마우스 상태에 따라서 따로 그려줘야함.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					// 마우스 오버도 1번이 되어야 한다. 마우스 오버를 체크해준다.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // 마우스 오버 했다.
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId)
				{
					// 이것이 0일때만 down 입력을 받아야 한다. 그래야 중복 down 입력이 안된다.
					if(m_MouseDownCheck[i] != 1)
					{
						m_MouseDownCheck[i] = 1;
						return 2;
					}
					else 
						return 0;
				}
				else
				{
					return 0;
				}
			}
		}
	}

	m_ZoneIdDown = 0;

	return 0;
}

BYTE CImageManager::Draw(char *ImageName, int tilenum, int destX, int destY, D3DCOLOR color)
{
	// 마우스 & 키보드 정보를 얻어온다.
	m_Keyboard.Acquire( TRUE );
	m_Keyboard.Read();
	m_Mouse.Acquire( TRUE );
	m_Mouse.Read();

	m_lMouseXPos	= m_Mouse.GetXPos();
	m_lMouseYPos	= m_Mouse.GetYPos();
	m_bLButtonDown	= m_Mouse.GetButtonState( MOUSE_LBUTTON );
	m_bRButtonDown	= m_Mouse.GetButtonState( MOUSE_RBUTTON );
	//////////////////////////////////
	m_bButtonUp = !( m_bLButtonDown || m_bRButtonDown );

	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{
			m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown ,m_ImageList[i]->flag1, m_ImageList[i]->flag2 );

			// 마우스 다운 입력을 다시 받게 해준다.
			if( m_bButtonUp )  // 마우스 오버일때는 여기서 초기화 시켜주면 안된다.
				m_MouseDownCheck[i] = false;

			// 버튼이 아닐떄
			if(m_ImageList[i]->type == 0)
			{
				// 일반 이미지이기 떄문에 그냥 0이다
				m_Tiles.Draw(m_nImageIndex[i], tilenum, destX, destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
				m_ZoneId= 0;
				return 0;
			}
			// 버튼 일때
			else if(m_ImageList[i]->type == 1)
			{
				// 버튼일떄. 마우스 상태에 따라서 따로 그려줘야함.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					m_Tiles.Draw(m_nImageIndex[i], tilenum+1, destX, destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					// 마우스 오버도 1번이 되어야 한다. 마우스 오버를 체크해준다.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // 마우스 오버 했다.
						return 1;
					}
					else
					{
						return 0;
					};
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId)
				{
					m_Tiles.Draw(m_nImageIndex[i], tilenum+2, destX, destY, color, m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					m_Mouse.SetXPos(-1);
					// 마우스 다운의 새로운 원리. 한번만 누르게 되되 누르고 있는 애니메이션 계속 출력되게한다.
					if(m_MouseDownCheck[i] != 1)
					{
						m_MouseDownCheck[i] = 1;
						return 2;
					}
					else 
						return 0;
				}
				else
				{
					m_Tiles.Draw(m_nImageIndex[i], tilenum, destX, destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					return 0;
				}
			}
			// 그림은 안그래도 마우스 좌표만 체크함
			else if(m_ImageList[i]->type == 4)
			{
				// 버튼일떄. 마우스 상태에 따라서 따로 그려줘야함.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					return 1;
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId)
				{
					m_Mouse.SetXPos(-1);
					m_ZoneId = 0;
					return 2;
				}
				else
				{
					return 0;
				}
			}
		}
	}
}
// 임펙트를 플레이 할 수 있게 하는 함수. 플레이할 변수 이름과, 한번 할껀지 계속 할껀지 등의 플레그, 그리고 플레이할 시간등이 들어간다.
void CImageManager::EffectPlay(char *ImageName, EFFECT_PLAY_TYPE n_flag, DWORD n_time, int position_x, int position_y)
{
	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{

			// 포지션이 설정이 안되있다면 로드리스트의 포지션으로 출력한다. 기본값이 -1
			if(position_x == -1 && position_y == -1)
			{
				if(m_ImageList[i]->type == 2)
					m_Effects.Play( EFFECTTYPE_BMP, m_nImageEffectIndex[i], m_ImageList[i]->destX, m_ImageList[i]->destY, n_flag, n_time );
				else
					m_Effects.Play( EFFECTTYPE_GIF, m_nGifEffectIndex[i],	m_ImageList[i]->destX, m_ImageList[i]->destY, n_flag, n_time );

				break;
			}
			// 포지션 설정이 되어있을떄.
			else
			{
				if(m_ImageList[i]->type == 2)
					m_Effects.Play( EFFECTTYPE_BMP, m_nImageEffectIndex[i], position_x, position_y, n_flag, n_time );
				else
					m_Effects.Play( EFFECTTYPE_GIF, m_nGifEffectIndex[i],	position_x, position_y, n_flag, n_time );

				break;
			}
		}
	}
}

// 임팩트들 랜더링 하는 함수.
void CImageManager::EffectDraw()
{
	m_Effects.Render();
}

// 이미지 리스트에 읽어들인 내용을 추가한다.
bool CImageManager::AddSurface( tsImageInfo* ImageInfo )
{
	if(m_ImageCount==0)
		m_ImageList = (tsImageInfo**)malloc(sizeof(tsImageInfo*));
	else
		m_ImageList = (tsImageInfo**)realloc(m_ImageList, sizeof(tsImageInfo*)*(m_ImageCount+1));

	if(ImageInfo)
	{
		m_ImageList[m_ImageCount++] = ImageInfo;
	}

	return 0;
}

// 외부에서 이미지 리스트에 접근하고자 할때 이것을 이용하여 이미지 리스트의 어디에 위치해 있나 알아 낼 수 있다.
int CImageManager::Get_ImageIndex(char *imagename)
{
	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(imagename, m_ImageList[i]->Name))
		{
			return i;
		}
	}

	return -1;
}

tsImageInfo*	CImageManager::Get_ImageList(char *imagename)
{
	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(imagename, m_ImageList[i]->Name))
		{
			return m_ImageList[i];
		}
	}

	return NULL;
}

bool	CImageManager::IsEndEffect(char *imagename)
{
	bool check = true;
	// 이미지 카운트 만큼 검사하낟.
	for(int i=0;i<m_ImageCount;i++)
	{
		// 입력 받은 값과 맞는 name 이라면
		if(!strcmp(imagename, m_ImageList[i]->Name))
		{
			check = m_Effects.Get_GIFEffects()->IsEndOfPlay(m_nGifEffectIndex[i]);
			break;
		}
	}

	return check;
}

// 이미지 메니져를 소멸 시켜 준다.
bool CImageManager::CleanUp()
{
	UINT i;

	if(m_ImageList)
	{
		for(i=0;i<m_ImageCount;i++)
		{
			if (m_ImageList[i] !=NULL)
			{
				delete m_ImageList[i];
			}
		}

		free(m_ImageList);
		m_ImageList = 0;
	}

	m_ImageCount = 0;

	m_Tiles.Free();
	m_MouseZone.Free();

	delete [] m_nImageIndex;
	delete [] m_nImageEffectIndex;
	delete [] m_nGifEffectIndex;
	delete [] m_MouseDownCheck;
	delete [] m_MouseOverCheck;

	return 0;
}
