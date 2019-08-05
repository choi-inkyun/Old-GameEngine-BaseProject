// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.


typedef enum RENDER_STATE
{
	RENDERSTATE_INVALID = 0,				// ��ȿ���� ���� ����
	RENDERSTATE_EXIT,						// ����
	RENDERSTATE_LOGIN,						// �α���
	RENDERSTATE_CHANNEL,					// ä��
	RENDERSTATE_LOBBY,						// �κ�
	RENDERSTATE_BOARD						// ����
};

// Windows ��� �����Դϴ�.
#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include <new.h>
// Windows ��� �����Դϴ�.
#include <winsock2.h>					// windows.h ���� ���� �����ؾ� �Ѵ�.
#include <windows.h>
#include <mmsystem.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d9.h>
#include <d3dx9.h>

#include "..\Engine_Library\Engine_Library.h"
#include "..\Engine_Library\D3DApplication.h"
#include "..\Engine_Utility\Manager.h"
#include "..\Engine_Interface\GrowableArray.h"
#include "..\Engine_Interface\UIDialog.h"
#include "..\Engine_Interface\UIDialogManager.h"
// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
