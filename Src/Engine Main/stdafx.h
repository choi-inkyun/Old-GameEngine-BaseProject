// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.


typedef enum RENDER_STATE
{
	RENDERSTATE_INVALID = 0,				// 유효하지 않은 상태
	RENDERSTATE_EXIT,						// 종료
	RENDERSTATE_LOGIN,						// 로그인
	RENDERSTATE_CHANNEL,					// 채널
	RENDERSTATE_LOBBY,						// 로비
	RENDERSTATE_BOARD						// 게임
};

// Windows 헤더 파일입니다.
#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include <new.h>
// Windows 헤더 파일입니다.
#include <winsock2.h>					// windows.h 보다 먼저 포함해야 한다.
#include <windows.h>
#include <mmsystem.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d9.h>
#include <d3dx9.h>

#include "..\Engine_Library\Engine_Library.h"
#include "..\Engine_Library\D3DApplication.h"
#include "..\Engine_Utility\Manager.h"
#include "..\Engine_Interface\GrowableArray.h"
#include "..\Engine_Interface\UIDialog.h"
#include "..\Engine_Interface\UIDialogManager.h"
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
