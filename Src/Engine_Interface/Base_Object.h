//******************************************************************//
// Base_Object.h	:	interface for the CBase_Object class.
// 사용법           :   CBase_Object<int>				m_BoomCard;
// 작성일			:	2002.6.10
// 설명				:	Template과 STL을 이용한 데이터 관리 오브젝트.
//						객체의 생성과 삭제, 추가등의 데이터가 
//						공통적으로 사용하는 기능을 모아놓고 데이터 관련 
//						클래스들이 기반 클래스로 사용한다.
//						임의의 객체의 포인터를 관리한다.
//******************************************************************//

#ifndef _BASE_OBJECT_
#define _BASE_OBJECT_

#pragma warning(disable:4786)

#include <map>
#include <algorithm>

#define _OBJECT_ADD_ -1
#define _OBJECT_CURRENT_ -1
#define _OBJECT_RETURN_ERR_ -1

using namespace std;	// STL 사용 

template<class OBJECT>	// Template 사용
class CBase_Object  
{

typedef map<int, OBJECT*> OBJ_LIST;
//typedef OBJ_LIST::value_type Val_Type;
//typedef OBJ_LIST::iterator OBJ_LIST_ITR;

public:
	CBase_Object(bool bDeAlloc = true) { Init(bDeAlloc); }
	virtual ~CBase_Object() { Release(); }

private:
	OBJ_LIST				*m_pList;   // 전체 Object 리스트 포인트
	int m_nCurIndex;
	bool m_bDeAlloc;
	// 초기화 
	void Init(bool bDeAlloc) 
	{
		m_pList			= new OBJ_LIST;
		m_nCurIndex		= 0;
		m_bDeAlloc		= bDeAlloc;
	};

	// 해제 
	void Release()
	{
		OBJECT *pObj;

		if(m_bDeAlloc)
		{
			for(int i=0; i<GetCount(); i++)
			{
				pObj = (*m_pList)[i];
				if(pObj) delete pObj;
			}
		}
		delete m_pList;
	};

public:
	OBJ_LIST* GetObjectList() { return m_pList; };

	int GetCount()		{ return m_pList->size(); };		// 총 Object의 갯수를 얻는다
	bool IsEmpty()		{ return m_pList->empty(); };		// List가 비어있는지?


	// 현재 선택되어 있는 Object의 Index를 얻는다
	int GetCurIndex()	{ return m_nCurIndex; };

	// 리스트의 메모리들을 해제 할것인지 
	void SetDeallocMem(bool bDealloc) { m_bDeAlloc = bDealloc; }

	// 현재 선택된 Object의 Index를 변경한다 Index = 0일경우 첫번째 원소로 지정 
	bool SetCurIndex(int nIndex = 0)
	{
		if( (IsEmpty()) || (nIndex>=GetCount()) || (nIndex<0)) return FALSE;
		m_nCurIndex = nIndex;
		return TRUE;
	};

	// 첫 원소를 현재 선택된 원소로 지정한다
	bool SetFirstIndex()
	{
		if(IsEmpty()) return false;
		m_nCurIndex = 0;
		return true;
	}

	// 마지막 원소를 현재 선택된 원소로 지정한다
	bool SetLastIndex()
	{
		if(IsEmpty()) return false;
		m_nCurIndex = GetCount()-1;
		return true;
	};

	// 현재 원소보다 한단계 전것을 현재 원소로 지정 
	bool SetMinusIndex()
	{
		if(IsEmpty() || (GetCount()==1) || (m_nCurIndex==0)) return false;
		m_nCurIndex--;
		return true;
	}

	// 현재 원소보다 한단계 앞선것을 현재 원소로 지정 
	bool SetPlusIndex()
	{
		if(IsEmpty() || ((GetCount()-1)==m_nCurIndex)) return false;
		m_nCurIndex++;
		return true;
	}

	// Object를 얻는다 //
	OBJECT* GetObject(int nIndex = _OBJECT_CURRENT_) // nIndex가 _OBJECT_CURRENT_이면 현재 선택된Object를 얻는다
	{
		if((IsEmpty()) || (nIndex>=GetCount()) || (nIndex<_OBJECT_CURRENT_)) return NULL;
		if(nIndex==-1)	return (*m_pList)[m_nCurIndex];	
		return (*m_pList)[nIndex];	
	};

	// 오브젝트를 List에 추가한다 //
	// nIndex의 위치에 오브젝트를 할당해서 삽입한다 , nIndex = _OBJECT_ADD_ -> 맨뒤에 추가
	OBJECT* InsertObject(int nIndex = _OBJECT_ADD_, OBJECT* pInsertObj = NULL) 
	{
		if((nIndex>GetCount()) || (nIndex<_OBJECT_ADD_)) return NULL;

		OBJECT* pNewObject;
		if(pInsertObj==NULL) pNewObject= new OBJECT;
		else pNewObject = pInsertObj;

		if(nIndex==_OBJECT_ADD_)
			m_pList->insert(Val_Type(GetCount(), pNewObject));
		else
		{
			for(int i=GetCount(); i>nIndex; i--)
				(*m_pList)[i] = (*m_pList)[i-1];
			(*m_pList)[nIndex] = pNewObject;
		}

		return pNewObject;
	};

	// 오브젝트를 List에서 삭제한다 
	// nIndex의 위치에 있는 오브젝트를 삭제한다 , nIndex = _OBJECT_CURRENT_-> 현재의 오브젝트 삭제
	bool DeleteObject(int nIndex = _OBJECT_CURRENT_)
	{
		if((IsEmpty()) || (nIndex>=GetCount()) || (nIndex<_OBJECT_CURRENT_)) return false;

		int nRealIndex = (nIndex==-1) ? m_nCurIndex : nIndex;

		OBJECT *pDelObj;		
		if(m_bDeAlloc)
		{
			pDelObj = (OBJECT *)(*m_pList)[nRealIndex];
			if(pDelObj) delete pDelObj;
		}

		OBJ_LIST *pTemp = new OBJ_LIST;

		int nCnt = GetCount() - 1;
		int nRealCnt = 0;

		for(int i=0; i<nCnt; i++)
		{
			if(i<nRealIndex) nRealCnt = i;
			else nRealCnt = i+1;

			pTemp->insert(Val_Type(i, (*m_pList)[nRealCnt]));
		}

		delete m_pList;
		m_pList = pTemp;

		if(!IsEmpty())
			if(GetCount() == m_nCurIndex) m_nCurIndex = GetCount()-1;

		return true;
	};

	// 모든 오브젝트를 삭제한다 
	void DeleteAll()
	{ 
		if(m_bDeAlloc)
		{
			OBJECT *pDelObj;
			int nCnt = GetCount();
			for(int i=0; i<nCnt; i++)
			{
				pDelObj = (*m_pList)[i];
				if(pDelObj) delete pDelObj;
			}
		}
		
		OBJ_LIST_ITR Del_itor;
		int nCnt = GetCount();
		for(int i=0; i<nCnt; i++)
		{
			Del_itor = m_pList->find(i);
			m_pList->erase(Del_itor);
		}

		m_nCurIndex = 0;
	};

};

#endif 
