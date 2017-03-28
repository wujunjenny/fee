// MultiThreadMng.h: interface for the CMultiThreadMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTITHREADMNG_H__D83C7C11_762A_4C5D_B8C7_BAC1560B7F03__INCLUDED_)
#define AFX_MULTITHREADMNG_H__D83C7C11_762A_4C5D_B8C7_BAC1560B7F03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "newmsgqueue.h"
/************************************************************************/
//使用方法：
//	必须重载 OnMessage			处理消息
//	必须重载 OnTime				处理每秒定时消息
//	必须重载 OnMountMessage()	处理维护事件（每秒会调用一次），如果返回FASLE，认为系统不能处理接收消息，系统将不再调用OnMessage,直到返回TURE才会恢复
//	使用方法顺序
//		1. init，初始化参数
//		2. start，启动
//	退出时
//		1.Destroy
//		2.delete
/************************************************************************/
class CNewMng
{
public:
	CNewMng();
	virtual ~CNewMng();
public:
	BOOL Init(LPCTSTR pQueName,DWORD maxbuffsize,DWORD maxMsgSize,DWORD maxMsgCount);
	virtual BOOL SendMessage(PTRMSG pMsg, DWORD nMsgLen ,DWORD npri = 0);
	// 消息进入函数, 不需要new !!
	
	virtual void Start();
	//构造之后处理线程不会自动启动，必须显示调用start
	//如果需要重载此函数，需要在重载函数中适当的位置调用基类的start
	virtual void Destroy();
	// 删除CManager类型的实例前，必须显式调用此函数
	//如果需要重载此函数，需要在重载函数中适当的位置调用基类的Destroy
protected:
	// 需要重载的函数
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
	// 消息处理基本函数, 实现的时候不需要删除消息包 !!
	//时钟触发
	virtual void OnTime() = NULL;
	//add by wujun 
	virtual BOOL OnMountMessage() = NULL;
protected:
	// 消息调度过程
	virtual void Run();
	
	
protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	DWORD m_nTimeTick;
	BOOL m_bMounted;
	//维护队列
	CCacheMsgQueue m_msgQueue; //业务队列
	//CMessageQueue m_msgMountQueue;
};

#endif // !defined(AFX_MULTITHREADMNG_H__D83C7C11_762A_4C5D_B8C7_BAC1560B7F03__INCLUDED_)
