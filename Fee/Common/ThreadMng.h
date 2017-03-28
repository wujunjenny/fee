#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "msgqueue.h"

class CThreadMng 
{
public:
	CThreadMng();
	virtual ~CThreadMng();
	
public:
	// *** SMGW25-32,2004-07-05,wzy modify begin *** //
	virtual BOOL SendMessage(PTRMSG pMsg, DWORD nMsgLen);
	// *** SMGW25-32,2004-07-05,wzy modify end *** //
	// 消息进入函数, 不需要new !!
	
	virtual void Destroy();
	// 删除CManager类型的实例前，必须显式调用此函数
	
protected:
	// 需要重载的函数
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
	// 消息处理基本函数, 实现的时候不需要删除消息包 !!
	
protected:
	// 消息调度过程
	virtual void Run();
	
protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	CMsgQueue m_msgQueue;
};

#endif
