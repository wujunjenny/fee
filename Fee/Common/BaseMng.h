// BaseMng.h: interface for the CBaseMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEMNG_H__CC102490_3708_47D0_A07C_2A851B2F6842__INCLUDED_)
#define AFX_BASEMNG_H__CC102490_3708_47D0_A07C_2A851B2F6842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewMng.h"
#include "newtimerman.h"
class CBaseMng : public CNewMng  
{
public:
	BOOL InitTimer(int nCount);
	CBaseMng();
	virtual ~CBaseMng();
	
protected:
	BOOL GetTimerParam(DWORD id,DWORD&param1,BYTE* param2,DWORD  &param2len);
	virtual void OnTickTime(DWORD nTimeID,DWORD param1,BYTE*pParam2,DWORD param2len);
	static void StOnTrickTime(DWORD sec,DWORD param,DWORD param1,BYTE* pParam2,DWORD param2len);
	void KillTimer(DWORD id);
	DWORD SetTimer(int sec,DWORD param1,BYTE* pparam2,DWORD param2len);
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
	// ��Ϣ�����������, ʵ�ֵ�ʱ����Ҫɾ����Ϣ�� !!
	//ʱ�Ӵ���
	virtual void OnTime();
	//add by wujun 
	virtual BOOL OnMountMessage() = NULL;
	
	CNewTimerManager* m_pTimeMng;
	
};

#endif // !defined(AFX_BASEMNG_H__CC102490_3708_47D0_A07C_2A851B2F6842__INCLUDED_)
