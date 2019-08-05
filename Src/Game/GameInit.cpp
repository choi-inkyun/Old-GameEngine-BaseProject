
#include "stdafx.h"
#include "Game.h"

/*
	Game.h, Game.cpp 와 역활은 같다
	단지 cpp 한곳에다가 몰아 넣으면 각 cpp 의 역활을 구분하기도 힘들고 복잡해 지기 때문에
	초기화하는 부분은 여기다가 처리하였다.
 */

CGame::CGame()
{
	// 윈도우를 설정해준다.
	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	// 윈도우 설정
	g_cMainEngine.SetWindowParameter(
		0, 
		0, 
		WINDOW_WIDTH, // GAME MAIN 에 DEFINE 으로 선언되어있다
		WINDOW_HEIGHT,// GAME MAIN 에 DEFINE 으로 선언되어있다
		WS_POPUP | WS_CLIPCHILDREN
		);
}

///// 소멸자 ///////
CGame::~CGame()
{
	m_Font.Free();
	g_cApplication.Cleanup3DEnvironment();
}

//////////////////////////////////////////////////////////////////////////////////
CGame* CGame::__singleton()
// : script callback
//////////////////////////////////////////////////////////////////////////////////
{
	return &g_cGame;
}

//////////////////////// 게임 소멸자 ////////////////////////
void CGame::release()
{
	if(m_WebBrowser.m_bNowWebBrowser == true)
		m_WebBrowser.ExplorerDestory();

//	g_cApplication.g_Movie->ReleaseAll();
}

//////////////////////////////////////////////////////////////
////////////////////////// 게임 초기화 /////////////////////////////
// 최대화 최소화 할때 이곳을 계속 불러준다. 어떤 scene 에서든지
void CGame::initialize()
{
	static bool	b_one = false;
	// 단 한번만 로딩이 되어야 한다.
	if(b_one == false)
	{
		// 데이터를 로드함.
		DataLoad();
		b_one = true;
	}

	// 공통적으로 로딩하는 것들.
	CommonInit();

	// 폰트를 생성한다.
	m_Font.Create(  g_cApplication.getgraphics(), "바탕체", 12, FALSE );

	// input 객체를 생성한다. input, mouse, keyboard 객체 생성
#ifdef _DEBUG
	ASSERT( m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() ) );
#else
	m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() );
#endif
	m_Keyboard.Create( &m_Input, KEYBOARD, g_cApplication.Get_Windowed() );
	m_Mouse.Create( &m_Input, MOUSE,  g_cApplication.Get_Windowed());

//	g_cApplication.g_Movie->InitAll(g_cApplication.gethInstance(), g_cApplication.gethWnd()); // 동영상 재생을 위한 초기화.
//	g_cApplication.g_Movie->PlayMovieInWindow("skiing.avi");
}

void CGame::InitMsg()
{
	// 객체를 생성하라고 알려준다.
	g_cStateManager.Push( CGame::GameProc, this );
}

void CGame::startup()
{
	// Directx 화면 적용
	g_cMainEngine.SetMode();

	// 윈도우 화면 강제 설정.
	if(m_WebBrowser.m_bNowWebBrowser == true)
		SetWindowPos(g_cApplication.gethWnd() , NULL, 224, 168, WINDOW_WIDTH,WINDOW_HEIGHT,NULL);
	else
		SetWindowPos(g_cApplication.gethWnd() , NULL, 0, 0, WINDOW_WIDTH,WINDOW_HEIGHT,NULL);
	// 초기화 시텨줌
//	g_cMainEngine.m_RenderState = RENDERSTATE_LOBBY; // 로비로 간다.

	//	initialize();
	// 마지막에 웹 브라우져를 띄워주자
//	if(m_WebBrowser.m_bNowWebBrowser == true)
//		m_WebBrowser.ExplorerCreate();

	// 다이얼로그를 생성합니다.
	g_cDlgMain.Create();
}

void CGame::DataLoad()
{
//	g_cApplication.g_Movie = new CDirectMovie; // 동영상 재생을 위한 객체를 생성함
	////////////////// 데이터 로드 //////////////////
	m_ImageManager.ImageLoad(BlowFish.TextDataSaveLoad("Data\\Image\\Image_Loadlist.txt"));
	/////////////////////////////////////////////////////////////////////
	////////////////////////// 사운드 처리 //////////////////////////////
	/////////////////  EFFECT INITIALIZE
	if (DSEffectManager.Initialize(BlowFish.TextDataSaveLoad("Data\\Sound\\effect_loadlist.txt"))==false)
	{
		// 사운드카드가 없을때.
		m_GameOption.sound_card = false;
		DSEffectManager.m_sound_off = true;
		DSBGMManager.m_sound_off = true;
//		g_cApplication.DisplayError( FALSE, "사운드카드를 찾을 수 없습니다. /사운드가 재생이 안 될 수 있습니다.");
	}
	else 
		// 사운드 카드가 있을때. 
		m_GameOption.sound_card = true;

	/////////////////  BGM INITIALIZE
	DSBGMManager.Initialize(BlowFish.TextDataSaveLoad("Data\\Sound\\bgm_loadlist.txt"));

	//////////////// 각종 옵션 사운드 정보 셋팅
	// 레지스트리에 저장되어 있는 각종 옵션을 불러 들입니다.
	LoadSetupInfo();

	// 사운드 정보를 셋팅합니다.
	if(m_GameOption.sound_card == true)
	{                                                                                                                                                                                                                                                                                                   
		// 사운드카드가 있을때만.
		DSBGMManager.m_sound_off = !m_GameOption.bgm_mode;
		DSEffectManager.m_sound_off = !m_GameOption.effect_mode;
	}
	DSBGMManager.SetVolume((float)m_GameOption.bgm_vol/100.0f);
	DSEffectManager.SetVolume((float)m_GameOption.effect_vol/100.0f);
	///////////////////////////////////////////////////////////////////
}

// 공통적으로 쓰는 초기화들..
void CGame::CommonInit()
{
	//// 윈도우 풀스크린, 윈도우 모두 변환. mousezone 이 같기 때문에 이렇게 셋팅을 일일이 해줘야함.
	//if(g_cApplication.Get_Windowed()==true)
	//{
	//	// 윈도우 모드라면 풀스크린 버튼이 눌릴 수 있어야한다.
	//	g_cGame.m_ImageManager.Get_MouseZone()->ActiveChange(false, g_cGame.m_ImageManager.Get_ImageList("WINDOW_WINDOWMODE_BUTTON")->flag1);
	//}
	//else
	//{
	//	// 풀스크린 모드라면 윈도우모드 버튼이 눌릴 수 있어야한다.
	//	g_cGame.m_ImageManager.Get_MouseZone()->ActiveChange(true, g_cGame.m_ImageManager.Get_ImageList("WINDOW_WINDOWMODE_BUTTON")->flag1);
	//}
}