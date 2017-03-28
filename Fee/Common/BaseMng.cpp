// BaseMng.cpp: implementation of the CBaseMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseMng::CBaseMng()
{
	m_pTimeMng =  NULL;
	
}

CBaseMng::~CBaseMng()
{
	if(m_pTimeMng)
		delete m_pTimeMng;
}


void CBaseMng::OnTime()
{
	if(m_pTimeMng)
		m_pTimeMng->TrigTimer();
}


DWORD CBaseMng::SetTimer(int sec, DWORD param1, BYTE *pparam2, DWORD param2len)
{
	if(m_pTimeMng)
		return m_pTimeMng->SetTimer(sec,(DWORD)this,param1,pparam2,param2len,StOnTrickTime);
	return 0xFFFFFFFF;
}

void CBaseMng::KillTimer(DWORD id)
{
	if(m_pTimeMng)
		m_pTimeMng->KillTimer(id);
}


void CBaseMng::StOnTrickTime(DWORD nTimeID, DWORD param, DWORD param1, BYTE *pParam2,DWORD param2len)
{
	CBaseMng* pBase = (CBaseMng*) param;
	if(pBase)
	{
		pBase->OnTickTime(nTimeID,param1,pParam2, param2len);
	}
}

void CBaseMng::OnTickTime(DWORD nTimeID, DWORD param1, BYTE *pParam2,DWORD param2len)
{
	
}

BOOL CBaseMng::GetTimerParam(DWORD id, DWORD &param1, BYTE *param2, DWORD &param2len)
{
	DWORD param;
	return m_pTimeMng->GetTimerParam(id,param,param1,param2,param2len);
}

BOOL CBaseMng::InitTimer(int nCount)
{
	m_pTimeMng = new CNewTimerManager(nCount);
	if(m_pTimeMng==NULL)
		return FALSE;
	return TRUE;
}
