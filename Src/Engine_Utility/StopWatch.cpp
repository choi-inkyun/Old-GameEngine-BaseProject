// StopWatch.cpp: implementation of the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StopWatch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStopWatch::CStopWatch()
{
	m_currentTime = 0.0f;
	m_startTime = 0.0f;
	m_targetTime = 0.0f;

	m_state = 0;  // false : not running     true : running.
}

CStopWatch::~CStopWatch()
{

}

void CStopWatch::Start(float time)
{
	m_currentTime = m_startTime = ((float)timeGetTime())*0.001f;
	m_state = true;
	m_targetTime = time;
}

void CStopWatch::SetTargetTime(float time)
{
	m_targetTime = time;
}

float CStopWatch::GetTargetTime()
{
	return m_targetTime;
}

void CStopWatch::AddTargetTime(float time)
{
	m_targetTime += time;
}

void CStopWatch::Stop()
{
	m_state = false;
}

float CStopWatch::GetCurrentTime()
{
	m_currentTime = ((float)timeGetTime())*0.001f;
	if(m_state)
		return m_currentTime-m_startTime;
	else
		return 0.0f;
}

float CStopWatch::GetReverseTime()
{
	m_currentTime = ((float)timeGetTime())*0.001f;
	if(m_state)
	{
		if(m_targetTime-(m_currentTime-m_startTime)<0.0f)
			return 0.0;
		else
			return m_targetTime-(m_currentTime-m_startTime);
	}
	else
		return 0.0f;
}

void CStopWatch::SetReverseTime(float time)
{
	if (m_state)
	{
		if (GetReverseTime() != time)
			m_targetTime = m_targetTime + time - GetReverseTime();
	}
	return;
}

bool CStopWatch::IsOver()
{
	m_currentTime = ((float)timeGetTime())*0.001f;

	if(m_targetTime-(m_currentTime-m_startTime) < 0.0f)
		return true;
	else
		return false;
}

bool CStopWatch::IsRunning()
{
	return m_state;
}
