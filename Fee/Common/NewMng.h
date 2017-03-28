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
//ʹ�÷�����
//	�������� OnMessage			������Ϣ
//	�������� OnTime				����ÿ�붨ʱ��Ϣ
//	�������� OnMountMessage()	����ά���¼���ÿ������һ�Σ����������FASLE����Ϊϵͳ���ܴ��������Ϣ��ϵͳ�����ٵ���OnMessage,ֱ������TURE�Ż�ָ�
//	ʹ�÷���˳��
//		1. init����ʼ������
//		2. start������
//	�˳�ʱ
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
	// ��Ϣ���뺯��, ����Ҫnew !!
	
	virtual void Start();
	//����֮�����̲߳����Զ�������������ʾ����start
	//�����Ҫ���ش˺�������Ҫ�����غ������ʵ���λ�õ��û����start
	virtual void Destroy();
	// ɾ��CManager���͵�ʵ��ǰ��������ʽ���ô˺���
	//�����Ҫ���ش˺�������Ҫ�����غ������ʵ���λ�õ��û����Destroy
protected:
	// ��Ҫ���صĺ���
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
	// ��Ϣ�����������, ʵ�ֵ�ʱ����Ҫɾ����Ϣ�� !!
	//ʱ�Ӵ���
	virtual void OnTime() = NULL;
	//add by wujun 
	virtual BOOL OnMountMessage() = NULL;
protected:
	// ��Ϣ���ȹ���
	virtual void Run();
	
	
protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	DWORD m_nTimeTick;
	BOOL m_bMounted;
	//ά������
	CCacheMsgQueue m_msgQueue; //ҵ�����
	//CMessageQueue m_msgMountQueue;
};

#endif // !defined(AFX_MULTITHREADMNG_H__D83C7C11_762A_4C5D_B8C7_BAC1560B7F03__INCLUDED_)
