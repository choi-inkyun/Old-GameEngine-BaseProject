
#include "stdafx.h"
#include "Game.h"

/*
	Game.h, Game.cpp �� ��Ȱ�� ����
	���� cpp �Ѱ����ٰ� ���� ������ �� cpp �� ��Ȱ�� �����ϱ⵵ ����� ������ ���� ������
	�ʱ�ȭ�ϴ� �κ��� ����ٰ� ó���Ͽ���.
 */

CGame::CGame()
{
	// �����츦 �������ش�.
	int ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

	// ������ ����
	g_cMainEngine.SetWindowParameter(
		0, 
		0, 
		WINDOW_WIDTH, // GAME MAIN �� DEFINE ���� ����Ǿ��ִ�
		WINDOW_HEIGHT,// GAME MAIN �� DEFINE ���� ����Ǿ��ִ�
		WS_POPUP | WS_CLIPCHILDREN
		);
}

///// �Ҹ��� ///////
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

//////////////////////// ���� �Ҹ��� ////////////////////////
void CGame::release()
{
	if(m_WebBrowser.m_bNowWebBrowser == true)
		m_WebBrowser.ExplorerDestory();

//	g_cApplication.g_Movie->ReleaseAll();
}

//////////////////////////////////////////////////////////////
////////////////////////// ���� �ʱ�ȭ /////////////////////////////
// �ִ�ȭ �ּ�ȭ �Ҷ� �̰��� ��� �ҷ��ش�. � scene ��������
void CGame::initialize()
{
	static bool	b_one = false;
	// �� �ѹ��� �ε��� �Ǿ�� �Ѵ�.
	if(b_one == false)
	{
		// �����͸� �ε���.
		DataLoad();
		b_one = true;
	}

	// ���������� �ε��ϴ� �͵�.
	CommonInit();

	// ��Ʈ�� �����Ѵ�.
	m_Font.Create(  g_cApplication.getgraphics(), "����ü", 12, FALSE );

	// input ��ü�� �����Ѵ�. input, mouse, keyboard ��ü ����
#ifdef _DEBUG
	ASSERT( m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() ) );
#else
	m_Input.initialize( g_cApplication.gethWnd(), g_cApplication.gethInstance() );
#endif
	m_Keyboard.Create( &m_Input, KEYBOARD, g_cApplication.Get_Windowed() );
	m_Mouse.Create( &m_Input, MOUSE,  g_cApplication.Get_Windowed());

//	g_cApplication.g_Movie->InitAll(g_cApplication.gethInstance(), g_cApplication.gethWnd()); // ������ ����� ���� �ʱ�ȭ.
//	g_cApplication.g_Movie->PlayMovieInWindow("skiing.avi");
}

void CGame::InitMsg()
{
	// ��ü�� �����϶�� �˷��ش�.
	g_cStateManager.Push( CGame::GameProc, this );
}

void CGame::startup()
{
	// Directx ȭ�� ����
	g_cMainEngine.SetMode();

	// ������ ȭ�� ���� ����.
	if(m_WebBrowser.m_bNowWebBrowser == true)
		SetWindowPos(g_cApplication.gethWnd() , NULL, 224, 168, WINDOW_WIDTH,WINDOW_HEIGHT,NULL);
	else
		SetWindowPos(g_cApplication.gethWnd() , NULL, 0, 0, WINDOW_WIDTH,WINDOW_HEIGHT,NULL);
	// �ʱ�ȭ ������
//	g_cMainEngine.m_RenderState = RENDERSTATE_LOBBY; // �κ�� ����.

	//	initialize();
	// �������� �� �������� �������
//	if(m_WebBrowser.m_bNowWebBrowser == true)
//		m_WebBrowser.ExplorerCreate();

	// ���̾�α׸� �����մϴ�.
	g_cDlgMain.Create();
}

void CGame::DataLoad()
{
//	g_cApplication.g_Movie = new CDirectMovie; // ������ ����� ���� ��ü�� ������
	////////////////// ������ �ε� //////////////////
	m_ImageManager.ImageLoad(BlowFish.TextDataSaveLoad("Data\\Image\\Image_Loadlist.txt"));
	/////////////////////////////////////////////////////////////////////
	////////////////////////// ���� ó�� //////////////////////////////
	/////////////////  EFFECT INITIALIZE
	if (DSEffectManager.Initialize(BlowFish.TextDataSaveLoad("Data\\Sound\\effect_loadlist.txt"))==false)
	{
		// ����ī�尡 ������.
		m_GameOption.sound_card = false;
		DSEffectManager.m_sound_off = true;
		DSBGMManager.m_sound_off = true;
//		g_cApplication.DisplayError( FALSE, "����ī�带 ã�� �� �����ϴ�. /���尡 ����� �� �� �� �ֽ��ϴ�.");
	}
	else 
		// ���� ī�尡 ������. 
		m_GameOption.sound_card = true;

	/////////////////  BGM INITIALIZE
	DSBGMManager.Initialize(BlowFish.TextDataSaveLoad("Data\\Sound\\bgm_loadlist.txt"));

	//////////////// ���� �ɼ� ���� ���� ����
	// ������Ʈ���� ����Ǿ� �ִ� ���� �ɼ��� �ҷ� ���Դϴ�.
	LoadSetupInfo();

	// ���� ������ �����մϴ�.
	if(m_GameOption.sound_card == true)
	{                                                                                                                                                                                                                                                                                                   
		// ����ī�尡 ��������.
		DSBGMManager.m_sound_off = !m_GameOption.bgm_mode;
		DSEffectManager.m_sound_off = !m_GameOption.effect_mode;
	}
	DSBGMManager.SetVolume((float)m_GameOption.bgm_vol/100.0f);
	DSEffectManager.SetVolume((float)m_GameOption.effect_vol/100.0f);
	///////////////////////////////////////////////////////////////////
}

// ���������� ���� �ʱ�ȭ��..
void CGame::CommonInit()
{
	//// ������ Ǯ��ũ��, ������ ��� ��ȯ. mousezone �� ���� ������ �̷��� ������ ������ �������.
	//if(g_cApplication.Get_Windowed()==true)
	//{
	//	// ������ ����� Ǯ��ũ�� ��ư�� ���� �� �־���Ѵ�.
	//	g_cGame.m_ImageManager.Get_MouseZone()->ActiveChange(false, g_cGame.m_ImageManager.Get_ImageList("WINDOW_WINDOWMODE_BUTTON")->flag1);
	//}
	//else
	//{
	//	// Ǯ��ũ�� ����� �������� ��ư�� ���� �� �־���Ѵ�.
	//	g_cGame.m_ImageManager.Get_MouseZone()->ActiveChange(true, g_cGame.m_ImageManager.Get_ImageList("WINDOW_WINDOWMODE_BUTTON")->flag1);
	//}
}