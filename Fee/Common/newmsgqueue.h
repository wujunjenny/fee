/********************************************************
*      newmsgqueue.h	...								*
********************************************************/

#ifndef __MSG_NEW_QUEUE_H
#define __MSG_NEW_QUEUE_H

typedef char* PTRMSG;

class CNewMessageQueue
{
public:
	CNewMessageQueue();
	~CNewMessageQueue();
	// operators:
	BOOL InitQueue(int maxbuffsize = 800000, int maxmsgsize = 10000);
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE, int* pRtSize = NULL);
	BOOL PutInMessage( PTRMSG pMsg, DWORD nSize );
	// �������FALSE����ʾ��Ϣ���й���
	
	//��ȡ��ǰ��Ϣ��
	DWORD GetMsgCount();
	
	//��ȡ��ǰ��Ч���ݴ�С
	DWORD GetDataSize();
	
	//��ȡ��ǰ�������ݴ�С
	DWORD GetFreeSize();
	
	//�����������
	void  ClearData();
	
	//����������Ϣ
	//���� pMsgs ��Ϣ���棬nSize �����С��nMsgCount ��Ϣ����
	BOOL BatchPutMessage(PTRMSG pMsgs , DWORD nSize , DWORD nMsgCount);
	//����������Ϣ
	//��� pMsgs ��Ϣ������գ�nSize ������ջ����С�����ʵ����Ϣ��С�����С����� nMsgCount ��Ϣ����
	BOOL BatchGetMessage(PTRMSG pMsgs , DWORD& nSize, DWORD& nMsgCount);
protected:
	// implementations:
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	DWORD	m_dwMsgCount;
	char*   m_pBuffer;
	char*   m_pTmpMsg;
	DWORD   m_dwTmpMsgSize;
	
	DWORD   m_dwQueueHead; 
	DWORD   m_dwQueueTail; 
	DWORD   m_dwQueueSize;
};

struct FILERECORD
{
	char Filename[2000];
	DWORD MessageCount;
	DWORD DataSize;
};


class CCacheMsgQueue
{
	
public:
	BOOL DumpNormalDataToFile();
	BOOL DumpL2CacheToFile();
	BOOL LoadFileCache();
	//��ʼ��һ�����л������������Ϣ�����������Ϣ����
	BOOL InitQueue(const char* pName, int nL1BuffSize,int nL1MaxMsgCount,int nL1MaxMsgLen);
	CCacheMsgQueue();
	~CCacheMsgQueue();
	//���շ��ȴ�����ȡ��Ϣ
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE , int* pnSize = NULL);
	
	//���ͷ�������Ϣ��
	//nPriΪ���ȼ���0��ʾ��ͨ��Ϣ��1��ʾ��ʱ��Ϣ
	//��ͨ��Ϣ����ж�������,��ʱ��Ϣ���Ỻ��
	BOOL PutInMessage( PTRMSG pMsg, DWORD nSize, DWORD nPri = 0 );
	
	void StartMountThread();
	void StopMountThread();
	
protected:
	BOOL CheckValidFile(FILERECORD* pRec);
	
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;//һ����Ϣ���д�����Ϣ�¼�
	HANDLE m_hFreeEvent;//һ����Ϣ��������ͨ���п����¼�
	HANDLE m_hThread;
	BOOL   m_bExit;
	CNewMessageQueue m_QuickQueue;	//���ȶ���
	CNewMessageQueue m_NormalQueue;//��ͨ����
	CNewMessageQueue m_L2CacheQueue;//�����������
	
	DWORD m_dwMaxMsgCount;
	DWORD m_dwSuggestBufferSize;
	virtual void MountRun();
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	
	//�ļ�����
	CPtrList m_FileList;
	DWORD m_dwFileNameSeq;
	
	//�ļ�������
	CString m_sPreName;
	
};





#endif
