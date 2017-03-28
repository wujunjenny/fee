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
	// ��Ϣ���뺯��, ����Ҫnew !!
	
	virtual void Destroy();
	// ɾ��CManager���͵�ʵ��ǰ��������ʽ���ô˺���
	
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
	CMsgQueue m_msgQueue;
};

#endif
