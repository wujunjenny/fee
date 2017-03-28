// MonthFee.h: interface for the CMonthFee class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONTHFEE_H__039590B9_1926_4F2A_8462_0A0740AFF4ED__INCLUDED_)
#define AFX_MONTHFEE_H__039590B9_1926_4F2A_8462_0A0740AFF4ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"


class CMonthFee  
{
public:
	void SetLoadFromDBIsOK();
	CMonthFee();
	virtual ~CMonthFee();

	BOOL Find(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr);
	BOOL Add(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr);
	BOOL Del(LPCSTR pFeeMonth, LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pUserAddr);
	//��ȡ��ǰ�Ʒ������ڰ��¿۷Ѽ�¼������
	DWORD GetCount()
	{
		DWORD count;
		EnterCriticalSection(&m_Lock);
		count = m_MonthFeeMap.GetCount();
		LeaveCriticalSection(&m_Lock);
		return count;
	};

	//��ȡ��ǰ�Ʒ�����
	CString GetFeeMonth()
	{
		CString FeeMonth;
		EnterCriticalSection(&m_Lock);
		FeeMonth = m_FeeMonth;
		LeaveCriticalSection(&m_Lock);

		return FeeMonth;
	};

protected:

	CMap<CString, LPCSTR, CString, CString&> m_MonthFeeMap;
	CRITICAL_SECTION m_Lock;
	//��ǰ�Ʒ�����
	CString m_FeeMonth;
	bool m_bLoadFromDBIsOK;
};

#endif // !defined(AFX_MONTHFEE_H__039590B9_1926_4F2A_8462_0A0740AFF4ED__INCLUDED_)
