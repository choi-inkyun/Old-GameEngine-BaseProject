// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� �����Դϴ�.
#include <windows.h>
// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

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

//#pragma comment (lib, "ScrCap.lib") // ĸ��
#ifdef _DEBUG
	#pragma comment (lib, "Engine_InterfaceD.lib")
	#pragma comment (lib, "Engine_UtilityD.lib")
	#pragma comment (lib, "Engine_LibraryD.lib")
#else
	#pragma comment (lib, "Engine_Interface.lib")
	#pragma comment (lib, "Engine_Utility.lib")
	#pragma comment (lib, "Engine_Library.lib")
#endif

#pragma comment (lib, "Enc_Blowfish.lib") // ��ȣȭ

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d9.h>
#include <d3dx9.h>

// ���� �߿�����
#include "..\Engine Main\Engine Main.h"
#include "..\Engine_Library\Engine_Library.h"
#include "..\Engine_Library\D3DApplication.h"

// ���� ��������
#include "..\Engine_Utility\Tile.h"
#include "..\Engine_Utility\BlowFishData.h"
#include "..\Engine_Utility\ScrCap.h"
#include "..\Engine_Utility\StopWatch.h"
#include "..\Engine_Interface\GrowableArray.h"
#include "..\Engine_Interface\Base_Object.h"

