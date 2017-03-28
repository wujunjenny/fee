// MultiThreadMng.cpp: implementation of the CMultiThreadMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewMng::CNewMng()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_nTimeTick = GetTickCount();
	m_bMounted = FALSE;
	m_hThread = NULL;
}

CNewMng::~CNewMng()
{
	
}

void CNewMng::Start()
{
	DWORD dwThreadID;
	m_hThread = CreateThread(
		NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}


void CNewMng::Destroy()
{
	
	m_msgQueue.StopMountThread();
	m_msgQueue.DumpL2CacheToFile();
	m_msgQueue.DumpNormalDataToFile();
	
	SetEvent(m_hKillEvent);
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		//ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
	CloseHandle(m_hThread);
	m_hThread=NULL;
	
	CloseHandle(m_hKillEvent);
    m_hKillEvent = NULL; 
}    


BOOL CNewMng::SendMessage(PTRMSG pMsg, DWORD nMsgLen,DWORD npri )
{
	
	return (CNewMng::m_msgQueue).PutInMessage(pMsg,nMsgLen,npri);
}



void CNewMng::Run()
{
	while( WaitForSingleObject(m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		
		if(m_bMounted)
		{
			PTRMSG pMsg = (CNewMng::m_msgQueue).WaitForNormalMessage(100);
			if( pMsg )
			{
				OnMessage(pMsg);
			}
		}
		else
		{
			Sleep(10);
		}
		
		if((unsigned long)(GetTickCount() - m_nTimeTick) > (unsigned long)1000)
		{			
			OnTime();
			m_nTimeTick = GetTickCount();
			m_bMounted = OnMountMessage();
		}
	}
}


DWORD WINAPI CNewMng::ThreadProc(LPVOID lpParam)
{
	//SMGW41-62, 2005-6-17, jdz, modi begin//
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	CNewMng* pVM = (CNewMng*)lpParam;
	ASSERT(pVM);
	pVM->Run();
	
	::CoUninitialize();
	//SMGW41-62, 2005-6-17, jdz, modi end//
	
	return 0L;
}


BOOL CNewMng::Init( LPCTSTR pQueName,DWORD maxbuffsize, DWORD maxMsgSize, DWORD maxMsgCount)
{
	BOOL rt;
	rt = m_msgQueue.InitQueue(pQueName, maxbuffsize , maxMsgCount , maxMsgSize);
	m_msgQueue.LoadFileCache();
	m_msgQueue.StartMountThread();
	return rt;
}
