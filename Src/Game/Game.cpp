
// * ���� : Release ��� �϶��� ASSERT() �� �׳� ����ġ�Ƿ� ���� ó���� �ؾ���.

/*
	���� : ���α�
	������ Main �̶�� �� �� �ִ� �κ��Դϴ�.
	g_cGame ��� �ν��Ͻ��� ���� ���� �� �� ������,
	�� ��ü�� �ν��ͽ��� ������ �ֱ� ������ �ſ� �߿��� ��Ȱ�� �ϰ� �˴ϴ�.
*/


#include "stdafx.h"
#include "Game.h"
//-----------------------------------------------------------------------------
// Global access sIngleton
//-----------------------------------------------------------------------------
_CGame			cGameBlock;
unsigned long	_init_CGame::ms_dwLong;

///////////////////////////////////////////////////////////////
////////////////////////// ���� ���μ��� ///////////////////////

bool CGame::Process()
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

	if( g_cMainEngine.m_RenderState == RENDERSTATE_INVALID ) return S_OK;

	// free all of resources
//	g_cMainEngine.Cleanup3DEnvironment();
	g_cStateManager.Pop();

	BOOL bExit = FALSE;

	// �߿��� ���� ���μ��� �̴�.
	// ȭ���� �ѱ� �� ����Ѵ�.
	// ����������Ʈ�� �ٲ�� �׿����� �� ȭ�鿡 �´� Ŭ������ proc �� push �ȴ�
	// �׸��� ���� �ٽ� �ʱ�ȭ�� �����ش�. �׷��� �� proc �� ���� ������ ���۵ǰ� �ȴ�.
	switch( g_cMainEngine.m_RenderState )
	{
	case RENDERSTATE_EXIT: // ������ �����Ѵ�.
		bExit = TRUE;
		SetGameState(GAME_EXIT);
		break;
	case RENDERSTATE_LOGIN: // �α��� ȭ������
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_LOBBY);
		break;
	case RENDERSTATE_CHANNEL: // ä�� ȭ������..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_CHANNEL);
		break;
	case RENDERSTATE_LOBBY: // �κ� ȭ������..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_LOBBY);
		break;
	case RENDERSTATE_BOARD: // ���� ȭ������..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_BOARD);
		break;
	default:
		ASSERT( !"invalid render state!" );
	}

	// �־������ϱ�. �ϰ͵� ���� ����.
	g_cMainEngine.m_RenderState = RENDERSTATE_INVALID;

	return true;
}
/////////////////////////////////////////////////////////////
//////////////////////// ���� ������ ////////////////////////

bool CGame::Render()
{
	// ���� �����̽��� clear ���ش�.
	g_cApplication.getgraphics()->Clear( D3DCOLOR_RGBA(0, 255, 0, 0) );

	// ȭ�鿡 �׸��� �����Ѵ�.
	if( g_cApplication.getgraphics()->BeginScene() == TRUE )
	{
		// �׸��� �׸��� �����Ѵ�.
		g_cApplication.getgraphics()->BeginSprite();

		g_cGame.m_ImageManager.Draw("GAME_MAIN");
		DrawFPS();

		g_cApplication.getgraphics()->EndSprite(); // �׸� �׸��⸦ ������.

		g_cApplication.getgraphics()->EndScene(); // ȭ�鿡 �׸��°��� ������.
	}

	// ���� ����Ϳ� �ѷ����� ȭ������ �ѷ��ش�.
	g_cApplication.getgraphics()->Display();

	return true;
}

void CGame::DrawFPS()
{
#ifdef _DEBUG
	// FPS �� ����ش�.
	m_Font.Begin();
	char Text[200];
	sprintf(Text, "Render : %.02f FPS", g_cApplication.render_get_fps());
	m_Font.Print( Text, 15, 395, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "��� : %.02f FPS", g_cApplication.render_arage_get_fps());
	m_Font.Print( Text, 140, 395, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "Process : %.02f FPS", g_cApplication.process_get_fps());
	m_Font.Print( Text, 15, 410, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "��� : %.02f FPS", g_cApplication.process_arage_get_fps());
	m_Font.Print( Text, 140, 410, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );	
	sprintf(Text, "x = %d y = %d", m_lMouseXPos, m_lMouseYPos);
	m_Font.Print( Text, 15, 425, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	m_Font.End();
#endif
}

// ���� ��带 �ٲ��ش�.
void CGame::SetGameState(GAME_STATE gamestate)
{
	m_nGameState = gamestate;
}

// �ϴ� ���� ���� ��Ҹ� ����.
void CGame::SoundPlayVoice(char *soudname, char *soundname2)
{
	// �ϴ� �ƹ��ų� ��������. ���߿� ������ �������鼭 �ϰ� ���ľ� �� �� ����.
	if(rand()%2)
	{
		DSEffectManager.OpenSound(soudname);
		DSEffectManager.PlaySound(soudname);
	}
	else
	{
		DSEffectManager.OpenSound(soundname2);
		DSEffectManager.PlaySound(soundname2);
	}
}
// �ϳ��� ���常 �÷��� �� �� �ְ� �Ѵ�.
void CGame::SoundPlay(char *soudname)
{
	DSEffectManager.OpenSound(soudname);
	DSEffectManager.PlaySound(soudname);
}

// ��ũ������ ���� �� �̿��ϴ� �Լ��̴�.
// ��� ���� ��ÿ� ��. �׷� ������ string �� ���´�.
// �׷��� ������ �� ���� ������ �̿��Ѵ�. 
char *CGame::GetTime()
{
	static char szTime[256];
	char szYear[100];
	char szMonth[100];
	char szDay[100];
	char szHour[100];
	char szMinute[100];
	char szSecond[100];
	char szMSecond[100];

	memset(szTime, 0, sizeof(char)*256);
	memset(szYear, 0, sizeof(char)*100);
	memset(szMonth, 0, sizeof(char)*100);
	memset(szDay, 0, sizeof(char)*100);
	memset(szHour, 0, sizeof(char)*100);
	memset(szMinute, 0, sizeof(char)*100);
	memset(szSecond, 0, sizeof(char)*100);
	memset(szMSecond, 0, sizeof(char)*100);

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	itoa((int)SystemTime.wYear, szYear, 10);
	if(SystemTime.wMonth<10)
	{
		szMonth[0] = '0';
		itoa((int)SystemTime.wMonth, &szMonth[1], 10);
	}
	else
		itoa((int)SystemTime.wMonth, szMonth, 10);

	if(SystemTime.wDay<10)
	{
		szDay[0] = '0';
		itoa((int)SystemTime.wDay, &szDay[1], 10);
	}
	else
		itoa((int)SystemTime.wDay, szDay, 10);

	if(SystemTime.wHour<10)
	{
		szHour[0] = '0';
		itoa((int)SystemTime.wHour, &szHour[1], 10);
	}
	else
		itoa((int)SystemTime.wHour, szHour, 10);

	if(SystemTime.wMinute<10)
	{
		szMinute[0] = '0';
		itoa((int)SystemTime.wMinute, &szMinute[1], 10);
	}
	else
		itoa((int)SystemTime.wMinute, szMinute, 10);

	if(SystemTime.wSecond<10)
	{
		szSecond[0] = '0';
		itoa((int)SystemTime.wSecond, &szSecond[1], 10);
	}
	else
		itoa((int)SystemTime.wSecond, szSecond, 10);

	//	itoa((int)SystemTime.wMilliseconds, szMSecond, 10);

	strcat(szTime, szYear);
	strcat(szTime, szMonth);
	strcat(szTime, szDay);
	strcat(szTime, "-"); 		// ���� �߰� 	
	strcat(szTime, szHour);
	strcat(szTime, szMinute);
	strcat(szTime, szSecond);
	//	strcat(szTime, szMSecond);

	return szTime;
}

// ��ũ�� ĸ�ĸ� �ϴ� �Լ��̴�.
void CGame::ScreenCapture(bool b_check)
{
	HDC hdc;

	SoundPlay("BUTTON_CLICK"); // ��ư Ŭ�� �� �� ���� �Ҹ�.

	// �����̸��� �ִ´�.
	char szSaveName[256];
	strcpy(szSaveName, "ScreenCapture\\");
	strcat(szSaveName, g_cGame.GetTime());
	strcat(szSaveName, ".jpg");

	// ��ũ��ĸ��
	hdc = ::GetDC(g_cApplication.gethWnd()); 
	ScrCaptureToJPG(hdc,szSaveName, 100); 
	::ReleaseDC(g_cApplication.gethWnd(),hdc); 
}
