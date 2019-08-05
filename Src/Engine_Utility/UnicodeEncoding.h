
#ifndef __UNICODEENCODING_H__
#define __UNICODEENCODING_H__

class cUnicodeEncoding
{
public:
	static inline int  WSTR2MBS( LPCSTR LPSTR, LPSTR lpMBS, int nMBytes )
	{
		return WideCharToMultiByte( 949, 0, LPSTR, -1, lpMBS, nMBytes, NULL, NULL );
	}

	static inline int  MBS2WSTR( LPCSTR lpMBS, LPSTR LPSTR, int nWBytes )
	{
		return MultiByteToWideChar( 949, 0, lpMBS, -1, LPSTR, nWBytes );
	}
};


#endif

