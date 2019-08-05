/*------------------------------------------------------------------------------
 * COMMENTS:
 *			타이머 클래스 정의
 *
 * FILENAME:
 *			Timer.h
 *
 * DESCRIPTION:
 *			사용자가 게임 진행중 타이머 기능을 위해 작성되었다.
 *
 * HISTORY:
 *			2004.07.21 - 처음으로 작성됨.
 *			2004.07.23 - 타이머 상태를 저장하기 위해 status 변수 추가됨.
 *---------------------------------------------------------------------------*/
#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef enum TIMER_STATUS
{
	TIMERSTATUS_INVALID = 0,		// 비정상적인 타이머 상태
	TIMERSTATUS_PENDING,			// 타이머 동작중
	TIMERSTATUS_COUNTDOWN,			// 타이머 시간 감소
	TIMERSTATUS_EXPIRED,			// 타이머 시간 완료
} TIMER_STATUS;

class cTimer
{
private:
	long timerCounter;				// 타이머카운터
	clock_t lastClock;				// 마지막 클럭 타임
	TIMER_STATUS status;			// 타이머 상태

public:
	cTimer()
	{ 
		reset(); 					// 생성시 타이머를 리셋한다.
	}

	inline void reset()
	{
		timerCounter	= 0;
		lastClock		= 0;
		status			= TIMERSTATUS_INVALID;
	}

	inline void set( const long counter )
	{
		timerCounter	= counter;
		lastClock		= clock();
		status			= TIMERSTATUS_PENDING;
	}

	inline TIMER_STATUS proc()
	{
		if( timerCounter <= 0 )
		{
			status = TIMERSTATUS_INVALID;
			return status;
		}

		clock_t nowClock = clock();

		     if( nowClock < lastClock ) timerCounter--;
		else if( nowClock > lastClock + CLOCKS_PER_SEC )
		{
			while( nowClock > lastClock + CLOCKS_PER_SEC )
			{
				timerCounter--;
				lastClock += CLOCKS_PER_SEC;
			}
		}
		else
		{
			status = TIMERSTATUS_PENDING;
			return status;
		}

		lastClock		= nowClock;
		if( timerCounter <= 0 ) status = TIMERSTATUS_EXPIRED;
		else                    status = TIMERSTATUS_COUNTDOWN;

		return status;
	}

	inline long get_counter() { return timerCounter; }
	inline clock_t get_clock() { return lastClock; }
	inline TIMER_STATUS get_status() { return status; }
};

#endif

