
#include "StdAfx.h"
#include "Encoder.h"

// 키 값으로 데이터를 변조하여 알 아 볼 수 없게 한다.
void cEncoder::EncodeFileToFile(char *inputfilename, char *outputfilename)
{
	char key[] = "인균이메롱인균이천재인균이바보알라방숑냠.씨";  //  43 byte key
	int c;

	FILE *in;
	FILE *out;
	char tempchar;

	in = fopen(inputfilename,"rb");
	out = fopen(outputfilename,"wb");

	if (!out)
	{
		char filename[100];
		sprintf(filename, "해당 파일을 열 수 없습니다. %s", inputfilename);
		g_cApplication.DisplayError( TRUE, filename);
	}

	if(out==NULL)
	{
		return;
	}

	c=0;

	while(!feof(in))
	{
		tempchar = fgetc(in);
		tempchar^= key[c%43];
		fputc(tempchar,out);
		//fputc(tempchar,out);
		c++;
	}
	fclose(out);
	fclose(in);

	return;
}


void cEncoder::Encoder_Save(char *inputfilename, char *outputfilename)
{
	EncodeFileToFile(inputfilename, outputfilename);
}

char* cEncoder::Encoder_Load(char *inputfilename, char *outputfilename)
{
	char currentFolder[512];
	char tempFolder[512];
	GetCurrentDirectory(512,currentFolder);
	GetTempPath(512,tempFolder);

	if(outputfilename == NULL)
	{
		strcat(tempFolder, "\\sdefs_sda_adfw.txt");
	}
	else
	{
		strcat(tempFolder, "\\");
		strcat(tempFolder, outputfilename);
	}

	EncodeFileToFile(inputfilename, tempFolder);

	return tempFolder;
}