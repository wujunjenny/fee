
//////////////////////////////////////////////////////////////////////

#if !defined(__ShareMemManager__h__)
#define __ShareMemManager__h__

#include "MsgQueueEx.h"

#define MAX_SHAREMEMMSG_SIZE           0XA000

class CShareMemManager  
{
public:
	CShareMemManager(
            LPCTSTR lpszShareMemName, 
            DWORD dwAccess = SMS_WRITE, 
            DWORD dwMemSize = 0xFFFF
            );
	virtual ~CShareMemManager();

    enum SHARE_MEM_STATE
    {
        SMS_READ = 0,
        SMS_WRITE = 1,
    };

public:

    BOOL SendMessageToQueue(void* pMsg, DWORD dwSize);

    BOOL Create();

	void Destroy();

protected:

    virtual BOOL OnMessage(void* pMsg, DWORD dwSize);

private:
	
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	HANDLE m_hThread;
	HANDLE m_hKillEvent;

    CString m_ShareName;
    DWORD m_dwMemSize;
    DWORD m_dwAccess;

    CMsgQueueEx m_MsgQueue;

    BYTE m_MsgBuff[MAX_SHAREMEMMSG_SIZE];

};

#endif
