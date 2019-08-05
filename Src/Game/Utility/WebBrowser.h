
#ifndef __WEBBROWSER_H__
#define __WEBBROWSER_H__

// explorer �� ����ϱ� ���� ������ϵ�
#include <msHtml.h>
#include <ExDisp.h>
#include <ExDispID.h>
#include <shlwapi.h>
#import <shdocvw.dll> 

#define EXPLORER_WINDOW_WIDTH 1024
#define EXPLORER_WINDOW_HEIGHT 768

class CWebBrowser
{
	///////// �� ������ ���� /////////
private:
	IWebBrowser2* m_pWebBrowser;
	IWebBrowser2* m_pWebBrowser2;

public :
	CWebBrowser();
	~CWebBrowser() { }

	BOOL			ExplorerCreate();
	void			ExplorerDestory(bool b_check = false);
	void			ExplorerToggle(int i_mode);

	HWND m_hWnd_Explorer;
	HWND m_hWnd_Explorer2;
	bool m_bNowWebBrowser;
	/////////////////////////////////////};
};

#endif