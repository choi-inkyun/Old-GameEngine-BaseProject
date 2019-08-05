//******************************************************************//
// Base_Object.h	:	interface for the CBase_Object class.
// ����           :   CBase_Object<int>				m_BoomCard;
// �ۼ���			:	2002.6.10
// ����				:	Template�� STL�� �̿��� ������ ���� ������Ʈ.
//						��ü�� ������ ����, �߰����� �����Ͱ� 
//						���������� ����ϴ� ����� ��Ƴ��� ������ ���� 
//						Ŭ�������� ��� Ŭ������ ����Ѵ�.
//						������ ��ü�� �����͸� �����Ѵ�.
//******************************************************************//

#ifndef _BASE_OBJECT_
#define _BASE_OBJECT_

#pragma warning(disable:4786)

#include <map>
#include <algorithm>

#define _OBJECT_ADD_ -1
#define _OBJECT_CURRENT_ -1
#define _OBJECT_RETURN_ERR_ -1

using namespace std;	// STL ��� 

template<class OBJECT>	// Template ���
class CBase_Object  
{

typedef map<int, OBJECT*> OBJ_LIST;
//typedef OBJ_LIST::value_type Val_Type;
//typedef OBJ_LIST::iterator OBJ_LIST_ITR;

public:
	CBase_Object(bool bDeAlloc = true) { Init(bDeAlloc); }
	virtual ~CBase_Object() { Release(); }

private:
	OBJ_LIST				*m_pList;   // ��ü Object ����Ʈ ����Ʈ
	int m_nCurIndex;
	bool m_bDeAlloc;
	// �ʱ�ȭ 
	void Init(bool bDeAlloc) 
	{
		m_pList			= new OBJ_LIST;
		m_nCurIndex		= 0;
		m_bDeAlloc		= bDeAlloc;
	};

	// ���� 
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

	int GetCount()		{ return m_pList->size(); };		// �� Object�� ������ ��´�
	bool IsEmpty()		{ return m_pList->empty(); };		// List�� ����ִ���?


	// ���� ���õǾ� �ִ� Object�� Index�� ��´�
	int GetCurIndex()	{ return m_nCurIndex; };

	// ����Ʈ�� �޸𸮵��� ���� �Ұ����� 
	void SetDeallocMem(bool bDealloc) { m_bDeAlloc = bDealloc; }

	// ���� ���õ� Object�� Index�� �����Ѵ� Index = 0�ϰ�� ù��° ���ҷ� ���� 
	bool SetCurIndex(int nIndex = 0)
	{
		if( (IsEmpty()) || (nIndex>=GetCount()) || (nIndex<0)) return FALSE;
		m_nCurIndex = nIndex;
		return TRUE;
	};

	// ù ���Ҹ� ���� ���õ� ���ҷ� �����Ѵ�
	bool SetFirstIndex()
	{
		if(IsEmpty()) return false;
		m_nCurIndex = 0;
		return true;
	}

	// ������ ���Ҹ� ���� ���õ� ���ҷ� �����Ѵ�
	bool SetLastIndex()
	{
		if(IsEmpty()) return false;
		m_nCurIndex = GetCount()-1;
		return true;
	};

	// ���� ���Һ��� �Ѵܰ� ������ ���� ���ҷ� ���� 
	bool SetMinusIndex()
	{
		if(IsEmpty() || (GetCount()==1) || (m_nCurIndex==0)) return false;
		m_nCurIndex--;
		return true;
	}

	// ���� ���Һ��� �Ѵܰ� �ռ����� ���� ���ҷ� ���� 
	bool SetPlusIndex()
	{
		if(IsEmpty() || ((GetCount()-1)==m_nCurIndex)) return false;
		m_nCurIndex++;
		return true;
	}

	// Object�� ��´� //
	OBJECT* GetObject(int nIndex = _OBJECT_CURRENT_) // nIndex�� _OBJECT_CURRENT_�̸� ���� ���õ�Object�� ��´�
	{
		if((IsEmpty()) || (nIndex>=GetCount()) || (nIndex<_OBJECT_CURRENT_)) return NULL;
		if(nIndex==-1)	return (*m_pList)[m_nCurIndex];	
		return (*m_pList)[nIndex];	
	};

	// ������Ʈ�� List�� �߰��Ѵ� //
	// nIndex�� ��ġ�� ������Ʈ�� �Ҵ��ؼ� �����Ѵ� , nIndex = _OBJECT_ADD_ -> �ǵڿ� �߰�
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

	// ������Ʈ�� List���� �����Ѵ� 
	// nIndex�� ��ġ�� �ִ� ������Ʈ�� �����Ѵ� , nIndex = _OBJECT_CURRENT_-> ������ ������Ʈ ����
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

	// ��� ������Ʈ�� �����Ѵ� 
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
