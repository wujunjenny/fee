#ifndef __CommThreadMng__h__
#define __CommThreadMng__h__

#include "ThreadAndTimer.h"

class CCommThreadMng : public CThreadAndTimer
{
public:
	CCommThreadMng(UINT nTimerCount = 2000);
	~CCommThreadMng();
	
public:
	void SendMessageToQueue(PTRMSG pMsg); // ��Ϣ���뺯��, ����Ҫnew !!
	
};

#endif
