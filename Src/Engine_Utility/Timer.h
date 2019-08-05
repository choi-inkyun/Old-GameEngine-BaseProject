/*------------------------------------------------------------------------------
 * COMMENTS:
 *			Ÿ�̸� Ŭ���� ����
 *
 * FILENAME:
 *			Timer.h
 *
 * DESCRIPTION:
 *			����ڰ� ���� ������ Ÿ�̸� ����� ���� �ۼ��Ǿ���.
 *
 * HISTORY:
 *			2004.07.21 - ó������ �ۼ���.
 *			2004.07.23 - Ÿ�̸� ���¸� �����ϱ� ���� status ���� �߰���.
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
	TIMERSTATUS_INVALID = 0,		// ���������� Ÿ�̸� ����
	TIMERSTATUS_PENDING,			// Ÿ�̸� ������
	TIMERSTATUS_COUNTDOWN,			// Ÿ�̸� �ð� ����
	TIMERSTATUS_EXPIRED,			// Ÿ�̸� �ð� �Ϸ�
} TIMER_STATUS;

class cTimer
{
private:
	long timerCounter;				// Ÿ�̸�ī����
	clock_t lastClock;				// ������ Ŭ�� Ÿ��
	TIMER_STATUS status;			// Ÿ�̸� ����

public:
	cTimer()
	{ 
		reset(); 					// ������ Ÿ�̸Ӹ� �����Ѵ�.
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

