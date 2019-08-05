
#ifndef __DLG_MAIN_H__
#define __DLG_MAIN_H__

#include"..\Game.h"

#include "..\define.h"

class _init_CDlgMain;

class CDlgMain
{
	friend _init_CDlgMain;

	public:
		CDlgMain(){}
		~CDlgMain();

		void Create();
		void Render();

		static void CALLBACK UIDialogProc( UINT nDialogID, int nControlID, UINT nEvent, cUIControl* pControl );
		LRESULT CALLBACK	 MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		// 방만들기
		void MakeRoom();

		UINT				m_nPopupMenu;			// 팝업 메뉴
	public:

	protected:
};


////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CDlgMain[sizeof(CDlgMain)];
extern __declspec(dllimport) _CDlgMain		cDlgMainBlock;
#define g_cDlgMain			(*(CDlgMain*) cDlgMainBlock)

// 싱글톤을 생성해야함.
#ifdef GAME_EXPORTS

// initializer dummy
class _init_CDlgMain {
protected:
	static unsigned long			ms_dwLong;

public:
	_init_CDlgMain()
	{
		if (ms_dwLong == 0)
		{
			new (cDlgMainBlock) CDlgMain;
		}

		ms_dwLong++;
	}
	~_init_CDlgMain()
	{
		if (ms_dwLong == 1)
		{
			((CDlgMain*) cDlgMainBlock)->~CDlgMain();
		}

		ms_dwLong--;
	}
};

// declare CGame singleton instance
static _init_CDlgMain			_init_cDlgMain;

#endif


#endif


