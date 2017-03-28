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
	// 如果返回FALSE，表示消息队列过载
	
	//获取当前消息数
	DWORD GetMsgCount();
	
	//获取当前有效数据大小
	DWORD GetDataSize();
	
	//获取当前空闲数据大小
	DWORD GetFreeSize();
	
	//清空所有数据
	void  ClearData();
	
	//批量导入消息
	//输入 pMsgs 消息缓存，nSize 缓存大小，nMsgCount 消息个数
	BOOL BatchPutMessage(PTRMSG pMsgs , DWORD nSize , DWORD nMsgCount);
	//批量倒出消息
	//输出 pMsgs 消息缓存接收，nSize 输入接收缓存大小，输出实际消息大小缓存大小，输出 nMsgCount 消息个数
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
	//初始化一级队列缓存区、最大消息个数、最大消息长度
	BOOL InitQueue(const char* pName, int nL1BuffSize,int nL1MaxMsgCount,int nL1MaxMsgLen);
	CCacheMsgQueue();
	~CCacheMsgQueue();
	//接收方等待并获取消息
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE , int* pnSize = NULL);
	
	//发送方发送消息，
	//nPri为优先级，0表示普通消息，1表示即时消息
	//普通消息会进行二级缓存,即时消息不会缓存
	BOOL PutInMessage( PTRMSG pMsg, DWORD nSize, DWORD nPri = 0 );
	
	void StartMountThread();
	void StopMountThread();
	
protected:
	BOOL CheckValidFile(FILERECORD* pRec);
	
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;//一级消息队列存在消息事件
	HANDLE m_hFreeEvent;//一级消息队列中普通队列空闲事件
	HANDLE m_hThread;
	BOOL   m_bExit;
	CNewMessageQueue m_QuickQueue;	//优先队列
	CNewMessageQueue m_NormalQueue;//普通队列
	CNewMessageQueue m_L2CacheQueue;//二级缓存队列
	
	DWORD m_dwMaxMsgCount;
	DWORD m_dwSuggestBufferSize;
	virtual void MountRun();
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	
	//文件队列
	CPtrList m_FileList;
	DWORD m_dwFileNameSeq;
	
	//文件名规则
	CString m_sPreName;
	
};





#endif
