 // Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__Thread__h__)
#define __Thread__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThread  
{
public:
	CThread();
	virtual ~CThread();
	BOOL IsActive();

    virtual BOOL Stop(DWORD dwWaitTime = 0xffffffff, BOOL bTerminate = FALSE);
    virtual BOOL Start();

protected:
	BOOL m_bAutoDel;
	BOOL IsExit();
	virtual int Run(DWORD param);
	HANDLE m_hThread;
	HANDLE m_hEvent;
private:
	//static DWORD WINAPI stRun(LPVOID lparam);
    static unsigned int WINAPI stRun(LPVOID lparam);
};

#endif // !defined(__Thread__h__)
