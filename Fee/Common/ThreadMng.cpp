///////////////////////////////////////////////////////////////////////////////////
//	此文件实现一个虚拟的任务管理员，主要提供消息驱动机制
#include "stdafx.h"
#include "ThreadMng.h"

//////////////////////////////////////////////////////////////////////////////
//	class CManager 
//		HANDLE m_hThread;
//		HANDLE m_hKillEvent;
//		CMsgQueue m_msgQueue;
CThreadMng::CThreadMng()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_hThread = CreateThread(
		NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

CThreadMng::~CThreadMng()
{
}

void CThreadMng::Destroy()
{
	SetEvent(m_hKillEvent);
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
	CloseHandle(m_hKillEvent);
	CloseHandle(m_hThread);
	
    m_hKillEvent = NULL; 
    m_hThread = NULL;
}

// *** SMGW25-32,2004-07-05,wzy modify begin *** //
BOOL CThreadMng::SendMessage(PTRMSG pMsg, DWORD nMsgLen)
{
	PTRMSG pNewMsg = NewMessage(pMsg, nMsgLen);
    if (NULL == pNewMsg)
    {
        ASSERT(0);
        return FALSE;
    }
	return m_msgQueue.PutInMessage(pNewMsg);
}
// *** SMGW25-32,2004-07-05,wzy modify end *** //

void CThreadMng::Run()
{
	while( WaitForSingleObject(m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		PTRMSG pMsg = m_msgQueue.WaitForNormalMessage(100);
		if( pMsg )
		{
			OnMessage(pMsg);
			DeleteMessage(pMsg);
		}
	}
}

DWORD WINAPI CThreadMng::ThreadProc(LPVOID lpParam)
{
	CThreadMng* pVM = (CThreadMng*)lpParam;
	ASSERT(pVM);
	pVM->Run();
	return 0L;
}

