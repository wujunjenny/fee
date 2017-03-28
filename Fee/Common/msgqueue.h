
#ifndef __MsgQueue_h
#define __MsgQueue_h


#define MAX_STOREMSG_COUNT	80000

typedef char* PTRMSG;

class CMsgQueue
{
public:
	int GetQueSize();
	CMsgQueue();
	~CMsgQueue();
	// operators:
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE);
	BOOL PutInMessage(PTRMSG pMsg);
	PTRMSG GetSpecialMsg(int dwMilliseconds);
	
protected:
	// implementations:
	PTRMSG PopupMessage();
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	CPtrList m_listMsg;
};


PTRMSG WINAPI NewMessage(const char* pData=NULL, int nLen=0);
void WINAPI DeleteMessage(PTRMSG pMsg);

class CNewMsgQueue
{
public:
	CNewMsgQueue();
	~CNewMsgQueue();
	// operators:
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE);
	BOOL PutInMessage( PTRMSG pMsg, DWORD nSize );
	
protected:
	// implementations:
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	
	char*   m_pBuffer;
	char*   m_pTmpMsg;
	DWORD   m_dwTmpMsgSize;
	
	DWORD   m_dwQueueHead; 
	DWORD   m_dwQueueTail; 
	DWORD   m_dwQueueSize;
};

#endif
