// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.
#include <windows.h>
// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

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

//#pragma comment (lib, "ScrCap.lib") // 캡쳐
#ifdef _DEBUG
	#pragma comment (lib, "Engine_InterfaceD.lib")
	#pragma comment (lib, "Engine_UtilityD.lib")
	#pragma comment (lib, "Engine_LibraryD.lib")
#else
	#pragma comment (lib, "Engine_Interface.lib")
	#pragma comment (lib, "Engine_Utility.lib")
	#pragma comment (lib, "Engine_Library.lib")
#endif

#pragma comment (lib, "Enc_Blowfish.lib") // 암호화

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d9.h>
#include <d3dx9.h>

// 엔중 중요파일
#include "..\Engine Main\Engine Main.h"
#include "..\Engine_Library\Engine_Library.h"
#include "..\Engine_Library\D3DApplication.h"

// 엔진 세부파일
#include "..\Engine_Utility\Tile.h"
#include "..\Engine_Utility\BlowFishData.h"
#include "..\Engine_Utility\ScrCap.h"
#include "..\Engine_Utility\StopWatch.h"
#include "..\Engine_Interface\GrowableArray.h"
#include "..\Engine_Interface\Base_Object.h"

