// SmppDrvMng.h: interface for the CSmppDrvMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SmppDrvMng__h__)
#define __SmppDrvMng__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DRVMANAGER\SockMng.h"
#include "smppdef.h"
#include "SmServiceProcDlg.h"
#include "Common\SmppCodeAndDecode.h"

enum smpp_drv_state
{
	smpp_connecting,
		smpp_logining,
		smpp_active,
		smpp_broken
};

class CSmppDrvMng : public CSockMng  
{
public:
	VOID SendDeliverAck(smpp_Message_header* pMsg, char * mgid);
	CSmppDrvMng();
	virtual ~CSmppDrvMng();
	
	int GetState();
	void SetState(int state);
	
	BOOL Init(LPCTSTR pAddress,WORD port);
	BOOL Send(smpp_Message_header* pMsg);
	virtual void Destroy();	
	
protected:
	CSmppCodeAndDecode decode;
	int m_nState;
	HANDLE m_hIF;
	HANDLE m_hTempIF;
	virtual int OnRead( HANDLE hIFHandle,	//接收接口句柄
		LPVOID pBuff,		//接收缓冲区
		int size			//接收缓冲区长度
		);
	//连接创建的事件,对于服务型的接口当连接成功时产生此事件
	virtual int OnAccept( HANDLE hIFHandle ,		//接口句柄
						  LPCTSTR pRemoteAddress,	//对端地址
						  int AddrSize,				//对端地址缓冲区大小
						  DWORD dwCodeType			//编码协议，此为创建SERVER时设置的编码协议
						  );
	
	//连接成功事件,对于客户型的接口当连接成功时产生此事件
	virtual int OnConnect( 
		HANDLE hIFHandle //接口句柄
		);
	
	//发生错误的事件
	virtual int OnError( 
		HANDLE hIFHandle ,	//接口句柄
		int ErrorCode,		//错误原因
		BOOL bRecover		//重连标志
		);
	
	//处理TRACE消息
	virtual int OnTrace( 
		HANDLE hIFHandle ,	//接口句柄
		BOOL bIn ,			//消息方向
		int size,			//消息长度
		LPVOID pBuff		//消息体
		);
	
};

#endif // !defined(__SmppDrvMng__h__)
