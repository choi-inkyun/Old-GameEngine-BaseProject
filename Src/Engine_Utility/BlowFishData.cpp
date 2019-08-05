
#include "StdAfx.h"
#include "BlowFishData.h"

int BlowFishData::FileSave(char *ReadFileName, char *SaveFileName)
{
	strcpy(_readFile, ReadFileName);
	strcpy(_writeFile,SaveFileName);
	strcpy(_password, "game make choi-in-kyun haha");

	CIPHER _cipher;

	//_cipher = DECRYPT_METHOD;
	_cipher	= ENCRYPT_EXE;

	FILE *readFile = fopen(_readFile, "rb");
	if (readFile == 0) return g_cApplication.DisplayError(false,"Unable to open source file (fopen).");

	const size_t bufferSize = 1024;
	int barSize = 0;

	// reaching the end of the file and getting position = getting file size (bytes).
	fseek(readFile, 0, SEEK_END);
	barSize = ftell(readFile);
	fseek(readFile, 0, SEEK_SET);
	if (barSize == -1) return g_cApplication.DisplayError(false,"Unable to get file size (_filelength).");
	barSize = barSize / bufferSize;

	char outfile[MAX_PATH];
	if (_cipher == DECRYPT_METHOD)
	{
		char *temp = generateTempFile();
		strcpy(outfile, temp);
		delete temp;
	}
	else
	{
		strcpy(outfile, _writeFile);
	}

	FILE *writeFile = fopen(outfile, "wb");
	if (writeFile == 0) return g_cApplication.DisplayError(false,"Unable to open destination file.");

	char readBuffer[bufferSize];
	char outBuffer[bufferSize];
	size_t readRet = 0;

	BlowFishEnc encryption(_password);

	bool abort = false;
	int encRet;
	while (!feof(readFile))
	{
		readRet = fread(readBuffer, sizeof(char), bufferSize, readFile);

		if ((_cipher == DECRYPT_METHOD) && (!abort))
		{
			encRet = encryption.decryptStream(readBuffer, (DWORD)readRet, outBuffer);
			if (feof(readFile) )
			{
				int pos = 0;
				// removing trailing zeros - encrypted file must be x8 bytes.
				while ((pos < 8) && ((outBuffer + encRet - pos)[0] == 0)) pos++;
				// if found trailing zeros - decreasing the writing buffer marker (not writing them).
				if (pos) encRet -= (pos - 1);
			}
		}
		else if (!abort)
		{
			encRet = encryption.encryptStream(readBuffer, (DWORD)readRet, outBuffer);
		}

		fwrite(outBuffer, sizeof(char), encRet, writeFile);

		//		stepProgressbar();

		// check if user aborted!
		//		if ((shouldTerminate()) || (abort))
		//		{
		//			fclose(readFile); fclose(writeFile);
		//			GenLib::FileWipe::wipeFile(outfile);
		//			return 0;
		//		}
	}

	fflush(writeFile);

	fclose(writeFile);
	fclose(readFile);

	ZeroMemory(outBuffer, bufferSize);
	ZeroMemory(readBuffer, bufferSize);

	// if encrypted should be created as self extracted.
	//	if (_cipher == ENCRYPT_EXE)
	//	{
	//		GenLib::SelfExtract attacher;
	//		attacher.detachFile(_writeFile);

	//		GenLib::SelfExtract builder(_writeFile);
	//		builder.attachFile(outfile, _readFile);
	//	}

	//	SetDlgItemText(ID_CLOSE_BUTTON, "&Close");

	return 0;
}

FILE* BlowFishData::FileLoad(char *ReadFileName)
{
	strcpy(_readFile, ReadFileName);
	strcpy(_password, "game make choi-in-kyun haha");

	CIPHER _cipher;

	_cipher = DECRYPT_METHOD;
	//_cipher	= ENCRYPT_EXE;

	FILE *readFile = fopen(_readFile, "rb");
	if (readFile == 0) 
	{
		g_cApplication.DisplayError(false,"Unable to open source file (fopen).");
		return NULL;
	}
	const size_t bufferSize = 1024;
	int barSize = 0;

	// reaching the end of the file and getting position = getting file size (bytes).
	fseek(readFile, 0, SEEK_END);
	barSize = ftell(readFile);
	fseek(readFile, 0, SEEK_SET);
	if (barSize == -1)
	{
		g_cApplication.DisplayError(false,"Unable to get file size (_filelength).");
		return NULL;
	}
	barSize = barSize / bufferSize;

	char outfile[MAX_PATH];
	if (_cipher == DECRYPT_METHOD)
	{
		char *temp = generateTempFile();
		strcpy(outfile, temp);
		delete temp;
	}
	else
	{
		strcpy(outfile, _writeFile);
	}

	FILE *writeFile = fopen(outfile, "wb");
	if (writeFile == 0)
	{
		g_cApplication.DisplayError(false,"Unable to open destination file.");
		return NULL;
	}
	char readBuffer[bufferSize];
	char outBuffer[bufferSize];
	size_t readRet = 0;

	BlowFishEnc encryption(_password);

	bool abort = false;
	int encRet;
	while (!feof(readFile))
	{
		readRet = fread(readBuffer, sizeof(char), bufferSize, readFile);

		if ((_cipher == DECRYPT_METHOD) && (!abort))
		{
			encRet = encryption.decryptStream(readBuffer, (DWORD)readRet, outBuffer);
			if (feof(readFile))
			{
				int pos = 0;
				// removing trailing zeros - encrypted file must be x8 bytes.
				while ((pos < 8) && ((outBuffer + encRet - pos)[0] == 0)) pos++;
				// if found trailing zeros - decreasing the writing buffer marker (not writing them).
				if (pos) encRet -= (pos - 1);
			}
		}
		else if (!abort)
		{
			encRet = encryption.encryptStream(readBuffer, (DWORD)readRet, outBuffer);
		}

		fwrite(outBuffer, sizeof(char), encRet, writeFile);

		//		stepProgressbar();

		// check if user aborted!
		//		if ((shouldTerminate()) || (abort))
		//		{
		//			fclose(readFile); fclose(writeFile);
		//			GenLib::FileWipe::wipeFile(outfile);
		//			return 0;
		//		}
	}

	fflush(writeFile);

	fclose(writeFile);
	fclose(readFile);

	ZeroMemory(outBuffer, bufferSize);
	ZeroMemory(readBuffer, bufferSize);

	// if encrypted should be created as self extracted.
	//	if (_cipher == ENCRYPT_EXE)
	//	{
	//		GenLib::SelfExtract attacher;
	//		attacher.detachFile(_writeFile);

	//		GenLib::SelfExtract builder(_writeFile);
	//		builder.attachFile(outfile, _readFile);
	//	}

	//	SetDlgItemText(ID_CLOSE_BUTTON, "&Close");

	return writeFile;
}


char* BlowFishData::ImageDataSaveLoad(char *FileName)
{
#ifdef _DEBUG
	// 디버그 모드일때는 리소스들을 cik 로 무저건 바꿔준다. 로드리스트와 마찬가지의 개념
	// 그러면 릴리즈 모드일때는 디버그모드로 한번 실행해주고 나서 릴리즈로 하면 깔끔하다.
	char LoadFileName[200];
	char SaveFileName[200];
	memset(LoadFileName, NULL, 200);
	memset(SaveFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(SaveFileName, FileName);
	strcpy(LoadFileName, FileName);
	// 확장자를 cik 로 바꾼다
	SaveFileName[n_strlen-1] = 'k';
	SaveFileName[n_strlen-2] = 'i';
	SaveFileName[n_strlen-3] = 'c';
	// cik 파일로 암호화한다.
	FileSave(LoadFileName, SaveFileName);
	// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
	FileLoad(SaveFileName);
#else
	// 릴리즈 모드일때는 gif 로 들어온 확장자를 cik 로 바꾼다. 그리고 그냥 로드하면 된다.
	char LoadFileName[200];
	memset(LoadFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(LoadFileName, FileName);
	// 확장자를 cik 로 바꾼다
	LoadFileName[n_strlen-1] = 'k';
	LoadFileName[n_strlen-2] = 'i';
	LoadFileName[n_strlen-3] = 'c';
	// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
	FileLoad(LoadFileName);
#endif

	return generateTempFile();
}


char* BlowFishData::TextDataSaveLoad(char *FileName)
{
#ifdef _DEBUG
	// 디버그 모드일때는 리소스들을 cik 로 무저건 바꿔준다. 로드리스트와 마찬가지의 개념
	// 그러면 릴리즈 모드일때는 디버그모드로 한번 실행해주고 나서 릴리즈로 하면 깔끔하다.
	char LoadFileName[200];
	char SaveFileName[200];
	memset(LoadFileName, NULL, 200);
	memset(SaveFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(SaveFileName, FileName);
	strcpy(LoadFileName, FileName);
	// 확장자를 cik 로 바꾼다
	SaveFileName[n_strlen-1] = 't';
	SaveFileName[n_strlen-2] = 'a';
	SaveFileName[n_strlen-3] = 'd';
	// cik 파일로 암호화한다.
	FileSave(LoadFileName, SaveFileName);
	// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
	FileLoad(SaveFileName);
#else
	// 릴리즈 모드일때는 gif 로 들어온 확장자를 cik 로 바꾼다. 그리고 그냥 로드하면 된다.
	char LoadFileName[200];
	memset(LoadFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(LoadFileName, FileName);
	// 확장자를 cik 로 바꾼다
	LoadFileName[n_strlen-1] = 't';
	LoadFileName[n_strlen-2] = 'a';
	LoadFileName[n_strlen-3] = 'd';
	// 이것을 부르면 temp 폴더에 파일이 생기므로 generateTempFile 로 해서 읽어드리면 되는것이다
	FileLoad(LoadFileName);
#endif

	return generateTempFile();
}

// 임시 폴더에 임시 파일에다가 저장하기 위해 그 폴더와 파일명을 리턴한다.
char *BlowFishData::generateTempFile()
{
	char *tempFile = new char[MAX_PATH];

	GetEnvironmentVariable("TEMP", tempFile, MAX_PATH);

	if (tempFile[strlen(tempFile)-1] != '\\') strcat(tempFile, "\\");
	strcat(tempFile, "fileenc.tmp");

	return tempFile;
}