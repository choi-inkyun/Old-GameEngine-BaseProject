// DSBGM.h: interface for the CDSBGM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSBGM_H__82208B17_E101_41A2_9E8C_1C4B1D9354B0__INCLUDED_)
#define AFX_DSBGM_H__82208B17_E101_41A2_9E8C_1C4B1D9354B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDSBGM  
{
public:
	bool repeat;
	char filename[256];
	char soundname[256];
	int index;
	CDSBGM();
	virtual ~CDSBGM();

};

#endif // !defined(AFX_DSBGM_H__82208B17_E101_41A2_9E8C_1C4B1D9354B0__INCLUDED_)
