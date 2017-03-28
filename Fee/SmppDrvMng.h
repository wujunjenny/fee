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
	virtual int OnRead( HANDLE hIFHandle,	//���սӿھ��
		LPVOID pBuff,		//���ջ�����
		int size			//���ջ���������
		);
	//���Ӵ������¼�,���ڷ����͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnAccept( HANDLE hIFHandle ,		//�ӿھ��
						  LPCTSTR pRemoteAddress,	//�Զ˵�ַ
						  int AddrSize,				//�Զ˵�ַ��������С
						  DWORD dwCodeType			//����Э�飬��Ϊ����SERVERʱ���õı���Э��
						  );
	
	//���ӳɹ��¼�,���ڿͻ��͵Ľӿڵ����ӳɹ�ʱ�������¼�
	virtual int OnConnect( 
		HANDLE hIFHandle //�ӿھ��
		);
	
	//����������¼�
	virtual int OnError( 
		HANDLE hIFHandle ,	//�ӿھ��
		int ErrorCode,		//����ԭ��
		BOOL bRecover		//������־
		);
	
	//����TRACE��Ϣ
	virtual int OnTrace( 
		HANDLE hIFHandle ,	//�ӿھ��
		BOOL bIn ,			//��Ϣ����
		int size,			//��Ϣ����
		LPVOID pBuff		//��Ϣ��
		);
	
};

#endif // !defined(__SmppDrvMng__h__)
