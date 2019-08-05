

#ifndef __GAME_H__
#define __GAME_H__

#include "define.h"

#include "dlg\DlgMain.h"
#include "Utility\WebBrowser.h"

#include "..\Engine_Utility\AniGIF.h"
#include "..\Engine_Utility\Encoder.h"
#include "..\Engine_Utility\ImageManager.h"
#include "..\Engine_Utility\BlowFishData.h"
#include "..\Game Main\Game Main\Game Main.h"
#include "..\Engine_Library\D3DInput.h"

#include "common\Player.h"

class _init_CGame;

enum GAME_STATE
{
	GAME_LOGIN,
	GAME_CHANNEL,
	GAME_LOBBY,
	GAME_BOARD,
	GAME_EXIT
};

// ���ӿ� ���� �������� �ɼǵ鿡 ���� ����ü
struct GameOption
{
	bool bgm_mode;			// true : �Ҹ��� , false : �Ҹ��ȳ�
	int	bgm_vol;			// ����
	bool effect_mode;		// ����Ʈ �Ҹ��� �ȳ�
	int effect_vol;			// ����Ʈ ����1
	bool sound_card;		// ���� ī�尡 �ֳ� ����.
	bool invite_mode;		// �ʴ�
	bool memo_mode;			// �޸�
	bool talk_mode;			// 1:1��ȭ
};

class __declspec(dllexport) CGame
{
friend _init_CGame;
public:
	CGame();
	~CGame();

	void initialize();
	void release();

	void InitMsg();
	void startup();

	bool Process();
	bool Render();

	static void   GameProc( void *Ptr, long Purpose );

	LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	int cardindex;

public:
	////////// input ���� /////////
	LONG			m_lMouseXPos;
	LONG			m_lMouseYPos;
	BOOL			m_bLButtonDown;
	BOOL			m_bRButtonDown;

	cInput			m_Input;
	cInputDevice	m_Keyboard;
	cInputDevice	m_Mouse;
	///////////////////////////////
	///////// Sound ���� //////////
	CDSBGMManager		DSBGMManager;		
	CDSEffectManager	DSEffectManager;	
	///////////////////////////////
	cEncoder		Encoder;

	// ��������
	CWebBrowser		m_WebBrowser;

	// �̹��� �Ŵ���
	CImageManager   m_ImageManager;

	// ���� �ɼ�
	GameOption m_GameOption;

	// ��ȣȭ Ŭ����
	BlowFishData BlowFish;

	// ����� ����
	CPlayer		m_MyInfo;
//	CPlayer		m_PlayerInfo[MAX_PLAYER];
public:
	// ���� ������ �ڵ��� �����صд�.
	HWND			m_MainWindowHwnd;
	// Ŀ��
	HCURSOR			m_hCursor;

	void			DrawFPS();

	// ���� ������Ʈ�� ��ų� ��Ƽ�ȴ�.
	void            SetGameState(GAME_STATE gamestate);
	GAME_STATE      GetGameState() { return m_nGameState; };

	void			CommonInit();

	// ���� �ð��� char ������ �����Ѵ�.
	// ��ũ��ĸ�� �������� ���δ�.
	char*			GetTime();
	void			ScreenCapture(bool b_check = true); // ĸ�ĸ� �Ѵ�.

	// ���������� ������ �Ǵ� ��ư��
	BOOL			CommonRenderButton();

	// ���带 ����Ѵ�.
	void			SoundPlay(char *soudname);
	void			SoundPlayVoice(char *soudname, char *soundname2);
protected:
	// �ڱ� �ڽ��� �̱����� ���� �ʹٸ� ���⼭ �����Ѵ�.
	static CGame*			__singleton();
	
	// ���� ���� ������Ʈ�� ������� �ľ��Ѵ�.
	GAME_STATE				m_nGameState;

	// �ʱ� ������ �ε� �Լ�
	void					DataLoad();

	void					SaveSetupInfo();
	void					LoadSetupInfo();
protected:
	cFont			m_Font;					// �Ϲ� ��Ʈ
};

////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CGame[sizeof(CGame)];
extern __declspec(dllimport) _CGame		cGameBlock;
#define g_cGame			(*(CGame*) cGameBlock)

// �̱����� �����ؾ���.
#ifdef GAME_EXPORTS

// initializer dummy
class _init_CGame {
protected:
	static unsigned long			ms_dwLong;

public:
	_init_CGame()
	{
		if (ms_dwLong == 0)
		{
			new (cGameBlock) CGame;
		}

		ms_dwLong++;
	}
	~_init_CGame()
	{
		if (ms_dwLong == 1)
		{
			((CGame*) cGameBlock)->~CGame();
		}

		ms_dwLong--;
	}
};

// declare CGame singleton instance
static _init_CGame			_init_cGame;

#endif


#endif