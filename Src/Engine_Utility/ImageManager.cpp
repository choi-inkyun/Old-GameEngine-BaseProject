
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

// �̹����� �ε��Ѵ�.
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
	// ������ gif �� �ε��Ϸ��� ���� �߰��� ��� �ϴµ� effect �� �׳� ���� �д�.
	int n_TileCount = 0, n_tempTileCount = 0;
	int n_ImageEffectCount = 0, n_tempImageEffectCount = 0;
	int n_GifEffectCount = 0, n_tempGifEffectCount = 0;

	int color_r, color_g, color_b;

	// input ��ü�� �����Ѵ�. input, mouse, keyboard ��ü ����
#ifdef _DEBUG
	ASSERT( m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() ) );
#else
	m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() );
#endif
	m_Keyboard.Create( &m_Input, KEYBOARD, g_cApplication.Get_Windowed() );
	m_Mouse.Create( &m_Input, MOUSE, g_cApplication.Get_Windowed() );

	FILE *fp;
	// ������ ������.
	fp = fopen(FileName, "rt");
	if (!fp)
	{
		g_cApplication.DisplayError( TRUE, "Image loadlist ������ �����ϴ�.");
		return false;
	}

	fgets(temp,256,fp); // ����. �ִ� 256 Byte ��ŭ Read ��
	sscanf(temp,"IMAGECOUNT=%d %d\n", &TotalImage, &TotalButton);

	fgets(temp,256,fp); // ����. �ִ� 256 Byte ��ŭ Read ��
	sscanf(temp,"CLOLORKEY=%d %d %d\n", &color_r, &color_g, &color_b);

	// ���� ������ ó���Ѵ�.
	while(!feof(fp))
	{
		char tmpparentname[512];

		fgets(temp,256,fp); // ����. �ִ� 256 Byte ��ŭ Read ��
		if(!strnicmp(temp,"//",2))
		{
			// �̰� �ּ� ���̹Ƿ� ���� ó���� ������ �ʴ°��� ��Ģ���� ���ش�.
		}
		// �� ���� �ε��� ����
		else if(!strnicmp(temp,"FILENAME=",9))
		{
			sscanf(temp,"FILENAME=%s %d %d\n", tmpParentName2, &tmpParentX, &tmpParentY);

			/////////////////////////////////////////////////////////     / -> \\ �ٲ��ִ� ��ƾ 
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

		// �ι�° �ٿ� �� ���Ͽ��� ��� Ÿ���� �о� ������ �����Ѱ� �ҷ��´�.
		else if(!strnicmp(temp,"PCOUNT=",7))
		{
			sscanf(temp,"PCOUNT=%d\n", &temcount);

			for(i=0;i<temcount;i++)
			{
				tsImageInfo *tmpImageInfo;

				tmpImageInfo = new tsImageInfo;

				// �о� ���δ�. ����������...
				fgets(temp,256,fp);
				sscanf(temp, "%d=%s %d %d %d %d %d %f %f %d %d %d %d\n", 
					&j, tmpImageInfo->Name,
					&tmpImageInfo->tilenum,
					&tmpImageInfo->cx, &tmpImageInfo->cy, &tmpImageInfo->destX, &tmpImageInfo->destY,
					&tmpImageInfo->scaleX, &tmpImageInfo->scaleY, &tmpImageInfo->type, &tmpImageInfo->flag1, &tmpImageInfo->flag2, &tmpImageInfo->format);

				strcpy(tmpImageInfo->ParentName,tmpparentname);  // daniel 2002.9.19

				// ī��Ʈ�� �˻�������Ѵ�. �����Ҵ��� ���ؼ��� �� �ڵ尡 �ʿ��ϴ�.
				if(tmpImageInfo->type == 0 || tmpImageInfo->type == 1 || tmpImageInfo->type == 4)
					n_tempTileCount++;
				else if(tmpImageInfo->type == 2)
					n_tempImageEffectCount++;
				else
					n_tempGifEffectCount++;

				// List �� �߰����شٰ� ���� �ȴ�.
				AddSurface(tmpImageInfo);
			}
		}
	}

	fclose(fp);

	// ���⼭ �����Ҵ��� ���ش�.
	// �ε����� �� �����ϰ� �־���Ѵ�.
	m_nImageIndex = new int[m_ImageCount];
	m_nImageEffectIndex = new int[m_ImageCount];
	m_nGifEffectIndex = new int[m_ImageCount];

	// ���� �Ҵ��� ���ش�. �̹��� ũ�⸸ŭ. �̰��� ���콺 down �� over �ÿ� ���� �̺�Ʈ�� üũ�ϴµ� ���̰� �� ���̴�.
	m_MouseDownCheck = new bool[m_ImageCount];
	m_MouseOverCheck = new bool[m_ImageCount];

	// ����Ʈ�� ��������
	m_Effects.Initialize(g_cApplication.getgraphics(), n_tempImageEffectCount, n_tempGifEffectCount);

	// ���� ��Ȳ üũ.
	if( TotalImage != m_ImageCount)
		g_cApplication.DisplayError( FALSE, "�̹��� ������ ���ڿ� �ҷ����� ���ڰ� ���� �ʽ��ϴ�.");
	
	// Ÿ���� ������.
	// ��ü Ÿ�� ī���� ���ڸ� �־��ش�.
#ifdef _DEBUG
	ASSERT(m_Tiles.Create(g_cApplication.getgraphics(), m_ImageCount));
#else if
	m_Tiles.Create(g_cApplication.getgraphics(), m_ImageCount);
#endif

	// ���콺 ���� �ʱ�ȭ ��Ų��.
	m_MouseZone.Initialize( TotalButton );

	// ��ȣȭ Ŭ����
	BlowFishData BlowFish;

	// ���� Ÿ���� �ε� �ؾ��Ѵ�.
	for(i = 0; i < m_ImageCount; i++)
	{
#ifdef _DEBUG
		// ����� ����϶��� ���ҽ����� cik �� ������ �ٲ��ش�. �ε帮��Ʈ�� ���������� ����
		// �׷��� ������ ����϶��� ����׸��� �ѹ� �������ְ� ���� ������� �ϸ� ����ϴ�.
		char LoadFileName[200];
		char SaveFileName[200];
		memset(LoadFileName, NULL, 200);
		memset(SaveFileName, NULL, 200);
		int n_strlen = strlen(m_ImageList[i]->ParentName);
		strcpy(SaveFileName, m_ImageList[i]->ParentName);
		strcpy(LoadFileName, m_ImageList[i]->ParentName);
		// Ȯ���ڸ� cik �� �ٲ۴�
		SaveFileName[n_strlen-1] = 'k';
		SaveFileName[n_strlen-2] = 'i';
		SaveFileName[n_strlen-3] = 'c';
		// cik ���Ϸ� ��ȣȭ�Ѵ�.
		BlowFish.FileSave(LoadFileName, SaveFileName);
		// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
		BlowFish.FileLoad(SaveFileName);
#else
		// ������ ����϶��� gif �� ���� Ȯ���ڸ� cik �� �ٲ۴�. �׸��� �׳� �ε��ϸ� �ȴ�.
		char LoadFileName[200];
		memset(LoadFileName, NULL, 200);
		int n_strlen = strlen(m_ImageList[i]->ParentName);
		strcpy(LoadFileName, m_ImageList[i]->ParentName);
		// Ȯ���ڸ� cik �� �ٲ۴�
		LoadFileName[n_strlen-1] = 'k';
		LoadFileName[n_strlen-2] = 'i';
		LoadFileName[n_strlen-3] = 'c';
		// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
		BlowFish.FileLoad(LoadFileName);
#endif

		// �ʱ�ȭ
		m_MouseDownCheck[i] = false;
		m_MouseOverCheck[i] = false;
		// �Ϲ� �̹��� �̰ų� ��ư�϶���
		if(m_ImageList[i]->type == 0 || m_ImageList[i]->type == 1 || m_ImageList[i]->type == 4)
		{
			if(m_ImageList[i]->type != 4)
			{
				// ���� �̹����� �ε��Ѵ�.
				if(m_Tiles.Load( n_TileCount, BlowFish.generateTempFile(), m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,255), D3DFORMAT(m_ImageList[i]->format) ) == false)
				{
					char c_error[200];
					sprintf(c_error, "%s �̹��� ������ �ҷ����� �� �����ϴ�.", m_ImageList[i]->ParentName);
					g_cApplication.DisplayError( true, c_error);
				}
				else
				{
					// �ε����� �����صд�. Draw �Ҷ� �ʿ��ϴ�.
					m_nImageIndex[i] = n_TileCount;
					n_TileCount++;
				}
			}
			else
			{
				// �ε����� �����صд�. Draw �Ҷ� �ʿ��ϴ�.
				m_nImageIndex[i] = n_TileCount;
				n_TileCount++;
			}
			// ���� ��ư�̸� or ���콺 ��ǥ�̸�
			if(m_ImageList[i]->type == 1 || m_ImageList[i]->type == 4)
			{
				// ���콺 ���� ������ ��� �Ѵ�.
				m_MouseZone.Add( m_ImageList[i]->flag1,	m_ImageList[i]->destX,   m_ImageList[i]->destY,  m_ImageList[i]->cx, m_ImageList[i]->cy, MOUSECLICK_LBUTTONDOWN );
				m_MouseZone.Add( m_ImageList[i]->flag2,	m_ImageList[i]->destX,   m_ImageList[i]->destY,  m_ImageList[i]->cx, m_ImageList[i]->cy, MOUSECLICK_LRBUTTONUP );
			}
		}
		// gif �ƴ� �׸����� �ִϸ��̼�
		else if(m_ImageList[i]->type == 2)
		{
			if(m_Effects.Get_BGMEffects()->Load(  n_ImageEffectCount,BlowFish.generateTempFile(),	m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,0),D3DFORMAT(m_ImageList[i]->format) ) == false)
			{
				char c_error[200];
				sprintf(c_error, "%s �̹��� ������ �ҷ����� �� �����ϴ�.", m_ImageList[i]->ParentName);
				g_cApplication.DisplayError( true, c_error);
			}
			else
			{
				// �ε����� �����صд�. Draw �Ҷ� �ʿ��ϴ�.
				m_nImageEffectIndex[i] = n_ImageEffectCount;
				n_ImageEffectCount++;
			}
		}
		// gif �ִϸ��̼�
		else if(m_ImageList[i]->type == 3)
		{
			if(m_Effects.Get_GIFEffects()->Load(  n_GifEffectCount, BlowFish.generateTempFile(),	m_ImageList[i]->cx, m_ImageList[i]->cy, D3DCOLOR_RGBA(color_r, color_g,color_b,0),D3DFORMAT(m_ImageList[i]->format) ) == false)
			{
				char c_error[200];
				sprintf(c_error, "%s �̹��� ������ �ҷ����� �� �����ϴ�.", m_ImageList[i]->ParentName);
				g_cApplication.DisplayError( true, c_error);
			}
			else
			{
				// �ε����� �����صд�. Draw �Ҷ� �ʿ��ϴ�.
				m_nGifEffectIndex[i] = n_GifEffectCount;
				n_GifEffectCount++;
			}
		}
	}

	return true;
}

// �̹��������� �Է��Ϲ� �� ������ �������� Draw �� ���ش�
BYTE CImageManager::Draw(char *ImageName, int x, int y, D3DCOLOR color)
{
	// ���콺 & Ű���� ������ ���´�.
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
		
	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{
			m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown ,m_ImageList[i]->flag1, m_ImageList[i]->flag2 );

			// ���콺 �ٿ� �Է��� �ٽ� �ް� ���ش�.
			if( m_bButtonUp )  // ���콺 �����϶��� ���⼭ �ʱ�ȭ �����ָ� �ȵȴ�.
				m_MouseDownCheck[i] = false;

			// ��ư�� �ƴҋ�
			if(m_ImageList[i]->type == 0)
			{
				// �Ϲ� �̹����̱� ������ �׳� 0�̴�
				if(x == 0 && y == 0) // x,y ��ǥ�� �Է��� �ȵǾ��ų� 0�̶��
					m_Tiles.Draw(m_nImageIndex[i], 0, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
				else
					m_Tiles.Draw(m_nImageIndex[i], 0, x, y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

				m_ZoneId= 0;
//				return 0;
			}
			// ��ư �϶�
			else if(m_ImageList[i]->type == 1) // ���콺 over
			{
				// ��ư�ϋ�. ���콺 ���¿� ���� ���� �׷������.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					if(x == 0 && y == 0) // x,y ��ǥ�� �Է��� �ȵǾ��ų� 0�̶��
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+1, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+1, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					// ���콺 ������ 1���� �Ǿ�� �Ѵ�. ���콺 ������ üũ���ش�.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // ���콺 ���� �ߴ�.
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId) // ���콺 down
				{
					// �̰��� 0�϶��� down �Է��� �޾ƾ� �Ѵ�. �׷��� �ߺ� down �Է��� �ȵȴ�.
					if(x == 0 && y == 0) // x,y ��ǥ�� �Է��� �ȵǾ��ų� 0�̶��
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+2, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum+2, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					// ���콺 �ٿ��� ���ο� ����. �ѹ��� ������ �ǵ� ������ �ִ� �ִϸ��̼� ��� ��µǰ��Ѵ�.
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
					if(x == 0 && y == 0) // x,y ��ǥ�� �Է��� �ȵǾ��ų� 0�̶��
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum, m_ImageList[i]->destX, m_ImageList[i]->destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					else
						m_Tiles.Draw(m_nImageIndex[i], m_ImageList[i]->tilenum, x,y, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);

					m_MouseOverCheck[i] = false;
					return 0;
				}
			}
			// �׸��� �ȱ׷��� ���콺 ��ǥ�� üũ��
			else if(m_ImageList[i]->type == 4)
			{
				// ��ư�ϋ�. ���콺 ���¿� ���� ���� �׷������.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					// ���콺 ������ 1���� �Ǿ�� �Ѵ�. ���콺 ������ üũ���ش�.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // ���콺 ���� �ߴ�.
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else if(m_ImageList[i]->flag1 == m_ZoneId)
				{
					// �̰��� 0�϶��� down �Է��� �޾ƾ� �Ѵ�. �׷��� �ߺ� down �Է��� �ȵȴ�.
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
	// ���콺 & Ű���� ������ ���´�.
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

	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{
			m_ZoneId = m_MouseZone.Check( m_lMouseXPos, m_lMouseYPos, m_bLButtonDown, m_bRButtonDown ,m_ImageList[i]->flag1, m_ImageList[i]->flag2 );

			// ���콺 �ٿ� �Է��� �ٽ� �ް� ���ش�.
			if( m_bButtonUp )  // ���콺 �����϶��� ���⼭ �ʱ�ȭ �����ָ� �ȵȴ�.
				m_MouseDownCheck[i] = false;

			// ��ư�� �ƴҋ�
			if(m_ImageList[i]->type == 0)
			{
				// �Ϲ� �̹����̱� ������ �׳� 0�̴�
				m_Tiles.Draw(m_nImageIndex[i], tilenum, destX, destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
				m_ZoneId= 0;
				return 0;
			}
			// ��ư �϶�
			else if(m_ImageList[i]->type == 1)
			{
				// ��ư�ϋ�. ���콺 ���¿� ���� ���� �׷������.
				if(m_ImageList[i]->flag2 == m_ZoneId)
				{
					m_Tiles.Draw(m_nImageIndex[i], tilenum+1, destX, destY, color,m_ImageList[i]->scaleX, m_ImageList[i]->scaleY);
					// ���콺 ������ 1���� �Ǿ�� �Ѵ�. ���콺 ������ üũ���ش�.
					if(m_MouseOverCheck[i] == false)
					{
						m_MouseOverCheck[i] = 1; // ���콺 ���� �ߴ�.
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
					// ���콺 �ٿ��� ���ο� ����. �ѹ��� ������ �ǵ� ������ �ִ� �ִϸ��̼� ��� ��µǰ��Ѵ�.
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
			// �׸��� �ȱ׷��� ���콺 ��ǥ�� üũ��
			else if(m_ImageList[i]->type == 4)
			{
				// ��ư�ϋ�. ���콺 ���¿� ���� ���� �׷������.
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
// ����Ʈ�� �÷��� �� �� �ְ� �ϴ� �Լ�. �÷����� ���� �̸���, �ѹ� �Ҳ��� ��� �Ҳ��� ���� �÷���, �׸��� �÷����� �ð����� ����.
void CImageManager::EffectPlay(char *ImageName, EFFECT_PLAY_TYPE n_flag, DWORD n_time, int position_x, int position_y)
{
	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
		if(!strcmp(ImageName, m_ImageList[i]->Name))
		{

			// �������� ������ �ȵ��ִٸ� �ε帮��Ʈ�� ���������� ����Ѵ�. �⺻���� -1
			if(position_x == -1 && position_y == -1)
			{
				if(m_ImageList[i]->type == 2)
					m_Effects.Play( EFFECTTYPE_BMP, m_nImageEffectIndex[i], m_ImageList[i]->destX, m_ImageList[i]->destY, n_flag, n_time );
				else
					m_Effects.Play( EFFECTTYPE_GIF, m_nGifEffectIndex[i],	m_ImageList[i]->destX, m_ImageList[i]->destY, n_flag, n_time );

				break;
			}
			// ������ ������ �Ǿ�������.
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

// ����Ʈ�� ������ �ϴ� �Լ�.
void CImageManager::EffectDraw()
{
	m_Effects.Render();
}

// �̹��� ����Ʈ�� �о���� ������ �߰��Ѵ�.
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

// �ܺο��� �̹��� ����Ʈ�� �����ϰ��� �Ҷ� �̰��� �̿��Ͽ� �̹��� ����Ʈ�� ��� ��ġ�� �ֳ� �˾� �� �� �ִ�.
int CImageManager::Get_ImageIndex(char *imagename)
{
	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
		if(!strcmp(imagename, m_ImageList[i]->Name))
		{
			return i;
		}
	}

	return -1;
}

tsImageInfo*	CImageManager::Get_ImageList(char *imagename)
{
	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
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
	// �̹��� ī��Ʈ ��ŭ �˻��ϳ�.
	for(int i=0;i<m_ImageCount;i++)
	{
		// �Է� ���� ���� �´� name �̶��
		if(!strcmp(imagename, m_ImageList[i]->Name))
		{
			check = m_Effects.Get_GIFEffects()->IsEndOfPlay(m_nGifEffectIndex[i]);
			break;
		}
	}

	return check;
}

// �̹��� �޴����� �Ҹ� ���� �ش�.
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
