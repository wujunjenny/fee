#ifndef __TimerMng__h__
#define __TimerMng__h__


class CTriggerList;
struct TIME_TRIGGER
{
	DWORD time;
	DWORD ticks;
	DWORD dwPara1;
	DWORD dwPara2;
	WORD wTriggerTimes;
	BYTE cSequenceNO;
	void (*pNotifyCallback)(DWORD, DWORD, DWORD);
	
	CTriggerList* pOwner;
	
	TIME_TRIGGER* pPrevTrigger;
	TIME_TRIGGER* pNextTrigger;
} ;

class CTriggerList
{
public:
	CTriggerList() { m_pHead = m_pTail = NULL; m_dwCount = 0; }
	TIME_TRIGGER* GetHead();
	TIME_TRIGGER* GetTail();
	TIME_TRIGGER* GetNext(TIME_TRIGGER* pCur);
	TIME_TRIGGER* GetPrev(TIME_TRIGGER* pCur);
	void AddTail(TIME_TRIGGER* pTrigger);
	void AddHead(TIME_TRIGGER* pTrigger);
	void InsertBefore(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger);
	void InsertAfter(TIME_TRIGGER* pCur, TIME_TRIGGER* pTrigger);
	void Remove(TIME_TRIGGER* pTrigger);
	void RemoveAll();
	DWORD GetCount() { return m_dwCount; }
protected:
	TIME_TRIGGER* m_pHead;
	TIME_TRIGGER* m_pTail;
	
	DWORD m_dwCount;
};

class CTimerMng
{
public:
	CTimerMng(int nCount);
	~CTimerMng();
	
	DWORD SetTimer(DWORD dwMilliSeconds, DWORD dwPara1, WORD wTimes,
		DWORD dwPara2, void(*pFunc)(DWORD, DWORD, DWORD));
	void KillTimer(DWORD nTimerID);
	void KillTimerUsingUserID(DWORD nUserID);
	
protected:
	CRITICAL_SECTION m_lock;
	CTriggerList m_workTriggers;
	CTriggerList m_idleTriggers;
	TIME_TRIGGER* m_pBuf;
	int m_nCount;
	
	void AddWorkTrigger(TIME_TRIGGER* pTrigger);
	
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	BOOL m_bAlive;
};

#endif
