#ifndef __CommThreadMng__h__
#define __CommThreadMng__h__

#include "ThreadAndTimer.h"

class CCommThreadMng : public CThreadAndTimer
{
public:
	CCommThreadMng(UINT nTimerCount = 2000);
	~CCommThreadMng();
	
public:
	void SendMessageToQueue(PTRMSG pMsg); // 消息进入函数, 不需要new !!
	
};

#endif
