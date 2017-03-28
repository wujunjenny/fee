#ifndef __BASE_MANAGER_H__
#define __BASE_MANAGER_H__

#include "ThreadMng.h"

#pragma pack(push)
#pragma pack(1)
class CTimerMng;

class CThreadAndTimer : public CThreadMng 
{
public:
    // nTimerCount: ���������Ա�ڲ������õ��Ķ�ʱ������;
	CThreadAndTimer(UINT nTimerCount = 2000);
	
	virtual ~CThreadAndTimer();
	
public:
    //MANAGER�˳�Ԥ������
	virtual void PreExit();
	// *** SMGW25-32,2004-07-05,wzy modify begin *** //
	virtual BOOL SendMessage(PTRMSG pMsg, DWORD nMsgLen);
	// *** SMGW25-32,2004-07-05,wzy modify end *** //
	// ��Ϣ���뺯��, ����Ҫnew !!
	
    // dwMilliSeconds: ʱ�Ӽ��ʱ�䣻
	// wTimes: ʱ����������, ���ֵΪ0��Ϊ���޴�;
	// dwPara: �û�����Ϣ;
	// ����ֵΪʱ�ӵ�IDentify, ÿһ��ʱ�Ӷ���һ��Ψһֵ
	DWORD SetTimer(DWORD dwMilliSeconds, WORD wTimes, DWORD dwPara);
	
    // nTimerID: ʱ��IDentify;
	void KillTimer(DWORD nTimerID);	
	
	virtual void Destroy();
	
    void SetServiceType(DWORD ulServiceType)
    {
        m_ulServiceType = ulServiceType;
    }
protected:
    DWORD m_ulServiceType;
	// ��Ҫ���صĺ���
	
	// ʱ�Ӵ��������Ѿ�������Ϣ���зַ�������Ա�Ĵ����߳��д���;
	virtual BOOL OnTimer(DWORD timeID, DWORD dwPara) = NULL;
	// timeID: ʱ��IDentify;
	// dwPara: �û�����Ϣ;
	
protected:
	// ��Ϣ���ȹ���, ������ʱ���¼�Դ:
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
