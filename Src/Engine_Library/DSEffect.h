// DSEffect.h: interface for the CDSEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSEFFECT_H__4C608C95_0EF4_4397_B958_F2ACD842C931__INCLUDED_)
#define AFX_DSEFFECT_H__4C608C95_0EF4_4397_B958_F2ACD842C931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDSEffect  
{
public:
	bool repeat;
	char filename[256];
	char soundname[256];
	int index;
	CDSEffect();
	virtual ~CDSEffect();

};

#endif // !defined(AFX_DSEFFECT_H__4C608C95_0EF4_4397_B958_F2ACD842C931__INCLUDED_)
