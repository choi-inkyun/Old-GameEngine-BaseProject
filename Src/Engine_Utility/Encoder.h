
#ifndef __ENGINE_ENCODER_H__
#define __ENGINE_ENCODER_H__

/*-----------------------------------------------------------------------------
* cDataPackage CLASS
*---------------------------------------------------------------------------*/

class __declspec(dllexport) cEncoder
{
protected:
	void EncodeFileToFile(char *inputfilename, char *outputfilename);
public:
	cEncoder(){};
	~cEncoder(){};

	void Encoder_Save(char *inputfilename, char *outputfilename);
	char* Encoder_Load(char *inputfilename, char *outputfilename = NULL);
};

#endif

