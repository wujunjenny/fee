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
	{//没有加载成功时，默认存在
		return TRUE;
	}

	int ret = m_FeeMonth.Compare(pFeeMonth);
	if(ret < 0)
	{//如果计费帐期变化了，默认不存在
		return FALSE;
	}
	else if(ret > 0)
	{//查找历史帐期的包月扣费数据，默认存在
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
	{//如果计费帐期变化了，则删除当前保存的包月扣费记录
		EnterCriticalSection(&m_Lock);
		m_MonthFeeMap.RemoveAll();
		m_FeeMonth = pFeeMonth;
		LeaveCriticalSection(&m_Lock);		
	}
	else if (ret > 0)
	{//添加历史帐期的包月扣费数据，默认已存在，不添加
		return TRUE;
	}

	//添加包月扣费记录
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
	{//是当前帐期才删除		
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
