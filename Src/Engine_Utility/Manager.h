/*
 * FILENAME:
 *			ENGINE_Manager.h
 *
 * DESCRIPTION:
 *			���±�� ���α׷����� ������ ���ÿ� ���� ������ ������ �����Ѵ�. �� ���´�
 *			�ϳ��� ��ü�� �Ϸ��� �Լ����� ��ǥ�ϸ�, �Լ��� �ʿ��� ���� �� �Լ��� ���ÿ�
 *			�߰��ϰ� ����� �ڿ� ���ÿ��� �� �Լ��� �����ϸ� �ȴ�.
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * HISTORY:
 *			2004.03.26 å�� ������ �״�� �����.
 */

#ifndef __ENGINE_MANAGER_H__
#define __ENGINE_MANAGER_H__

enum PURPOSE
{
	PURPOSE_INVALID = 0,
	PURPOSE_CREATE,							/* ����	ȣ��		*/
	PURPOSE_INITIALIZE,						/* �ʱ�ȭ ȣ��		*/
	PURPOSE_MSGPROC,						/* �޽��� ó��		*/
	PURPOSE_RENDER,							/* ������ ȣ��		*/
	PURPOSE_PROCESS,						/* ó�� ȣ��		*/
	PURPOSE_RELEASE,						/* ���� ȣ��		*/
	PURPOSE_DESTROY,						/* �Ҹ� ȣ��		*/
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

// �̱��� ��ü ���� Ŭ���� ����
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

// �̱����� �����ؾ���.
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

