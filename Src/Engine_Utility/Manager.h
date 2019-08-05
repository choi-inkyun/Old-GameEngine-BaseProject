/*
 * FILENAME:
 *			ENGINE_Manager.h
 *
 * DESCRIPTION:
 *			상태기반 프로그래밍은 상태의 스택에 따라 실행의 방향을 변경한다. 각 상태는
 *			하나의 객체나 일련의 함수들을 대표하며, 함수가 필요할 때는 그 함수를 스택에
 *			추가하고 사용한 뒤에 스택에서 그 함수를 제거하면 된다.
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * HISTORY:
 *			2004.03.26 책의 내용을 그대로 사용함.
 */

#ifndef __ENGINE_MANAGER_H__
#define __ENGINE_MANAGER_H__

enum PURPOSE
{
	PURPOSE_INVALID = 0,
	PURPOSE_CREATE,							/* 생성	호출		*/
	PURPOSE_INITIALIZE,						/* 초기화 호출		*/
	PURPOSE_MSGPROC,						/* 메시지 처리		*/
	PURPOSE_RENDER,							/* 랜더링 호출		*/
	PURPOSE_PROCESS,						/* 처리 호출		*/
	PURPOSE_RELEASE,						/* 삭제 호출		*/
	PURPOSE_DESTROY,						/* 소멸 호출		*/
};

struct sMsgParam
{
	HWND hWnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
	BOOL bHandled;
};

/*-----------------------------------------------------------------------------
 * cStateManager CLASS
 *---------------------------------------------------------------------------*/

// 싱글톤 객체 생성 클레스 선언
class _init_CState;

class __declspec(dllexport) cStateManager
{
	friend _init_CState;
	typedef struct sState
	{
		void (*Function)( void *Ptr, long Purpose );
		sState *Next;

		sState() { Function = NULL, Next = NULL; }
		~sState() { delete Next; }
	} sState;

protected:
	sState *m_StateParent;

protected:
	static cStateManager*			__singleton();

public:
	cStateManager();
	~cStateManager();

	void Push( void (*Function)( void *Ptr, long Purpose), void *DataPtr = NULL );
	BOOL Pop( void *DataPtr = NULL );
	void PopAll( void *DataPtr = NULL );
	BOOL Process( void *DataPtr = NULL, PURPOSE purpose = PURPOSE_RENDER );
};


////////////////////////// Singleton ///////////////////////////////////////

typedef unsigned char	_CStateM[sizeof(cStateManager)];
extern __declspec(dllimport) _CStateM		_CStateMBlock;
#define g_cStateManager			(*(cStateManager*) _CStateMBlock)

// 싱글톤을 생성해야함.
#ifdef ENGINE_UTILITY_EXPORTS

// initializer dummy
class _init_CState {
protected:
	static unsigned long			ms_dwLong;

public:
	_init_CState()
	{
		if (ms_dwLong == 0)
		{
			new (_CStateMBlock) cStateManager;
		}

		ms_dwLong++;
	}
	~_init_CState()
	{
		if (ms_dwLong == 1)
		{
			((cStateManager*) _CStateMBlock)->~cStateManager();
		}

		ms_dwLong--;
	}
};

// declare cStateManager singleton instance
static _init_CState			_init_cState;

#endif
/////////////////////////////////////////////////////////////////////////////////////////


/*-----------------------------------------------------------------------------
 * cProcessManager CLASS
 *---------------------------------------------------------------------------*/

class cProcessManager
{
	typedef struct sProcess
	{
		void (*Function)( void *Ptr, long Purpose );
		sProcess *Next;

		sProcess() { Function = NULL, Next = NULL; }
		~sProcess() { delete Next; }
	} sProcess;

protected:
	sProcess *m_ProcessParent;

public:
	cProcessManager();
	~cProcessManager();

	void Push( void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL );
	BOOL Pop( void *DataPtr = NULL );
	void PopAll( void *DataPtr = NULL );
	void Process( void *Ptr = NULL, PURPOSE purpose = PURPOSE_RENDER );
};

#endif

