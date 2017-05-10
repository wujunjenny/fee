// MulitSmsThread.cpp: implementation of the CSMFeeThread class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "smserviceproc.h"
#include "SMFeeThread.h"
#include "eventlog.h"
#include "SmppCodeAndDecode.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include <locale>
#include <codecvt>
#include "PaymentSupport.h"
#include "GlobalDef.h"




std::wstring ConvertUTF8ToWString(std::string& sUtf8);
std::string ConvertWStringToUTF8(std::wstring& ws);
std::string ConvertWStringToString(std::wstring& ws);
std::wstring ConvertStringToWString(std::string s);
std::wstring ConvertNWStringToWString(std::wstring wsnet);
std::string EncodeBuff(std::string sutf8);
std::string DecodeBuff(const std::string& str);
// *** SMGW25-30,2004-06-29,wzy add begin *** //
//int G_FIRST = 0;
// *** SMGW25-30,2004-06-29,wzy add end *** //

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//��ʱ����־
#define NORMALFLAG 1
#define PREPAYFLAG 2


//-----SMGW42-109,2005-11-10,thy add begin-----//

//��ȡ����ת��ҵ������
#define  GWTANSFER_MSGTYPE_P2P				0
#define  GWTANSFER_MSGTYPE_MO				1
#define  GWTANSFER_MSGTYPE_MT				2
#define  GWTANSFER_MSGTYPE_FWDPAY			3
#define  GWTANSFER_MSGTYPE_UN				-1
//�ж����еļǷ�����
#define  FEETYPE_FREE				0
#define  FEETYPE_MSG_PIECE			1
#define  FEETYPE_MSG_MONTH_FREE		2
#define  FEETYPE_MSG_TOP_FEE		3
#define  FEETYPE_MSG_MONTH_PAY		4
#define  FEETYPE_MSG_CDR_BILLS		5
#define  FEETYPE_MSKG_UNKNOWN		-1

//�ж��Ƿ���Ԥ��������
//Ԥ�����û� ���� �󸶷��û���Ԥ��������
#define  PREPAY_FEE_EXCECUTE_PREPAYPLOW		0
//����Ԥ������Ϣʧ��
#define  PREPAY_FEE_CACHE_PREPAYMAS_ERROR	1
//�мǷѵ� ������Ԥ�������� �γɻ���
#define  PREPAY_FEE_NOT_EXCECUTE_PREPAYPLOW	2
//û�мǷѵ� ������Ԥ�������� �γɻ���
#define  PREPAY_NOT_FEE						-1

//����
#define  OPEN_MEMORY_CACHE_FIRST			1
#define  OPEN_MEMORY_CACHE_NOT  			0

//�ж��Ƿ�ȴ�״̬����
#define  WHETHER_WAIT_REPORT_BACK_WAIT		1
#define  WHETHER_WAIT_REPORT_BACK_NOTWAIT	0

//�жϿ۷���Ϣ���ͷ�ʽ
//��Ԥ���ѿ۷���Ϣ���ͷ�ʽ
#define  MSG_MONTH_FEE_SEND_MODE_PREPAY		1
//������Ϣ��Forwardģʽ
#define  MSG_MONTH_FEE_SEND_MODE_FORWARD	0


//-----SMGW42-109,2005-11-10,thy add end-----//
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMFeeThread::CSMFeeThread()
{	
	
	// ������ʱ��
	DWORD nSaveFileInterval = g_SaveFileTime * 1000;
	DWORD nTimerSwitchInterval = 1000;
	
    char  InitName[]=".\\FeeConfig.ini";
	char  sTemp[100];
	int Ret;
	Ret = GetPrivateProfileString("BillConfig", 
		"FeeInterval", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "24");
		WritePrivateProfileString("BillConfig",
			"FeeInterval",
			sTemp,
			InitName);
	}
	unsigned int FeeInterval = atoi(sTemp);
	if(FeeInterval > 24 || FeeInterval < 1)
		FeeInterval = 24;
	
	
	Ret = GetPrivateProfileString("BillConfig", 
		"FeeOutTime", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "7200");
		WritePrivateProfileString("BillConfig",
			"FeeOutTime",
			sTemp,
			InitName);
	}
	unsigned int FeeOutTime = atoi(sTemp);
	
	memset(m_SwitchPos, 0, sizeof(m_SwitchPos));
	int count = 24 / FeeInterval;
	m_SwitchPos[0] = 0;
	m_SwitchPos[1] = FeeOutTime;
	for(int i = 2; i < 2 * count; )
	{
		m_SwitchPos[i++] = m_SwitchPos[i-3] + FeeInterval * 3600;
		m_SwitchPos[i++] = m_SwitchPos[i-1] + FeeOutTime;
	}
	
	
	m_DoneTimeSrc = true;
	
	Ret = GetPrivateProfileString("BillConfig", 
		"DoneTimeSrc", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "1");
		WritePrivateProfileString("BillConfig",
			"DoneTimeSrc",
			sTemp,
			InitName);
	}
	if(atoi(sTemp))
	{
		m_DoneTimeSrc = true;
	}
	else
	{
		m_DoneTimeSrc = false;
	}
	


	m_bconvertUTF8 = false;

	Ret = GetPrivateProfileString("BillConfig", 
		"ConvertUTF8", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "1");
		WritePrivateProfileString("BillConfig",
			"ConvertUTF8",
			sTemp,
			InitName);
	}
	if(atoi(sTemp))
	{
		m_bconvertUTF8 = true;
	}
	else
	{
		m_bconvertUTF8 = false;
	}

	
	Ret = GetPrivateProfileString("CacheSetting", 
		"MaxBuffSize", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "1000000");
		WritePrivateProfileString("CacheSetting",
			"MaxBuffSize",
			sTemp,
			InitName);
	}
	g_MaxBuffSize = atoi(sTemp);
	
	Ret = GetPrivateProfileString("CacheSetting", 
		"MaxBuffMsgCount", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "10000");
		WritePrivateProfileString("CacheSetting",
			"MaxBuffMsgCount",
			sTemp,
			InitName);
	}
	g_MaxMsgCount = atoi(sTemp);
	
	Ret = GetPrivateProfileString("CacheSetting", 
		"MaxMemMsgCount", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "10000");
		WritePrivateProfileString("CacheSetting",
			"MaxMemMsgCount",
			sTemp,
			InitName);
	}
	g_MsgMapMaxCount = atoi(sTemp);
	g_PreMsgMaxMapCount = g_MsgMapMaxCount;
	
	Ret = GetPrivateProfileString("CacheSetting", 
		"InitHashTabSize", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "0.25");
		WritePrivateProfileString("CacheSetting",
			"InitHashTabSize",
			sTemp,
			InitName);
	}
	
	double HashSizePercent = atof(sTemp);
	//m_BufferMap.InitHashTable(HashSizePercent * g_MsgMapMaxCount);
	m_BufferMap.rehash(HashSizePercent * g_MsgMapMaxCount);
	Ret = GetPrivateProfileString("CacheSetting", 
		"InitPrepayHashTabSize", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "0.25");
		WritePrivateProfileString("CacheSetting",
			"InitPrepayHashTabSize",
			sTemp,
			InitName);
	}
	
	double HashPreSizePercent = atof(sTemp);
	m_PrePayMap.InitHashTable(HashSizePercent * g_MsgMapMaxCount);
	
	// *** SMGW41-04,2005-02-05,wzy add begin *** //
	Ret = GetPrivateProfileString("NetMode", 
		"mode", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "0");
		WritePrivateProfileString("NetMode",
			"mode",
			sTemp,
			InitName);
	}
	g_NetMode = atoi(sTemp);
	// *** SMGW41-04,2005-02-05,wzy add end *** //
	
	//  SMGW42-12,2005-05-31,wzy add begin  //
	Ret = GetPrivateProfileString("NetMode", 
		"FeeMonthNeedReport", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "1");
		WritePrivateProfileString("NetMode",
			"FeeMonthNeedReport",
			sTemp,
			InitName);
	}
	g_FeeMonthMsgNeedReport = atoi(sTemp);
	//  SMGW42-12,2005-05-31,wzy add end  //
	
	
	
	
	
	//-----SMGW41-39,2005-04-12,zxh,modi begin----//
	m_dwSequence_NO = 0;
	//-----SMGW41-39,2005-04-12,zxh,modi end----//
	
	//-----SMGW41-48,2005-04-14,zxh,modi begin----//
	// ��ȡ��MTF/MTFF���͵İ��¿۷���Ϣ�Ƿ�����ص��޳���������
	Ret = GetPrivateProfileString("MTF_MTFF", 
		"MTF_MTFF_RepeatCheck", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "0");
		WritePrivateProfileString("MTF_MTFF",
			"MTF_MTFF_RepeatCheck",
			sTemp,
			InitName);
	}
	m_iMTFCheck = atoi(sTemp);
	//-----SMGW41-48,2005-04-14,zxh,modi end----//
	
//-----SMGW42-109,2005-11-10,thy add begin-----//
	Ret = GetPrivateProfileString("PayResultErrCode", 
		"ErrCode", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "306");
		WritePrivateProfileString("PayResultErrCode",
			"ErrCode",
			sTemp,
			InitName);
		m_iPayResultErrCode = 306;
	}
	m_iPayResultErrCode = atoi(sTemp);
//-----SMGW42-109,2005-11-10,thy add end-----//
}


CSMFeeThread::~CSMFeeThread()
{
	// ֹͣɨ�軺�����Ķ�ʱ��
	/*	KillTimer(m_nScanBufferTimerID);
	KillTimer(m_nSaveFileTimerID);
	KillTimer(m_nTimeSwitchTimerID);*/
}


void CSMFeeThread::OnDeliverSM(LPCTSTR pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	//��SP�������ػظ�Ӧ��
	SendDeliverResp(pHeader->Sequence_No,pHeader->nRcvID);
	
	smpp_Submit_Sm *pSub = (smpp_Submit_Sm *)(pMsg + sizeof(smpp_Message_header));

	SaveShortMessage data;
	memset(&data, 0, sizeof(data));
	//��ȡ��������
	GetMsgContent(pSub, data,pHeader->nRcvID);

	//��ҵ�����ת��Ϊ��д
	strupr(data.serviceID);
	
	//add for debug
	//PrintMessage("OnDeliverSM  unikey[%s] orgaddr[%s] destaddr[%s] status[%s] GWName[%s]",data.unikey,data.orgAddr,data.destAddr,data.status,data.SendGWName);

	int status = atoi(data.status);
	if (status != SUCCESS_DELIVER_MESSAGE)
	{//״̬ʧ��
		DeliverStatusFail(data);
	}
	else
	{//״̬�ɹ�
 		DeliverStatusSuc(data);
	}

	return ;
}
//-----SMGW42-109,2005-11-10,thy modi end-----//


VOID CSMFeeThread::OnReportSM(LPCTSTR pMsg)
{
    smpp_Message_header *pHeader = (smpp_Message_header*)pMsg;
    smpp_Submit_Sm *pSub = (smpp_Submit_Sm*)(pMsg + sizeof(smpp_Message_header));
	
    smpp_Message_header resHeader;
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//smpp_Submit_Sm_Resp resSub;
	// SMGW431-06, 2005-10-27, wzy modify end //
    
	
	resHeader.Command_ID = ESME_SHREPORT_SM_RESP;
    resHeader.Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm_Resp);
    resHeader.Command_Status = E_SUCCESS;
    resHeader.Sequence_No = pHeader->Sequence_No;
//modify by gyx
	resHeader.nSndID = pHeader->nRcvID;
	
	if (((CSmServiceProcApp*)AfxGetApp())->pDrv->Send((smpp_Message_header*)&resHeader))
		g_nRecvDeliverCount++;
	
	
	if(!ProcessorReport(pMsg))
	{
		CacheReport(pMsg);
	}
}


VOID CSMFeeThread::OnPayConfirmSM(LPCTSTR pMsg)
{
    smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
    Payment_Confirm_Sm *pSub = (Payment_Confirm_Sm *)(pMsg + sizeof(smpp_Message_header));
    smpp_Message_header ResHeader;
	
	// Resoponsed the ack message
	ResHeader.Command_ID = ESME_PAYCONFIRM_SM_RESP;
    ResHeader.Command_Length = sizeof(smpp_Message_header);
    ResHeader.Command_Status = E_SUCCESS;
    ResHeader.Sequence_No = pHeader->Sequence_No;	
	if (((CSmServiceProcApp*)AfxGetApp())->pDrv->Send((smpp_Message_header*)&ResHeader))
	{
		g_nRecvDeliverCount++;
	}
	
	CString strUnikey = pSub->DestMsgID;
	
	// Find the message order by IndexMsgid
	SaveShortMessage data;
	BOOL bFound = m_PrePayMap.Lookup(strUnikey, data);
	if (bFound)
	{
		int MessageType = atoi(data.messageType);
		int FeeType = atoi(data.feeType);
		
		
		// Remote the memory data
		m_PrePayMap.RemoveKey(strUnikey);
		KillTimer(data.TimerID);
		
		int nResult = pSub->Status;
 		//�۷ѳɹ� �����û�Ԥ���ѷ�����		
		if (0 == nResult || m_iPayResultErrCode == nResult)
		{//������Ԥ�����û������Ԥ����
			sprintf(data.status, "0");
			sprintf(data.serviceUpID, "DELIVRD");

			if (m_iPayResultErrCode == nResult)
			{//��Ԥ�����û����¼Ʒ��û���Ԥ���ѱ�־
				data.payUserType = NOT_PREPAY_USER;
			}
			
			// Send report to Destination
			SendStatusReport(data);		
		}
		else
		{
			// Responsed the error report
			sprintf(data.status, "%d",  nResult);
			sprintf(data.serviceUpID, "UNDELIV");

			SendStatusReport( data );

			if (IsMTMsg(atoi(data.msgType)) //��������Ϣ
				&& (FeeType == 2 && MessageType == 8))//�ǰ��¿۷���Ϣ
			{
				//����ɾ�����¿۷��ص���¼����Ϣ
				SendDelFixedMsg(data);
			}
		}

		//д����
		SendWriteFileBill(&data);
	}
	else
	{
		// Not found the message in memory
		ASSERT(0);
		PrintMessage("Not found the message in memory when received the Payment Result"); 
	}
	//-----SMGW42-109,2005-11-10,thy modi end-----//
}



//��check��Ϣ��Ӧ��
void CSMFeeThread::SendCheckFixMsgResp(unsigned long int SequenceNo, int ErrCode,long RecevId)
{
	char ResMsg[sizeof(smpp_Message_header) + 1];
	memset(ResMsg, 0, sizeof(ResMsg));
	
	smpp_Message_header * pResHeader = (smpp_Message_header *)ResMsg;
	
	pResHeader->Command_ID = ESME_CHECK_VALID_SM_RESP;
	pResHeader->Command_Length = sizeof(smpp_Message_header);
	pResHeader->Command_Status = ErrCode;
	pResHeader->Sequence_No = SequenceNo;
	pResHeader->nSndID = RecevId;
	
	CSmServiceProcApp * pApp = (CSmServiceProcApp *)AfxGetApp();
	if (pApp->pDrv->Send((smpp_Message_header* )&ResMsg))
	{
		g_nRecvDeliverCount++;
	}
}



VOID CSMFeeThread::OnLookupRes(LPCTSTR pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	SaveShortMessage *pData = (SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	
	//-----SMGW42-109,2005-11-10,thy modi begin-----//
	ReportDeal(*pData);
	//-----SMGW42-109,2005-11-10,thy modi end-----//
}


BOOL CSMFeeThread::OnMessage(PTRMSG pMsg)
{
	smpp_Message_header* pHeader=(smpp_Message_header*)pMsg;
	
	switch(pHeader->Command_ID)
	{
	case ESME_DELIVER_SM:
		OnDeliverSM(pMsg);
		break;	
	case ESME_SHREPORT_SM:
		OnReportSM(pMsg);
		break;
	case ESME_PAYCONFIRM_SM:
		OnPayConfirmSM(pMsg);
		break;
	case ESME_CHECK_VALID_SM:
		OnCheckFixMsg(pMsg);
		break;
	case CMD_FIND_DB_BUFFER_RESULT:
		OnLookupRes(pMsg);
		break;
	case CMD_CHECK_EXPIRE_RESULT:
		OnExpireReport(pMsg);
		break;
	case CMD_CLEAR_MEMORY_MSG:
		OnClearMemoryMsg(pMsg);
		break;
	default:
		break;
	}
	
	return TRUE;
}


BOOL CSMFeeThread::OnTimer(DWORD timeID, DWORD dwPara)
{
	return TRUE;
}


void CSMFeeThread::GetTLVItem(CString &strDestString, CTLV &tlv, DWORD tag, int DataType)
{
	//***SMGW40-01, 2004-12-23, jdz, modi begin***//
	DWORD dwLen;
	const BYTE *pTempValue = NULL;
	long int temp;
	
	const DWORD dwBuffLen = 300;
	char szDestBuff[dwBuffLen];
	memset(szDestBuff, 0, dwBuffLen);
	
	switch(DataType)
	{
	case Tag_LongInt: 
		temp = tlv.GetLongIntItemValue(tag);
		strDestString.Format("%d", temp);
		break;
		
	default:
	case Tag_String:
		pTempValue = tlv.GetItem(tag, dwLen);
		if(pTempValue)
		{
			memcpy(szDestBuff, (char *)pTempValue, min(dwBuffLen - 1, dwLen));
			strDestString.Format("%s", szDestBuff);
			strDestString.TrimLeft();
			strDestString.TrimRight();
		}
		else
		{
			strDestString.Format("%s", "");
		}
		break;
		
	case Tag_Integer:
		pTempValue = tlv.GetItem(tag, dwLen);
		if(pTempValue)
		{
			if (dwLen == sizeof(DWORD))
				strDestString.Format("%d", *(DWORD *)pTempValue);
			else
				strDestString.Format("%d", *pTempValue);
		}
		else
		{
			strDestString.Format("%s", "");
		}
		break;		
	}
	
	//***SMGW40-01, 2004-12-23, jdz, modi end***//
}

//SMGW42-55, 2005-7-12, jdz, modi begin//

void CSMFeeThread::GetMsgContent(smpp_Submit_Sm* pSub, FEECHECKMSG &msg)
{
	if (pSub && pSub->ext_data_len > 0)
	{
		CTLV tlv;
		DWORD Pos = 0;
		tlv.FromStream((unsigned char *)pSub->ext_data, pSub->ext_data_len, Pos);		
		
		memset(&msg, 0, sizeof(FEECHECKMSG));
		
		CString str;	
		// submitTime
		GetTLVItem(str, tlv, Tag_EX_SubTime, Tag_String);
		strncpy(msg.SubmitTime, str, sizeof(msg.SubmitTime) - 1);
		
		// FeeAddr
		GetTLVItem(str, tlv, Tag_Fee_termial_id, Tag_String);
		strncpy(msg.FeeAddr, str, sizeof(msg.FeeAddr) - 1);
		
		// SPID
		GetTLVItem(str, tlv, Tag_Ex_CPID, Tag_String);
		strncpy(msg.SPID, str, sizeof(msg.SPID) - 1);
		
		// OrgAddress
		if (pSub->source_addr != NULL)
		{
			str.Empty();
			str.Format("%s",pSub->source_addr);
			str.TrimRight();
			strncpy(msg.OrgAdd, str, sizeof(msg.OrgAdd) - 1);
		}
		else
		{
			strcpy(msg.OrgAdd, "\0");
		}
		
		// DestAddress
		if (pSub->destination_addr != NULL)
		{
			str.Empty();
			str.Format("%s", pSub->destination_addr);
			str.TrimRight();
			strncpy(msg.DestAddr, str, sizeof(msg.DestAddr) - 1);
		}
		else
		{
			strcpy(msg.DestAddr, "\0");
		}
		
		
		//ServiceID
		GetTLVItem(str, tlv, Tag_ServiceID, Tag_String);
		strncpy(msg.ServiceID, str, sizeof(msg.ServiceID) - 1);	

	}
}


void CSMFeeThread::GetMsgContent(smpp_Submit_Sm* pSub, SaveShortMessage &msg,long RecvID)
{
	if (pSub && pSub->ext_data_len > 0)
	{
		CTLV tlv;
		DWORD Pos = 0;
		tlv.FromStream((unsigned char *)pSub->ext_data, pSub->ext_data_len, Pos);		
		
		memset(&msg, 0, sizeof(SaveShortMessage));
		
		msg.saveTime = GetTickCount();
		
		CString str;
		
		// smid
		GetTLVItem(str, tlv, Tag_SMID, Tag_String);
		strncpy(msg.smid, str, sizeof(msg.smid) - 1);
		
		// sourceID
		GetTLVItem(str, tlv, Tag_SOURCEID, Tag_String);
		strncpy(msg.sourceID, str, sizeof(msg.sourceID) - 1);
		
		// unikey
		GetTLVItem(str, tlv, Tag_UNIKEY, Tag_String);
		strncpy(msg.unikey, str, sizeof(msg.unikey) - 1);
		
		// rcvAccount
		GetTLVItem(str, tlv, Tag_EX_RecvAccount, Tag_String);
		strncpy(msg.rcvAccount, str, sizeof(msg.rcvAccount) - 1);
		
		// sendAccount
		GetTLVItem(str, tlv, Tag_EX_SendAccount, Tag_String);
		strncpy(msg.sendAccount, str, sizeof(msg.sendAccount) -1);
		
		// submitTime
		GetTLVItem(str, tlv, Tag_EX_SubTime, Tag_String);
		strncpy(msg.submitTime, str, sizeof(msg.submitTime) - 1);
		
		// deliverTime
		GetTLVItem(str, tlv, Tag_EX_DlrTime, Tag_String);
		strncpy(msg.deliverTime, str, sizeof(msg.deliverTime) - 1);
		
		// MessgeType
		GetTLVItem(str, tlv, Tag_MsgType, Tag_Integer);
		strncpy(msg.messageType, str, sizeof(msg.messageType) - 1);
		
		// MsgType
		GetTLVItem(str, tlv, Tag_Ex_MsgType, Tag_LongInt);
		strncpy(msg.msgType, str, sizeof(msg.msgType) - 1);
		
		// SMCNo
		GetTLVItem(str, tlv, Tag_SmcNo, Tag_String);
		strncpy(msg.smcCode, str, sizeof(msg.smcCode) - 1);
		
		// State
		GetTLVItem(str, tlv, Tag_Status, Tag_LongInt);
		strncpy(msg.status, str, sizeof(msg.status) - 1);
		
		// SMGNo
		GetTLVItem(str, tlv, Tag_SrcSMGNo, Tag_String);
		strncpy(msg.smgno, str, sizeof(msg.smgno) - 1);
		
		// FWDSMGNo
		GetTLVItem(str, tlv, Tag_DestSMGNo, Tag_String);
		strncpy(msg.fwdsmgno, str, sizeof(msg.fwdsmgno) - 1);
		
		// FeeUserType
		GetTLVItem(str, tlv, Tag_Fee_UserType, Tag_Integer);
		strncpy(msg.feeUserType, str, sizeof(msg.feeUserType) -1);
		
		// FeeType
		GetTLVItem(str, tlv, Tag_FeeType, Tag_Integer);
		strncpy(msg.feeType, str, sizeof(msg.feeType) - 1);
		
		// FeeCode
		GetTLVItem(str, tlv, Tag_FeeCode, Tag_String);
		strncpy(msg.feeCode, str, sizeof(msg.feeCode) - 1);
		
		// FeeAddr
		GetTLVItem(str, tlv, Tag_Fee_termial_id, Tag_String);
		strncpy(msg.feeAddr, str, sizeof(msg.feeAddr) - 1);
		
		// FixedFee
		GetTLVItem(str, tlv, Tag_FixedFee, Tag_String);
		strncpy(msg.fixedFee, str, sizeof(msg.fixedFee) - 1);
		
		// GivenCode
		//	GetTLVItem(str, tlv, Tag_GivenCode, Tag_String);
		//	strncpy(msg.givenCode, str, sizeof(msg.givenCode) - 1);
		
		// MoFlag
		GetTLVItem(str, tlv, Tag_MtFlag, Tag_Integer);
		strncpy(msg.moFlag, str, sizeof(msg.moFlag) - 1);
		
		// AreaCode
		GetTLVItem(str, tlv, Tag_Ex_AreaCode, Tag_String);
		strncpy(msg.areaCode, str, sizeof(msg.areaCode) - 1);
		
		// OrgAddress
		if (pSub->source_addr != NULL)
		{
			str.Empty();
			str.Format("%s",pSub->source_addr);
			str.TrimRight();
			strncpy(msg.orgAddr, str, sizeof(msg.orgAddr) - 1);
		}
		else
		{
			strcpy(msg.orgAddr, "\0");
		}
		
		// DestAddress
		if (pSub->destination_addr != NULL)
		{
			str.Empty();
			str.Format("%s", pSub->destination_addr);
			str.TrimRight();
			strncpy(msg.destAddr, str, sizeof(msg.destAddr) - 1);
		}
		else
		{
			strcpy(msg.destAddr, "\0");
		}
		
		
		if(m_bconvertUTF8)
		{
			int headlen = 0;
			if(pSub->data_coding == 15)
			{//multi byte coding
				std::wstring ws = ::ConvertStringToWString(std::string((char*)(pSub->short_message+headlen),(pSub->sm_length-headlen)));
				auto utf8 = ::ConvertWStringToUTF8(ws);
				auto encodestr = ::EncodeBuff(utf8);
				strcpy(msg.UD, encodestr.c_str());
				// MsgLen
				msg.msgLen = encodestr.size();
				
			}
			else if( (pSub->data_coding&0x0c) == 8)
			{//ucs2 coding
				std::wstring ws = ::ConvertNWStringToWString(std::wstring((wchar_t*)(pSub->short_message+headlen),(pSub->sm_length-headlen)/2));
				auto utf8 = ::ConvertWStringToUTF8(ws);
				auto encodestr = ::EncodeBuff(utf8);
				strcpy(msg.UD, encodestr.c_str());
				// MsgLen
				msg.msgLen = encodestr.size();
			}
			else
			{//default or bin code
				std::wstring ws = ::ConvertStringToWString(std::string((char*)(pSub->short_message+headlen),(pSub->sm_length-headlen)));
				auto utf8 = ::ConvertWStringToUTF8(ws);
				auto encodestr = ::EncodeBuff(utf8);
				strcpy(msg.UD, encodestr.c_str());
				// MsgLen
				msg.msgLen = encodestr.size();
			}
		}
		else
		{
			// UD
			CCodeAndDecode tool;
			char buff[1000 - 1];
			memset(buff, 0, 1000 - 1);
		
			char dcs = pSub->data_coding&0x0c;
			bool udhi = false;
			int headlen = 0;
			if(pSub->esm_class&ESM_CLASS_UDHI)
			{
				udhi = true;
				headlen = pSub->short_message[0] + 1;
				if(headlen >= pSub->sm_length )
					headlen = 0;
			}

			//PrintMessage("msg %d %d",pSub->esm_class,pSub->short_message[0]); 

			//if(pSub->data_coding == 8)
			if(dcs == 8)
				tool.ConvertUCS2ToText((unsigned char *)pSub->short_message+headlen, pSub->sm_length-headlen, buff);
			else if (dcs == 4)
				strcpy(buff,"����������"); 
			else
				strncpy(buff, pSub->short_message+headlen, pSub->sm_length-headlen);
		
			str.Format("%s", buff);
			str.TrimLeft();
			str.TrimRight();
		
			// Filter the content "\r","\n" and "\'" character
			char UD[300];
			memset(UD, 0, sizeof(UD));
			strncpy(UD, str, sizeof(UD));
			char *p = UD;
		
			for (int i = 0; i < sizeof(UD); i++)
			{
				if (*p == '\r' || *p == '\n' || *p == '|' || *p == '\'')
					*p = ' ';
				p++;
			}
		
			strcpy(msg.UD, UD);
			// MsgLen
			msg.msgLen = strlen(buff);

		}
		
		
		
		// priority
		str.Format("%d", pSub->priority_flag);
		strncpy(msg.priority, str, sizeof(msg.priority) - 1);
		
		//SMGW45-17, 2006-2-9, jdz, modi begin//
		// servideID
		GetTLVItem(str, tlv, Tag_ServiceID, Tag_String);
		strncpy(msg.serviceID, str, sizeof(msg.serviceID) - 1);
		//SMGW45-17, 2006-2-9, jdz, modi end//
		
		// ServiceUpID
		GetTLVItem(str, tlv, Tag_ServiceU, Tag_String);
		strncpy(msg.serviceUpID, str, sizeof(msg.serviceUpID) - 1);
		
		// CPID
		GetTLVItem(str, tlv, Tag_Ex_CPID, Tag_String);
		strncpy(msg.spcode, str, sizeof(msg.spcode) - 1);
		
		// PayUserType
		GetTLVItem(str, tlv, Tag_PayUserType, Tag_LongInt);
		TRACE1("PayUserType:%s\n", str);
		msg.payUserType = atoi(str);
		
		// PayFlow
		GetTLVItem(str, tlv, Tag_PayFlow, Tag_LongInt);
		TRACE1("PayFlow:%s\n", str);
		msg.payFlow = atoi(str);
		
		// payBeforeSupport
		GetTLVItem(str, tlv, Tag_PayBeforeSupport, Tag_LongInt);
		TRACE1("payBeforeSupport:%s\n", str);
		msg.payBeforeSupport = atoi(str);
		
		//add by wj
		GetTLVItem(str,tlv,Tag_Ex_SendOrgAddr,Tag_String); //#define Tag_Ex_SendOrgAddr			 0x0304
		strncpy( msg.SendOrgAddr,(LPCTSTR)str,sizeof(msg.SendOrgAddr));

		GetTLVItem(str,tlv,Tag_Ex_SendDestAddr,Tag_String); //#define Tag_Ex_SendDestAddr			 0x030A
		strncpy( msg.SendDestAddr,(LPCTSTR)str,sizeof(msg.SendDestAddr));

		GetTLVItem(str,tlv,Tag_Ex_SendServiceCode,Tag_String); //#define Tag_Ex_SendServiceCode			 0x0305
		strncpy( msg.SendServiceID,(LPCTSTR)str,sizeof(msg.SendServiceID));

		GetTLVItem(str,tlv,Tag_Ex_SendCPID,Tag_String); //#define Tag_Ex_SendCPID			 0x0306
		strncpy( msg.SendCPID,(LPCTSTR)str,sizeof(msg.SendCPID));

		GetTLVItem(str,tlv,Tag_Ex_SourceIpAddr,Tag_String); //#define Tag_Ex_SourceIpAddr		0x0307
		strncpy( msg.SourceIpAdd,(LPCTSTR)str,sizeof(msg.SourceIpAdd));

		GetTLVItem(str,tlv,Tag_Ex_SourceCodeType,Tag_LongInt); //#define Tag_Ex_SourceCodeType		0x0308
		msg.SourceCodeType=atoi(str);

		GetTLVItem(str,tlv,Tag_Ex_ReturnFirst,Tag_LongInt); //#define Tag_Ex_ReturnFirst		0x0309
		msg.ReturnFirst=atoi(str);
		//end add


		//SMGW42-65, 2005-7-22, jdz, modi begin//
		//��Ȩ������
		GetTLVItem(str, tlv, Tag_AuthErrCode, Tag_LongInt);
		int tmp = atoi(str);
		if(tmp >= 700 && tmp <= 767)
		{
			msg.AuthErrCode = tmp - 550;
		}
		else
		{
			msg.AuthErrCode = tmp;
		}
		//?????
		//ת��status������Ϊ��Ȩ������
		int Err = atoi(msg.status);
		if(Err >= 700 && Err <= 767)
		{//statusΪ��Ȩ������ʱ��AuthErrCode��status����һ��
			msg.AuthErrCode = Err - 550;
		}
		
		switch(Err)
		{//statusΪ��Ȩ������ʱ��AuthErrCode��status����һ��
		case 513://������Ϣʱ��������������
			msg.AuthErrCode = 128;
			break;

		case 530://������Ƿ�
			msg.AuthErrCode = 141;
			break;
			
		case 576://SP�����·����¿۷���Ϣ
			msg.AuthErrCode = 145;
			break;
			
		case 577://�ظ����¿۷���Ϣ
			msg.AuthErrCode = 146;
			break;

		case 578://���Ͱ��¿۷���Ϣ֮ǰδ���Ͱ��������Ϣ
			msg.AuthErrCode = 147;
			break;
			
		case 600://��Ϣ�а���Υ����
			msg.AuthErrCode = 148;
			break;
			
		case 601://ҵ��û�д�������״̬
			msg.AuthErrCode = 149;
			break;
		}
		//SMGW42-65, 2005-7-22, jdz, modi end//
		
		
		// paymntFlag		
		GetTLVItem(str, tlv, Tag_PaymntFlag, Tag_LongInt);		
		msg.paymntFlag = atoi(str);
		
		// SMType
		GetTLVItem(str, tlv, Tag_SMType, Tag_Integer);
		TRACE1("SMType:%s\n", str);
		if (str.IsEmpty())
			msg.SMType = 5;
		else
			msg.SMType = atoi(str);
		
		// reportFlag
		sprintf(msg.reportFlag, "%d", pSub->registered_delivery_flag);
		
		
		// FromNo
		GetTLVItem(str, tlv, Tag_FromNo, Tag_String);
		strncpy(msg.FromNo, str, sizeof(msg.FromNo) - 1);
		
		// ToNo
		GetTLVItem(str, tlv, Tag_ToNo, Tag_String);
		strncpy(msg.ToNo, str, sizeof(msg.ToNo) - 1);
		
		//***SMGW40-01, 2004-12-3, jdz, add begin***//
		//AuthPriceID
		GetTLVItem(str, tlv, Tag_AuthPriceID, Tag_String);
		strncpy(msg.AuthPriceID, str, sizeof(msg.AuthPriceID) - 1);
		
		//OperatorType
		GetTLVItem(str, tlv, Tag_OperatorType, Tag_LongInt);
		msg.OperatorType = atoi(str);
		
		//*** SMGW43-03, 2005-09-09, ZF add begin
		GetTLVItem(str, tlv, Tag_GiftAddr, Tag_String);
		strncpy(msg.GiftAddr, str, sizeof(msg.GiftAddr) - 1);
		//*** SMGW43-03, 2005-09-09, ZF add end

//modify by gyx
		GetTLVItem(str, tlv, Tag_Ex_SendGWName, Tag_String);
		strncpy(msg.SendGWName, str, sizeof(msg.SendGWName) - 1);
		msg.RecvID = RecvID;
//end
	}
}
//SMGW42-55, 2005-7-12, jdz, modi end//


//-----SMGW41-39,2005-04-12,zxh,modi begin----//
VOID  CSMFeeThread::SendPaymntRequest(SaveShortMessage data)
{
	smpp_Message_header Header;
	smpp_Message_header * pHeader = &Header; 
	pHeader->Command_ID = ESME_PAYMENT_SM;
	pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(Payment_Request_Sm);
	pHeader->Command_Status = 0;
	pHeader->Sequence_No = m_dwSequence_NO++;
	
	//-----SMGW41-39,2005-04-12,zxh,modi end----//	
	Payment_Request_Sm Sm;
	memset(&Sm, 0, sizeof(Sm));
	
	//***smgw2.5-18, 2004-06-01, jdz, add begin***//
	Sm.FwdMsgType = atoi(data.msgType);
	//***smgw2.5-18, 2004-06-01, jdz, add begin***//
	
	strncpy(Sm.AreaCode, data.areaCode, sizeof(Sm.AreaCode));
	Sm.CDRType = 3;
	strncpy(Sm.ChargeTermID, data.feeAddr, sizeof(Sm.ChargeTermID));
	strncpy(Sm.CPID, data.spcode, sizeof(Sm.CPID));
	
	int FeeType = atoi(data.feeType);
	int MessageType = atoi(data.messageType);
	
	strncpy(Sm.DestMsgID, data.unikey, sizeof(Sm.DestMsgID));
	strncpy(Sm.OrigMsgID, data.sourceID, sizeof(Sm.OrigMsgID));
	strncpy(Sm.Smid, data.smid, sizeof(Sm.Smid));
	
	strncpy(Sm.FeeCode, data.feeCode, sizeof(Sm.FeeCode));
	strncpy(Sm.FeeType , data.feeType, sizeof(Sm.FeeType));
	strncpy(Sm.FixedFee, data.fixedFee, sizeof(Sm.FixedFee));
	strncpy(Sm.FwdSMGNo , data.fwdsmgno, sizeof(Sm.FwdSMGNo));
	
	Sm.MsgLen = data.msgLen;
	Sm.MsgType = htons(atoi(data.messageType));
	

	Sm.Priority = atoi(data.priority);
	strncpy(Sm.ServiceID, data.serviceID, sizeof(Sm.ServiceID));
	strncpy(Sm.SMCNo, data.smcCode, sizeof(Sm.SMCNo));
	strncpy(Sm.SMGNo, data.smgno, sizeof(Sm.SMGNo));
	strncpy(Sm.PreNwkNd, data.FromNo, sizeof(Sm.PreNwkNd));
	strncpy(Sm.NxtNwkNd, data.ToNo, sizeof(Sm.NxtNwkNd));
	
	COleVariant varSubmitTime(data.submitTime);
	COleVariant varDoneTime(data.deliverTime);
	COleDateTime tmSubmitTime(varSubmitTime);
	COleDateTime tmDoneTime(varDoneTime);
	CString strSubmitTime = tmSubmitTime.Format("%Y%m%d%H%M%S");
	CString strDoneTime = tmDoneTime.Format("%Y%m%d%H%M%S");
	strncpy(Sm.SubmitTime, strSubmitTime, sizeof(Sm.SubmitTime));
	strncpy(Sm.DoneTime , strDoneTime, sizeof(Sm.DoneTime));
	
	strncpy(Sm.DestTermID, data.destAddr, sizeof(Sm.DestTermID));
	strncpy(Sm.szOrgAdd, data.orgAddr, sizeof(Sm.szOrgAdd));
	
	Sm.SMType = data.SMType;
	
	char resMsg[sizeof(smpp_Message_header) + sizeof(Payment_Request_Sm)];
	memset(resMsg, 0, sizeof(resMsg));
	
	
	memcpy(resMsg, pHeader, sizeof(smpp_Message_header));
	memcpy(	resMsg + sizeof(smpp_Message_header), &Sm, sizeof(Payment_Request_Sm));
	
	if (((CSmServiceProcApp *)AfxGetApp())->pDrv->Send((smpp_Message_header *)resMsg))
	{
		SendCount++;
	}
	else
	{
		PrintMessage("CSMFeeThread::SendPaymntRequest() send Fail:"
			"Smid=%s, SPID=%s, ServiceID=%s, OrgAddr=%s, DestAddr=%s, SubmitTime=%s",
			data.smid, data.spcode, data.serviceID, data.orgAddr, data.destAddr, data.submitTime);	
	}
}

//��������״̬���� ���� tianhy
//-----SMGW41-39,2005-04-12,zxh,modi begin----//
VOID CSMFeeThread::SendStatusReport( SaveShortMessage data )
//-----SMGW41-39,2005-04-12,zxh,modi end----//
{
	
	char Msg[SMPP_MESSAGE_LEN + 500];
	memset(Msg, 0, sizeof(Msg));
	
	smpp_Message_header* pMsgHeader = (smpp_Message_header*)Msg;
	smpp_Submit_Sm* pMsgSub = (smpp_Submit_Sm*)(Msg + sizeof(smpp_Message_header));
	
	pMsgHeader->Command_ID = ESME_SHREPORT_SM;
	pMsgHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
	pMsgHeader->Command_Status = 0;
	//modify by gyx

	//PrintMessage("SendStatusReport,send out wgname[%s]",data.SendGWName);
	CMainProcMng* pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);
	long int IfId = pMainMng->RequireRoutIf(data.RecvID,data.SendGWName);
	if( IfId == -1 )
	{
		//�Ҳ������ͽӿ�
		PrintMessage("SendStatusReport,send out can not find if");
		return;
	}
	pMsgHeader->nSndID = IfId;
	//end
	//-----SMGW41-39,2005-04-12,zxh,modi begin----//
	//	pMsgHeader->Sequence_No = pHeader->Sequence_No;
	pMsgHeader->Sequence_No = m_dwSequence_NO++;
	//-----SMGW41-39,2005-04-12,zxh,modi end----//
	
	strncpy(pMsgSub->destination_addr, data.orgAddr, sizeof(pMsgSub->destination_addr));
	strncpy(pMsgSub->source_addr, data.destAddr, sizeof(pMsgSub->source_addr));	
	pMsgSub->esm_class = pMsgSub->esm_class | ESM_CLASS_STATUS_REPORT;
	
	Sm_Report * report = (Sm_Report *)pMsgSub->short_message;
	pMsgSub->sm_length = sizeof(Sm_Report);
	
	strncpy((char *)report->Msg_ID, data.smid, sizeof(report->Msg_ID));
	strncpy((char *)report->MSG_SRC_ID, data.sourceID, sizeof(report->MSG_SRC_ID));
	
	COleDateTime tmTime = COleDateTime::GetCurrentTime();
	CString strTime = tmTime.Format(_T("%y%m%d%H%M"));
				
	//SMGW41-24, 2005-3-31, jdz, modi begin//
	CString strSubmitTime(data.submitTime);
	CString strDlvTime(data.deliverTime);
	//SMGW41-24, 2005-3-31, jdz, modi end//
	
	if (strSubmitTime.IsEmpty())
	{
		strncpy((char *)report->Submit_Time, strTime, sizeof(report->Submit_Time));
	}
	else
	{
		COleVariant varSubmitTime(data.submitTime);
		COleDateTime tmSubmitTime(varSubmitTime);
		
		//SMGW41-24, 2005-3-31, jdz, add begin//
		if(tmSubmitTime.GetStatus() == COleDateTime::invalid)
		{
			tmSubmitTime = COleDateTime::GetCurrentTime();
		}
		//SMGW41-24, 2005-3-31, jdz, add end//
		
		CString strSubmitTime = tmSubmitTime.Format(_T("%y%m%d%H%M"));
		strncpy((char *)report->Submit_Time, strSubmitTime, sizeof(report->Submit_Time));
	}
	
	if (strDlvTime.IsEmpty())
	{
		strncpy((char *)report->Done_Time, strTime, sizeof(report->Done_Time));
	}
	else
	{
		COleVariant varDoneTime(data.deliverTime);
		COleDateTime tmDoneTime(varDoneTime);
		
		//SMGW41-24, 2005-3-31, jdz, add begin//
		if(tmDoneTime.GetStatus() == COleDateTime::invalid)
		{
			tmDoneTime = COleDateTime::GetCurrentTime();
		}
		//SMGW41-24, 2005-3-31, jdz, add end//
		
		CString strDoneTime = tmDoneTime.Format(_T("%y%m%d%H%M"));
		strncpy((char *)report->Done_Time, strDoneTime, sizeof(report->Done_Time));
	}
	
	report->Protocol = 0;
	if(strlen(data.unikey)==0 )
	{
		if(data.ReturnFirst==3)
			sprintf(report->StateCode,"GW:%.4d",atoi(data.status));//erro product by send to gw
		else
			sprintf(report->StateCode,"AU:%.4d",atoi(data.status));//erro product by inner
	}
	else
	{
		memcpy(report->StateCode, data.serviceUpID, 7);
	}
	report->state = atoi(data.status);
	strncpy((char *)report->ErrCode, data.status, sizeof(report->ErrCode));
	
	int nMsgType = atoi(data.msgType);
	if (MOC == nMsgType || MOFF == nMsgType)
	{//MO״̬�����ʽ
		report->MOFlag = 1;
	}
	else
	{//MT״̬�����ʽ
		report->MOFlag = 0;
	}
	
	report->SMType = data.SMType;
	report->ReportType = 1;
	report->SubType = atoi(data.messageType);
	
	//SMGW40-11, 2005-3-7, jdz, modi begin//
	sprintf(report->RcvActName, "%s", data.sendAccount);
	sprintf(report->SndActName, "%s", data.rcvAccount);
	//SMGW40-11, 2005-3-7, jdz, modi end//
	
	CString strLen;
	strLen.Format("%03d", data.msgLen);
	
	memcpy(report->Text, (LPCTSTR)strLen, 3);
	memcpy(report->Text + 3, data.UD, 17);
	
	CTLV Tlv;	
	Tlv.AddItem(Tag_SMID, 65, (BYTE*)data.smid);
	Tlv.AddItem(Tag_AuthPriceID, 65, (BYTE*)data.AuthPriceID); 
	Tlv.AddItem(Tag_ServiceID, 10, (BYTE*)data.serviceID);
	//*** SMGW43-03, 2005-09-09, ZF add begin
	Tlv.AddItem(Tag_GiftAddr, 21, (BYTE*)data.GiftAddr);
	//*** SMGW43-03, 2005-09-09, ZF add end	
	Tlv.AddItem(Tag_Fee_termial_id, 21, (BYTE*)data.feeAddr); 
	Tlv.AddItem(Tag_Ex_CPID, 10, (BYTE*)data.spcode); 
	//��չ��Ϣ�����ڴ�������ʱ��ת��Ϊ����
	int iEXMsgType = atoi(data.msgType);
	Tlv.AddLongIntItem(Tag_Ex_MsgType, iEXMsgType); 
	//ת��������ʱ�轫�Ʒ�����ת��Ϊ��ֵ�����ַ�������ʽ�ӵ�TLV��
	int FeeType = atoi(data.feeType);
	Tlv.AddItem(Tag_FeeType, 2, (BYTE*)(&FeeType)); 
	Tlv.AddItem(Tag_FeeCode, 6, (BYTE*)data.feeCode); 
	Tlv.AddItem(Tag_FixedFee, 6, (BYTE*)data.fixedFee); 	
	Tlv.AddLongIntItem(Tag_OperatorType, data.OperatorType); 
	//�Ƿ���Ҫת��״̬�������ڴ�������ʱ��ת��Ϊ����
	int ReportFlag = atoi(data.reportFlag);
	Tlv.AddLongIntItem(Tag_ReportSendFlag, ReportFlag); 
	int Status = atoi(data.status);
	Tlv.AddLongIntItem(Tag_Status, Status);
	
	UL ExtLen = 0;
	Tlv.ToStream((BYTE*)pMsgSub->ext_data, ExtLen);
	pMsgSub->ext_data_len = ExtLen;
	
	if( data.ReturnFirst == 2 )
		pMsgSub->data_coding = 99;
	else
		pMsgSub->data_coding = 15;
	
	//-----SMGW41-38,2005-04-12,zxh,modi begin----//
	if (((CSmServiceProcApp *)AfxGetApp())->pDrv->Send((smpp_Message_header *)Msg))
	{
		SendCount++;
	}
	else
	{
		PrintMessage("CSMFeeThread::SendStatusReport() Send Fail:"
			"Smid=%s, OrgAddr=%s, DestAddr=%s, SPID=%s, ServiceID=%s, SubmitTime=%s",
			data.smid, data.orgAddr, data.destAddr, data.spcode, data.serviceID, data.submitTime);	
	}
	//-----SMGW41-38,2005-04-12,zxh,modi end----//
}

//�����ⲿ��״̬�����ڲ������ת����ȥ tianhy 
VOID CSMFeeThread::TranslateStatusReport(smpp_Message_header *pHeader, smpp_Submit_Sm *pSub, SaveShortMessage data)
{
	char resMsg[SMPP_MESSAGE_LEN + 500];
	memset(resMsg, 0, sizeof(resMsg));
	
	smpp_Message_header* pMsgHeader = (smpp_Message_header*)resMsg;
	smpp_Submit_Sm* pMsgSub = (smpp_Submit_Sm*)(resMsg + sizeof(smpp_Message_header));
	
	pMsgHeader->Command_ID = ESME_SHREPORT_SM;
	pMsgHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
	pMsgHeader->Command_Status = pHeader->Command_Status;
	pMsgHeader->Sequence_No = pHeader->Sequence_No;

	//PrintMessage("TranslateStatusReport,send to gwname[%s]",data.SendGWName);
	//modify by gyx
	CMainProcMng* pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);
	long int IfId = pMainMng->RequireRoutIf(data.RecvID,data.SendGWName);
	if( IfId == -1 )
	{
		//�Ҳ������ͽӿ�
		PrintMessage("TranslateStatusReport,can not find send if");
		return;
	}
	pMsgHeader->nSndID = IfId;
	//end
	//PrintMessage("TranslateStatusReport,get send if[%ld]",IfId);
	strcpy(pMsgSub->service_type, pSub->service_type);
	
	pMsgSub->data_coding = 15;
	pMsgSub->dest_addr_npi = pSub->dest_addr_npi;
	pMsgSub->dest_addr_ton = pSub->dest_addr_ton;
	strcpy(pMsgSub->destination_addr, pSub->destination_addr);
	pMsgSub->esm_class = pSub->esm_class;
	pMsgSub->priority_flag = pSub->priority_flag;
	pMsgSub->protocol_ID = pSub->protocol_ID;
	pMsgSub->registered_delivery_flag = pSub->registered_delivery_flag;
	pMsgSub->replace_if_present_flag = pSub->replace_if_present_flag;
	strcpy(pMsgSub->schedule_delivery_time, pSub->schedule_delivery_time);
	pMsgSub->sm_default_msg_id = pSub->sm_default_msg_id;
	pMsgSub->sm_length = pSub->sm_length;
	strcpy(pMsgSub->source_addr, pSub->source_addr);
	pMsgSub->source_addr_npi = pSub->source_addr_npi;
	pMsgSub->source_addr_ton = pSub->source_addr_ton;
	strcpy(pMsgSub->validity_period, pSub->validity_period);
	
	Sm_Report * pOrgReport = (Sm_Report *)pSub->short_message;
	Sm_Report * report = (Sm_Report *)pMsgSub->short_message;
	memcpy(report, pOrgReport, sizeof(Sm_Report));
	strncpy((char *)report->Msg_ID, data.smid, sizeof(report->Msg_ID));
	strncpy((char *)report->MSG_SRC_ID, data.sourceID, sizeof(report->MSG_SRC_ID));	
	report->ReportType = 0;	
	report->SMType = data.SMType;
	report->SubType = atoi(data.messageType);
	
	//SMGW40-11, 2005-3-7, jdz, modi begin//
	sprintf(report->RcvActName, "%s", data.sendAccount);
	sprintf(report->SndActName, "%s", data.rcvAccount);
	//SMGW40-11, 2005-3-7, jdz, modi end//
	
	int nMsgType = atoi(data.msgType);
	if (MOC == nMsgType || MOFF == nMsgType)
	{//MO״̬�����ʽ
		report->MOFlag = 1;
	}
	else
	{//MT״̬�����ʽ
		report->MOFlag = 0;
	}
	
	//SMGW41-22, 2005-3-30, wzy, add begin//
	CString strLen;
	strLen.Format("%03d", data.msgLen);
	memcpy(report->Text, (LPCTSTR)strLen, 3);
	memcpy(report->Text + 3, data.UD, 17);
	//SMGW41-22, 2005-3-30, wzy, add end//
	
	CTLV Tlv;	
	Tlv.AddItem(Tag_SMID, 65, (BYTE*)data.smid);
	Tlv.AddItem(Tag_ServiceID, 10, (BYTE*)data.serviceID);
	//*** SMGW43-03, 2005-09-09, ZF add begin
	Tlv.AddItem(Tag_GiftAddr, 21, (BYTE*)data.GiftAddr);
	//*** SMGW43-03, 2005-09-09, ZF add end	
	Tlv.AddItem(Tag_AuthPriceID, 65, (BYTE*)data.AuthPriceID); 
	Tlv.AddItem(Tag_Fee_termial_id, 21, (BYTE*)data.feeAddr); 
	Tlv.AddItem(Tag_Ex_CPID, 10, (BYTE*)data.spcode); 
	//ת��������ʱ�轫�Ʒ�����ת��Ϊ��ֵ�����ַ�������ʽ�ӵ�TLV��
	int FeeType = atoi(data.feeType);
	Tlv.AddItem(Tag_FeeType, 2, (BYTE*)(&FeeType)); 
	Tlv.AddItem(Tag_FeeCode, 6, (BYTE*)data.feeCode); 
	Tlv.AddItem(Tag_FixedFee, 6, (BYTE*)data.fixedFee); 
	//��չ��Ϣ�����ڴ�������ʱ��ת��Ϊ����
	int iEXMsgType = atoi(data.msgType);
	Tlv.AddLongIntItem(Tag_Ex_MsgType, iEXMsgType); 
	Tlv.AddLongIntItem(Tag_OperatorType, data.OperatorType); 
	//�Ƿ���Ҫת��״̬�������ڴ�������ʱ��ת��Ϊ����
	int ReportFlag = atoi(data.reportFlag);
	Tlv.AddLongIntItem(Tag_ReportSendFlag, ReportFlag); 
	int Status = atoi(data.status);
	Tlv.AddLongIntItem(Tag_Status, Status);
	
	UL ExtLen = 0;
	Tlv.ToStream((BYTE*)pMsgSub->ext_data, ExtLen);
	pMsgSub->ext_data_len = ExtLen;
//modify gyx
	pMsgHeader->Command_Length += ExtLen;

	
	//-----SMGW41-38,2005-04-12,zxh,modi begin----//
	if (((CSmServiceProcApp *)AfxGetApp())->pDrv->Send((smpp_Message_header *)resMsg))
	{
		SendCount++;
	}
	else
	{
		PrintMessage("CSMFeeThread::TranslateStatusReport() Send Fail:"
			"Smid=%s, OrgAddr=%s, DestAddr=%s, SPID=%s, ServiceID=%s, SubmitTime=%s",
			data.smid, data.orgAddr, data.destAddr, data.spcode, data.serviceID, data.submitTime);	
	}
	//-----SMGW41-38,2005-04-12,zxh,modi end----//
}
//***SMGW40-01, 2004-12-3, jdz, add end***//





//-----SMGW41-39,2005-04-12,zxh,modi begin----//
VOID CSMFeeThread::SendDBBuffer(SaveShortMessage data)
{
	smpp_Message_header Header;
	smpp_Message_header * pHeader = &Header; 	
	pHeader->Command_ID = CMD_ADD_DB_BUFFER;
	pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
	pHeader->Command_Status = 0;
	pHeader->Sequence_No = m_dwSequence_NO++;
	
	//-----SMGW41-39,2005-04-12,zxh,modi begin----//
	
	char DBMsg[SAVE_MESSAGE_LEN + 1];
	memset(DBMsg, 0, sizeof(DBMsg));
	
	memcpy(DBMsg, pHeader, sizeof(smpp_Message_header));
	memcpy(DBMsg + sizeof(smpp_Message_header), &data, sizeof(SaveShortMessage));
	
	// *** SMGW25-32,2004-07-05,wzy modify begin *** //
	if (!((CSmServiceProcApp*)AfxGetApp())->pDBMng->SendMessageToQueue(DBMsg))
	{
		PrintMessage("CSMFeeThread::SendDBBuffer():Send fail:"
			"unikey = %s, smid = %s, destAddr = %s, orgAddr = %s, serviceID = %s, SPID = %s, SubmitTime = %s",
			data.unikey, data.smid, data.destAddr, data.orgAddr, data.serviceID, data.spcode, data.submitTime);
	}
	// *** SMGW25-32,2004-07-05,wzy modify end *** //
}



VOID CSMFeeThread::SendWriteFileBill(SaveShortMessage *pData)
{
	// SMGW42-26, 2005-06-01, wzy add begin //
	// ���°��¿۷ѱ�
	if (IsMTMsg(atoi(pData->msgType))//��������Ϣ
		&& (atoi(pData->feeType) == 2 && atoi(pData->messageType) == 8))
	{
		tagFeeMonthStatus info;
		memset(&info, 0, sizeof(tagFeeMonthStatus));
		
		strncpy(info.SPID, pData->spcode, sizeof(info.SPID));
		strncpy(info.UserAddr, pData->destAddr, sizeof(info.UserAddr));
		strncpy(info.ServiceID, pData->serviceID, sizeof(info.ServiceID));		
		strncpy(info.FeeAddr, pData->feeAddr, sizeof(info.FeeAddr));
		strncpy(info.FeeDate, pData->submitTime, sizeof(info.FeeDate));
		info.Status = atoi(pData->status);
		// SMGW45-07, 2006-1-6, wzy add begin //
		strncpy(info.FixedFee, pData->fixedFee, sizeof(info.FixedFee));
		strncpy(info.SendAccount, pData->sendAccount, sizeof(info.SendAccount));
		// SMGW45-07, 2006-1-6, wzy add end //
		
		SendUpdateFeeMonthInfo(&info);
	}
	// SMGW42-26, 2005-06-01, wzy add end //
	
	
	smpp_Message_header Header;
	smpp_Message_header * pHeader = &Header;
	
	tagSmsSave SM;
	memset(&SM, 0, sizeof(tagSmsSave));
	strncpy(SM.Unikey, pData->unikey, sizeof(SM.Unikey));
	strncpy(SM.Smid, pData->smid, sizeof(SM.Smid));
	strncpy(SM.OrgAddr, pData->orgAddr, sizeof(SM.OrgAddr));
	strncpy(SM.DestAddr, pData->destAddr, sizeof(SM.DestAddr));
	strncpy(SM.submitTime, pData->submitTime, sizeof(SM.submitTime));
	strncpy(SM.deliverTime, pData->deliverTime, sizeof(SM.deliverTime));
	strncpy(SM.UD, pData->UD, sizeof(SM.UD));
	SM.UD[sizeof(SM.UD) - 1] = '\0';
	SM.MsgType = atoi(pData->msgType);
	SM.MessageType = atoi(pData->messageType);
	SM.Status = atoi(pData->status);
	strncpy(SM.serviceID, pData->serviceID, 10);
	SM.FeeUserType = atoi(pData->feeUserType);
	strncpy(SM.FeeType, pData->feeType, sizeof(SM.FeeType));
	strncpy(SM.FeeCode, pData->feeCode, sizeof(SM.FeeCode));
	strncpy(SM.FeeAddr, pData->feeAddr, sizeof(SM.FeeAddr));
	strncpy(SM.RcvAccount, pData->rcvAccount, sizeof(SM.RcvAccount));
	strncpy(SM.SendAccount, pData->sendAccount, sizeof(SM.SendAccount));
	SM.ReportFlag = atoi(pData->reportFlag);
	SM.MoFlag = atoi(pData->moFlag);
	strncpy(SM.GivenCode, pData->givenCode, sizeof(SM.GivenCode));
	strncpy(SM.ServiceUpID, pData->serviceUpID, sizeof(SM.ServiceUpID));
	strncpy(SM.SpCode, pData->spcode, sizeof(SM.SpCode));
	strncpy(SM.FixedFee, pData->fixedFee, sizeof(SM.FixedFee));
	strncpy(SM.SMGNo, pData->smgno, sizeof(SM.SMGNo));
	strncpy(SM.FwdSMGNO, pData->fwdsmgno, sizeof(SM.FwdSMGNO));
	strncpy(SM.SMCNo, pData->smcCode, sizeof(SM.SMCNo));
	strncpy(SM.AreaCode, pData->areaCode, sizeof(SM.AreaCode));
	strncpy(SM.SourceID, pData->sourceID, sizeof(SM.SourceID));
	SM.Priority = atoi(pData->priority);
	
	CString tmp;
	tmp.Format("%d", pData->msgLen);
	SM.MsgLen = atoi(tmp);
	
	SM.PayUserType = pData->payUserType;
	SM.PayFlow = pData->payFlow;
	SM.PayBefore = pData->payBeforeSupport;
	SM.AuthErrCode = pData->AuthErrCode;
	SM.PaymntFlag = pData->paymntFlag;
	SM.SMType = pData->SMType;
	
	strncpy(SM.FromNo, pData->FromNo, sizeof(SM.FromNo));
	strncpy(SM.ToNo, pData->ToNo, sizeof(SM.ToNo));

	//add by wj
	strncpy(SM.SendOrgAddr, pData->SendOrgAddr, sizeof(SM.SendOrgAddr));
	strncpy(SM.SendDestAddr, pData->SendDestAddr, sizeof(SM.SendDestAddr));
	strncpy(SM.SendServiceID, pData->SendServiceID, sizeof(SM.SendServiceID));
	strncpy(SM.SendCPID, pData->SendCPID, sizeof(SM.SendCPID));
	strncpy(SM.SourceIpAdd, pData->SourceIpAdd, sizeof(SM.SourceIpAdd));
	SM.SourceCodeType = pData->SourceCodeType;
	SM.ReturnFirst = pData->ReturnFirst;
	//end add


	pHeader->Command_ID = CMD_SAVE_FILE_BILL;
	pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
	pHeader->Command_Status = 0;
	pHeader->Sequence_No = m_dwSequence_NO++;
	
	char pBuff[SAVE_MESSAGE_LEN + 1];
	memset(pBuff, 0, sizeof(pBuff));
	
	memcpy(pBuff, pHeader, sizeof(smpp_Message_header));
	memcpy(pBuff + sizeof(smpp_Message_header), &SM, sizeof(tagSmsSave));
	
	if (!((CSmServiceProcApp*)AfxGetApp())->pBillFileMng->SendMessageToQueue(pBuff) )
	{
		PrintMessage("CSMFeeThread::SendWriteFileBill() send Fail:"
			"Smid=%s, SPID=%s, ServiceID=%s, OrgAddr=%s, DestAddr=%s, SubmitTime=%s",
			pData->smid, pData->spcode, pData->serviceID, pData->orgAddr, pData->destAddr, pData->submitTime);	
	}
}





BOOL CSMFeeThread::CheckStatusReport(smpp_Submit_Sm *pSub, SaveShortMessage &data)
{
	Sm_Report * report = (Sm_Report *)pSub->short_message;

	sprintf(data.serviceUpID, report->StateCode);
	
	if (CString(report->StateCode) == CString("DELIVRD") && atoi(report->ErrCode) != 0)
	{
		sprintf(data.status, "%s", "550");
		return FALSE;
	}
	else if (CString(report->StateCode) != CString("DELIVRD") && atoi(report->ErrCode) == 0)
	{
		sprintf(data.status, "%s", "551");
		return FALSE;
	}
	else if (CString(report->StateCode) != CString("DELIVRD"))
	{
		sprintf(data.status, "%d", atoi(report->ErrCode));
		return FALSE;
	}
	else
	{
		sprintf(data.status, "0");
		sprintf(data.serviceUpID, "DELIVRD");
		return TRUE;
	}
}


BOOL CSMFeeThread::CheckStatusReport(SaveShortMessage *pData)
{
	if (!(pData->serviceUpID) || pData->serviceUpID[0] == '\0')
		return FALSE;
	
	if (atoi(pData->status) != 0 || stricmp(pData->serviceUpID, "DELIVRD") != 0)
		return FALSE;
	
	return TRUE;
}




BOOL CSMFeeThread::ProcessorReport(LPCTSTR pMsg)
{	
    smpp_Message_header *pHeader = (smpp_Message_header*)pMsg;
    smpp_Submit_Sm *pSub = (smpp_Submit_Sm*)(pMsg + sizeof(smpp_Message_header));
	
	//PrintMessage("ProcessorReport ");
	// Check the report msgid
	CTLV tlv;
	CString IndexMsgID;
	DWORD Pos = 0;
	tlv.FromStream((unsigned char *)pSub->ext_data, pSub->ext_data_len, Pos);	
	GetTLVItem(IndexMsgID, tlv, Tag_UNIKEY, Tag_String);
	
	ASSERT(!IndexMsgID.IsEmpty());
	if (IndexMsgID.IsEmpty())
	{
		PrintMessage("The report message IndexMsgID is null");
		return TRUE;
	}	

	//
	//PrintMessage("ProcessorReport unikey[%s]",(LPCTSTR)IndexMsgID);

	// Find the message from memory
	//SaveShortMessage data;
	auto itr = m_BufferMap.find((LPCTSTR)IndexMsgID);

	//BOOL bFound = m_BufferMap.Lookup(IndexMsgID, data);
	//if (bFound)
	if(itr != m_BufferMap.end())
	{	
		//
		//m_BufferMap.RemoveKey(IndexMsgID);

		auto pdata = itr->second;
		m_BufferMap.erase(itr);
		//KillTimer(data.TimerID);
		
		if(m_DoneTimeSrc)
		{
			//���»����еĴ������ʱ��
			//UpdateDoneTimeFromReport(pSub, data);
			UpdateDoneTimeFromReport(pSub, *pdata);
		}

		//״̬���洦��
		ReportDeal(pSub,pHeader,*pdata);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//-----SMGW42-109,2005-11-10,thy delete end-----//
void CSMFeeThread::SendReportToDB(LPCTSTR pMsg)
{	
    smpp_Message_header *pHeader = (smpp_Message_header*)pMsg;
    smpp_Submit_Sm *pSub = (smpp_Submit_Sm*)(pMsg + sizeof(smpp_Message_header));
	
	//SMGW42-109,2005-11-19,thy modi begin//
	SaveShortMessage data;
	memset(&data, 0, sizeof(data));

	CTLV tlv;
	CString IndexMsgID;
	DWORD Pos = 0;
	tlv.FromStream((unsigned char *)pSub->ext_data, pSub->ext_data_len, Pos);	
	GetTLVItem(IndexMsgID, tlv, Tag_UNIKEY, Tag_String);
	strncpy(data.unikey, IndexMsgID, sizeof(data.unikey) - 1);
	
	//���»����еĴ������ʱ��
	UpdateSubmitTimeFromReport(pSub, data);
	//���»����е��ύʱ��
	UpdateDoneTimeFromReport(pSub, data);

	CheckStatusReport(pSub, data);
	//SMGW42-109,2005-11-19,thy modi end//
	
	char ResMsg[sizeof(smpp_Message_header) + sizeof(SaveShortMessage)];
	memset(ResMsg, 0, sizeof(ResMsg));
	pHeader->Command_ID = CMD_FIND_DB_BUFFER;	
	pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
	memcpy(ResMsg, pHeader, sizeof(smpp_Message_header));
	memcpy(ResMsg + sizeof(smpp_Message_header), &data, sizeof(SaveShortMessage));

	//-----SMGW41-38,2005-04-12,zxh,modi begin----//
	if( !((CSmServiceProcApp*)AfxGetApp())->pDBMng->SendMessageToQueue(ResMsg) )
	{
		PrintMessage("SendReportToDB() ������ϢCMD_FIND_DB_BUFFERʧ��");
	}
	//-----SMGW41-38,2005-04-12,zxh,modi end----//
	
}

void CSMFeeThread::CacheReport(LPCTSTR pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header*)pMsg;
	int size = pHeader->Command_Length;
	size+=sizeof(DWORD)+4;
	char* pCache= new char[size];
	memcpy(pCache+sizeof(DWORD),pMsg,pHeader->Command_Length);
	*((DWORD*)pCache) = GetTickCount();
	m_CacheReportList.AddTail(pCache);
	
}

int CSMFeeThread::GetCacheReport(LPVOID pMsg, int &size)
{
	POSITION pos;
	pos = m_CacheReportList.GetHeadPosition();
	if(pos==NULL)
		return 0;
	char* p = NULL;
	p=(char*)m_CacheReportList.GetAt(pos);
	int msgsize=0;
	smpp_Message_header *pHeader = (smpp_Message_header*)(p+sizeof(DWORD));
	msgsize=pHeader->Command_Length;
	DWORD tick;
	if(msgsize>size)
		return -1;
	tick=GetTickCount()-*((DWORD*)p);
	if(tick >1000)
	{
		size = msgsize;
		memcpy(pMsg,p+sizeof(DWORD),size);
		delete p;
		m_CacheReportList.RemoveAt(pos);
		return 1;
	}
	else
		return 0;
	
	return 0;
}
VOID CSMFeeThread::OnExpireReport(PTRMSG pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header*)pMsg;
	SaveShortMessage data = *(SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	
	
	if(g_IsAskReport == 1)
	{
		SendStatusReport(data);	
	}
	
	//-----SMGW42-109,2005-11-10,thy modi begin-----//
	if (IsMTMsg(atoi(data.msgType)) //��������Ϣ
		&& (atoi(data.feeType) == 2 && atoi(data.messageType) == 8) )//�ǰ��¿۷���Ϣ
	{
		//����ɾ�����¿۷��ص���¼����Ϣ
		SendDelFixedMsg(data);
	}
	//-----SMGW42-109,2005-11-10,thy modi end-----//

	SendWriteFileBill(&data);
}




//�Ʒѵ㲻��SP����ƽ̨�İ��¿۷��ص������Ϣ
void CSMFeeThread::OnCheckFixMsg(PTRMSG pMsg)
{
	smpp_Message_header * pHeader = (smpp_Message_header *)pMsg;
    smpp_Submit_Sm * pSub = (smpp_Submit_Sm *)(pMsg + sizeof(smpp_Message_header));
	

	// Get the deliver message content	
	FEECHECKMSG data;
	memset(&data, 0, sizeof(FEECHECKMSG));
	GetMsgContent(pSub, data);

	long int SequenceNo = pHeader->Sequence_No;
	
	if (0 == m_iMTFCheck)//�����а��¿۷��ص����
	{
		//��ֱ�ӷ��ؽ��
		SendCheckFixMsgResp(SequenceNo, E_SUCCESS,pHeader->nRcvID);
		return;
	}

	//��ҵ�����ת��Ϊ��д
	strupr(data.ServiceID);
	
	//У����¿۷���Ϣ
	int CheckResult = CheckFixMsg(data.SPID, data.ServiceID, data.FeeAddr, data.DestAddr);
	if (E_SUCCESS == CheckResult)
	{
		SendCheckFixMsgResp(SequenceNo, E_SUCCESS,pHeader->nRcvID);
	}
	else
	{
		//�ظ��İ��¿۷���Ϣ
		SendCheckFixMsgResp(SequenceNo, 577,pHeader->nRcvID);
	}
}




//����ɾ�����¿۷��ص���¼����Ϣ
VOID CSMFeeThread::SendDelFixedMsg(SaveShortMessage data)
{
	if (0 == m_iMTFCheck)
	{//���û�����ý����ص����
		return;
	}

	COleVariant varTime(data.submitTime);
	COleDateTime tmTime(varTime);
	if(tmTime.GetStatus() == COleDateTime::invalid)
	{
		tmTime = COleDateTime::GetCurrentTime();
		sprintf(data.submitTime, "%s", tmTime.Format("%Y/%m/%d %H:%M:%S"));
	}	

	char FeeMonth[32];
	memset(FeeMonth,0, sizeof(FeeMonth));
	CalFixMsgFeeMonth(data.submitTime, FeeMonth);

	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();	
	if (pApp->GetMonthFee()->Del(FeeMonth, data.spcode, data.serviceID, data.feeAddr, data.destAddr))
	{
		smpp_Message_header Header;
		smpp_Message_header * pHeader = &Header;
		pHeader->Command_ID = CMD_DELFIXEDMSG;
		pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
		pHeader->Command_Status = 0;
		pHeader->Sequence_No = m_dwSequence_NO++;
		
		
		FIXMSGINFO FixMsg;
		memset(&FixMsg, 0, sizeof(FixMsg));	
		
		strncpy(FixMsg.FeeMonth, FeeMonth, sizeof(FixMsg.FeeMonth));
		strncpy(FixMsg.SPID, data.spcode, sizeof(FixMsg.SPID));
		strncpy(FixMsg.ServiceID, data.serviceID, sizeof(FixMsg.ServiceID));
		strncpy(FixMsg.UserAddr, data.destAddr, sizeof(FixMsg.UserAddr));
		strncpy(FixMsg.FeeAddr, data.feeAddr, sizeof(FixMsg.FeeAddr));
		strncpy(FixMsg.Submittime, data.submitTime, sizeof(FixMsg.Submittime));
		
		char sendbuff[1024];
		memcpy(sendbuff, pHeader, sizeof(smpp_Message_header));
		memcpy(sendbuff + sizeof(smpp_Message_header), &FixMsg, sizeof(FixMsg));
		
		CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
		int nRet = pApp->pDBMng->SendMessageToQueue(sendbuff);
		
		if (0 == nRet)
		{
			PrintMessage("CSMFeeThread::SendDelFixedMsg() send Fail:"
				"SPID=%s, ServiceID=%s, FeeAddr=%s, OrgAddr=%s, DestAddr=%s, SubmitTime=%s, Smid=%s",
				data.spcode, data.serviceID, data.feeAddr, data.orgAddr, data.destAddr, data.submitTime, data.smid);	
		}
	}
}



void CSMFeeThread::OnTickTime(DWORD nTimeID,DWORD param1,BYTE*pParam2,DWORD param2len)
{
	switch(param1)
	{
	case NORMALFLAG:
		//OnNormalTimeOut(pParam2,param2len);
		break;
	case PREPAYFLAG:
		OnPrepayTimeOut(pParam2,param2len);
		break;
	default:
		break;
	}
}

void CSMFeeThread::OnNormalTimeOut(BYTE*pParam2,DWORD param2len)
{
	CString key;
	SaveShortMessage data;




	/*if(!m_BufferMap.Lookup((char*)pParam2,data))
	{
		ASSERT(0);
		return;
	}
	m_BufferMap.RemoveKey((char*)pParam2);*/
	
	
	//***SMGW30-01, 2004-12-29, jdz, modi begin***//
	COleDateTime tmCurrentTime = COleDateTime::GetCurrentTime();
	//***SMGW30-01, 2004-12-29, jdz, modi end***//
	
	//SMGW41-23, 2005-3-31, jdz, modi begin//
	//if( (GetTickCount() - atoi(data.saveTime)) >= (g_AskReportTime * 60 * 1000) )
	if( (GetTickCount() - data.saveTime) >= (g_AskReportTime * 60 * 1000) )
		//SMGW41-23, 2005-3-31, jdz, modi end//	
	{
		sprintf(data.status, "%s", "560");
		sprintf(data.serviceUpID, "%s", "EXPIRED");
		
		//SMGW41-23, 2005-3-31, jdz, modi begin//
		//CString DoneTime = tmCurrentTime.Format("%Y-%m-%d %H:%M:%S");
		CString DoneTime = tmCurrentTime.Format("%Y/%m/%d %H:%M:%S");
		//SMGW41-23, 2005-3-31, jdz, modi end//
		
		sprintf(data.deliverTime, "%s", DoneTime);
		
		//-----SMGW41-39,2005-04-12,zxh,modi begin----//
		//smpp_Message_header header;
		SendWriteFileBill(&data);
		//-----SMGW41-39,2005-04-12,zxh,modi end----//
		
		if(g_IsAskReport == 1)
		{
			SendStatusReport( data);
		}
		
		TRACE1("The Report sended and data deleted:%d\n", m_BufferMap.size());
	}
	//***SMGW30-02, 2004-12-29, jdz, modi begin***//
	else
	{
		//-----SMGW41-39,2005-04-12,zxh,modi begin----//
		SendDBBuffer(data);
		//-----SMGW41-39,2005-04-12,zxh,modi end----//
	}
	//***SMGW30-02, 2004-12-29, jdz, modi end***//
}

void CSMFeeThread::OnPrepayTimeOut(BYTE*pParam2,DWORD param2len)
{
	
	SaveShortMessage data;
	//***SMGW30-05, 2004-12-30, wzy, modi begin***//
	if(m_PrePayMap.Lookup((char*)pParam2,data))
	{//��ʱ
		m_PrePayMap.RemoveKey((char*)pParam2);
		sprintf(data.status, "580");
		sprintf(data.serviceUpID, "UNDELIV");

		//д����
		SendWriteFileBill(&data);
		//��״̬����
		SendStatusReport( data);

		
		//-----SMGW42-109,2005-11-10,thy modi begin-----//
		if (IsMTMsg(atoi(data.msgType)) //��������Ϣ
			&& (atoi(data.feeType) == 2 && atoi(data.messageType) == 8) )//�ǰ��¿۷���Ϣ
		{
			//����ɾ�����¿۷��ص���¼����Ϣ
			SendDelFixedMsg(data);
		}
		//-----SMGW42-109,2005-11-10,thy modi end-----//
	}	
	
	
}

void CSMFeeThread::OnTime()
{
	//����״̬�����ȵ������
	{
		char* pTemp = new char[2000];
		int size = 2000;
		while(GetCacheReport(pTemp,size)>0)
		{
			if(!ProcessorReport(pTemp))
			{
				SendReportToDB(pTemp);
			}
			size=2000;
		}
		delete pTemp;
	}
	
	
	static DWORD lastSaveFileTimer = time(NULL);
	
	if( (time(NULL) - lastSaveFileTimer) > g_SaveFileTime)
	{
		lastSaveFileTimer = time(NULL);
		char FileMsg[SMPP_MESSAGE_LEN];
		memset(FileMsg, 0, sizeof(FileMsg));
		smpp_Message_header header;
		
		header.Command_ID  = CMD_CLEAR_TEMP_FILE;
		header.Command_Length = SMPP_MESSAGE_LEN;
		memcpy(FileMsg, &header, sizeof(smpp_Message_header));
		
		//-----SMGW41-38,2005-04-12,zxh,modi begin----//
		if( !((CSmServiceProcApp*)AfxGetApp())->pBillFileMng->SendMessageToQueue(FileMsg) )
		{
			PrintMessage("OnTime() ������Ϣ1 CMD_CLEAR_TEMP_FILE ʧ��");	
		}
		//-----SMGW41-38,2005-04-12,zxh,modi end----//
	}
	
	
	//�ж�������
	CTime CurrentTime = CTime::GetCurrentTime();
	int Sec = CurrentTime.GetHour() * 3600 + CurrentTime.GetMinute() * 60 + CurrentTime.GetSecond();
	for(int i = 0; i< 48; i++)
	{
		if(Sec == m_SwitchPos[i])
		{
			char FileMsg[SMPP_MESSAGE_LEN];
			memset(FileMsg, 0, sizeof(FileMsg));
			smpp_Message_header header;
			
			header.Command_ID = CMD_CLEAR_TEMP_FILE;
			header.Command_Length = SMPP_MESSAGE_LEN;
			memcpy(FileMsg, &header, sizeof(smpp_Message_header));
			
			//-----SMGW41-38,2005-04-12,zxh,modi begin----//
			if( !((CSmServiceProcApp*)AfxGetApp())->pBillFileMng->SendMessageToQueue(FileMsg) )
			{
				PrintMessage("OnTime() ������Ϣ2 CMD_CLEAR_TEMP_FILE ʧ��");		
			}
			//-----SMGW41-38,2005-04-12,zxh,modi end----//
			
			break;
		}
	}
	
	int count = 0;
	auto itr =  m_time_index.begin();
	while(itr != m_time_index.end())
	{
		auto ptr= itr->lock();
		if(ptr == nullptr)
		{
			//��Чָ�룬ɾ��
			itr=m_time_index.erase(itr);
		}
		else
		{
			if(m_BufferMap.size()>g_MsgMapMaxCount)
			{
				SendDBBuffer(*ptr);
				m_BufferMap.erase(ptr->unikey);
				itr=m_time_index.erase(itr);
			}
			else
			{

				if( (time(nullptr) - ptr->saveTime) >= (g_AskReportTime * 60) )
					//SMGW41-23, 2005-3-31, jdz, modi end//	
				{
					sprintf(ptr->status, "%s", "560");
					sprintf(ptr->serviceUpID, "%s", "EXPIRED");
		
					COleDateTime tmCurrentTime = COleDateTime::GetCurrentTime();
					//SMGW41-23, 2005-3-31, jdz, modi begin//
					//CString DoneTime = tmCurrentTime.Format("%Y-%m-%d %H:%M:%S");
					CString DoneTime = tmCurrentTime.Format("%Y/%m/%d %H:%M:%S");
					//SMGW41-23, 2005-3-31, jdz, modi end//
		
					sprintf(ptr->deliverTime, "%s", DoneTime);
		
					//-----SMGW41-39,2005-04-12,zxh,modi begin----//
					//smpp_Message_header header;
					SendWriteFileBill(ptr.get());
					//-----SMGW41-39,2005-04-12,zxh,modi end----//
		
					if(g_IsAskReport == 1)
					{
						SendStatusReport(*ptr);
					}
		
					TRACE1("The Report sended and data deleted:%d\n", m_BufferMap.size());
					m_BufferMap.erase(ptr->unikey);
					itr=m_time_index.erase(itr);
				}
				else
				{
					break;
				}
			}
						

		}
		count++;
		if(count>10000)
			break;
	}


	//g_MsgMapCount = m_BufferMap.GetCount();
	g_MsgMapCount = m_BufferMap.size();
	g_PreMsgMapCount = m_PrePayMap.GetCount();
	
	CBaseMng::OnTime();
}



BOOL CSMFeeThread::OnMountMessage()
{
	return TRUE;
}


BOOL CSMFeeThread::CaheNormaldata(SaveShortMessage& data)
{
	BOOL rt = FALSE;
	DWORD id = 0xFFFFFFFF;
	//if(m_BufferMap.GetCount() > g_MsgMapMaxCount)
	////if(m_BufferMap.size() > g_MsgMapMaxCount)
	//{
	//	rt = FALSE;
	//}

	//else if((id=SetTimer((DWORD)g_DataExistTime,NORMALFLAG,(BYTE*)data.unikey,strlen(data.unikey))) == 0xFFFFFFFF)
	//{
	//	rt = FALSE;
	//}
	//else
	//{
	//	data.TimerID = id;
	//	m_BufferMap.SetAt(data.unikey, data);
	//	rt = TRUE;
	//}
	
	
	return rt;
}

BOOL CSMFeeThread::CahePrepaydata(SaveShortMessage& data)
{
	char  sUnikey[65];
	memset(sUnikey, 0, sizeof(sUnikey));
	int FeeType = atoi(data.feeType);
	int MessageType = atoi(data.messageType);
	
	sprintf(sUnikey, "%s", data.unikey);
	BOOL rt = FALSE;
	DWORD id = 0xFFFFFFFF;
	if(m_PrePayMap.GetCount() > g_PreMsgMaxMapCount)
	{
		rt = FALSE;
	}
	else if((id=SetTimer(g_PaymentAffirmTimeOut,PREPAYFLAG,(BYTE*)sUnikey,strlen(sUnikey)))==0xFFFFFFFF)
	{
		rt = FALSE;
	}
	else
	{
		data.TimerID = id;
		m_PrePayMap.SetAt(sUnikey, data);
		rt = TRUE;
	}
	
	
	return rt;
}


//add end


void CSMFeeThread::OnClearMemoryMsg(PTRMSG pMsg)
{
	//POSITION pos = m_BufferMap.GetStartPosition();
	DWORD nCount = 0;
	//CString key;
	//SaveShortMessage data;
	
	
	//while (pos != NULL)
	while (m_BufferMap.size())
	{
		//m_BufferMap.GetNextAssoc(pos, key, data);
		//-----SMGW41-39,2005-04-12,zxh,modi begin----//
		//smpp_Message_header header;
		auto itr = m_BufferMap.begin();
		//SendDBBuffer(data);
		SendDBBuffer(*itr->second);
		//-----SMGW41-39,2005-04-12,zxh,modi end----//
		//m_BufferMap.RemoveKey(key);
		m_BufferMap.erase(itr);
		nCount++;
		//if (nCount % 100 == 0)
		//	Sleep(100);
	}
	
}

// SMGW42-26, 2005-06-01, wzy add begin //
void CSMFeeThread::SendUpdateFeeMonthInfo(tagFeeMonthStatus *pInfo)
{
	smpp_Message_header head;
	smpp_Message_header * pHead = &head;
	pHead->Command_ID = CMD_UPDATE_FEEMONTH_STATE;
	pHead->Command_Length = sizeof(smpp_Message_header) + sizeof(tagFeeMonthStatus);
	pHead->Command_Status = 0;
	pHead->Sequence_No = m_dwSequence_NO++;
	
	
	char buff[256];
	memset(buff, 0, sizeof(buff));
	memcpy(buff, pHead, sizeof(smpp_Message_header));
	memcpy(buff + sizeof(smpp_Message_header), pInfo, sizeof(tagFeeMonthStatus));
	
	
	BOOL iRet = ((CSmServiceProcApp *)AfxGetApp())->pDBMng->SendMessageToQueue(buff);
	if (!iRet)
	{
		PrintMessage("CSMFeeThread::SendUpdateFeeMonthInfo() send fail:"
			"SPID=%s, ServiceID=%s, FeeDate=%s, FeeAddr=%s, DestAddr=%s, Status=%d, SendAcnt=%s",
			pInfo->SPID, pInfo->ServiceID, pInfo->FeeDate, pInfo->FeeAddr, pInfo->UserAddr, pInfo->Status, pInfo->SendAccount);	
	}

}
// SMGW42-26, 2005-06-01, wzy add end //


//SMGW42-109,2005-11-19,thy modi begin//

//���棬�ȴ�״̬����
void CSMFeeThread::CacheShortMessage(SaveShortMessage &data)
{
	if(g_OpenCache == OPEN_MEMORY_CACHE_NOT)
	{//û�п����ڴ滺��
		SendDBBuffer(data);
	}
	else
	{
		auto rt = m_BufferMap.emplace(std::make_pair<std::string,std::shared_ptr<SaveShortMessage>>
								(data.unikey,std::make_shared<SaveShortMessage>(data)));


		//SaveShortMessage tmpdata;
 		//if(!m_BufferMap.Lookup(data.unikey,tmpdata))
		if(rt.second)
		{
			/*if(CaheNormaldata(data))
			{
			}
			else
			{
				SendDBBuffer(data);
			}*/
			rt.first->second->saveTime = time(nullptr);

			m_time_index.push_back(rt.first->second);
		}
		else
		{
			//�����ظ���Key
			PrintMessage("CSMFeeThread::CacheShortMessage:The key repeat,"
				"unikey = %s, smid = %s, destAddr = %s, orgAddr = %s, serviceID = %s, SPID = %s, SubmitTime = %s",
				data.unikey, data.smid, data.destAddr, data.orgAddr, data.serviceID, data.spcode, data.submitTime);
		}			
	}		
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//�ж��Ƿ���Ҫ����Ԥ��������
BOOL  CSMFeeThread::NeedSendPrePay(const SaveShortMessage &data)
{
	BOOL bNeedSendPrePay = FALSE;
	if (FEEDEFINE_IN_SPMS == data.paymntFlag)  
	{//�Ʒѵ���SP����ƽ̨
		if (SUPPORT_PREPAY == data.payBeforeSupport) //��Ԥ�������� 
		{//��Ԥ��������
			//Ԥ�����û� ���� �󸶷��û���Ԥ�������� 
			if ((PREPAY_USER == data.payUserType) //Ԥ�����û�
				|| ((NOT_PREPAY_USER == data.payUserType) && (NOTPREPAY_USER_SUPPORT_PREPAY == data.payFlow))) //�󸶷��û����Һ�Ԥ�����û���Ԥ��������
			{
				int FeeType = atoi(data.feeType);
				int FeeCode = atoi(data.feeCode);
				int FixFee = atoi(data.fixedFee);
				int MessageType = atoi(data.messageType);
				if((1 == FeeType && 0 != FeeCode) //�Ʒ�����Ϊ�����۷ѡ�CDR����ʱ���ʷѷ�0
					|| (2 == FeeType && 8 == MessageType && 0 != FixFee) //���¿۷���Ϣ���ʷѷ�0
					|| (3 == FeeType && 0 != FeeCode)//�Ʒ�����Ϊ�ⶥ�Ʒ�ʱ���ʷѷ�0
					)
				{
					//��Ҫ����Ԥ��������
					bNeedSendPrePay = TRUE;
				}
			}
		}
	}

	return bNeedSendPrePay;
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//����Ԥ���ѿ۷�
BOOL CSMFeeThread::SendPrePayMsg(SaveShortMessage &data)
{
	//����Ԥ�������� ����Ԥ���ѿ۷�����
 	data.saveTime = GetTickCount();
	if( CahePrepaydata(data) )
	{
		SendPaymntRequest( data);
		return TRUE;
	}
	else
	{
		// Responsed the error report
		strcpy(data.status, "599");
		strcpy(data.serviceUpID, "UNDELIV");

		//����״̬����
		SendStatusReport( data );
		//д����
		SendWriteFileBill(&data);
		
		int MessageType = atoi(data.messageType);
		int FeeType = atoi(data.feeType);
		if (IsMTMsg(atoi(data.msgType)) //��������Ϣ
			&& (FeeType == 2 && MessageType == 8))//�ǰ��¿۷���Ϣ
		{
			//����ɾ�����¿۷��ص���¼����Ϣ
			SendDelFixedMsg(data);
		}

		PrintMessage("CSMFeeThread::SendPrePayMsg() Cache Fail:"
			"SPID=%s, ServiceID=%s, FeeAddr=%s, OrgAddr=%s, DestAddr=%s, SubmitTime=%s, Smid=%s",
			data.spcode, data.serviceID, data.feeAddr, data.orgAddr, data.destAddr, data.submitTime, data.smid);	


		return FALSE;
	} 	
}
//SMGW42-109,2005-11-19,thy modi end//



//SMGW42-109,2005-11-19,thy modi begin//

//�ж���Ϣ�ļƷ�����
int CSMFeeThread::GetSubmitFeeType(const SaveShortMessage &data)
{
	int MessageType = atoi(data.messageType);
	int FeeType= atoi(data.feeType);

	if (FeeType == 0)
	{
		return FEETYPE_FREE; //���
	}

	if (FeeType == 1 && MessageType != 8)
	{
		return FEETYPE_MSG_PIECE;//����
	}

	if (FeeType == 2 && MessageType != 8)
	{
		return FEETYPE_MSG_MONTH_FREE;//�������
	}

	if (FeeType == 3 )
	{
		return FEETYPE_MSG_TOP_FEE;//�ⶥ
	}

	if (FeeType == 2 && MessageType == 8)
	{
		return FEETYPE_MSG_MONTH_PAY;//���¿۷�
	}

	if (FeeType == 1 && MessageType == 8)
	{
		return FEETYPE_MSG_CDR_BILLS;//CDR����
	}

	return FEETYPE_MSKG_UNKNOWN;
}
//SMGW42-109,2005-11-19,thy modi end//




//�Ʒѵ���SP����ƽ̨ʱ�����¿۷��ص����
int CSMFeeThread::CheckFixMsg(LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pDestAddr)
{
	if(!pSPID || !pServiceID || !pFeeAddr || !pDestAddr)
	{
		ASSERT(0);
		return 577;
	}

	//ȡ��ǰʱ�������Ϣ�Ʒ���
	COleDateTime CurTime = COleDateTime::GetCurrentTime();
	CString sCurTime = CurTime.Format("%Y/%m/%d %H:%M:%S");
	char FeeMonth[32];
	memset(FeeMonth, 0, sizeof(FeeMonth));
	CalFixMsgFeeMonth(sCurTime, FeeMonth);

	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();

	if (pApp->GetMonthFee()->Find(FeeMonth, pSPID, pServiceID, pFeeAddr, pDestAddr))
	{
		//�ظ����¿۷�
		return 577;
	}
	else
	{
		if(pApp->GetMonthFee()->Add(FeeMonth, pSPID, pServiceID, pFeeAddr, pDestAddr))
		{//��ӳɹ������浽���ݿ�
			FIXMSGINFO SM;
			strncpy(SM.FeeMonth, FeeMonth, sizeof(SM.FeeMonth));
			strncpy(SM.SPID, pSPID, sizeof(SM.SPID));
			strncpy(SM.ServiceID, pServiceID, sizeof(SM.ServiceID));
			strncpy(SM.FeeAddr, pFeeAddr, sizeof(SM.FeeAddr));
			strncpy(SM.UserAddr, pDestAddr, sizeof(SM.UserAddr));
			strncpy(SM.Submittime, sCurTime, sizeof(SM.Submittime));
			
			SendSaveFixMsgToDB(&SM);			
		}
		else
		{
			ASSERT(0);
			PrintMessage("��Ӱ��¿۷Ѽ�¼ʧ��, FeeMonth=%s,SPID=%s,ServiceID=%s,FeeAddr=%s,UserAddr=%s",
				FeeMonth, pSPID, pServiceID, pFeeAddr, pDestAddr);
		}

		return E_SUCCESS;
	}

}



//SMGW42-109,2005-11-19,thy modi begin//

//����DELIVER��Ϣ��Ӧ��
void CSMFeeThread::SendDeliverResp(long int Sequence_No,long lSendId)
{
	// Responsed the ack message
    smpp_Message_header ResHeader;

	ResHeader.Command_ID = ESME_DELIVER_SM_RESP;
    ResHeader.Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm_Resp);
    ResHeader.Command_Status = E_SUCCESS;
    ResHeader.Sequence_No = Sequence_No;
	ResHeader.nSndID = lSendId;
	
	if (((CSmServiceProcApp*)AfxGetApp())->pDrv->Send((smpp_Message_header*)&ResHeader))
		g_nRecvDeliverCount++;
	else
	{
		PrintMessage("reply ack error [%d]",lSendId);
	}
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//DELIVER��Ϣ״̬Ϊʧ��ʱ�Ĵ���
void CSMFeeThread::DeliverStatusFail(SaveShortMessage &data)
{
	sprintf(data.serviceUpID, "UNDELIV");

	//SMGW45-06, 2006-1-12, jdz, add begin//	
	int MessageType = atoi(data.messageType);
	int FeeType= atoi(data.feeType);
	int status = atoi(data.status);
//gyx???
	if(data.ReturnFirst != 0)
	{
		SendStatusReport( data);
	}
	else
	if( (status >= 256 && status < 511)
		|| status > 767 //�����ڲ������Ĵ��󣬳�Ԥ���Ѵ�������
		|| 515 == status//�Զ�ACK��ʱ
		|| 516 == status//��Ϣ�ڵȴ������г���������
		|| 517 == status//�ӿڶ���
		|| 518 == status//ͨ��HDRV�����ҵ�IF
		|| 521 == status//��Ч��IF��ID
		)
	{
		//���������Ѿ��ظ��ɹ�ACK��������Ҫ����״̬����
		if(atoi(data.msgType) != FWDPAYMSG 
			&& atoi(data.msgType) != FWDQUERYMSG )
		{//�����ϼ������·���Ԥ���ѿ۷�/��ѯ��Ϣ	
			SendStatusReport( data);
		}

		if (IsMTMsg(atoi(data.msgType)) //��������Ϣ
			&& (FeeType == 2 && MessageType == 8))//�ǰ��¿۷���Ϣ
		{
			//��Ҫ����ɾ�����¿۷��ص���¼����Ϣ
			SendDelFixedMsg(data);
		}
	}
	
	if (541 == status //ϵͳ�������Ĵ���
		&& FEEDEFINE_NOTIN_SPMS == data.paymntFlag //�Ʒѵ㲻��SP����ƽ̨
		&& IsMTMsg(atoi(data.msgType)) //��������Ϣ
		&& (FeeType == 2 && MessageType == 8))//�ǰ��¿۷���Ϣ
	{
		//�Ʒѵ㲻��SP����ƽ̨�İ��¿۷���Ϣ�������ȸ�SP�ظ����ɹ�ACK�����������ص��޳����ڽ���ת��ʱ����ϵͳ����������
		//��Ҫ����״̬����
		if(atoi(data.msgType) != FWDPAYMSG 
			&& atoi(data.msgType) != FWDQUERYMSG )
		{//�����ϼ������·���Ԥ���ѿ۷�/��ѯ��Ϣ	
			SendStatusReport( data);
		}
		//��Ҫ����ɾ�����¿۷��ص���¼����Ϣ
		SendDelFixedMsg(data);
	}
	// SMGW43-29, 2005-11-14, jdz modify end //
	
	if(atoi(data.msgType) == FWDPAYMSG 
		|| atoi(data.msgType) == FWDQUERYMSG )
	{//�ϼ������·���Ԥ���ѿ۷�/��ѯ��Ϣ	
		//ת���ڲ���Ϣת������
		strcpy(data.msgType, "10");
	}
	//SMGW45-06, 2006-1-12, jdz, add end//	
	//д����
	SendWriteFileBill(&data);

	return ;
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//�ж��Ƿ�������Ϣ
BOOL CSMFeeThread::IsMTMsg(int MsgType)
{
	BOOL bIsMTMsg = FALSE;
	
	switch(MsgType) 
	{
	case MT://����CP�������ն�
	case MTC://���CP�������ն�
	case MTF://����CP������ն�
	case MTFF://���CP������ն�
		{//������Ϣ
			bIsMTMsg = TRUE;
		}
	}
	
	return bIsMTMsg;
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//��ȡ����ת��ҵ������
int CSMFeeThread::GetGWTransferMsgType(int MsgType)
{
	switch(MsgType) 
	{
		case MCC://�����ز�ͬ���ĵ�Ե�
		case MCF://�����ն˵�����ն�
		case MCAF://�����ն˵������ն�
		case MCFF://����ն˵�����ն�
		case MC://����ն˵������ն�
		case MCA://�����ն˵������ն�
		case MCAFF://����ն˵������ն�
		case MACFF://�����ն˵�����ն�
	        {//��Ե���Ϣ
				return GWTANSFER_MSGTYPE_P2P;
	        }

	    case MOF://�����ն˵����CP
		case MO://�����ն˵�����CP//�����ն˵��߼�����ģ��
		case MOFF://����ն˵����CP
		case MOC://����ն˵�����CP
	        {//������Ϣ
				return GWTANSFER_MSGTYPE_MO;
	        }

	    case MT://����CP�������ն�
		case MTC://���CP�������ն�
		case MTF://����CP������ն�
		case MTFF://���CP������ն�
	        {//������Ϣ
				return GWTANSFER_MSGTYPE_MT;
	        }

			//SMGW45-06, 2006-1-12, jdz, add begin//
		case FWDPAYMSG://�ϼ������·���Ԥ���ѿ۷���Ϣ
		case FWDQUERYMSG://�ϼ������·���Ԥ���Ѳ�ѯ��Ϣ
			{
				return GWTANSFER_MSGTYPE_FWDPAY;
			}
			//SMGW45-06, 2006-1-12, jdz, add end//

		default:
			{
				return GWTANSFER_MSGTYPE_UN;
			}
	}
	return GWTANSFER_MSGTYPE_UN;
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//DELIVER��Ϣ״̬Ϊ�ɹ�ʱ�Ĵ���
void CSMFeeThread::DeliverStatusSuc(SaveShortMessage &data)
{
	// Mark the message status on "5000" indicated the message need waitting report message
	sprintf(data.status, "5000");

	//�ж���Ϣ����
 	int GWTransMsgType = GetGWTransferMsgType(atoi(data.msgType));
	switch (GWTransMsgType)
	{
	case GWTANSFER_MSGTYPE_P2P://��Ե���Ϣ
		{
			//���棬�ȴ�״̬����
			CacheShortMessage (data);
			return ;
		}
		
	case GWTANSFER_MSGTYPE_MO://������Ϣ
		{
			if(MOSMIsLocalSP(atoi(data.msgType)))
			{//Ŀ�ĺ����Ǳ���SP
				//�ж��Ƿ���Ԥ��������
				if (NeedSendPrePay(data))
				{
					//����Ԥ��������
					SendPrePayMsg(data);
					return ;
				}
				else
				{
					sprintf(data.status, "0");
					sprintf(data.serviceUpID, "DELIVRD");
					
					//����״̬����
					SendStatusReport( data);
					//д����
					SendWriteFileBill(&data); 				
					
					return ;
				}
			}
			else
			{//Ŀ�ĺ��벻�Ǳ���SP
				//���棬�ȴ�״̬����
				CacheShortMessage(data);
				return ;
			}
		}
		
	case GWTANSFER_MSGTYPE_MT://������Ϣ
		{
			int iSubmitFeeType = GetSubmitFeeType(data);
			
			switch(iSubmitFeeType)
			{
			case FEETYPE_MSG_CDR_BILLS://CDR����
				{
					//�жϼƷѵ�
					if(FEEDEFINE_IN_SPMS == data.paymntFlag)
					{//�Ʒѵ���SP����ƽ̨
						//�ж��Ƿ���Ԥ��������
						if (NeedSendPrePay(data))
						{
							//����Ԥ��������
							SendPrePayMsg(data);
							return;
						}
						else
						{
							sprintf(data.status, "0");
							sprintf(data.serviceUpID, "DELIVRD");
							
							//����״̬����
							SendStatusReport( data);								
							//д����
							SendWriteFileBill(&data);
							return;
						}
					}
					else
					{//�Ʒѵ㲻��SP����ƽ̨
						//���棬�ȴ�״̬����
						CacheShortMessage(data);
						return;
					}
					
					break;
				}
				
				
			case FEETYPE_MSG_MONTH_PAY://���¿۷�
				{
					//�жϼƷѵ�
					if(FEEDEFINE_IN_SPMS == data.paymntFlag)
					{//�Ʒѵ���SP����ƽ̨
						//�ж��Ƿ���Ҫ������¿۷��ص����
						if(1 == m_iMTFCheck && atoi(data.fixedFee) != 0)
						{//���¿۷��ص����
							if (E_SUCCESS == CheckFixMsg(data.spcode, data.serviceID, data.feeAddr, data.destAddr))
							{//���¿۷Ѽ��ͨ��
								//�ж��Ƿ���Ԥ��������
								if (NeedSendPrePay(data))
								{
									//����Ԥ��������
									SendPrePayMsg(data);
								}
								else
								{
									sprintf(data.status, "0");
									sprintf(data.serviceUpID, "DELIVRD");
									
									//����״̬����
									SendStatusReport( data);								
									//д����
									SendWriteFileBill(&data);
								}
							}
							else
							{//���¿۷Ѽ�鲻ͨ��
								sprintf(data.status, "577");
								sprintf(data.serviceUpID, "UNDELIV");
								
								//����״̬����
								SendStatusReport( data);								
								//д����
								SendWriteFileBill(&data);
							}

							return;
						}
						else
						{
							//�ж��Ƿ���Ԥ��������
							if (NeedSendPrePay(data))
							{
								//����Ԥ��������
								SendPrePayMsg(data);
								return;
							}
							else
							{
								sprintf(data.status, "0");
								sprintf(data.serviceUpID, "DELIVRD");
								
								//����״̬����
								SendStatusReport( data);								
								//д����
								SendWriteFileBill(&data);
								return;
							}
						}
					}
					else
					{//�Ʒѵ㲻��SP����ƽ̨
						//�жϰ��¿۷ѷ���ģʽ
						if(g_NetMode == MSG_MONTH_FEE_SEND_MODE_PREPAY)
						{//��Ԥ���ѿ۷���Ϣ���ͷ�ʽ
							//��ʱ��û��״̬���淵��
							sprintf(data.status, "0");
							sprintf(data.serviceUpID, "DELIVRD");
							
							//����״̬����
							SendStatusReport(data);								
							//д����
							SendWriteFileBill(&data);
							return;
						}
						else
						{//��submit/forward��Ϣת��
							//�ж��Ƿ�ȴ�״̬����
							if(g_FeeMonthMsgNeedReport == WHETHER_WAIT_REPORT_BACK_NOTWAIT)
							{//���ò��ȴ�״̬���棨�Զ˲�����״̬���棩
								sprintf(data.status, "0");
								sprintf(data.serviceUpID, "DELIVRD");
								
								//����״̬����
								SendStatusReport( data);									
								//д����
								SendWriteFileBill(&data);
								return;
							}
							else
							{//���õȴ�״̬����
								//���棬�ȴ�״̬����
								CacheShortMessage(data);
								return;
							}
						}							
					}
					
					return;
				}
				
			case FEETYPE_FREE://�������
			case FEETYPE_MSG_PIECE: //�����Ʒ�
			case FEETYPE_MSG_MONTH_FREE: //�������
			case FEETYPE_MSG_TOP_FEE: //�ⶥ
				{
					//���棬�ȴ�״̬����
					CacheShortMessage(data);
					return ;
				}
				
			default :
				{
					ASSERT(0);
					PrintMessage("DeliverStatusSuc() ��Ϣ�Ʒ����ʹ���%d", iSubmitFeeType);
					return ;
				}
			}
		}
		
		//SMGW45-06, 2006-1-12, jdz, add begin//
	case GWTANSFER_MSGTYPE_FWDPAY://�ϼ������·���Ԥ���ѿ۷�/��ѯ��Ϣ
		{
			sprintf(data.status, "0");
			sprintf(data.serviceUpID, "DELIVRD");
			
			//ת���ڲ���Ϣת������
			strcpy(data.msgType, "10");
			//д����
			SendWriteFileBill(&data); 	
			
			break;
		}
		//SMGW45-06, 2006-1-12, jdz, add end//
			
	default :
		{
			ASSERT(0);
			PrintMessage("DeliverStatusSuc() ��Ϣת�����ʹ���%d", GWTransMsgType);
			return ;
		}
	}

	return ;
}
//SMGW42-109,2005-11-19,thy modi end//



//SMGW42-109,2005-11-19,thy modi begin//

//���ڴ滺�����ҵ�ԭʼ��Ϣ���״̬���洦��
void CSMFeeThread::ReportDeal(smpp_Submit_Sm *&pSub, smpp_Message_header *&pHeader, SaveShortMessage &data)
{
	// Check the report validate, if error then direct transmit report to sender
	char DeliverTime[21];
	memset(DeliverTime, 0, 21);
	memcpy(DeliverTime, data.deliverTime, 20);

	int MessageType = atoi(data.messageType);
	int FeeType= atoi(data.feeType);

	//���״̬����
 	if (!CheckStatusReport(pSub, data))
	{//״̬����Ϊ�����		
		//ת��״̬����
		TranslateStatusReport(pHeader, pSub, data);
		//д����
		SendWriteFileBill(&data);

		//�ж��Ƿ���Ҫɾ�����¿۷��ص�����¼
		if (IsMTMsg(atoi(data.msgType))//��������Ϣ
			&& (2 == FeeType && 8 == MessageType))//�ǰ��¿۷���Ϣ
		{
			SendDelFixedMsg(data);
		}

		return ;
	}
	else
	{//�ɹ���״̬����
		//��ȡ��Ϣת������
		int GWTransMsgType = GetGWTransferMsgType(atoi(data.msgType));
		switch (GWTransMsgType)
		{
		case GWTANSFER_MSGTYPE_MO: //������Ϣ
			{
				if(!MOSMIsLocalSP(atoi(data.msgType)))
				{//Ŀ�ĺ��벻�Ǳ���SP
					//�ж��Ƿ���Ԥ��������
					if (NeedSendPrePay(data))
					{
						//����Ԥ��������
						SendPrePayMsg(data);
						return ;
					}
					else
					{
						//ת��״̬����
						TranslateStatusReport(pHeader, pSub, data);						
						//д����
						SendWriteFileBill(&data);
						return ;
					}
				}
				else
				{
					//Ŀ�ĺ����Ǳ���SP������£�״̬����������������
					ASSERT(0);
					PrintMessage("CSMFeeThread::ReportDeal()2 ������Ϣ״̬���洦�����̴���,"
						"MsgType=%s, Smid=%s, OrgAddr=%s, DestAddr=%s, FeeAddr=%s, SubmitTime=%s, SPID=%s, ServiceID=%s",
						data.msgType, data.smid, data.orgAddr, data.destAddr, data.feeAddr, data.submitTime, data.spcode, data.serviceID);
				}
			}

		case GWTANSFER_MSGTYPE_P2P://��Ե���Ϣ 
			{
				//�ж��Ƿ���Ԥ��������
				if (NeedSendPrePay(data))
				{
					//����Ԥ��������
					SendPrePayMsg(data);
					return ;
				}
				else
				{
					//ת��״̬����
					TranslateStatusReport(pHeader, pSub, data);					
					//д����
					SendWriteFileBill(&data);
					return ;
				}
			}
			
		case GWTANSFER_MSGTYPE_MT://������Ϣ
			{
				int iSubmitFeeType = GetSubmitFeeType(data);
				
				switch(iSubmitFeeType)
				{					
				case FEETYPE_MSG_CDR_BILLS://CDR����
				case FEETYPE_MSG_MONTH_PAY://���¿۷�
				case FEETYPE_MSG_MONTH_FREE://���������Ϣ
				case FEETYPE_FREE://���������Ϣ
					{
						//ת��״̬����
						TranslateStatusReport(pHeader, pSub, data);						
						//д����
						SendWriteFileBill(&data);
						return ;
					}
					
				case FEETYPE_MSG_PIECE://�����Ʒ�
				case FEETYPE_MSG_TOP_FEE://�ⶥ					
					{
						//�ж��Ƿ���Ԥ��������
						if (NeedSendPrePay(data))
						{
							//����Ԥ��������
							SendPrePayMsg(data);
							return ;
						}
						else
						{
							//ת��״̬����
							TranslateStatusReport(pHeader, pSub, data);						
							//д����
							SendWriteFileBill(&data);
							return ;
						}
					}
					
				default:
					{
						ASSERT(0);
						PrintMessage("DeliverStatusSuc() ��Ϣ�Ʒ����ʹ���%d", iSubmitFeeType);
						return ;
					}
				}	
				
				return;
			}
			
		default:
			{
				ASSERT(0);
				PrintMessage("DeliverStatusSuc() ��Ϣת�����ʹ���%d", GWTransMsgType);
				return;
			}
		}
	}	
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//�����ݿ⻺�����ҵ�ԭʼ����Ϣ���״̬���洦��
void CSMFeeThread::ReportDeal(SaveShortMessage &data)
{
	// Check the report validate, if error then direct transmit report to sender
	char DeliverTime[21];
	memset(DeliverTime, 0, 21);
	memcpy(DeliverTime, data.deliverTime, 20);

	int MessageType = atoi(data.messageType);
	int FeeType= atoi(data.feeType);

	//���״̬����
 	if (!CheckStatusReport(&data))
	{//״̬����Ϊ�����		
		//����״̬����
		SendStatusReport(data);
		//д����
		SendWriteFileBill(&data);

		//�ж��Ƿ���Ҫɾ�����¿۷��ص�����¼
		if (IsMTMsg(atoi(data.msgType))//��������Ϣ
			&& (2 == FeeType && 8 == MessageType))//�ǰ��¿۷���Ϣ
		{
			SendDelFixedMsg(data);
		}

		return ;
	}
	else
	{//�ɹ���״̬����
		//��ȡ��Ϣת������
		int GWTransMsgType = GetGWTransferMsgType(atoi(data.msgType));
		switch (GWTransMsgType)
		{
		case GWTANSFER_MSGTYPE_MO: //������Ϣ
			{
				if(!MOSMIsLocalSP(atoi(data.msgType)))
				{//Ŀ�ĺ��벻�Ǳ���SP
					//�ж��Ƿ���Ԥ��������
					if (NeedSendPrePay(data))
					{
						//����Ԥ��������
						SendPrePayMsg(data);
						return ;
					}
					else
					{
						//����״̬����
						SendStatusReport(data);						
						//д����
						SendWriteFileBill(&data);
						return ;
					}
				}
				else
				{
					//Ŀ�ĺ����Ǳ���SP������£�״̬����������������
					ASSERT(0);
/*
					PrintMessage("CSMFeeThread::ReportDeal()2 ������Ϣ״̬���洦�����̴���,"
						"MsgType=%s, Smid=%s, OrgAddr=%s, DestAddr=%s, FeeAddr=%s, SubmitTime=%s, SPID=%s, ServiceID=%s",
						data.msgType, data.smid, data.orgAddr, data.destAddr, data.feeAddr, data.submitTime, data.spcode, data.serviceID);
*/
				}
			}

		case GWTANSFER_MSGTYPE_P2P://��Ե���Ϣ 
			{
				//�ж��Ƿ���Ԥ��������
				if (NeedSendPrePay(data))
				{
					//����Ԥ��������
					SendPrePayMsg(data);
					return ;
				}
				else
				{
					//����״̬����
					SendStatusReport(data);					
					//д����
					SendWriteFileBill(&data);
					return ;
				}
			}
			
		case GWTANSFER_MSGTYPE_MT://������Ϣ
			{
				int iSubmitFeeType = GetSubmitFeeType(data);
				
				switch(iSubmitFeeType)
				{					
				case FEETYPE_MSG_CDR_BILLS://CDR��������������¼Ʒѵ㲻��SP����ƽ̨��
				case FEETYPE_MSG_MONTH_PAY://���¿۷ѣ���������¼Ʒѵ㲻��SP����ƽ̨��
				case FEETYPE_MSG_MONTH_FREE://���������Ϣ
				case FEETYPE_FREE://���������Ϣ
					{
						//����״̬����
						SendStatusReport(data);						
						//д����
						SendWriteFileBill(&data);
						return ;
					}
					
				case FEETYPE_MSG_PIECE://�����Ʒ�
				case FEETYPE_MSG_TOP_FEE://�ⶥ					
					{
						//�ж��Ƿ���Ԥ��������
						if (NeedSendPrePay(data))
						{
							//����Ԥ��������
							SendPrePayMsg(data);
							return ;
						}
						else
						{
							//����״̬����
							SendStatusReport(data);						
							//д����
							SendWriteFileBill(&data);
							return ;
						}
					}
					
				default:
					{
						ASSERT(0);
						PrintMessage("DeliverStatusSuc() ��Ϣ�Ʒ����ʹ���%d", iSubmitFeeType);
						return ;
					}
				}	
				
				return;
			}
			
		default:
			{
				ASSERT(0);
				PrintMessage("DeliverStatusSuc() ��Ϣת�����ʹ���%d", GWTransMsgType);
				return;
			}
		}
	}	
}
//SMGW42-109,2005-11-19,thy modi end//


//SMGW42-109,2005-11-19,thy modi begin//

//�ж�������Ϣ�Ƿ��Ƿ��͸�����SP��
BOOL CSMFeeThread::MOSMIsLocalSP(int MsgType)
{
	BOOL IsLocalSP = FALSE;

	//SMGW42-110, 2005-12-8, ZF modi begin//
	switch(MsgType)
	{
	case MO://�����ն˵�����CP
	case MOC://����ն˵�����CP
		{//�Ǳ���SP
			IsLocalSP = TRUE;
			break ;
		}

	default:
		{
			IsLocalSP = FALSE;
			break ;
		}
	}
	//SMGW42-110, 2005-12-8, ZF modi end//

	return IsLocalSP;
}
//SMGW42-109,2005-11-19,thy modi end//



//SMGW42-109,2005-11-19,thy modi begin//

//��״̬�����л�ȡ�������ʱ�䣬�����µ�������
void CSMFeeThread::UpdateDoneTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data)
{
	Sm_Report * report = (Sm_Report *)pSub->short_message;
	CString done_time(report->Done_Time);
	
	if (!done_time.IsEmpty())
	{
		sprintf(data.deliverTime,
			"20%s/%s/%s %s:%s:00", 
			done_time.Mid(0, 2), 
			done_time.Mid(2, 2),
			done_time.Mid(4, 2),
			done_time.Mid(6, 2),
			done_time.Mid(8, 2));
	}

	COleVariant varTime(data.deliverTime);
	COleDateTime tmTime(varTime);
	if(tmTime.GetStatus() == COleDateTime::invalid)
	{
		tmTime = COleDateTime::GetCurrentTime();
		sprintf(data.deliverTime, "%s", tmTime.Format("%Y/%m/%d %H:%M:%S"));
	}
}
//SMGW42-109,2005-11-19,thy modi end//



//��״̬�����л�ȡ�ύʱ�䣬�����µ�������
void CSMFeeThread::UpdateSubmitTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data)
{
	Sm_Report * report = (Sm_Report *)pSub->short_message;
	CString submit_time(report->Submit_Time);
	
	if (!submit_time.IsEmpty())
	{
		sprintf(data.submitTime,
			"20%s/%s/%s %s:%s:00", 
			submit_time.Mid(0, 2), 
			submit_time.Mid(2, 2),
			submit_time.Mid(4, 2),
			submit_time.Mid(6, 2),
			submit_time.Mid(8, 2));
	}
	
	COleVariant varTime(data.submitTime);
	COleDateTime tmTime(varTime);
	if(tmTime.GetStatus() == COleDateTime::invalid)
	{
		tmTime = COleDateTime::GetCurrentTime();
		sprintf(data.submitTime, "%s", tmTime.Format("%Y/%m/%d %H:%M:%S"));
	}
}


bool CSMFeeThread::SendSaveFixMsgToDB(const LPFIXMSGINFO  pSM)
{
	smpp_Message_header head;
	smpp_Message_header * pHead = &head;
	pHead->Command_ID = CMD_SAVEFIXEDMSG;
	pHead->Command_Length = sizeof(smpp_Message_header) + sizeof(FIXMSGINFO);
	pHead->Command_Status = 0;
	pHead->Sequence_No = m_dwSequence_NO++;
	
	
	char buff[256];
	memset(buff, 0, sizeof(buff));
	memcpy(buff, pHead, sizeof(smpp_Message_header));
	memcpy(buff + sizeof(smpp_Message_header), pSM, sizeof(FIXMSGINFO));
	
	
	BOOL iRet = ((CSmServiceProcApp *)AfxGetApp())->pDBMng->SendMessageToQueue(buff);
	if (!iRet)
	{
		PrintMessage("CSMFeeThread::SendSaveFixMsgToDB send fail:"
		"FeeMonth=%s, SPID=%s, ServiceID=%s, FeeAddr=%s, UserAddr=%s, SubmitTime=%s,",
		pSM->FeeMonth, pSM->SPID , pSM->ServiceID, pSM->FeeAddr, pSM->UserAddr, pSM->Submittime);	
		return false;
	}

	return true;
}



void CalFixMsgFeeMonth(const char * MsgSubmitTime, char * pFeeMonth)
{
	ASSERT(MsgSubmitTime);
	ASSERT(pFeeMonth);


	COleVariant varMsgTime(MsgSubmitTime);
	COleDateTime tmMsgTime(varMsgTime);
	COleDateTime tmCurTime = COleDateTime::GetCurrentTime();

	if (tmMsgTime.GetStatus() == COleDateTime::invalid)
	{
		//��Ϣʱ�䲻�Ϸ���ȡ��ǰʱ��
		tmMsgTime = tmCurTime;
	}


	
	//��Ϣʱ��
	CString sMsgTime = tmMsgTime.Format(_T("%Y%m%d"));
	//��Ϣ������
	CString sMsgMonth = tmMsgTime.Format(_T("%m"));
	//��Ϣ�����µļƷ���ֹʱ��
	CString sFeeEndTime = tmMsgTime.Format(_T("%Y%m"));
	//��Ϣ�����µļƷ���ֹ����
	CString sEndDate;

	switch (atoi(sMsgMonth))
	{
	case 1:
		sEndDate = g_TimeTab.Jan;
		break;
	case 2:
		sEndDate = g_TimeTab.Feb;
		break;
	case 3:
		sEndDate = g_TimeTab.Mar;
		break;
	case 4:
		sEndDate = g_TimeTab.Apr;
		break;
	case 5:
		sEndDate = g_TimeTab.May;
		break;
	case 6:
		sEndDate = g_TimeTab.Jun;
		break;
	case 7:
		sEndDate = g_TimeTab.Jul;
		break;
	case 8:
		sEndDate = g_TimeTab.Aug;
		break;
	case 9:
		sEndDate = g_TimeTab.Sep;
		break;
	case 10:
		sEndDate = g_TimeTab.Oct;
		break;
	case 11:
		sEndDate = g_TimeTab.Nov;
		break;
	case 12:
		sEndDate = g_TimeTab.Dec;
		break;
	default:
		ASSERT(0);
	}

	sEndDate = sEndDate.Right(2);
	sFeeEndTime += sEndDate;

	
	if (sMsgTime.Compare(sFeeEndTime) <= 0)
	{
		sprintf(pFeeMonth, "%s", sMsgTime.Left(6));
	}
	else
	{
		int nMonth = (atoi(sMsgTime.Mid(4, 2)) + 1);
		int nYear = atoi(sMsgTime.Mid(0, 4));
		if (nMonth > 12)
		{
			nYear += 1;
			nMonth = 1;	
		}

		sprintf(pFeeMonth, "%04d%02d", nYear, nMonth);
	}

}










std::wstring ConvertUTF8ToWString(std::string& sUtf8)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	return std::move(cv.from_bytes(sUtf8));
}

std::string ConvertWStringToUTF8(std::wstring& ws)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.to_bytes(ws);
}

std::string ConvertWStringToString(std::wstring& ws)
{
	std::locale sys_local("");
	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;
	const CodecvtFacet& cvt = std::use_facet<CodecvtFacet>(sys_local);
	std::wstring_convert<CodecvtFacet> cv((CodecvtFacet*)&std::use_facet<CodecvtFacet>(sys_local));
	std::string retstr;
	try {
		retstr = cv.to_bytes(ws);
	}
	catch (...)
	{
	}
	return	retstr;
}

std::wstring ConvertStringToWString(std::string s)
{
	std::locale sys_local("");
	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;
	const CodecvtFacet& cvt = std::use_facet<CodecvtFacet>(sys_local);
	std::wstring_convert<CodecvtFacet> cv((CodecvtFacet*)&std::use_facet<CodecvtFacet>(sys_local));
	return cv.from_bytes(s);
}

std::wstring ConvertWStringToNWString(std::wstring wslocal)
{
	std::wstring nws = wslocal;
	for (int i = 0;i<nws.size();i++)
	{
		nws[i] = htons(nws[i]);
	}
	return nws;
}

std::wstring ConvertNWStringToWString(std::wstring wsnet)
{
	std::wstring wslocal = wsnet;
	for (int i = 0;i<wslocal.size();i++)
	{
		wslocal[i] = ntohs(wslocal[i]);
	}
	return wslocal;
}

#define HEX(x) ( ((char)x) > 9 ? ((char)x) + 55 : ((char)x) + 48)

std::string EncodeBuff(std::string sutf8)
{
	std::string rt;
	for(int i=0;i<sutf8.size();i++)
	{
		char c = sutf8[i];

		switch (c)
		{
			case 0:
			case '\r':
			case '\n':
			case '%':
			case '|':
			{
				rt.push_back('%');
				rt.push_back(HEX(c>>4));
				rt.push_back(HEX(c&0xF));

				break;
			}
			default:
				rt += sutf8[i];
		}
	}
	return rt;
}

unsigned char FromHex(unsigned char x)   
{   
    unsigned char y =0;  
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
    else if (x >= '0' && x <= '9') y = x - '0';  
    else assert(0);  
    return y;  
} ; 

std::string DecodeBuff(const std::string& str)  
{  
    std::string strTemp = "";  
    size_t length = str.length();  
    for (size_t i = 0; i < length; i++)  
    {  
        if(str[i] == '%')  
        {  
            assert(i + 2 < length);  
            unsigned char high = FromHex((unsigned char)str[++i]);  
            unsigned char low = FromHex((unsigned char)str[++i]);  
			strTemp.push_back((char)(high*16 + low));  
        }  
		else strTemp.push_back(str[i]);  
    }  
    return strTemp;  
}  

