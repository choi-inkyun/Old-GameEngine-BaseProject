
// * 주의 : Release 모드 일때는 ASSERT() 를 그냥 지나치므로 따로 처리를 해야함.

/*
	제작 : 최인균
	게임의 Main 이라고 할 수 있는 부분입니다.
	g_cGame 라는 인스턴스를 통해 접근 할 수 있으며,
	각 객체의 인스터스도 가지고 있기 때문에 매우 중요한 역활을 하게 됩니다.
*/


#include "stdafx.h"
#include "Game.h"
//-----------------------------------------------------------------------------
// Global access sIngleton
//-----------------------------------------------------------------------------
_CGame			cGameBlock;
unsigned long	_init_CGame::ms_dwLong;

///////////////////////////////////////////////////////////////
////////////////////////// 게임 프로세스 ///////////////////////

bool CGame::Process()
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

	if( g_cMainEngine.m_RenderState == RENDERSTATE_INVALID ) return S_OK;

	// free all of resources
//	g_cMainEngine.Cleanup3DEnvironment();
	g_cStateManager.Pop();

	BOOL bExit = FALSE;

	// 중요한 게임 프로서세 이다.
	// 화면을 넘길 때 사용한다.
	// 랜더스테이트가 바뀌면 그에따라 그 화면에 맞는 클레스의 proc 가 push 된다
	// 그리고 나서 다시 초기화를 시켜준다. 그러면 그 proc 로 게임 정보가 전송되게 된다.
	switch( g_cMainEngine.m_RenderState )
	{
	case RENDERSTATE_EXIT: // 게임을 종료한다.
		bExit = TRUE;
		SetGameState(GAME_EXIT);
		break;
	case RENDERSTATE_LOGIN: // 로그인 화면으로
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_LOBBY);
		break;
	case RENDERSTATE_CHANNEL: // 채널 화면으로..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_CHANNEL);
		break;
	case RENDERSTATE_LOBBY: // 로비 화면으로..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_LOBBY);
		break;
	case RENDERSTATE_BOARD: // 게임 화면으로..
		g_cStateManager.Push( CGame::GameProc, this);
		SetGameState(GAME_BOARD);
		break;
	default:
		ASSERT( !"invalid render state!" );
	}

	// 넣어줬으니까. 암것도 없게 셋팅.
	g_cMainEngine.m_RenderState = RENDERSTATE_INVALID;

	return true;
}
/////////////////////////////////////////////////////////////
//////////////////////// 게임 랜더링 ////////////////////////

bool CGame::Render()
{
	// 게임 서페이스를 clear 해준다.
	g_cApplication.getgraphics()->Clear( D3DCOLOR_RGBA(0, 255, 0, 0) );

	// 화면에 그리기 시작한다.
	if( g_cApplication.getgraphics()->BeginScene() == TRUE )
	{
		// 그림을 그리기 시작한다.
		g_cApplication.getgraphics()->BeginSprite();

		g_cGame.m_ImageManager.Draw("GAME_MAIN");
		DrawFPS();

		g_cApplication.getgraphics()->EndSprite(); // 그림 그리기를 끝낸다.

		g_cApplication.getgraphics()->EndScene(); // 화면에 그리는것을 끝낸다.
	}

	// 실제 모니터에 뿌려지는 화면으로 뿌려준다.
	g_cApplication.getgraphics()->Display();

	return true;
}

void CGame::DrawFPS()
{
#ifdef _DEBUG
	// FPS 를 찍어준다.
	m_Font.Begin();
	char Text[200];
	sprintf(Text, "Render : %.02f FPS", g_cApplication.render_get_fps());
	m_Font.Print( Text, 15, 395, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "평균 : %.02f FPS", g_cApplication.render_arage_get_fps());
	m_Font.Print( Text, 140, 395, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "Process : %.02f FPS", g_cApplication.process_get_fps());
	m_Font.Print( Text, 15, 410, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	sprintf(Text, "평균 : %.02f FPS", g_cApplication.process_arage_get_fps());
	m_Font.Print( Text, 140, 410, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );	
	sprintf(Text, "x = %d y = %d", m_lMouseXPos, m_lMouseYPos);
	m_Font.Print( Text, 15, 425, 0, 0, D3DCOLOR_RGBA(255,255,255,255) );
	m_Font.End();
#endif
}

// 게임 모드를 바꿔준다.
void CGame::SetGameState(GAME_STATE gamestate)
{
	m_nGameState = gamestate;
}

// 일단 남자 여자 목소리 랜덤.
void CGame::SoundPlayVoice(char *soudname, char *soundname2)
{
	// 일단 아무거나 랜덤으로. 나중엔 성별을 가려가면서 하게 고쳐야 할 것 같다.
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
// 하나의 사운드만 플레이 할 수 있게 한다.
void CGame::SoundPlay(char *soudname)
{
	DSEffectManager.OpenSound(soudname);
	DSEffectManager.PlaySound(soudname);
}

// 스크린샷을 찍을 때 이용하는 함수이다.
// 몇월 몇일 몇시에 뭐. 그런 정보를 string 로 얻어온다.
// 그래서 저장할 때 파일 명으로 이용한다. 
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
	strcat(szTime, "-"); 		// 라인 추가 	
	strcat(szTime, szHour);
	strcat(szTime, szMinute);
	strcat(szTime, szSecond);
	//	strcat(szTime, szMSecond);

	return szTime;
}

// 스크린 캡쳐를 하는 함수이다.
void CGame::ScreenCapture(bool b_check)
{
	HDC hdc;

	SoundPlay("BUTTON_CLICK"); // 버튼 클릭 할 때 나는 소리.

	// 파일이름을 넣는다.
	char szSaveName[256];
	strcpy(szSaveName, "ScreenCapture\\");
	strcat(szSaveName, g_cGame.GetTime());
	strcat(szSaveName, ".jpg");

	// 스크린캡쳐
	hdc = ::GetDC(g_cApplication.gethWnd()); 
	ScrCaptureToJPG(hdc,szSaveName, 100); 
	::ReleaseDC(g_cApplication.gethWnd(),hdc); 
}
