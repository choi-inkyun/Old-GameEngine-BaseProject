

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

// 게임에 대한 여러가지 옵션들에 대한 구조체
struct GameOption
{
	bool bgm_mode;			// true : 소리남 , false : 소리안남
	int	bgm_vol;			// 볼륨
	bool effect_mode;		// 임펙트 소리남 안남
	int effect_vol;			// 임펙트 볼륨1
	bool sound_card;		// 사운드 카드가 있나 없나.
	bool invite_mode;		// 초대
	bool memo_mode;			// 메모
	bool talk_mode;			// 1:1대화
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
	////////// input 관련 /////////
	LONG			m_lMouseXPos;
	LONG			m_lMouseYPos;
	BOOL			m_bLButtonDown;
	BOOL			m_bRButtonDown;

	cInput			m_Input;
	cInputDevice	m_Keyboard;
	cInputDevice	m_Mouse;
	///////////////////////////////
	///////// Sound 관련 //////////
	CDSBGMManager		DSBGMManager;		
	CDSEffectManager	DSEffectManager;	
	///////////////////////////////
	cEncoder		Encoder;

	// 웹브라우져
	CWebBrowser		m_WebBrowser;

	// 이미지 매니져
	CImageManager   m_ImageManager;

	// 게임 옵션
	GameOption m_GameOption;

	// 암호화 클레스
	BlowFishData BlowFish;

	// 사용자 정보
	CPlayer		m_MyInfo;
//	CPlayer		m_PlayerInfo[MAX_PLAYER];
public:
	// 메인 윈도우 핸들을 저장해둔다.
	HWND			m_MainWindowHwnd;
	// 커서
	HCURSOR			m_hCursor;

	void			DrawFPS();

	// 게임 스테이트를 얻거나 셋티안다.
	void            SetGameState(GAME_STATE gamestate);
	GAME_STATE      GetGameState() { return m_nGameState; };

	void			CommonInit();

	// 현재 시간을 char 형으로 리턴한다.
	// 스크린캡쳐 같은데서 쓰인다.
	char*			GetTime();
	void			ScreenCapture(bool b_check = true); // 캡쳐를 한다.

	// 공통적으로 랜더링 되는 버튼들
	BOOL			CommonRenderButton();

	// 사운드를 재생한다.
	void			SoundPlay(char *soudname);
	void			SoundPlayVoice(char *soudname, char *soundname2);
protected:
	// 자기 자신이 싱글톤을 쓰고 싶다면 여기서 참조한다.
	static CGame*			__singleton();
	
	// 현재 게임 스테이트가 어떤것인지 파악한다.
	GAME_STATE				m_nGameState;

	// 초기 데이터 로드 함수
	void					DataLoad();

	void					SaveSetupInfo();
	void					LoadSetupInfo();
protected:
	cFont			m_Font;					// 일반 폰트
};

////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CGame[sizeof(CGame)];
extern __declspec(dllimport) _CGame		cGameBlock;
#define g_cGame			(*(CGame*) cGameBlock)

// 싱글톤을 생성해야함.
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