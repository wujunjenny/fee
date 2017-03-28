///////////////////////////////////////////////////////////////////
//
// ISMP����ϵͳ--ISMGW
//
// Copyright(c) 2005 ������Э����Ϣ�������޹�˾
//
//-----------------------------------------------------------------
//
// ������: SmServiceProc
// �ļ���: SMFeeThread.h
// ������: 
// �ա���: 2005-4-11
// �衡��: SMFeeThread�ļ���Ҫ����CSMFeeThread�࣬��CSMFeeThread��Ҫ
//		   �������д�����ģ�鷢�͹�������Ϣ��OnMessageΪ��Ϣ��ں��� 
// �桡��: V4.1
//-----------------------------------------------------------------
// 
///////////////////////////////////////////////////////////////////

// CSMFeeThread.h: interface for the CSMFeeThread class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__SMFeeThread__h__)
#define __SMFeeThread__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#import "msado15.dll" no_namespace rename("EOF","adoEOF")
#include "COMMON\CommThreadMng.h"
#include "COMMON\MultiThreadMng.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
#include "AdoObj.h"
#include <string>
#include "tlv.h"
#include "MainProcMng.h"
#include "smppdef.h"
#include "BaseMng.h"
//#include <map>
#include "globaldef.h"
#include "NewMap.h"

#include "icrsint.h"
#include <unordered_map>
#include <memory>
#include <list>


const int NORMAL_DELIVER_MSG = 0;       // ������DELIVER��Ϣ
const int MONTH_SERVICE_MSG = 2;        // ���·������Ϣ
const int MONTH_CHARGE_MSG = 2;         // ���¿۷ѵ���Ϣ
const int MONTH_ONLEY_CHARGE_MSG = 8;   // ���½����ڿ۷ѵ���Ϣ
const int SP_CHARGE_MSG = 4;            // SPʵ�ּƷѵ���Ϣ
const int PAYMEMT_BEFORE_USER = 0;      // Ԥ�����û�
const int PAYMENT_BEHIAND_USER = 1;     // �󸶷��û�
const int PAYMENT_BEFORE_FLOW = 0;      // Ԥ��������
const int PAYMENT_BEHIND_FLOW = 1;      // �󸶷�����

const int MO_CHARGE = 1;
const int MT_CHARGE = 2;
const int INFO_CAHRGE = 3;


class CSMFeeThread : public CBaseMng  
{
public:
	CSMFeeThread();
	virtual ~CSMFeeThread();

public:

//////////////////////////////////////////////////////////////////////
// ������: OnMessage
// ������: �������д�����ģ�鷢������Ϣ���ҽ���Ϣ�ַ���������Ϣ������
//		   ��
// ������: BOOL  - 
// �Ρ���: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	virtual BOOL OnMessage(PTRMSG pMsg);

//////////////////////////////////////////////////////////////////////
// ������: nDeliverSM
// ������: ��������ת����Ϣ�ɹ���ʧ�ܺ��͹�������Ϣ�����ת���ɹ�����
//		   ������Ҫ���յ�����Ϣ���浽�ڴ�����ݿ��еȴ�״̬�����ִ.
// ������: O - 
// �Ρ���: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	void OnDeliverSM(LPCTSTR pMsg);

//////////////////////////////////////////////////////////////////////
// ������: OnReportSM
// ������: ����״̬������Ϣ�������˺�����Ҫ���ܣ�
//		   1.����״̬�����һ�ԭ����Ӧ����Ϣ��ת����ϢID��
//		   2.���мƷѴ���
//		   3.�������ú�״̬�����Ƿ�ɹ�������Ԥ��������
// ������: VOID  - 
// �Ρ���: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnReportSM(LPCTSTR pMsg);
	


//////////////////////////////////////////////////////////////////////
// ������: OnCheckFixMsg
// ������: ������¿۷�������Ϣ
// ������: void  - 
// �Ρ���: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	void OnCheckFixMsg(PTRMSG pMsg);

public:


//////////////////////////////////////////////////////////////////////
// ������: OnExpireReport
// ������: ���ճ�ʱδ��״̬�������Ϣ������״̬����
// ������: VOID  - 
// �Ρ���: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnExpireReport(PTRMSG pMsg);

//////////////////////////////////////////////////////////////////////
// ������: OnPayConfirmSM
// ������: ����Ԥ����ƽ̨�۷ѽ����Ϣ����Ҫ�ǼƷѴ�������״̬����
// ������: VOID  - 
// �Ρ���: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnPayConfirmSM(LPCTSTR pMsg);

//////////////////////////////////////////////////////////////////////
// ������: OnLookupRes
// ������: �������ݿ⻺�����һص�ԭ��Ϣ��Ĵ������̺���
// ������: VOID  - 
// �Ρ���: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnLookupRes(LPCTSTR pMsg);


	// ��ʱ������
	BOOL OnTimer(DWORD timeID, DWORD dwPara);


	// �洢��Ҫ��״̬���������
	//CNewMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_BufferMap;
	//CMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_BufferMap1;
	std::unordered_map<std::string,std::shared_ptr<SaveShortMessage>> m_BufferMap;
	std::list<std::weak_ptr<SaveShortMessage>> m_time_index;
	// �洢��Ҫ�ȿ۷ѽ��������
	//CNewMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_PrePayMap;
	CMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_PrePayMap;
	
protected:
	CPtrList m_CacheReportList;	
	int		 m_SwitchPos[48];
	bool	 m_DoneTimeSrc;
	bool	 m_bconvertUTF8;

//-----SMGW41-39,2005-04-12,zxh,modi begin----//
	DWORD    m_dwSequence_NO;  /*Э�����к�*/
//-----SMGW41-39,2005-04-12,zxh,modi end----//


//-----SMGW41-48,2005-04-14,zxh,modi begin----//
	int		m_iMTFCheck;	/*MTF/MTFF���͵İ��¿۷���Ϣ�Ƿ�����ص��޳�*/
//-----SMGW41-48,2005-04-14,zxh,modi end----//
//-----tianhy add 20051114 begin----//
	//�������ط��صĸ����û����͵��ж�
	int m_iPayResultErrCode ; 
//-----tianhy add 20051114 end----//
	// ת��״̬���浽���ݿ��߳���
	void SendReportToDB(LPCTSTR pMsg);

	//-----SMGW41-39,2005-04-12,zxh,modi begin----//

	// �ڴ���Ϣ��ʱ��ת�������ݿ⻺��
	VOID SendDBBuffer(SaveShortMessage data);
	// ����д�ļ������ķ���
	VOID SendWriteFileBill( SaveShortMessage *pData);
	// ���¿۷�ʧ�ܺ��͸��°����û�״̬��Ϣ
	VOID SendDelFixedMsg(SaveShortMessage data);
	// SMGW42-26, 2005-06-01, wzy add begin //
	VOID SendUpdateFeeMonthInfo(tagFeeMonthStatus *pInfo);
	// SMGW42-26, 2005-06-01, wzy add end //



	// ��������״̬���浽����ģ��
	VOID SendStatusReport( SaveShortMessage data );
	// ����Ԥ������������ģ��
	VOID  SendPaymntRequest(SaveShortMessage data);

	//-----SMGW41-39,2005-04-12,zxh,modi end----//

	// ת��״̬���浽����ģ��
	VOID TranslateStatusReport(smpp_Message_header *pHeader, smpp_Submit_Sm *pSub, SaveShortMessage data);	

	/*
	// ���Ͱ��¿۷�֪ͨ
	VOID SendDeliver(SaveShortMessage data);
	// У������û�״̬��ת�����¿۷���Ϣ������ģ��
	*/

//	VOID ResumeMsg(SaveShortMessage data);


	// У��״̬������ȷ�ķ���
	BOOL CheckStatusReport(SaveShortMessage *pData);
	BOOL CheckStatusReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);
	
	// ȡTLV���ݺ���
	VOID GetTLVItem(CString &strDestString, CTLV &tlv, DWORD tag, int DataType);
	// ȡ��Ϣ���ݺ���
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	void GetMsgContent(smpp_Submit_Sm* pSub, SaveShortMessage &msg,long RecvID);
	void GetMsgContent(smpp_Submit_Sm* pSub, FEECHECKMSG &msg);
	void SendCheckFixMsgResp(unsigned long int SequenceNo, int ErrCode,long RecevId);
	//SMGW42-55, 2005-7-12, jdz, modi end//

	// ����״̬���溯���������Ȼ�״̬������ACK������
	int GetCacheReport(LPVOID pMsg,int& size);
	void CacheReport(LPCTSTR pMsg);
	BOOL ProcessorReport(LPCTSTR pMsg);

	// ���ڴ滺����Ϣת�����ݿ⻺�����Ϣ�����������ս����̷߳��͵���Ϣ
	void OnClearMemoryMsg(PTRMSG pMsg);

	virtual BOOL OnMountMessage();

	// ����״̬����ķ���
	BOOL CaheNormaldata(SaveShortMessage& data);
	// ����Ԥ������󻺴�״̬����ķ���
	BOOL CahePrepaydata(SaveShortMessage& data);
	// ��ʱ����ʱ����Ϣ������
	void OnNormalTimeOut(BYTE*pParam2,DWORD param2len);
	void OnPrepayTimeOut(BYTE*pParam2,DWORD param2len);

	// ��ʱ����Ϣ
	virtual void OnTickTime(DWORD nTimeID,DWORD param1,BYTE*pParam2,DWORD param2len);
	virtual void OnTime();	
	
	//-----SMGW42-109,2005-11-19,thy add begin-----//
	//////////////////////////////////////////////////////////////////////
	// ������: CacheShortMessage
	// ������: �������Ϣ���ȴ�״̬����
	// ������: void  - 
	// �Ρ���:	SaveShortMessage ���� 
	//////////////////////////////////////////////////////////////////////
	//���棬�ȴ�״̬����
	void CacheShortMessage(SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// ������: NeedSendPrePay
	// ������: �Ƿ���Ԥ��������
	// ������: TRUE ����Ԥ�������̣�FASLSE ������Ԥ��������
	// �Ρ���: SaveShortMessage ���� 
	//////////////////////////////////////////////////////////////////////
	BOOL NeedSendPrePay(const SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// ������: SendPrePayMsg
	// ������: ����Ԥ��������
	// ������: BOOL  TRUE:  �Ȼ�����ϢȻ�� ����Ԥ�������̳ɹ�
	//				FALSE:  ������Ϣʧ�� û�з���Ԥ�������̳ɹ�
	// �Ρ���:  SaveShortMessage ���� 
	//////////////////////////////////////////////////////////////////////
	BOOL SendPrePayMsg(SaveShortMessage &data);


	//////////////////////////////////////////////////////////////////////
	// ������	:	CheckFixMsg
	// ������	:	������¿۷��ص����
	// ������	:	int --������
	//////////////////////////////////////////////////////////////////////
	int CheckFixMsg(LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pDestAddr);
	//////////////////////////////////////////////////////////////////////
	// ������: SendDeliverResp
	// ������: ������Ϣ���ذ�
	// ������: void
	// �Ρ���: long int Sequence_No  
	//////////////////////////////////////////////////////////////////////
	void SendDeliverResp(long int Sequence_No,long lSendId);

	//////////////////////////////////////////////////////////////////////
	// ������: DeliverStatusFail
	// ������: DELIVER��Ϣ״̬Ϊʧ��ʱ�Ĵ���
	// ������: void
	// �Ρ���:  SaveShortMessage 
	//////////////////////////////////////////////////////////////////////
	void DeliverStatusFail(SaveShortMessage &data);
	
	//////////////////////////////////////////////////////////////////////
	// ������: DeliverStatusSuc
	// ������: DELIVER��Ϣ״̬Ϊ�ɹ�ʱ�Ĵ���
	// ������: void
	// �Ρ���:  SaveShortMessage   
	//////////////////////////////////////////////////////////////////////
 	void DeliverStatusSuc(SaveShortMessage &data);
	
	//////////////////////////////////////////////////////////////////////
	// ������: ReportDeal
	// ������: ״̬������Ϣ����
	// ������: void
	// �Ρ���:  SaveShortMessage ����  
	//			smpp_Message_header ָ������
	//			smpp_Submit_Sm ָ������
	//////////////////////////////////////////////////////////////////////
	void ReportDeal(smpp_Submit_Sm *&pSub,smpp_Message_header *&pHeader,SaveShortMessage &data);
	void ReportDeal(SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// ������: GetSubmitFeeType
	// ������: �ж����еļǷ�����
	// ������:  0:���
	//			1:����
	//			2:�������
	//			3:�ⶥ
	//			4:���¿۷�
	//			5:CDR����
	//			-1:δ֪��
	// �Ρ���:  SaveShortMessage ����  
	//////////////////////////////////////////////////////////////////////
	int GetSubmitFeeType(const SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// ������: DeliverStatusSucc
	// ������: ��ȡ����ת��ҵ������
	// ������:  int
	//			0:��Ե�
	//			1������
	//			2������
	//			-1��δ֪������
	// �Ρ���:  int 
	//////////////////////////////////////////////////////////////////////
	int GetGWTransferMsgType(int MsgType);

	
	//�ж��Ƿ�������Ϣ
	BOOL IsMTMsg(int MsgType);		
	//�ж�������Ϣ�Ƿ��Ƿ��͸�����SP��
	BOOL MOSMIsLocalSP(int MsgType);
	//��״̬�����л�ȡ�ύʱ�䣬�����µ�������
	void UpdateSubmitTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);
	//��״̬�����л�ȡ�������ʱ�䣬�����µ�������
	void UpdateDoneTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);

	bool SendSaveFixMsgToDB(const LPFIXMSGINFO pSM);

	//-----SMGW42-109,2005-11-19,thy add end-----//
};


//������¿۷���Ϣ���ڵļƷ���
void CalFixMsgFeeMonth(const char * MsgSubmitTime, char * pFeeMonth);

#endif // !defined(__SMFeeThread__h__)
