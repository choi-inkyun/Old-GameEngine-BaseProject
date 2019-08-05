
// 암호화 알고리즘을 사용한다.

#ifndef __BLOWFISHDATA_H__
#define __BLOWFISHDATA_H__

enum CIPHER
{
	ENCRYPT_METHOD = 0,
	DECRYPT_METHOD,
	ENCRYPT_EXE
};

#include "Blowfish.h"

#ifdef _AFXDLL
#define DLLFunction  __declspec(dllexport)
#else
#define DLLFunction  __declspec(dllimport)
#endif


class BlowFishData
{
public:
	DLLFunction int		__stdcall	FileSave(char *ReadFileName, char *SaveFileName);
	DLLFunction	FILE*				FileLoad(char *ReadFileName);

	// 로드 할 수있게 char 형으로 넘겨준다
	DLLFunction	char*				ImageDataSaveLoad(char *FileName);
	// 위와 동일하지만 확장자가 다르다
	DLLFunction	char*				TextDataSaveLoad(char *FileName);

	DLLFunction	char*				generateTempFile();
protected:

private:

	char _readFile[MAX_PATH];
	char _writeFile[MAX_PATH];
	char _password[100];

};

#endif