// MonthFee.cpp: implementation of the CMonthFee class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "tt.h"
#include "MonthFee.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonthFee::CMonthFee()
{
	m_MonthFeeMap.InitHashTable( 20011, FALSE );
	InitializeCriticalSection(&m_Lock);
	m_bLoadFromDBIsOK = false;
}

CMonthFee::~CMonthFee()
{	
	m_MonthFeeMap.RemoveAll();
	DeleteCriticalSection(&m_Lock);
}

BOOL CMonthFee::Find(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr)
{
	if(!pSPID || !pServiceID || !pUserAddr || !pFeeAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	if(false == m_bLoadFromDBIsOK)
	{//û�м��سɹ�ʱ��Ĭ�ϴ���
		return TRUE;
	}

	int ret = m_FeeMonth.Compare(pFeeMonth);
	if(ret < 0)
	{//����Ʒ����ڱ仯�ˣ�Ĭ�ϲ�����
		return FALSE;
	}
	else if(ret > 0)
	{//������ʷ���ڵİ��¿۷����ݣ�Ĭ�ϴ���
		return TRUE;
	}

	CString key, value;
	key.Format("%s%s%s%s", pSPID, pServiceID, pFeeAddr, pUserAddr);
	key.MakeUpper();

	EnterCriticalSection(&m_Lock);
	if(m_MonthFeeMap.Lookup(key, value))
	{
		LeaveCriticalSection(&m_Lock);	
		return TRUE;
	}
	else
	{
		LeaveCriticalSection(&m_Lock);	
		return FALSE;
	}
}


BOOL CMonthFee::Add(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr)
{
	if(!pFeeMonth || !pSPID || !pServiceID || !pUserAddr || !pFeeAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	int ret = m_FeeMonth.Compare(pFeeMonth);
	if(ret < 0)
	{//����Ʒ����ڱ仯�ˣ���ɾ����ǰ����İ��¿۷Ѽ�¼
		EnterCriticalSection(&m_Lock);
		m_MonthFeeMap.RemoveAll();
		m_FeeMonth = pFeeMonth;
		LeaveCriticalSection(&m_Lock);		
	}
	else if (ret > 0)
	{//�����ʷ���ڵİ��¿۷����ݣ�Ĭ���Ѵ��ڣ������
		return TRUE;
	}

	//��Ӱ��¿۷Ѽ�¼
	CString key;
	key.Format("%s%s%s%s", pSPID, pServiceID, pFeeAddr, pUserAddr);
	key.MakeUpper();
	
	EnterCriticalSection(&m_Lock);
	m_MonthFeeMap[key] = "";
	LeaveCriticalSection(&m_Lock);	

	return TRUE;
}


BOOL CMonthFee::Del(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr)
{
	if(!pFeeMonth || !pSPID || !pServiceID || !pUserAddr || !pFeeAddr)
	{
		ASSERT(0);
		return FALSE;
	}

	if(m_FeeMonth.Compare(pFeeMonth) == 0)
	{//�ǵ�ǰ���ڲ�ɾ��		
		CString key;
		key.Format("%s%s%s%s", pSPID, pServiceID, pFeeAddr, pUserAddr);
		key.MakeUpper();
		
		EnterCriticalSection(&m_Lock);
		m_MonthFeeMap.RemoveKey( key );
		LeaveCriticalSection(&m_Lock);	

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



void CMonthFee::SetLoadFromDBIsOK()
{
	EnterCriticalSection(&m_Lock);
	m_bLoadFromDBIsOK = true;
	LeaveCriticalSection(&m_Lock);	
}
