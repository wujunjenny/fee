// SockConnect.h: interface for the CSockConnect class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__SockConnect__h__)
#define __SockConnect__h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"
#include "SockMsgQueue.h"

#ifndef __public_drvmanager__
#include "smcmacro.h"
#else
#define CODE_NO_LEN     0
#define CODE_ERROR      -1
#endif

typedef int (*LPONREAD)(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
typedef int (*LPONERROR)(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
typedef int (*LPONCONNECT)(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);

#define MAX_TEMP_BUFF	10000
#define STATUS_ACTIVE			0		//活动态
#define STATUS_BROKEN			1		//断连
#define STATUS_ABORT			2		//废弃


class CSockConnect : public CThread  
{
public:
	//生成客户端DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT句柄
		LPCTSTR pAddr,				//地址字符串指针
		WORD wPort,					//对端端口号
		BOOL bAuto,					//是否自动连接
		DWORD dwTime,				//重连时间
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser				//回调注册句柄
		);
	
	//生成服务器端DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT句柄
		HANDLE hFile,				//SOCKET句柄
		LPONREAD pOnRead,			//读回调函数指针
		LPONERROR pOnError,			//出错回调函数指针
		LPONCONNECT pOnConnect,		//连接回调函数指针
		HANDLE hUser				//回调注册句柄
		);
	
	virtual ~CSockConnect();
	
	int Send(
		LPVOID pBuff,				//发送缓冲区指针
		int size					//发送数据大小
		);
	
	int GetStatus()
	{return m_iState;};
	
protected:
	HANDLE m_hDrvObject;			//DRVOBJECT句柄
	
	BOOL m_bServer;					//是服务器端接口
	
	char m_RemoteAddr[40];			//远端的IP地址
	WORD m_wPort;					//远端PORT号
	
	//是否自动重连
	BOOL m_bAuto;
	DWORD m_dwTime;					//重连时间
	
	int m_iState;					//DRV状态
	
	CRITICAL_SECTION m_Critical;
	LPONREAD m_pOnReadFun;
	LPONERROR m_pOnErrorFun;
	LPONCONNECT m_pOnConnectFun;
	HANDLE m_hUser;
	HANDLE m_hFile;					//SOCKET句柄
	CSockMsgQueue m_RcvPool;				//接收缓冲区
	CSockMsgQueue m_SndPool;				//发送缓冲区
	
	char m_TempBuff[MAX_TEMP_BUFF];
	BOOL IsActive()
	{return m_iState == STATUS_ACTIVE;};
	BOOL Connect(LPCTSTR pAddr, unsigned short wPort, DWORD timeout);
	virtual int OnRead(LPVOID pData,int size);
	virtual int OnError(int ErrorCode);
	virtual int OnConnect();
	virtual int Run(DWORD param);
	int DrvRun();
	
private:
	int CloseSocketSafe();
};

#endif // !defined(__SockConnect__h__)
