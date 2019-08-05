
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
	// ����� ����϶��� ���ҽ����� cik �� ������ �ٲ��ش�. �ε帮��Ʈ�� ���������� ����
	// �׷��� ������ ����϶��� ����׸��� �ѹ� �������ְ� ���� ������� �ϸ� ����ϴ�.
	char LoadFileName[200];
	char SaveFileName[200];
	memset(LoadFileName, NULL, 200);
	memset(SaveFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(SaveFileName, FileName);
	strcpy(LoadFileName, FileName);
	// Ȯ���ڸ� cik �� �ٲ۴�
	SaveFileName[n_strlen-1] = 'k';
	SaveFileName[n_strlen-2] = 'i';
	SaveFileName[n_strlen-3] = 'c';
	// cik ���Ϸ� ��ȣȭ�Ѵ�.
	FileSave(LoadFileName, SaveFileName);
	// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
	FileLoad(SaveFileName);
#else
	// ������ ����϶��� gif �� ���� Ȯ���ڸ� cik �� �ٲ۴�. �׸��� �׳� �ε��ϸ� �ȴ�.
	char LoadFileName[200];
	memset(LoadFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(LoadFileName, FileName);
	// Ȯ���ڸ� cik �� �ٲ۴�
	LoadFileName[n_strlen-1] = 'k';
	LoadFileName[n_strlen-2] = 'i';
	LoadFileName[n_strlen-3] = 'c';
	// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
	FileLoad(LoadFileName);
#endif

	return generateTempFile();
}


char* BlowFishData::TextDataSaveLoad(char *FileName)
{
#ifdef _DEBUG
	// ����� ����϶��� ���ҽ����� cik �� ������ �ٲ��ش�. �ε帮��Ʈ�� ���������� ����
	// �׷��� ������ ����϶��� ����׸��� �ѹ� �������ְ� ���� ������� �ϸ� ����ϴ�.
	char LoadFileName[200];
	char SaveFileName[200];
	memset(LoadFileName, NULL, 200);
	memset(SaveFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(SaveFileName, FileName);
	strcpy(LoadFileName, FileName);
	// Ȯ���ڸ� cik �� �ٲ۴�
	SaveFileName[n_strlen-1] = 't';
	SaveFileName[n_strlen-2] = 'a';
	SaveFileName[n_strlen-3] = 'd';
	// cik ���Ϸ� ��ȣȭ�Ѵ�.
	FileSave(LoadFileName, SaveFileName);
	// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
	FileLoad(SaveFileName);
#else
	// ������ ����϶��� gif �� ���� Ȯ���ڸ� cik �� �ٲ۴�. �׸��� �׳� �ε��ϸ� �ȴ�.
	char LoadFileName[200];
	memset(LoadFileName, NULL, 200);
	int n_strlen = strlen(FileName);
	strcpy(LoadFileName, FileName);
	// Ȯ���ڸ� cik �� �ٲ۴�
	LoadFileName[n_strlen-1] = 't';
	LoadFileName[n_strlen-2] = 'a';
	LoadFileName[n_strlen-3] = 'd';
	// �̰��� �θ��� temp ������ ������ ����Ƿ� generateTempFile �� �ؼ� �о�帮�� �Ǵ°��̴�
	FileLoad(LoadFileName);
#endif

	return generateTempFile();
}

// �ӽ� ������ �ӽ� ���Ͽ��ٰ� �����ϱ� ���� �� ������ ���ϸ��� �����Ѵ�.
char *BlowFishData::generateTempFile()
{
	char *tempFile = new char[MAX_PATH];

	GetEnvironmentVariable("TEMP", tempFile, MAX_PATH);

	if (tempFile[strlen(tempFile)-1] != '\\') strcat(tempFile, "\\");
	strcat(tempFile, "fileenc.tmp");

	return tempFile;
}