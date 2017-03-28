#ifndef __BASE_MANAGER_H__
#define __BASE_MANAGER_H__

#include "ThreadMng.h"

#pragma pack(push)
#pragma pack(1)
class CTimerMng;

class CThreadAndTimer : public CThreadMng 
{
public:
    // nTimerCount: 本任务管理员内部所需用到的定时器个数;
	CThreadAndTimer(UINT nTimerCount = 2000);
	
	virtual ~CThreadAndTimer();
	
public:
    //MANAGER退出预处理函数
	virtual void PreExit();
	// *** SMGW25-32,2004-07-05,wzy modify begin *** //
	virtual BOOL SendMessage(PTRMSG pMsg, DWORD nMsgLen);
	// *** SMGW25-32,2004-07-05,wzy modify end *** //
	// 消息进入函数, 不需要new !!
	
    // dwMilliSeconds: 时钟间隔时间；
	// wTimes: 时钟启动次数, 如果值为0则为无限次;
	// dwPara: 用户绑定信息;
	// 返回值为时钟的IDentify, 每一个时钟都有一个唯一值
	DWORD SetTimer(DWORD dwMilliSeconds, WORD wTimes, DWORD dwPara);
	
    // nTimerID: 时钟IDentify;
	void KillTimer(DWORD nTimerID);	
	
	virtual void Destroy();
	
    void SetServiceType(DWORD ulServiceType)
    {
        m_ulServiceType = ulServiceType;
    }
protected:
    DWORD m_ulServiceType;
	// 需要重载的函数
	
	// 时钟处理函数，已经经过消息队列分发到管理员的处理线程中处理;
	virtual BOOL OnTimer(DWORD timeID, DWORD dwPara) = NULL;
	// timeID: 时钟IDentify;
	// dwPara: 用户绑定信息;
	
protected:
	// 消息调度过程, 增加了时钟事件源:
	virtual void Run();
	
	//private:
protected:
	static void OnTimmerTrigger(DWORD, DWORD, DWORD);
	CTimerMng * m_pTM;
	enum BASE_MESSAGE_TYPE
	{
		NORMAL_MESSAGE,
			TIMER_MESSAGE,
	};
	typedef struct tagINNER_TIMER_MSG
	{
		char msgType;
		DWORD para1;
		DWORD para2;
	}INNER_TIMER_MSG;
};
#pragma pack(pop)
#endif
