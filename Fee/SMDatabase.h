// SMDatabase.h: interface for the CSMDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SMDatabase__H__)
#define __SMDatabase__H__

#import "msado15.dll" no_namespace rename("EOF","adoEOF")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "smcmacro.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
#include "SmppDef.h"
#include "globaldef.h"
#include "newmng.h"


const int Tag_Integer = 0;
const int Tag_String = 1;
const int Tag_LongInt = 2;


class CSMDatabase : public CNewMng
{
	
public:
	CSMDatabase();
	virtual ~CSMDatabase();
	
public :
	//��Ϣ���պ���
	BOOL OnMessage(PTRMSG pMsg);
	//��Ϣ���ͺ���
	BOOL SendMessageToQueue(PTRMSG pMsg);

	//��Ϣ���浽���ݿ⻺�����
	void OnAddDBBuffer(LPCTSTR pMsg);
	//����״̬����������ݿ⻺����в���ԭ��Ϣ
	void OnFindMsgInDBBuffer(LPCTSTR pMsg);
	//����WEB���¿۷ѱ�(MonthFee)״̬
	void OnUpdateFeeMonthTable(LPCTSTR pMSG);
	//���ؼ�¼�����ݿ��еİ��¿۷���Ϣ
	bool LoadFixMsg(LPCTSTR pFeeMonth);
	//������¿۷Ѽ�¼�����ݿ���
	void OnSaveFixMsg(PTRMSG pMsg);
	//ɾ�����ݿ��еİ��¿۷Ѽ�¼
	void OnDeleteFixMsg(PTRMSG pMsg);
	//����Ϣ��SMS�߳�
	bool SendMsgToSMSThread(PTRMSG pMsg);
	
private:
	//���ݿ�����ָ��
	CLinkObject * m_pLink;

	//�洢��������
	CString m_strLoadFixMsg;
	CString m_strSaveFixMsg;
	CString m_strDelFixMsg;
	CString m_strUpdteMonthFee;
	CString m_strAddSMBuff;
	CString m_strFindSMBuff;
	

	//ADO��COMMAND����
	_CommandPtr m_pLoadFixMsgCmd;
	_CommandPtr m_pSaveFixMsgCmd;
	_CommandPtr m_pDelFixMsgCmd;
	_CommandPtr m_pUpdateMonthFeeCmd;
	_CommandPtr m_pAddSMBuffCmd;
	_CommandPtr m_pFindSMBuffCmd;	

	//�����ݿ�����ȡDoneTimeʱ��ķ�ʽ true:״̬������յ�ʱ��false:��Ϣ���յ�ʱ��
	bool m_DoneTimeSrc;

	//���һ��ִ�����ݿ������ʱ��
	DWORD m_LastExeDBTime;
	//���ݿ���е����ʱ�䣬���������ʱ�䣬�ͶϿ������ݿ������
	DWORD m_NotExeDBTime;
	
	bool m_bLoadMonthFee;

protected:


	//����CNewMsg����ִ�еĲ�������
	void OnTime();
	BOOL OnMountMessage();

	//������������
	void ReturnLink(CLinkObject *pLink, BOOL bOK = TRUE);
	//��ӡ���ݿ�������־
	void PrintProviderError(LPCTSTR pszMsg, _ConnectionPtr pConnection);
	//��ʼ��ADO COMMAND����
	void InitCommand();
	//��ȡDBLinkָ��
	bool GetDBLink(); 
	
	//������Ϣ��״̬����Ϊ��־
	void DumpSM(LPSTR pData, int len, SaveShortMessage * pSM);
	
	//DB Library��ʽ�������ݿ�

	//�������ò���
	void LoadConfig();

};

#endif
