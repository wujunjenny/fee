// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"

#include <process.h>    /* _beginthread, _endthread */
//end add

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
{
    m_hThread = NULL;
    m_hEvent = NULL;
	m_hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
}

CThread::~CThread()
{
	//Stop();
	//	TRACE("Thread Close Event %d\n",m_hEvent);
    if (NULL != m_hEvent)
		CloseHandle(m_hEvent);
	
    if (NULL != m_hThread)
    {
        CloseHandle(m_hThread);
    }
    //end change
}

int CThread::Run(DWORD param)
{
	return TRUE;
}


BOOL CThread::Start()
{
	//DWORD id;
	//m_hThread=::CreateThread(NULL,NULL,stRun,this,NULL,&id);
    
    //m_hThread=::CreateThread(NULL, 1024 * 1024 * 3, stRun,this,NULL, NULL); 
    unsigned int iThreadID;
    m_hThread=(HANDLE)(::_beginthreadex(NULL, NULL, stRun, this, NULL, &iThreadID)); 
    //end change
	if(m_hThread==NULL)
    {
        ASSERT(0);
		return FALSE;
    }
	return TRUE;
}

//DWORD WINAPI CThread::stRun(LPVOID lparam)
unsigned int WINAPI CThread::stRun(LPVOID lparam)
{
	CThread* pTh=(CThread*)lparam;
	if(pTh==NULL)
		return -1;
	DWORD rt=pTh->Run(NULL);
	return rt;
}

BOOL CThread::IsExit()
{
	return WaitForSingleObject(m_hEvent,0)==WAIT_OBJECT_0;
}

BOOL CThread::Stop(DWORD dwWaitTime, BOOL bTerminate)
{
	//SetEvent(m_hEvent);
    if( !IsExit())
    {
        SetEvent(m_hEvent);
    }
    //end change
    DWORD res;
	
    //如果无限等待
    if (0xffffffff == dwWaitTime)
    {
        //如果立即退出
        if (bTerminate)
        {
            TerminateThread(m_hThread, -2);     //强制退出
            return TRUE;
        }
		
        res = WaitForSingleObject(m_hThread, INFINITE);
    }
    else
        res = WaitForSingleObject(m_hThread, dwWaitTime);
	
    if (res == WAIT_OBJECT_0)
        return TRUE;
    else
    {
        if (bTerminate)
        {
            ASSERT(0); 
            TerminateThread(m_hThread, -2);     //强制退出
            return TRUE;
        }
        else
            return FALSE;
    }
}

BOOL CThread::IsActive()
{
	if(WaitForSingleObject(m_hThread,0)==WAIT_TIMEOUT)
		return TRUE;
	else
		return FALSE;
}