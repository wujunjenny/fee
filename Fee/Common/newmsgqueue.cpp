#include "stdafx.h"
#include "newmsgqueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//new message queue

CNewMessageQueue::CNewMessageQueue()
{
	InitializeCriticalSection(&m_lock);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pBuffer = NULL;
	
	m_dwQueueHead = 0; 
	m_dwQueueTail = 0; 
	m_dwQueueSize = NULL;
	
	m_pTmpMsg = NULL;
	m_dwTmpMsgSize = NULL;
	m_dwMsgCount = 0;
}


CNewMessageQueue::~CNewMessageQueue()
{
	DeleteCriticalSection(&m_lock); 
	CloseHandle(m_hEvent);
	if(m_pBuffer)
		delete [] m_pBuffer;
	if(m_pTmpMsg)
		delete [] m_pTmpMsg;
}


BOOL CNewMessageQueue::InitQueue(int maxbuffsize , int maxmsgsize)
{
	m_pBuffer = new char[maxbuffsize+20];
	m_dwQueueSize =  maxbuffsize;
	m_pTmpMsg = new char[maxmsgsize];
	m_dwTmpMsgSize = maxmsgsize;
	return TRUE;
}

PTRMSG CNewMessageQueue::WaitForNormalMessage(int dwMilliseconds , int* pRtSize)
{
	DWORD dwWaitResult;
	
	if( GetDataSize() <= 0 )
	{
		
		dwWaitResult = WaitForSingleObject(m_hEvent, dwMilliseconds);	
		if (WAIT_TIMEOUT == dwWaitResult || WAIT_FAILED == dwWaitResult)
		{//��ʱ���¼�
			//�ݴ���
			if(m_dwMsgCount != 0)
				m_dwMsgCount  = 0;
			
			return NULL;
		}
	}
	
	EnterCriticalSection(&m_lock);
	
	if (m_dwQueueHead == m_dwQueueTail)
	{//��Ϣ����Ϊ��
		LeaveCriticalSection(&m_lock);
		return NULL;
	}
	
	DWORD dwMsgLen;
	DWORD dwTailToTurn = m_dwQueueSize - m_dwQueueTail;
	
	if (m_dwQueueTail < m_dwQueueHead)
	{
		dwMsgLen = *(DWORD *)&m_pBuffer[m_dwQueueTail];
		m_dwQueueTail += sizeof(DWORD);
		//���dwMsgLen�ĺϷ���
		if(dwMsgLen > GetDataSize()||dwMsgLen > m_dwTmpMsgSize)
		{
			//��Ϣ�����Ѿ����ƻ�������ʧ�ܲ���ʼ��
			m_dwQueueTail=0;
			m_dwQueueHead=0;
			m_dwMsgCount=0;
			LeaveCriticalSection(&m_lock);
			//����澯��Ϣ
			ASSERT(0);
			return NULL;
		}
		memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
		m_dwQueueTail += dwMsgLen;
		
		if(pRtSize)
			*pRtSize = dwMsgLen;
	}
	else if (dwTailToTurn >= sizeof(DWORD))
	{
		dwMsgLen = *(DWORD *)&m_pBuffer[m_dwQueueTail];
		//���dwMsgLen�ĺϷ���
		if(dwMsgLen > GetDataSize()||dwMsgLen > m_dwTmpMsgSize)
		{
			//��Ϣ�����Ѿ����ƻ�������ʧ�ܲ���ʼ��
			m_dwQueueTail=0;
			m_dwQueueHead=0;
			m_dwMsgCount=0;
			LeaveCriticalSection(&m_lock);
			//����澯��Ϣ
			ASSERT(0);
			return NULL;
		}
		
		m_dwQueueTail = (m_dwQueueTail + sizeof(DWORD)) % m_dwQueueSize;
		dwTailToTurn -= sizeof(DWORD);
		
		if(pRtSize)
			*pRtSize = dwMsgLen;
		if (dwTailToTurn >= dwMsgLen)
		{
			memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
			m_dwQueueTail = (m_dwQueueTail + dwMsgLen) % m_dwQueueSize;
		}
		else
		{
			if (dwTailToTurn)
				memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwTailToTurn);
			memcpy((char *)m_pTmpMsg + dwTailToTurn, m_pBuffer, dwMsgLen - dwTailToTurn); 
			m_dwQueueTail = dwMsgLen - dwTailToTurn;
		}
	}
	else
	{
		memcpy(&dwMsgLen, &m_pBuffer[m_dwQueueTail], dwTailToTurn);
		memcpy((char *)&dwMsgLen + dwTailToTurn, m_pBuffer, sizeof(DWORD) - dwTailToTurn);
		
		//���dwMsgLen�ĺϷ���
		if(dwMsgLen > GetDataSize()||dwMsgLen > m_dwTmpMsgSize)
		{
			//��Ϣ�����Ѿ����ƻ�������ʧ�ܲ���ʼ��
			m_dwQueueTail=0;
			m_dwQueueHead=0;
			m_dwMsgCount=0;
			LeaveCriticalSection(&m_lock);
			//����澯��Ϣ
			ASSERT(0);
			return NULL;
		}
		m_dwQueueTail = sizeof(DWORD) - dwTailToTurn;
		memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
		if(pRtSize)
			*pRtSize = dwMsgLen;
		m_dwQueueTail += dwMsgLen;
	}
	
	m_dwMsgCount--;
	
	LeaveCriticalSection(&m_lock);
	
	if (m_dwQueueTail != m_dwQueueHead)
		SetEvent(m_hEvent);
	else
		ResetEvent(m_hEvent);
	return m_pTmpMsg;
}


BOOL CNewMessageQueue::PutInMessage(PTRMSG pMsg ,DWORD nSize)
{
	if(pMsg == NULL || nSize == 0 || nSize >= m_dwQueueSize || nSize >= m_dwTmpMsgSize )
		return FALSE;
	DWORD dwSaveBagLen = (sizeof(DWORD) + nSize);
	EnterCriticalSection(&m_lock);
	DWORD dwFreeSize = (m_dwQueueSize - 1 + m_dwQueueTail - m_dwQueueHead) % m_dwQueueSize;
	if (dwFreeSize < dwSaveBagLen)
	{
		LeaveCriticalSection(&m_lock);
		return FALSE;
	}
	DWORD dwHeadToTurn = m_dwQueueSize - m_dwQueueHead;
	
	if ((dwHeadToTurn >= dwSaveBagLen) || (m_dwQueueTail > m_dwQueueHead))
	{
		*(DWORD *)&m_pBuffer[m_dwQueueHead] = nSize;
		m_dwQueueHead += sizeof(DWORD);
		
		memcpy(&m_pBuffer[m_dwQueueHead], pMsg, nSize);
		m_dwQueueHead = (m_dwQueueHead + nSize) % m_dwQueueSize;
	}
	else if (dwHeadToTurn >= sizeof(DWORD))
	{
		*(DWORD *)&m_pBuffer[m_dwQueueHead] = nSize;
		m_dwQueueHead = (m_dwQueueHead + sizeof(DWORD)) % m_dwQueueSize;
		dwHeadToTurn -=  sizeof(DWORD);
		if ( dwHeadToTurn )
		{
			memcpy(&m_pBuffer[m_dwQueueHead], pMsg, dwHeadToTurn);
			pMsg = (char *)pMsg + dwHeadToTurn;
			nSize -= dwHeadToTurn;
		}
		
		memcpy(m_pBuffer, pMsg, nSize);
		m_dwQueueHead = nSize;
	}
	else
	{
		/*set dwSize while write head point turns*/
		memcpy(&m_pBuffer[m_dwQueueHead], &nSize, dwHeadToTurn);
		m_dwQueueHead = sizeof(DWORD) - dwHeadToTurn;
		memcpy(m_pBuffer, (char *)&nSize + dwHeadToTurn, m_dwQueueHead);
		memcpy(&m_pBuffer[m_dwQueueHead], pMsg, nSize);
		m_dwQueueHead += nSize;
	}
	
	m_dwMsgCount ++ ;
	
	LeaveCriticalSection(&m_lock);
	SetEvent(m_hEvent);
	return TRUE;
}



DWORD CNewMessageQueue::GetMsgCount()
{
	return m_dwMsgCount;
}


DWORD CNewMessageQueue::GetDataSize()
{
	DWORD size = 0;
	size = (m_dwQueueSize + m_dwQueueHead - m_dwQueueTail) % m_dwQueueSize;
	return size;
}

DWORD CNewMessageQueue::GetFreeSize()
{
	DWORD size = 0;
	size = (m_dwQueueSize - 1 + m_dwQueueTail - m_dwQueueHead) % m_dwQueueSize;
	return size;
}

void  CNewMessageQueue::ClearData()
{
	m_dwMsgCount = 0;
	m_dwQueueHead = 0;
	m_dwQueueTail = 0;
}

BOOL CNewMessageQueue::BatchPutMessage(PTRMSG pMsgs , DWORD nSize , DWORD nMsgCount)
{
	//�ú������ڲ�ʹ�ú��������뱣֤���������׼ȷ�ԣ���
	if (nSize > GetFreeSize())
		return FALSE;
	
	EnterCriticalSection(&m_lock);
	
	DWORD dwHeadToTurn = m_dwQueueSize - m_dwQueueHead;
	
	if ((dwHeadToTurn >= nSize) || (m_dwQueueTail > m_dwQueueHead))
	{
		
		memcpy(&m_pBuffer[m_dwQueueHead], pMsgs, nSize);
		m_dwQueueHead = (m_dwQueueHead + nSize) % m_dwQueueSize;
	}
	else 
	{
		
		memcpy(&m_pBuffer[m_dwQueueHead], pMsgs, dwHeadToTurn);
		pMsgs = (char *)pMsgs + dwHeadToTurn;
		nSize -= dwHeadToTurn;
		memcpy(m_pBuffer, pMsgs, nSize);
		m_dwQueueHead = nSize;
	}
	
	m_dwMsgCount = m_dwMsgCount + nMsgCount;
	LeaveCriticalSection(&m_lock);
	
	return TRUE;
}


BOOL CNewMessageQueue::BatchGetMessage(PTRMSG pMsgs , DWORD& nSize, DWORD& nMsgCount)
{
	
	//�ú������ڲ�ʹ�ú��������뱣֤���������׼ȷ�ԣ���
	if (nSize < GetDataSize())
		return FALSE;
	EnterCriticalSection(&m_lock);
	
	if (m_dwQueueHead > m_dwQueueTail )
	{
		memcpy(pMsgs,&m_pBuffer[m_dwQueueTail],  GetDataSize());
		nSize = GetDataSize();
		nMsgCount = m_dwMsgCount;
	}
	else if (m_dwQueueHead == m_dwQueueTail)
	{
		nSize = 0;
		nMsgCount = 0;
	}
	else
	{
		DWORD dwHeadToTurn = m_dwQueueSize - m_dwQueueTail;
		nSize = GetDataSize();
		nMsgCount = m_dwMsgCount;
		memcpy(pMsgs,&m_pBuffer[m_dwQueueTail],  dwHeadToTurn);
		memcpy(&pMsgs[dwHeadToTurn],m_pBuffer,nSize - dwHeadToTurn );
		
	}
	LeaveCriticalSection(&m_lock);
	
	return TRUE;
}



//�ļ�������������,"·��+��������+����ʱ����+���.cache"
//�ļ���ʽ
//���	10�ֽ�
//��Ϣ��	4�ֽ�
//���ݳ���  4�ֽ�
//����		n�ֽ�

CCacheMsgQueue::CCacheMsgQueue()
{
	
	InitializeCriticalSection(&m_lock);
	//��ʼ�����ź�
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//��ʼ�����ź�
	m_hFreeEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	m_hThread = NULL;
	
	m_dwMaxMsgCount = 0;
	m_dwSuggestBufferSize = 0;
	m_bExit = FALSE;
	m_dwFileNameSeq = 0;
}

CCacheMsgQueue::~CCacheMsgQueue()
{
	//clear filelist
	POSITION pos;
	pos = m_FileList.GetHeadPosition();
	while(pos!=NULL)
	{
		FILERECORD* pfile = (FILERECORD*)m_FileList.GetNext(pos);
		if(pfile)
			delete pfile;
		
	}
	m_FileList.RemoveAll();
	
	DeleteCriticalSection(&m_lock); 
	CloseHandle(m_hEvent);
	CloseHandle(m_hFreeEvent);
	CloseHandle(m_hThread);
}


DWORD WINAPI CCacheMsgQueue::ThreadProc(LPVOID lpParam)
{
	CCacheMsgQueue* pVM = (CCacheMsgQueue*)lpParam;
	ASSERT(pVM);
	pVM->MountRun();
	return 0L;
}


PTRMSG CCacheMsgQueue::WaitForNormalMessage(int dwMilliseconds , int * pnSize )
{
	PTRMSG pmsg = NULL;
	
	//�������Ϣ�����򲻱صȴ���Ϣ�¼�
	if(m_QuickQueue.GetDataSize()>0||m_NormalQueue.GetDataSize()>0)
	{
		EnterCriticalSection(&m_lock);
		if(m_QuickQueue.GetDataSize())
		{
			pmsg = m_QuickQueue.WaitForNormalMessage(dwMilliseconds,pnSize);
		}
		else if( m_NormalQueue.GetDataSize())
		{
			pmsg = m_NormalQueue.WaitForNormalMessage(dwMilliseconds,pnSize);
			SetEvent(m_hFreeEvent);
		}
		LeaveCriticalSection(&m_lock);
		
		return pmsg;
	}
	
	//���û�����ݵȵ�ָ����ʱ��
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(m_hEvent, dwMilliseconds);	
	if (WAIT_TIMEOUT == dwWaitResult || WAIT_FAILED == dwWaitResult)
	{//��ʱ���¼�
		SetEvent(m_hFreeEvent);
		return NULL;
	}
	
	//�ȵ���Ϣ���ȡ����
	EnterCriticalSection(&m_lock);
	if(m_QuickQueue.GetDataSize())
	{
		pmsg = m_QuickQueue.WaitForNormalMessage(dwMilliseconds,pnSize);
	}
	else if( m_NormalQueue.GetDataSize())
	{
		pmsg = m_NormalQueue.WaitForNormalMessage(dwMilliseconds,pnSize);
		SetEvent(m_hFreeEvent);
	}
	LeaveCriticalSection(&m_lock);
	
	return pmsg;
	
}

BOOL CCacheMsgQueue::PutInMessage( PTRMSG pMsg, DWORD nSize , DWORD nPri )
{
	
	BOOL rt = FALSE;
	EnterCriticalSection(&m_lock);
	
	SetEvent(m_hEvent);
	if( nPri > 0 )//��ʱ��Ϣ
	{
		rt = m_QuickQueue.PutInMessage(pMsg,nSize);
	}
	else
	{
		//����������������ݣ����浽��������
		//���һ�����й��أ����浽��������
		if(m_L2CacheQueue.GetDataSize() > 0 || m_NormalQueue.GetDataSize() > m_dwSuggestBufferSize || m_NormalQueue.GetMsgCount() > m_dwMaxMsgCount )
		{
			if(m_L2CacheQueue.GetFreeSize()<nSize || m_L2CacheQueue.GetMsgCount()+1 > m_dwMaxMsgCount )
			{
				DumpL2CacheToFile();
			}
			rt = m_L2CacheQueue.PutInMessage(pMsg,nSize);
		}
		else
		{//ֱ�ӱ��浽һ������
			rt = m_NormalQueue.PutInMessage(pMsg,nSize);
		}
	}
	
	LeaveCriticalSection(&m_lock);
	
	return rt;
}


void CCacheMsgQueue::StartMountThread()
{
	DWORD dwThreadID;
	m_hThread = CreateThread(
		NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
	m_bExit =  FALSE;
	
}

void CCacheMsgQueue::StopMountThread()
{
	
	m_bExit =  TRUE;
	SetEvent(m_hFreeEvent);
	if( WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0 )
	{
		TerminateThread(m_hThread, 4);
	}
	CloseHandle(m_hThread);
	m_hThread = NULL;
	
}

void CCacheMsgQueue::MountRun()
{
	while(1)
	{
		WaitForSingleObject(m_hFreeEvent, 100) ;
		if(m_bExit)
			return ;
		
		EnterCriticalSection(&m_lock);
		
		if( m_NormalQueue.GetDataSize() < m_dwSuggestBufferSize && m_NormalQueue.GetMsgCount() < m_dwMaxMsgCount  )
			//���һ�����п��жɴﵽһ�����
		{
			if(m_FileList.GetCount() > 0 )
			{//��ȡ�Ŷ��ļ�
				
				FILERECORD *pRec = (FILERECORD *)m_FileList.GetHead();
				if(pRec->DataSize > m_NormalQueue.GetFreeSize())
				{
					//����ռ䲻�������ȴ�
				}
				else
				{
					//�����ļ�
					
					BOOL rt = FALSE;
					if(!CheckValidFile(pRec))
					{
						//����ļ���ʶ����ֹ�Ƿ��ļ�
						//������ݳ��ȣ���ֹ�ļ����ƻ�
						//�����Ϣ��
						DeleteFile(pRec->Filename);
					}
					else
					{
						
						CFile f;
						rt = f.Open(pRec->Filename,CFile::modeRead);
						
						if(rt == FALSE)
						{//�ļ���ʧ�ܣ�������û����ļ�����¼��־
							DeleteFile(pRec->Filename);
						}
						else
						{
							//�����ļ�
							char* pTempBuff = NULL;
							DWORD nBuffSize = NULL;
							pTempBuff = new char[f.GetLength()+100];
							
							f.Read(pTempBuff,f.GetLength());
							
							m_NormalQueue.BatchPutMessage(pTempBuff+18,pRec->DataSize,pRec->MessageCount);
							delete [] pTempBuff;
							f.Close();
							
							//ɾ���ļ�
							DeleteFile(pRec->Filename);
						}
					}
					delete pRec;
					m_FileList.RemoveHead();
				}
			}
			else
			{//û���ļ�
				if(m_L2CacheQueue.GetDataSize()>0)
				{//�ж�������
					char* pTmpBuff = NULL;
					DWORD TmpBuffSize = 0;
					DWORD MsgCount = 0;
					TmpBuffSize = (m_L2CacheQueue.GetDataSize() / 1024 +1 ) * 1024;
					pTmpBuff = new char[TmpBuffSize];
					
					//��L2���浼��
					m_L2CacheQueue.BatchGetMessage(pTmpBuff,TmpBuffSize,MsgCount);
					m_NormalQueue.BatchPutMessage(pTmpBuff,TmpBuffSize,MsgCount);
					//���L2����
					m_L2CacheQueue.ClearData();
					delete [] pTmpBuff ;
				}
			}
			
			
		}
		LeaveCriticalSection(&m_lock);
		
	}
	
}


BOOL CCacheMsgQueue::InitQueue(const char* pName,int nL1BuffSize,int nL1MaxMsgCount,int nL1MaxMsgLen)
{
	m_QuickQueue.InitQueue(nL1BuffSize,nL1MaxMsgLen);
	m_NormalQueue.InitQueue(nL1BuffSize*2,nL1MaxMsgLen);
	m_L2CacheQueue.InitQueue(nL1BuffSize,nL1MaxMsgLen);
	m_dwMaxMsgCount = nL1MaxMsgCount;
	m_dwSuggestBufferSize = nL1BuffSize;
	if(pName)
		m_sPreName = pName;
	return TRUE;
}

BOOL CCacheMsgQueue::LoadFileCache()
{
	WIN32_FIND_DATA finddata;
	
	char findstr[500];
	sprintf(findstr,"%s*.cache",(LPCTSTR)m_sPreName);
	HANDLE hFiles = FindFirstFile(findstr,&finddata);
	while( hFiles != INVALID_HANDLE_VALUE)
	{
		FILERECORD* pRec = new FILERECORD;
		strcpy(pRec->Filename,finddata.cFileName);
		if(CheckValidFile(pRec))
		{
			//���ļ�����С�������
			POSITION pos;
			
			pos = m_FileList.GetTailPosition();
			if(pos == NULL)
				m_FileList.AddTail(pRec);
			while(pos!=NULL)
			{
				POSITION oldpos = pos ;
				FILERECORD *pTmp = (FILERECORD* )m_FileList.GetPrev(pos);
				int rt = strcmp(pTmp->Filename,pRec->Filename);
				if(rt < 0)
				{
					m_FileList.InsertAfter(oldpos,pRec);
					break;
				}
				else
					if(pos == NULL)
					{
						m_FileList.InsertBefore(oldpos,pRec);
					}
					
					
			}
			
		}
		else
		{
			delete pRec;
		}
		
		if(FindNextFile(hFiles,&finddata)==0)
		{
			//�Ҳ����ļ���
			break;
		}
	}
	FindClose(hFiles);
	
	
	return TRUE;
}

BOOL CCacheMsgQueue::CheckValidFile(FILERECORD *pRec)
{
	BOOL bValid = FALSE;
	int len = 0,rt = 0;
	char tmp[30];
	CFile f;
	if(!f.Open(pRec->Filename,CFile::modeRead))
		return FALSE;
	
	len = f.GetLength();
	rt = f.Read(tmp,20);
	if(rt < 18 )
		bValid = FALSE;
	else
	{
		if (strncmp(tmp,"share v1.0",10 ))
		{//��ʶ����ȷ
			bValid = FALSE;
		}
		else
		{
			pRec->MessageCount = * (DWORD*)&tmp[10];
			pRec->DataSize = * (DWORD*)&tmp[14];
			
			if (len - 18  < (int)pRec->DataSize)
			{
				//�ļ����Ȳ���ȷ
				bValid = FALSE;
			}
			else
				bValid = TRUE;
		}
		
	}
	f.Close();
	return bValid;
}

BOOL CCacheMsgQueue::DumpL2CacheToFile()
{
	CFile f;
	char filename[1000];
	time_t t;
	t = time(NULL);
	struct tm* ptm = localtime(&t);
	sprintf(filename,"%s%04d%02d%02d%02d%02d%02d%05d.cache",
		LPCTSTR(m_sPreName),
		ptm->tm_year+1900,
		ptm->tm_mon+1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		m_dwFileNameSeq);
	
	m_dwFileNameSeq++;
	if(m_dwFileNameSeq > 5000)
		m_dwFileNameSeq = 0;
	
	BOOL rt;
	rt = f.Open(filename,CFile::modeWrite|CFile::modeCreate);
	if(!rt)
	{
		return FALSE;
	}
	//���浽�ļ�
	
	//�ļ���ʽ
	//���	10�ֽ�	share v1.0
	//��Ϣ��	4�ֽ�
	//���ݳ���  4�ֽ�
	//����		n�ֽ�
	
	char* pTmpBuff = NULL;
	DWORD TmpBuffSize = 0;
	DWORD MsgCount = 0;
	TmpBuffSize = (m_L2CacheQueue.GetDataSize() / 1024 +1 ) * 1024;
	pTmpBuff = new char[TmpBuffSize];
	//��L2���浼��
	m_L2CacheQueue.BatchGetMessage(pTmpBuff,TmpBuffSize,MsgCount);
	
	f.Write("share v1.0",10);//���	10�ֽ�
	f.Write(&MsgCount,sizeof(MsgCount));//��Ϣ��	4�ֽ�
	f.Write(&TmpBuffSize,sizeof(TmpBuffSize));//���ݳ��� 4�ֽ�
	f.Write(pTmpBuff,TmpBuffSize);
	
	delete [] pTmpBuff ;
	
	
	f.Close();
	//��¼���ļ��б�
	FILERECORD* pRec = new FILERECORD;
	memset(pRec,0,sizeof(FILERECORD));
	strcpy(pRec->Filename,filename);
	pRec->DataSize = m_L2CacheQueue.GetDataSize();
	pRec->MessageCount = m_L2CacheQueue.GetMsgCount();
	m_FileList.AddTail(pRec);
	//�����������
	m_L2CacheQueue.ClearData();
	
	return TRUE;
}

BOOL CCacheMsgQueue::DumpNormalDataToFile()
{
	CFile f;
	char filename[1000];
	time_t t;
	t = time(NULL);
	struct tm* ptm = localtime(&t);
	sprintf(filename,"%s%04d%02d%02d%02d%02d%02d%05d.cache",
		LPCTSTR(m_sPreName),
		ptm->tm_year+1900-1,
		ptm->tm_mon+1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		0);
	//Ϊ��ȷ���Ŷ�������ǰ���Խ���ݼ�1
	
	if(m_dwFileNameSeq > 5000)
		m_dwFileNameSeq = 0;
	
	BOOL rt;
	rt = f.Open(filename,CFile::modeWrite|CFile::modeCreate);
	if(!rt)
	{
		return FALSE;
	}
	//���浽�ļ�
	
	//�ļ���ʽ
	//���	10�ֽ�	share v1.0
	//��Ϣ��	4�ֽ�
	//���ݳ���  4�ֽ�
	//����		n�ֽ�
	
	char* pTmpBuff = NULL;
	DWORD TmpBuffSize = 0;
	DWORD MsgCount = 0;
	TmpBuffSize = (m_NormalQueue.GetDataSize() / 1024 +1 ) * 1024;
	pTmpBuff = new char[TmpBuffSize];
	//��һ�����浼��
	m_NormalQueue.BatchGetMessage(pTmpBuff,TmpBuffSize,MsgCount);
	
	f.Write("share v1.0",10);//���	10�ֽ�
	f.Write(&MsgCount,sizeof(MsgCount));//��Ϣ��	4�ֽ�
	f.Write(&TmpBuffSize,sizeof(TmpBuffSize));//���ݳ��� 4�ֽ�
	f.Write(pTmpBuff,TmpBuffSize);
	
	delete [] pTmpBuff ;
	
	
	f.Close();
	//��¼���ļ��б�
	FILERECORD* pRec = new FILERECORD;
	memset(pRec,0,sizeof(FILERECORD));
	strcpy(pRec->Filename,filename);
	pRec->DataSize = m_NormalQueue.GetDataSize();
	pRec->MessageCount = m_NormalQueue.GetMsgCount();
	m_FileList.AddHead(pRec);
	//�����������
	m_NormalQueue.ClearData();
	return TRUE;
}
