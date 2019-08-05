
#include "StdAfx.h"
#include "Encoder.h"

// Ű ������ �����͸� �����Ͽ� �� �� �� �� ���� �Ѵ�.
void cEncoder::EncodeFileToFile(char *inputfilename, char *outputfilename)
{
	char key[] = "�α��̸޷��α���õ���α��̹ٺ��˶�����.��";  //  43 byte key
	int c;

	FILE *in;
	FILE *out;
	char tempchar;

	in = fopen(inputfilename,"rb");
	out = fopen(outputfilename,"wb");

	if (!out)
	{
		char filename[100];
		sprintf(filename, "�ش� ������ �� �� �����ϴ�. %s", inputfilename);
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