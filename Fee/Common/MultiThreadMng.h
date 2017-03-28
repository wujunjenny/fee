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
	// ��Ϣ���뺯��, ����Ҫnew !!
	
	virtual void Destroy();
	// ɾ��CManager���͵�ʵ��ǰ��������ʽ���ô˺���
	
	static CMsgQueue m_msgQueue;
protected:
	// ��Ҫ���صĺ���
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
	// ��Ϣ�����������, ʵ�ֵ�ʱ����Ҫɾ����Ϣ�� !!
	
protected:
	// ��Ϣ���ȹ���
	virtual void Run();
	
protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
};

#endif // !defined(__MultiThreadMng__h__)
