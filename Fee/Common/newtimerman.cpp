#include "stdafx.h"
#include "newtimerman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIME_HASH	20000

DWORD MakeTimerIDNew(BYTE sequence, DWORD nIdx)
{
	DWORD nRet = sequence;
	nRet = nRet << 24;
	nRet += nIdx;
	return nRet;
} 

DWORD GetTimerIndexFromIDNew(DWORD timer)
{
	return timer & 0x00FFFFFF;
}

////////////////////////////////////////////////////////////////
//	class CNewTriggerList
//		TIME_TRIGGER_NEW* m_pHead;
//		TIME_TRIGGER_NEW* m_pTail;
TIME_TRIGGER_NEW* CNewTriggerList::GetHead()
{
	
	if( m_pHead )
	{
		ASSERT(m_pHead->pOwner == this);
	}
	
	return m_pHead;
}

TIME_TRIGGER_NEW* CNewTriggerList::GetTail()
{
	
	if( m_pTail )
	{
		ASSERT(m_pTail->pOwner == this);
	}
	
	return m_pTail;
}

TIME_TRIGGER_NEW* CNewTriggerList::GetNext(TIME_TRIGGER_NEW* pCur)
{
	if( pCur )
	{
		ASSERT(pCur->pOwner == this);
		
		if( pCur->pNextTrigger )
		{
			ASSERT(pCur->pNextTrigger->pOwner == this);
		}
		
		return pCur->pNextTrigger;
	}
	return NULL;
}

TIME_TRIGGER_NEW* CNewTriggerList::GetPrev(TIME_TRIGGER_NEW* pCur)
{
	if( pCur )
	{
		
		ASSERT(pCur->pOwner == this);
		if( pCur->pPrevTrigger )
		{
			ASSERT(pCur->pPrevTrigger->pOwner == this);
		}
		
		return pCur->pPrevTrigger;
	}
	return NULL;
}

void CNewTriggerList::AddTail(TIME_TRIGGER_NEW* pTrigger)
{
	ASSERT(pTrigger);
	
	pTrigger->pOwner = this;
	
	
	if( m_pTail )
	{
		m_pTail->pNextTrigger = pTrigger;
		pTrigger->pPrevTrigger = m_pTail;
		m_pTail = pTrigger;
		m_pTail->pNextTrigger = NULL;
	}
	else {
		ASSERT(m_pHead==NULL);
		m_pHead = m_pTail = pTrigger;
		pTrigger->pNextTrigger = NULL;
		pTrigger->pPrevTrigger = NULL;
	}
	
	m_dwCount++;
	
	/* 最低级调试用：特别影响速度：
	#ifdef _DEBUG
	TIME_TRIGGER_NEW* pCur = m_pHead;
	while( pCur )
	{
	ASSERT(pCur->pOwner == this);
	if( pCur->pNextTrigger )
	ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
	if( pCur->pPrevTrigger )
	ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
	pCur = pCur->pNextTrigger;
	}
	#endif
	*/
}

void CNewTriggerList::AddHead(TIME_TRIGGER_NEW* pTrigger)
{
	ASSERT(pTrigger);
	
	pTrigger->pOwner = this;
	
	if( m_pHead )
	{
		pTrigger->pNextTrigger = m_pHead;
		m_pHead->pPrevTrigger = pTrigger;
		m_pHead = pTrigger;
		m_pHead->pPrevTrigger = NULL;
	}
	else {
		ASSERT(m_pTail==NULL);
		m_pHead = m_pTail = pTrigger;
		pTrigger->pNextTrigger = NULL;
		pTrigger->pPrevTrigger = NULL;
	}
	
	m_dwCount++;
	
	/* 最低级调试用：特别影响速度：
	#ifdef _DEBUG
	TIME_TRIGGER_NEW* pCur = m_pHead;
	while( pCur )
	{
	ASSERT(pCur->pOwner == this);
	if( pCur->pNextTrigger )
	ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
	if( pCur->pPrevTrigger )
	ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
	pCur = pCur->pNextTrigger;
	}
	#endif
	*/
}

void CNewTriggerList::InsertBefore(TIME_TRIGGER_NEW* pCur, TIME_TRIGGER_NEW* pTrigger)
{
	ASSERT(pTrigger);
	ASSERT(pCur);
	ASSERT(m_pHead);
	ASSERT(m_pTail);
	
	pTrigger->pOwner = this;
	if( pCur->pOwner != this )
	{
		//		AfxMessageBox("Time Manager Error!");
		
	}
	
	if( pCur == m_pHead )
		m_pHead = pTrigger;
	pTrigger->pPrevTrigger = pCur->pPrevTrigger;
	pTrigger->pNextTrigger = pCur;
	
	if( pTrigger->pPrevTrigger )
		pTrigger->pPrevTrigger->pNextTrigger = pTrigger;
	pCur->pPrevTrigger = pTrigger;
	
	m_dwCount++;
	
	/* 最低级调试用：特别影响速度：
	#ifdef _DEBUG
	pCur = m_pHead;
	while( pCur )
	{
	ASSERT(pCur->pOwner == this);
	if( pCur->pNextTrigger )
	ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
	if( pCur->pPrevTrigger )
	ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
	pCur = pCur->pNextTrigger;
	}
	#endif
	*/
}

void CNewTriggerList::InsertAfter(TIME_TRIGGER_NEW* pCur, TIME_TRIGGER_NEW* pTrigger)
{
	ASSERT(pTrigger);
	ASSERT(pCur);
	ASSERT(m_pHead);
	ASSERT(m_pTail);
	
	pTrigger->pOwner = this;
	if( pCur->pOwner != this )
	{
		//		AfxMessageBox("Time Manager Error!");
		
	}
	
	if( pCur == m_pTail )
		m_pTail = pTrigger;
	pTrigger->pNextTrigger = pCur->pNextTrigger;
	if( pTrigger->pNextTrigger )
		pTrigger->pNextTrigger->pPrevTrigger = pTrigger;
	pTrigger->pPrevTrigger = pCur;
	pCur->pNextTrigger = pTrigger;
	
	m_dwCount++;
	
	/* 最低级调试用：特别影响速度：
	#ifdef _DEBUG
	pCur = m_pHead;
	while( pCur )
	{
	ASSERT(pCur->pOwner == this);
	if( pCur->pNextTrigger )
	ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
	if( pCur->pPrevTrigger )
	ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
	pCur = pCur->pNextTrigger;
	}
	#endif
	*/
}

void CNewTriggerList::Remove(TIME_TRIGGER_NEW* pTrigger)
{
	
	
	if( pTrigger->pOwner != this )
	{
		//        AfxMessageBox("Time Manager Error!");
	}
	
	
	ASSERT(m_pHead);
	ASSERT(m_pTail);
	
	if (pTrigger == m_pHead)
	{
		m_pHead = pTrigger->pNextTrigger;
	}
	else
	{
		pTrigger->pPrevTrigger->pNextTrigger = pTrigger->pNextTrigger;
	}
	if (pTrigger == m_pTail)
	{
		m_pTail = pTrigger->pPrevTrigger;
	}
	else
	{
		pTrigger->pNextTrigger->pPrevTrigger = pTrigger->pPrevTrigger;
	}
	
	pTrigger->pOwner = NULL;
	
	
	m_dwCount--;
	
	/* 最低级调试用：特别影响速度：
	#ifdef _DEBUG
	TIME_TRIGGER_NEW* pCur = m_pHead;
	while( pCur )
	{
	ASSERT(pCur->pOwner == this);
	if( pCur->pNextTrigger )
	ASSERT(pCur == pCur->pNextTrigger->pPrevTrigger);
	if( pCur->pPrevTrigger )
	ASSERT(pCur == pCur->pPrevTrigger->pNextTrigger);
	pCur = pCur->pNextTrigger;
	}
	#endif
	*/
}

/////////////////////////////////////////////////////////////////
//	class CTimerManager
//	typedef struct tagTIMERTRIGGER
//	{
//		DWORD time;
//		DWORD dwPara1;
//		DWORD dwPara2;
//		void (*m_pNotifyCallback)(WORD, DWORD);
//	} TIME_TRIGGER_NEW;
//	CRITICAL_SECTION m_lock;
//	CPtrArray m_workTriggers;
//	CPtrArray m_idleTriggers;
//	TIME_TRIGGER_NEW* m_pBuf;
//	int m_nCount;
CNewTimerManager::CNewTimerManager(int nCount)
{
	InitializeCriticalSection(&m_lock);
	m_nCount = nCount;
	m_pBuf = new TIME_TRIGGER_NEW[m_nCount];
	memset(m_pBuf, 0, sizeof(TIME_TRIGGER_NEW)*m_nCount);
	/* 当触发器个数特别多时，使用数组进行管理效率较低
	*/
	// 新的方法使用自己定义的链表
	for( int i=0; i<m_nCount; i++ )
	{
		m_pBuf[i].cSequenceNO = 1;
		m_idleTriggers.AddTail(&m_pBuf[i]);
	}
	
	m_pworkTriggers = new CNewTriggerList[TIME_HASH];
	m_pWorkingTimer = NULL;
	m_lasttime = time(NULL);
}

CNewTimerManager::~CNewTimerManager()
{
	delete []m_pBuf;
	DeleteCriticalSection(&m_lock);
	delete []m_pworkTriggers;
}

DWORD CNewTimerManager::SetTimer(DWORD dwSeconds, DWORD dwPara, DWORD dwPara1, 
								 BYTE* sPara2, DWORD nPara2Len, TIMECALLBACKFUN pFunc)
{
	// 为了避免时钟资源紧张时，发生误删除有效时钟的情况，使用序列号作为ID的一部分。
	DWORD nRet = 0xFFFFFFFF;
	
	EnterCriticalSection(&m_lock);
	TIME_TRIGGER_NEW* pTrigger = m_idleTriggers.GetHead();
	if( pTrigger )
	{
		m_idleTriggers.Remove(pTrigger);
		DWORD lValue = (DWORD)pTrigger - (DWORD)m_pBuf;
		nRet = lValue/sizeof(TIME_TRIGGER_NEW);
		pTrigger->time = time(NULL) + dwSeconds;
		pTrigger->dwPara = dwPara;
		pTrigger->dwPara1 = dwPara1;
		if(nPara2Len < MAX_PARAM2_LEN && nPara2Len >0 && sPara2!=NULL)
		{
			memset(pTrigger->sPara2,0,MAX_PARAM2_LEN);
			memcpy(pTrigger->sPara2,sPara2,nPara2Len);
			pTrigger->cPara2Len = (unsigned char)nPara2Len;			
		}
		//pTrigger->dwPara2 = dwID;
		pTrigger->pNotifyCallback = pFunc;
		AddWorkTrigger(pTrigger);
		nRet = MakeTimerIDNew(pTrigger->cSequenceNO, nRet);
		
	}
	else
	{
	}
	//END
	LeaveCriticalSection(&m_lock);
	return nRet;
}

void CNewTimerManager::AddWorkTrigger(TIME_TRIGGER_NEW* pTrigger)
{
	// m_pworkTriggers按照时间的秒数建立的hash表
	//按照时间找到指定的列表进行插入
	//插入顺序按照指定的时间先后
	
	CNewTriggerList* pworklist = NULL;
	
	pworklist = &m_pworkTriggers[pTrigger->time%TIME_HASH];
	
	TIME_TRIGGER_NEW* pCur = pworklist->GetTail();
	while( pCur )
	{
		if( pTrigger->time >= pCur->time )	
		{
			pworklist->InsertAfter(pCur, pTrigger);
			
			return;
		}
		pCur = pworklist -> GetPrev(pCur);
	}
	pworklist->AddHead(pTrigger);
	
}


void CNewTimerManager::KillTimer(DWORD timer)
{
	
	DWORD nIdx = GetTimerIndexFromIDNew(timer);
	ASSERT(nIdx<(DWORD)m_nCount);
	EnterCriticalSection(&m_lock);
	if( nIdx >= (DWORD)m_nCount )
	{
		LeaveCriticalSection(&m_lock);
		return;
	}
	TIME_TRIGGER_NEW* pTrigger = &m_pBuf[nIdx];
	if( MakeTimerIDNew(pTrigger->cSequenceNO, nIdx) != timer )
	{
		LeaveCriticalSection(&m_lock);
		return;
	}
    
	//如果当前定时器处于保护区,不进行处理
	if( pTrigger == m_pWorkingTimer )
	{
		LeaveCriticalSection(&m_lock);
		return;
	}
	
    CNewTriggerList* pworklist =&m_pworkTriggers[pTrigger->time%TIME_HASH];
	if( pTrigger->pOwner != pworklist )
	{
		//ASSERT(0);
		LeaveCriticalSection(&m_lock);
		return;
	}
    
	pTrigger->cSequenceNO++;
	
	//清空Triger;
	pTrigger->dwPara1 = 0;
	memset(pTrigger->sPara2,0,MAX_PARAM2_LEN);
	pTrigger->cPara2Len = 0;
	
	pworklist->Remove(pTrigger);
	m_idleTriggers.AddTail(pTrigger);
	LeaveCriticalSection(&m_lock);
	
}

void CNewTimerManager::TrigTimer()
{
	
	EnterCriticalSection(&m_lock);
	time_t now = time(NULL);
	
	int count = now - m_lasttime;
	for(int i = 0 ; i< min(count,TIME_HASH); i++)
	{
		CNewTriggerList* pworklist =&m_pworkTriggers[(m_lasttime+i)%TIME_HASH];
		TrigOnTime(pworklist,now);
	}
	m_lasttime = now;
	LeaveCriticalSection(&m_lock);
	
}

/*
DWORD WINAPI CTimerManager::ThreadProc(LPVOID lpParam)
{
CTimerManager* pTM = (CTimerManager*)lpParam;
CTypedPtrArray<CPtrArray, TIME_TRIGGER_NEW*> triggerArray;
CTypedPtrArray<CPtrArray, TIME_TRIGGER_NEW*> templeArray;

  BEGIN_POINT:
  triggerArray.SetSize(0, 256);
  DWORD timeCurrent = GetTickCount();
  EnterCriticalSection(&pTM->m_lock);
  BOOL bGoOn = TRUE;
  TIME_TRIGGER_NEW* pWT = pTM->m_workTriggers.GetTail();
  while( pWT && bGoOn )
  {
		TIME_TRIGGER_NEW* pPrev = pTM->m_workTriggers.GetPrev(pWT);
		DWORD nElapse = timeCurrent - pWT->time;
		if( nElapse < 0x7FFFFFFF )
		{
		// 当前时间晚于pWT的触发时间，需触发之：
		if( pWT->pNotifyCallback )
		{
		triggerArray.Add(pWT); 
		}
		//else if( pWT->dwPara1 )
		//	SetEvent(pWT->dwPara1);
		if( pWT->wTriggerTimes == 1 )
		{
		pTM->m_workTriggers.Remove(pWT);
		pTM->m_idleTriggers.AddTail(pWT);
		}
		else{
		if( pWT->wTriggerTimes )
		pWT->wTriggerTimes--;
		
		  pWT->time = timeCurrent + pWT->ticks;
		  pTM->m_workTriggers.Remove(pWT);
		  
			templeArray.Add(pWT);
			}
			}
			else {
			bGoOn = FALSE;
			}
			pWT = pPrev;
			}
			for( int j=0;j<templeArray.GetSize();j++)
			{
			pTM->AddWorkTrigger(templeArray[j]);
			}
			templeArray.RemoveAll();
			
			  LeaveCriticalSection(&pTM->m_lock);
			  
				for( int i=0; i<triggerArray.GetSize(); i++ )
				{
				triggerArray[i]->pNotifyCallback(triggerArray[i]->dwPara1,
				MakeTimerID(triggerArray[i]->cSequenceNO,
				((DWORD)triggerArray[i]-(DWORD)pTM->m_pBuf)/sizeof(TIME_TRIGGER_NEW)),
				triggerArray[i]->dwPara2);
				}
				if( pTM->m_bAlive )
				{
				Sleep(10);
				goto BEGIN_POINT;
				}
				return 0;
				}
*/

void CNewTimerManager::TrigOnTime(CNewTriggerList *pList, time_t now)
{
	TIME_TRIGGER_NEW* pCur = pList->GetHead();
	while( pCur )
	{
		TIME_TRIGGER_NEW* pOldCur = pCur;
		
		if( now >= (long)pOldCur->time )	
		{
			//保护pCur,确保回调函数不对pCur进行修改
			m_pWorkingTimer = pCur;
			
			pOldCur->pNotifyCallback(
				MakeTimerIDNew(pOldCur->cSequenceNO,((char*)pOldCur-(char*)m_pBuf)/sizeof(TIME_TRIGGER_NEW)),
				pOldCur->dwPara,
				pOldCur->dwPara1,
				pOldCur->sPara2,
				pOldCur->cPara2Len
				);
			
			//解除保护
			m_pWorkingTimer = NULL;
			
			pCur = pList->GetNext(pCur);
			pList->Remove(pOldCur);
			m_idleTriggers.AddTail(pOldCur);
			//清空Triger;
			pOldCur->dwPara1 = 0;
			memset(pOldCur->sPara2,0,MAX_PARAM2_LEN);
			pOldCur->cPara2Len = 0;
			if(pCur)
				ASSERT(pCur->pOwner==pList);
			
		}
		else
		{
			break;
		}
		
	}
	
}

BOOL CNewTimerManager::GetTimerParam(DWORD id,DWORD& param,DWORD& param1, BYTE *param2, DWORD &paran2len)
{
	DWORD nIdx = GetTimerIndexFromIDNew(id);
	ASSERT(nIdx<(DWORD)m_nCount);
	EnterCriticalSection(&m_lock);
	if( nIdx >= (DWORD)m_nCount )
	{
		LeaveCriticalSection(&m_lock);
		return FALSE;
	}
	TIME_TRIGGER_NEW* pTrigger = &m_pBuf[nIdx];
	if( MakeTimerIDNew(pTrigger->cSequenceNO, nIdx) != id )
	{
		LeaveCriticalSection(&m_lock);
		return FALSE;
	}
	
	param = pTrigger->dwPara;
	param1 = pTrigger->dwPara1;
	
	if(param2!=NULL && paran2len > pTrigger->cPara2Len && pTrigger->cPara2Len != 0)
	{
		memcpy(param2,pTrigger->sPara2,pTrigger->cPara2Len);
		paran2len = pTrigger->cPara2Len;
	}
	
	LeaveCriticalSection(&m_lock);
	return TRUE;
}

DWORD CNewTimerManager::GetFreeCount()
{
	return m_idleTriggers.GetCount();
}
