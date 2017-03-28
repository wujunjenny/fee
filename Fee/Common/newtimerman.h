#ifndef __TIMER_MANAGER_1_
#define __TIMER_MANAGER_1_

#define MAX_PARAM2_LEN 100
#define INVALID_TIMER_ID	0xFFFFFFFF

typedef void (*TIMECALLBACKFUN)(DWORD, DWORD, DWORD,BYTE*,DWORD);

class CNewTriggerList;
struct TIME_TRIGGER_NEW
{
	DWORD time;//触发时间time_t类型
	DWORD dwPara;
	DWORD dwPara1;//参数1
	BYTE  sPara2[MAX_PARAM2_LEN];//参数2
	BYTE  cPara2Len;
	BYTE cSequenceNO;//计数器
	TIMECALLBACKFUN pNotifyCallback;//回调函数
	
	CNewTriggerList* pOwner;
	
	TIME_TRIGGER_NEW* pPrevTrigger;
	TIME_TRIGGER_NEW* pNextTrigger;
} ;

class CNewTriggerList
{
	//因为MFC提供的CList类存在使用过多指针的缺点，并且增删操作涉及到new.delete内存，对于
	// 定时器这种大量使用的资源不太合适，故自己设计一个类实现之。
public:
	CNewTriggerList() { m_pHead = m_pTail = NULL; m_dwCount = 0; }
	TIME_TRIGGER_NEW* GetHead();
	TIME_TRIGGER_NEW* GetTail();
	TIME_TRIGGER_NEW* GetNext(TIME_TRIGGER_NEW* pCur);
	TIME_TRIGGER_NEW* GetPrev(TIME_TRIGGER_NEW* pCur);
	void AddTail(TIME_TRIGGER_NEW* pTrigger);
	void AddHead(TIME_TRIGGER_NEW* pTrigger);
	void InsertBefore(TIME_TRIGGER_NEW* pCur, TIME_TRIGGER_NEW* pTrigger);
	void InsertAfter(TIME_TRIGGER_NEW* pCur, TIME_TRIGGER_NEW* pTrigger);
	void Remove(TIME_TRIGGER_NEW* pTrigger);
	void RemoveAll();
	DWORD GetCount() { return m_dwCount; }
protected:
	TIME_TRIGGER_NEW* m_pHead;
	TIME_TRIGGER_NEW* m_pTail;
	DWORD m_dwCount;
};

class CNewTimerManager
{
public:
	CNewTimerManager(int nCount);
	~CNewTimerManager();
	
	DWORD SetTimer(DWORD dwSeconds, DWORD dwPara,DWORD dwPara1,
		BYTE* sPara2,DWORD nPara2Len ,TIMECALLBACKFUN pFun);
	void KillTimer(DWORD nTimerID);
	
protected:
	void TrigOnTime(CNewTriggerList* pList,time_t now);
	void AddWorkTrigger(TIME_TRIGGER_NEW* pTrigger);
	
protected:
	CRITICAL_SECTION m_lock;
	CNewTriggerList*  m_pworkTriggers;
	CNewTriggerList m_idleTriggers;
	TIME_TRIGGER_NEW* m_pBuf;
	int m_nCount;
	time_t m_lasttime;
	TIME_TRIGGER_NEW* m_pWorkingTimer;
	
public:	
	DWORD GetFreeCount();
	BOOL GetTimerParam(DWORD id,DWORD& param,DWORD& param1,BYTE* param2,DWORD &paran2len );
	void TrigTimer();
	
	//static DWORD WINAPI ThreadProc(LPVOID lpParam);
	
};

#endif
