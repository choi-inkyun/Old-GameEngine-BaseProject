// StopWatch.h: interface for the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOPWATCH_H__05350725_030B_4DD5_B882_EFAD081B9984__INCLUDED_)
#define AFX_STOPWATCH_H__05350725_030B_4DD5_B882_EFAD081B9984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mmsystem.h"

#ifdef _AFXDLL
#define DLLFunction  __declspec(dllexport)
#else
#define DLLFunction  __declspec(dllimport)
#endif

class CStopWatch  
{
public:
	DLLFunction	__stdcall	CStopWatch();
	DLLFunction	__stdcall	~CStopWatch();

	float m_currentTime;
	float m_startTime;
	float m_targetTime;

	int m_state;  // false : not running     true : running.

	DLLFunction	void __stdcall	Start(float time = 0.0f);
	DLLFunction	void __stdcall	SetTargetTime(float time);
	DLLFunction	float __stdcall	GetTargetTime();
	DLLFunction	void __stdcall	AddTargetTime(float time);
	DLLFunction	void __stdcall	Stop();

	DLLFunction	float __stdcall	GetCurrentTime();
	DLLFunction	float __stdcall	GetReverseTime();
	DLLFunction	void __stdcall	SetReverseTime(float time);
	DLLFunction	bool __stdcall	IsOver();
	DLLFunction	bool __stdcall	IsRunning();
};

#endif // !defined(AFX_STOPWATCH_H__05350725_030B_4DD5_B882_EFAD081B9984__INCLUDED_)
