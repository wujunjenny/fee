// MultiThreadMng.cpp: implementation of the CMultiThreadMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smserviceproc.h"
#include "MultiThreadMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMsgQueue CMultiThreadMng::m_msgQueue;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiThreadMng::CMultiThreadMng()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_hThread = CreateThread(
		NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

CMultiThreadMng::~CMultiThreadMng()
{
	
}

void CMultiThreadMng::Destroy()
{
	SetEvent(m_hKillEvent);
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
	CloseHandle(m_hThread);
	m_hThread=NULL;
	
	CloseHandle(m_hKillEvent);
    m_hKillEvent = NULL; 
}    


BOOL CMultiThreadMng::SendMessage(PTRMSG pMsg, DWORD nMsgLen)
{
	PTRMSG pNewMsg = NewMessage(pMsg, nMsgLen);
    if (NULL == pNewMsg)
    {
        ASSERT(0);
        return FALSE;
    }
	return (CMultiThreadMng::m_msgQueue).PutInMessage(pNewMsg);
}

void CMultiThreadMng::Run()
{
	while( WaitForSingleObject(m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		PTRMSG pMsg = (CMultiThreadMng::m_msgQueue).WaitForNormalMessage(100);
		if( pMsg )
		{
			OnMessage(pMsg);
			DeleteMessage(pMsg);
		}
	}
}

DWORD WINAPI CMultiThreadMng::ThreadProc(LPVOID lpParam)
{
	CMultiThreadMng* pVM = (CMultiThreadMng*)lpParam;
	ASSERT(pVM);
	pVM->Run();
	return 0L;
}

