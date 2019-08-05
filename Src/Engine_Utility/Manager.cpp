/*
 * FILENAME:
 *			Manager.cpp
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

#include "StdAfx.h"
#include "Manager.h"

// 싱글톤을 다른 프로젝트에 알린다.
_CStateM		_CStateMBlock;
unsigned long		_init_CState::ms_dwLong;

/*-----------------------------------------------------------------------------
 * cStateManager CLASS
 *---------------------------------------------------------------------------*/

cStateManager::cStateManager()
{
	m_StateParent = NULL;
}

cStateManager::~cStateManager()
{
	PopAll();
}

//////////////////////////////////////////////////////////////////////////////////
cStateManager* cStateManager::__singleton()
// : script callback
//////////////////////////////////////////////////////////////////////////////////
{
	return &g_cStateManager;
}


// push a function on to the stack
void cStateManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr )
{
	// Don't push a NULL value
	if( Function != NULL )
	{
		// Allocate a new state and push it on stack
		sState *StatePtr = new sState();

		StatePtr->Function = Function;
		StatePtr->Next = m_StateParent;

		m_StateParent = StatePtr;

		// Call state with init purpose
		StatePtr->Function( DataPtr, PURPOSE_CREATE );
	}
}

BOOL cStateManager::Pop( void *DataPtr )
{
	sState *StatePtr;

	// Remove the head of stack (if any)
	if( (StatePtr = m_StateParent) != NULL )
	{
		// First call with shutdown purpose
		m_StateParent->Function( DataPtr, PURPOSE_DESTROY );

		m_StateParent = StatePtr->Next;
		StatePtr->Next = NULL;
		delete StatePtr;
	}

	// return TRUE if more states exist, FALSE otherwise.
	if( m_StateParent == NULL ) return FALSE;

	return TRUE;
}

void cStateManager::PopAll( void *DataPtr )
{
	while( Pop( DataPtr ) == TRUE );
}

BOOL cStateManager::Process( void *DataPtr, PURPOSE purpose )
{
	// return an error if no more states.
	if( m_StateParent == NULL ) return FALSE;

	// process the top-most state
	m_StateParent->Function( DataPtr, purpose );

	return TRUE;
}


/*-----------------------------------------------------------------------------
 * cProcessManager CLASS
 *---------------------------------------------------------------------------*/

cProcessManager::cProcessManager()
{
	m_ProcessParent = NULL;
}

cProcessManager::~cProcessManager()
{
	PopAll();
}

// push a function on to the stack
void cProcessManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr )
{
	// Don't push a NULL value
	if( Function != NULL )
	{
		// Allocate a new Process and push it on stack
		sProcess *ProcessPtr = new sProcess();
		ProcessPtr->Function = Function;
		ProcessPtr->Next = m_ProcessParent;
		m_ProcessParent = ProcessPtr;

		// Call Process with init purpose
		ProcessPtr->Function( DataPtr, PURPOSE_CREATE );
	}
}

BOOL cProcessManager::Pop( void *DataPtr )
{
	sProcess *ProcessPtr;

	// Remove the head of stack (if any)
	if( (ProcessPtr = m_ProcessParent) != NULL )
	{
		// First call with shutdown purpose
		m_ProcessParent->Function( DataPtr, PURPOSE_DESTROY );
		m_ProcessParent = ProcessPtr->Next;
		ProcessPtr->Next = NULL;
		delete ProcessPtr;
	}

	// return TRUE if more Processs exist, FALSE otherwise.
	if( m_ProcessParent == NULL ) return FALSE;

	return TRUE;
}

void cProcessManager::PopAll( void *DataPtr )
{
	while( Pop( DataPtr ) == TRUE );

}

// Process all functions
void cProcessManager::Process( void *DataPtr, PURPOSE purpose )
{
	sProcess *ProcessPtr = m_ProcessParent;

	while( ProcessPtr != NULL )
	{
		ProcessPtr->Function( DataPtr, purpose );
		ProcessPtr = ProcessPtr->Next;
	}
}

