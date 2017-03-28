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
#define STATUS_ACTIVE			0		//�̬
#define STATUS_BROKEN			1		//����
#define STATUS_ABORT			2		//����


class CSockConnect : public CThread  
{
public:
	//���ɿͻ���DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT���
		LPCTSTR pAddr,				//��ַ�ַ���ָ��
		WORD wPort,					//�Զ˶˿ں�
		BOOL bAuto,					//�Ƿ��Զ�����
		DWORD dwTime,				//����ʱ��
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser				//�ص�ע����
		);
	
	//���ɷ�������DRV
	CSockConnect(
		HANDLE hDrvObject,			//DRVOBJECT���
		HANDLE hFile,				//SOCKET���
		LPONREAD pOnRead,			//���ص�����ָ��
		LPONERROR pOnError,			//����ص�����ָ��
		LPONCONNECT pOnConnect,		//���ӻص�����ָ��
		HANDLE hUser				//�ص�ע����
		);
	
	virtual ~CSockConnect();
	
	int Send(
		LPVOID pBuff,				//���ͻ�����ָ��
		int size					//�������ݴ�С
		);
	
	int GetStatus()
	{return m_iState;};
	
protected:
	HANDLE m_hDrvObject;			//DRVOBJECT���
	
	BOOL m_bServer;					//�Ƿ������˽ӿ�
	
	char m_RemoteAddr[40];			//Զ�˵�IP��ַ
	WORD m_wPort;					//Զ��PORT��
	
	//�Ƿ��Զ�����
	BOOL m_bAuto;
	DWORD m_dwTime;					//����ʱ��
	
	int m_iState;					//DRV״̬
	
	CRITICAL_SECTION m_Critical;
	LPONREAD m_pOnReadFun;
	LPONERROR m_pOnErrorFun;
	LPONCONNECT m_pOnConnectFun;
	HANDLE m_hUser;
	HANDLE m_hFile;					//SOCKET���
	CSockMsgQueue m_RcvPool;				//���ջ�����
	CSockMsgQueue m_SndPool;				//���ͻ�����
	
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
