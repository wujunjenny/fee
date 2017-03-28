// MultiThreadMng.h: interface for the CMultiThreadMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__MultiThreadMng__h__)
#define __MultiThreadMng__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "msgqueue.h"

class CMultiThreadMng
{
public:
	CMultiThreadMng();
	virtual ~CMultiThreadMng();
public:
	virtual BOOL SendMessage(PTRMSG pMsg, DWORD nMsgLen);
	// 消息进入函数, 不需要new !!
	
	virtual void Destroy();
	// 删除CManager类型的实例前，必须显式调用此函数
	
	static CMsgQueue m_msgQueue;
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
};

#endif // !defined(__MultiThreadMng__h__)
