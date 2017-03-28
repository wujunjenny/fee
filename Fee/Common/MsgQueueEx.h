// MsgQueueEx.h: interface for the CMessageQueueEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__MsgQueueEx__h__)
#define __MsgQueueEx__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NAME_LEN_EX 128


struct   tagQueueInShMem
{
	DWORD   dwQueueHead; // write data index in queue
	DWORD   dwQueueTail; // read  data index in queue
	DWORD   dwQueueSize;
	char    buf[1];
} ;
typedef struct tagQueueInShMem * PtrLoopQueue;

class CMsgQueueEx  
{
public:
    CMsgQueueEx();
	CMsgQueueEx(LPCTSTR pszName, DWORD dwQueueSize);
	virtual ~CMsgQueueEx();
	BOOL   Create();
    BOOL   Create(LPCTSTR pszName, DWORD dwQueueSize);
	
	void    Empty();
	BOOL    WaitForNormalMessage(void *buf, DWORD& dwSize, DWORD dwMilliseconds=INFINITE);
	BOOL    PutInMessage(void *pMsg, DWORD dwSize);
	
private:
	PtrLoopQueue m_pMsgQueue;
	
	DWORD   m_dwQueueSize;
	CString m_strQueueName;
	CString m_strMutexName;
	CString m_strEventName;
	
	HANDLE  m_hMutex;
	HANDLE  m_hEvent;
	HANDLE  m_hShMem;
};

#endif // !defined(__MsgQueueEx__h__)
