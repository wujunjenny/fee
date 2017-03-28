// DBMng.cpp: implementation of the CSMDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMDatabase.h"
#include "LinkObject.h"
//#include "SqlObj.h"
#include "SmServiceProc.h"
#include "Shlwapi.h"
#include "GlobalDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void CalFixMsgFeeMonth(const char * MsgSubmitTime, char * pFeeMonth);

int err_handler(DBPROCESS  *dbproc, int severity, int dberr,int oserr,const char* dberrstr,const char* oserrstr);
int msg_handler (DBPROCESS *dbproc,DBINT msgno,int msgstate,int severity,const char *msgtext,const char *srvname,const char   *procname,DBUSMALLINT line);

CSMDatabase::CSMDatabase()
{		
	LoadConfig();
	InitCommand();

	m_pLink = NULL;
	m_LastExeDBTime = GetTickCount();

	//dberrhandle(err_handler);
	//dbmsghandle (msg_handler);
	
	m_bLoadMonthFee = false;
}


CSMDatabase::~CSMDatabase()
{
    Destroy();

	CSmServiceProcApp * pApp = (CSmServiceProcApp *)AfxGetApp();
	CMainProcMng * pMainMng = (CMainProcMng *)pApp->pMainMng;
	if (m_pLink && pMainMng)
	{
		pMainMng->ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
	}
}


BOOL CSMDatabase::OnMessage(PTRMSG pMsg)
{
	ASSERT(pMsg);
	smpp_Message_header * pMsgHead = (smpp_Message_header *)pMsg;
	
	switch (pMsgHead->Command_ID)
	{
	case CMD_ADD_DB_BUFFER:
		OnAddDBBuffer(pMsg);
		break;
		
	case CMD_FIND_DB_BUFFER:
		OnFindMsgInDBBuffer(pMsg);
		break;

	case CMD_SAVEFIXEDMSG:
		OnSaveFixMsg(pMsg);
		break;
		
	case CMD_DELFIXEDMSG:
		OnDeleteFixMsg(pMsg);
		break;
		
	case CMD_UPDATE_FEEMONTH_STATE:
		OnUpdateFeeMonthTable(pMsg);
		break;
		
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}


BOOL CSMDatabase::SendMessageToQueue(PTRMSG pMsg) // 消息进入函数
{
	
	ASSERT(pMsg);
	if(!pMsg)
		return FALSE;
	smpp_Message_header* pMsgHead = (smpp_Message_header*)pMsg;

	return SendMessage(pMsg, pMsgHead->Command_Length);
	
}


void CSMDatabase::OnAddDBBuffer(LPCTSTR pMsg)
{
	m_LastExeDBTime = GetTickCount();

	smpp_Message_header * pHeader = (smpp_Message_header *)pMsg;
	SaveShortMessage  *pSaveSm = (SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	
	if (!GetDBLink())
	{	
		PrintMessage("Link is NULL On Call OnAddDBBuffer");
		char stmp[5000];
		memset(stmp, 0, 5000);
		DumpSM(stmp, 5000, pSaveSm);
		PrintMessage(stmp);
		return;
	}
	
	switch(m_pLink->GetSrcType())
	{
	case ADO_LIB:
		break;
	case MS_SQL_DB_LIB:
		break;
	default:
		break;
	}
	
	
	CAdoObj * pDbObj = (CAdoObj *)m_pLink;
	m_pAddSMBuffCmd->ActiveConnection = pDbObj->m_pConnection;
	
	long ColIndex = 0;
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->unikey));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->smid));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->orgAddr));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->destAddr));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->submitTime));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->deliverTime));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->UD));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->msgType));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->messageType));	
	CString str;
	str.Format("%d", atoi(pSaveSm->status));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(str));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->serviceID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->feeUserType));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->feeType));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->feeCode));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->feeAddr));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->rcvAccount));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->sendAccount));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->reportFlag));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->moFlag));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->givenCode));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->serviceUpID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->spcode));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->fixedFee));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->smgno));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->fwdsmgno));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->smcCode));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->areaCode));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->sourceID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->priority));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->msgLen);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->payUserType);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->payFlow);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->payBeforeSupport);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->AuthErrCode);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->paymntFlag);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->SMType);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->FromNo));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->ToNo));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->AuthPriceID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->OperatorType);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(""));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->GiftAddr));

//modify by gyx
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SendOrgAddr));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SendDestAddr));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SendServiceID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SendCPID));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SourceIpAdd));
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)pSaveSm->SourceCodeType);
	m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pSaveSm->SendGWName));
//end

	try
	{	
		m_pAddSMBuffCmd->CommandText = (_bstr_t)(LPCSTR)m_strAddSMBuff;
		m_pAddSMBuffCmd->CommandType = adCmdStoredProc;
		m_pAddSMBuffCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	
	catch(_com_error)
	{	
		PrintProviderError("Execute OnAddDBBuffer error", pDbObj->m_pConnection );	
		char stmp[8000];
		memset(stmp, 0, 8000);
		DumpSM(stmp, 8000, pSaveSm);
		PrintMessage(stmp);
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
		g_nConnDBTimeOutCount++;
		return;
	}
	
	
	try
	{
		_variant_t ret = m_pAddSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->GetValue();
		int IsRepeat = ret.intVal;
		
		//如写入数据库缓存失败
		if (0 == IsRepeat)
		{
			PrintMessage("OnAddDBBuffer() 写入数据库缓存失败");
			return;
		}	
		
	}

	catch (_com_error)
	{
		PrintMessage("OnAddDBBuffer() 从记录集中获取某些字段时失败");
		return;
	}
}


void CSMDatabase::OnFindMsgInDBBuffer(LPCTSTR pMsg)
{
	m_LastExeDBTime = GetTickCount();
	
    smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	SaveShortMessage *pData = (SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	
	if (!GetDBLink())
	{
		ASSERT(0);
		PrintMessage("The m_pLink is null错误:Report <MsgID:%s,SrcAddr:%s,DestAddr:%s, SubmitTime:%s, DoneTime:%s, Stat:%s>\n",
			pData->unikey, pData->orgAddr, pData->destAddr, pData->submitTime, pData->deliverTime, pData->serviceUpID);
		return;
	}
		
	// 从数据库中找回消息
	CString sql;
	_RecordsetPtr rs;
	CString sub_time, done_time, err_code;
	sub_time = pData->submitTime;
	done_time = pData->deliverTime;
	err_code = pData->serviceUpID;
	
	COleDateTime tmTime = COleDateTime::GetCurrentTime();
	if (sub_time.IsEmpty())
		sub_time = tmTime.Format("%Y/%m/%d %H:%M:%S");
	if (done_time.IsEmpty())
		done_time = tmTime.Format("%Y/%m/%d %H:%M:%S");
	
	switch(m_pLink->GetSrcType())
	{
	case ADO_LIB:
		break;
	case MS_SQL_DB_LIB:
		break;
	default:
		break;
		
	}
	

	CAdoObj * pDbObj = (CAdoObj *)m_pLink;
	m_pFindSMBuffCmd->ActiveConnection = pDbObj->m_pConnection;


	_RecordsetPtr pRs;
	long ColIndex = 0;
	m_pFindSMBuffCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pData->unikey));
	
	
	try
	{	
		m_pFindSMBuffCmd->CommandText = (_bstr_t)(LPCSTR)m_strFindSMBuff;
		m_pFindSMBuffCmd->CommandType = adCmdStoredProc;
		pRs = m_pFindSMBuffCmd->Execute(NULL , NULL , adCmdStoredProc);
	}
	
	catch (_com_error)
	{
		PrintProviderError( "Execute OnFindMsgInDBBuffer error", pDbObj->m_pConnection );	
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
		return;
	}
	
	
	try
	{	
		if ((pRs != NULL) && (pRs->State == adStateOpen) && (pRs->adoEOF == FALSE))
		{
			SaveShortMessage data;
			memset(&data, 0, sizeof(data));
			
			// 取数据库中消息的内容
			CString strFieldValue;
			_variant_t varValue;

			varValue = pRs->GetCollect("FSmid");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.smid, strFieldValue, sizeof(data.smid) - 1);
			
			varValue = pRs->GetCollect("FUnikey");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.unikey, strFieldValue, sizeof(data.unikey) - 1);
			
			
			varValue = pRs->GetCollect("FSourceID");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.sourceID, strFieldValue, sizeof(data.sourceID) - 1);
			
			varValue = pRs->GetCollect("FOrgAddr");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.orgAddr, strFieldValue, sizeof(data.orgAddr) - 1);
			
			varValue = pRs->GetCollect("FDestAddr");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.destAddr, strFieldValue, sizeof(data.destAddr) - 1);
			
			varValue = pRs->GetCollect("FSUBMITTIME");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.submitTime, strFieldValue, sizeof(data.submitTime) - 1);
			
			if(m_DoneTimeSrc)
			{
				//取文本话单格式中处理结束时间字段的值取状态报告中的done_time时间
				strncpy(data.deliverTime, done_time, sizeof(data.deliverTime) - 1);
			}
			else
			{
				//取文本话单格式中处理结束时间字段的值按协议方式取值
				varValue = pRs->GetCollect("FDeliverTime");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.deliverTime, strFieldValue, sizeof(data.deliverTime) - 1);
			}
			
			varValue = pRs->GetCollect("FFeeType");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;	
			strFieldValue.TrimRight();
			strncpy(data.feeType, strFieldValue, sizeof(data.feeType) - 1);
			
			varValue = pRs->GetCollect("FFeeAddr");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;	
			strFieldValue.TrimRight();
			strncpy(data.feeAddr, strFieldValue, sizeof(data.feeAddr) - 1);
			
			varValue = pRs->GetCollect("FMessageType");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.messageType, strFieldValue, sizeof(data.messageType) - 1);
			
			varValue = pRs->GetCollect("FMsgType");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.msgType, strFieldValue, sizeof(data.msgType) - 1);
			
			varValue = pRs->GetCollect("FServiceID");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.serviceID, strFieldValue, sizeof(data.serviceID) - 1);
			
			
			strncpy(data.serviceUpID, err_code, sizeof(data.serviceUpID) - 1);
			varValue = pRs->GetCollect("FCPID");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.spcode, strFieldValue, sizeof(data.spcode) - 1);
			
			varValue = pRs->GetCollect("FFeeCode");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.feeCode, strFieldValue, sizeof(data.feeCode) - 1);
			
			varValue = pRs->GetCollect("FFixedFee");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.fixedFee, strFieldValue, sizeof(data.fixedFee) - 1);
			
			varValue = pRs->GetCollect("FPriority");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.priority, strFieldValue, sizeof(data.priority) - 1);
			
			varValue = pRs->GetCollect("FAreaCode");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.areaCode, strFieldValue, sizeof(data.areaCode) - 1);
			
			varValue = pRs->GetCollect("FSMGNo");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.smgno, strFieldValue, sizeof(data.smgno) - 1);
			
			varValue = pRs->GetCollect("FSendAccount");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.sendAccount, strFieldValue, sizeof(data.sendAccount) - 1);
			
			varValue = pRs->GetCollect("FRcvAccount");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.rcvAccount, strFieldValue, sizeof(data.rcvAccount) - 1);
			
			varValue = pRs->GetCollect("FFwdSMGNo");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.fwdsmgno, strFieldValue, sizeof(data.fwdsmgno) - 1);
			
			varValue = pRs->GetCollect("FReportFlag");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.reportFlag, strFieldValue, sizeof(data.reportFlag) - 1);
			
			varValue = pRs->GetCollect("FGivenCode");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.givenCode, strFieldValue, sizeof(data.givenCode) - 1);
			
			varValue = pRs->GetCollect("FSMCNo");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.smcCode, strFieldValue, sizeof(data.smcCode) - 1);
			
			varValue = pRs->GetCollect("FMsgLen");
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			data.msgLen = atoi(strFieldValue);
			
			varValue = pRs->GetCollect("FPayUserType");
			if (varValue.vt != VT_NULL)
				data.payUserType = varValue.lVal;
			
			varValue = pRs->GetCollect("FPayFlow");
			if (varValue.vt != VT_NULL)
				data.payFlow = varValue.lVal;
			
			varValue = pRs->GetCollect("FPayBefore");
			if (varValue.vt != VT_NULL)
				data.payBeforeSupport = varValue.lVal;
			
			varValue = pRs->GetCollect("FPaymntFlag");
			if (varValue.vt != VT_NULL)
				data.paymntFlag = varValue.lVal;
			
			varValue = pRs->GetCollect("FSMType");
			if (varValue.vt != VT_NULL)
				data.SMType = varValue.lVal;
			
			varValue = pRs->GetCollect("FFromNo");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.FromNo, strFieldValue, sizeof(data.FromNo) - 1);
			
			varValue = pRs->GetCollect("FToNo");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.ToNo, strFieldValue, sizeof(data.ToNo) - 1);

			strncpy(data.status, pData->status, sizeof(data.status) - 1);

			
			varValue = pRs->GetCollect("FUD");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.UD, strFieldValue, sizeof(data.UD) - 1);
			
			
	
			varValue = pRs->GetCollect("FAuthPriceID");
			if (varValue.vt != VT_NULL)
			{
				strFieldValue = varValue.bstrVal;
			}
			strFieldValue.TrimRight();
			strncpy(data.AuthPriceID, strFieldValue, sizeof(data.AuthPriceID) - 1);
			
			varValue = pRs->GetCollect("FOperatorType");
			if (varValue.vt != VT_NULL)
				data.OperatorType = varValue.lVal;
			
			varValue = pRs->GetCollect("FGiftAddr");
			if (varValue.vt != VT_NULL)
			{
				strFieldValue = varValue.bstrVal;
			}
			strFieldValue.TrimRight();
			strncpy(data.GiftAddr, strFieldValue, sizeof(data.GiftAddr) - 1);

//modify by gyx
			varValue = pRs->GetCollect("SendOrgAddr");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SendOrgAddr, strFieldValue, sizeof(data.SendOrgAddr) - 1);	


			varValue = pRs->GetCollect("SendDestAddr");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SendDestAddr, strFieldValue, sizeof(data.SendDestAddr) - 1);	

			varValue = pRs->GetCollect("SendServiceID");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SendServiceID, strFieldValue, sizeof(data.SendServiceID) - 1);	

			varValue = pRs->GetCollect("SendCPID");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SendCPID, strFieldValue, sizeof(data.SendCPID) - 1);	


			varValue = pRs->GetCollect("SourceIpAdd");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SourceIpAdd, strFieldValue, sizeof(data.SourceIpAdd) - 1);	

			varValue = pRs->GetCollect("SendGWName");
			strFieldValue.Empty();
			if (varValue.vt != VT_NULL)
				strFieldValue = varValue.bstrVal;
			strFieldValue.TrimRight();
			strncpy(data.SendGWName, strFieldValue, sizeof(data.SendGWName) - 1);	

			varValue = pRs->GetCollect("SourceCodeType");
			if (varValue.vt != VT_NULL)
				data.SourceCodeType = varValue.lVal;

			data.RecvID = 0;
//end

			if (atoi(data.status) == 560)
			{
				return;
			}

			//发送消息到SMS线程中处理
			SendMsgToSMSThread((char *)&data);

		}
		else
		{
			PrintMessage("The message MsgID not found(LoolUp)错误:Report<MsgID:%s,SrcAddr:%s,DestAddr:%s, SubmitTime:%s, DoneTime:%s, Stat:%s>\n",
				pData->unikey, pData->orgAddr, pData->destAddr, pData->submitTime, pData->deliverTime, pData->serviceUpID);
		}	
	}

	catch (_com_error)
	{
		PrintMessage("OnFindMsgInDBBuffer() 从记录集中获取某些字段时出错");
		return;
	}
		
}



void CSMDatabase::ReturnLink(CLinkObject *pLink,BOOL bOK)
{
	CMainProcMng *pMainMng = (CMainProcMng *)(((CSmServiceProcApp *)AfxGetApp())->pMainMng);
	pMainMng->ReturnLink(pLink, bOK);
}



void CSMDatabase::PrintProviderError(LPCTSTR pszMsg, _ConnectionPtr pConnection)
{
	HRESULT  hr = S_OK;
    _bstr_t  strError;
    ErrorPtr  pErr = NULL;
	
    try
    {
        long nCount = pConnection->Errors->Count;
		
        for(long i = 0; i < nCount; i++)
        {
            pErr = pConnection->Errors->GetItem(i);
            TRACE("Error #%d\n", pErr->Number);
            TRACE(" %s\n",(LPCSTR)pErr->Description);
            TRACE(" (Source: %s)\n",(LPCSTR)pErr->Source);
            TRACE(" (SQL State: %s)\n",(LPCSTR)pErr->SQLState);
            TRACE(" (NativeError: %d)\n",(LPCSTR)pErr->NativeError);
			
			PrintMessage("%s Error#%d %s\n",pszMsg,pErr->Number,(LPCSTR)pErr->Description);
        }
    }

    catch (_com_error &e)
    {
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
    }
};


void CSMDatabase::InitCommand()
{
	m_pAddSMBuffCmd.CreateInstance( "ADODB.Command");
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("unikey"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("smid"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("orgAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("destAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("submitTime"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("deliverTime"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("UD"), adBSTR, adParamInput, 300, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("msgType"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("messageType"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("status"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("serviceID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("feeUserType"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("feeType"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("feeCode"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("feeAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("rcvAccount"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("sendAccount"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("reportFlag"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("MoFlag"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("givenCode"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("serviceUpID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("spcode"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("fixedFee"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SMGNo"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("FwdSMGNo"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SMCNo"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("AreaCode"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SourceID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("priority"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("msgLen"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("payUserType"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("payFlow"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("payBefore"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("AuthErrCode"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("paymntFlag"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SMType"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("FromNo"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("ToNo"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("AuthPriceID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("OperatorType"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("OrderServiceID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("GiftAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
//modify by gyx
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SendOrgAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SendDestAddr"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SendServiceID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SendCPID"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SourceIpAdd"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SourceCodeType"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("SendGWName"), adBSTR, adParamInput, 100, _variant_t(1234L)));
//end

	m_pAddSMBuffCmd->GetParameters()->Append(m_pAddSMBuffCmd->CreateParameter(_bstr_t("ret"), adInteger, adParamOutput, 100, _variant_t(1234L)));

	m_pFindSMBuffCmd.CreateInstance( "ADODB.Command");
	m_pFindSMBuffCmd->GetParameters()->Append(m_pFindSMBuffCmd->CreateParameter(_bstr_t("Unikey"), adBSTR, adParamInput, 100, _variant_t(1234L)));
	m_pFindSMBuffCmd->GetParameters()->Append(m_pFindSMBuffCmd->CreateParameter(_bstr_t("ret"), adInteger, adParamOutput, 100, _variant_t(1234L)));


	m_pLoadFixMsgCmd.CreateInstance("ADODB.Command");
	m_pLoadFixMsgCmd->GetParameters()->Append(m_pLoadFixMsgCmd->CreateParameter(_bstr_t("FeeMonth"), adBSTR, adParamInput, 7, _variant_t(1234L)));
	
	m_pSaveFixMsgCmd.CreateInstance("ADODB.Command");
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("FeeMonth"), adBSTR, adParamInput, 10, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("SPID"), adBSTR, adParamInput, 11, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("ServiceID"), adBSTR, adParamInput, 11, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("FeeAddr"), adBSTR, adParamInput, 22, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("UserAddr"), adBSTR, adParamInput, 22, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("SubmitTime"), adBSTR, adParamInput, 30, _variant_t(1234L)));
	m_pSaveFixMsgCmd->GetParameters()->Append(m_pSaveFixMsgCmd->CreateParameter(_bstr_t("ret"), adInteger, adParamOutput, 4, _variant_t(1234L)));
	
	m_pDelFixMsgCmd.CreateInstance("ADODB.Command");
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("FeeMonth"), adBSTR, adParamInput, 10, _variant_t(1234L)));
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("SPID"), adBSTR, adParamInput, 11, _variant_t(1234L)));
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("ServiceID"), adBSTR, adParamInput, 11, _variant_t(1234L)));
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("FeeAddr"), adBSTR, adParamInput, 22, _variant_t(1234L)));
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("UserAddr"), adBSTR, adParamInput, 22, _variant_t(1234L)));
	m_pDelFixMsgCmd->GetParameters()->Append(m_pDelFixMsgCmd->CreateParameter(_bstr_t("ret"), adInteger, adParamOutput, 4, _variant_t(1234L)));

	m_pUpdateMonthFeeCmd.CreateInstance( "ADODB.Command");
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("mobile"), adBSTR, adParamInput, 22, _variant_t("")));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("UserAddr"), adBSTR, adParamInput, 22, _variant_t("")));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("entcode"), adBSTR, adParamInput, 11, _variant_t("")));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("srvcode"), adBSTR, adParamInput, 11, _variant_t("")));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("feedate"), adBSTR, adParamInput, 20, _variant_t("")));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("Status"), adBSTR, adParamInput, 10, _variant_t(0L)));
	// SMGW45-07, 2006-1-6, wzy add begin //	
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("FixedFee"), adBSTR, adParamInput, 7, _variant_t(0L)));
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("SendAccount"), adBSTR, adParamInput, 30, _variant_t(0L)));
	// SMGW45-07, 2006-1-6, wzy add end //
	m_pUpdateMonthFeeCmd->GetParameters()->Append(m_pUpdateMonthFeeCmd->CreateParameter(_bstr_t("ret"), adInteger, adParamOutput, 4, _variant_t(0L)));
}




void CSMDatabase::DumpSM(LPSTR pData, int len, SaveShortMessage *pSM)
{
	CString s, str;
	str.Format ("<unikey:%s,",pSM->smid);
	s += str;
	str.Format ("smid:%s,",pSM->smid);
	s += str;
	str.Format ("orgAddr:%s,",pSM->orgAddr);
	s += str;
	str.Format ("destAddr:%s,",pSM->destAddr);
	s += str;
	str.Format ("submitTime:%s,",pSM->submitTime);
	s += str;
	str.Format ("deliverTime:%s,",pSM->deliverTime);
	s += str;
	str.Format ("UD:%s,",pSM->UD);
	s += str;
	str.Format ("msgType:%s,",pSM->msgType);
	s += str;
	str.Format ("messageType:%s,",pSM->messageType);
	s += str;
	str.Format ("status:%s,",pSM->status);
	s += str;
	str.Format ("serviceID:%s,",pSM->serviceID);
	s += str;
	str.Format ("feeUserType:%s,",pSM->feeUserType);
	s += str;
	str.Format ("feeType:%s,",pSM->feeType);
	s += str;
	str.Format ("feeCode:%s,",pSM->feeCode);
	s += str;
	str.Format ("feeAddr:%s,",pSM->feeAddr);
	s += str;
	str.Format ("rcvAccount:%s,",pSM->rcvAccount);
	s += str;
	str.Format ("sendAccount:%s,",pSM->sendAccount);
	s += str;
	str.Format ("reportFlag:%s,",pSM->reportFlag);
	s += str;
	str.Format ("moFlag:%s,",pSM->moFlag);
	s += str;
	str.Format ("givenCode:%s,",pSM->givenCode);
	s += str;
	str.Format ("serviceUpID:%s,",pSM->serviceUpID);
	s += str;
	str.Format ("spcode:%s,",pSM->spcode);
	s += str;
	str.Format ("fixedFee:%s,",pSM->fixedFee);
	s += str;
	str.Format ("smgno:%s,",pSM->smgno);
	s += str;
	str.Format ("fwdsmgno:%s,",pSM->fwdsmgno);
	s += str;
	str.Format ("smcCode:%s,",pSM->smcCode);
	s += str;
	str.Format ("areaCode:%s,",pSM->areaCode);
	s += str;
	str.Format ("sourceID:%s,",pSM->sourceID);
	s += str;
	str.Format ("priority:%s,",pSM->priority);
	s += str;
	str.Format ("msgLen:%d,",pSM->msgLen);
	s += str;
	str.Format ("payUserType:%d,",pSM->payUserType);
	s += str;
	str.Format ("payFlow:%d,",pSM->payFlow);
	s += str;
	str.Format ("payBeforeSupport:%d,",pSM->payBeforeSupport);
	s += str;
	str.Format ("AuthErrCode:%d,",pSM->AuthErrCode);
	s += str;
	str.Format ("paymntFlag:%d,",pSM->paymntFlag);
	s += str;
	str.Format ("SMType:%d,",pSM->SMType);
	s += str;
	str.Format ("FromNo:%s,",pSM->FromNo);
	s += str;
	str.Format ("ToNo:%s",pSM->ToNo);
	s += str;
	str.Format ("AuthPriceID:%s",pSM->AuthPriceID);
	s += str;
	str.Format ("OperatorType:%d",pSM->OperatorType);
	s += str;
	str.Format ("GiftAddr:%s>",pSM->GiftAddr);
	s += str;
	
	if (strlen(s) >= len)
		return;
	strcpy(pData,s);
	
}







int err_handler(DBPROCESS   *dbproc, int severity, int dberr,int oserr,const char* dberrstr,const char* oserrstr)
{
	/*sprintf(g_error,"DB-Library error:\n\t%s\n", dberrstr);
	if(dberr == 10007)
	{
		PrintMessage(g_message);
	}
	if(dberr == 10042)
	{
		return INT_CONTINUE;
	}
	PrintMessage(g_error);
	if (severity == EXCOMM && (oserr != DBNOERR || oserrstr))
	{
		sprintf(g_error,"Net-Lib error %d:  %s\n", oserr, oserrstr);
		PrintMessage(g_error);
	}
	
	if (oserr != DBNOERR)
	{
		sprintf(g_error,"Operating-system error:\n\t%s\n", oserrstr);
		PrintMessage(g_error);
		
	}
	
	if ((dbproc == NULL) || (DBDEAD(dbproc)))
		return(INT_EXIT);
	else
		return(INT_CANCEL);*/
	return 0;
}


int msg_handler (DBPROCESS *dbproc,DBINT msgno,int msgstate,int severity,const char *msgtext,const char *srvname, const char   *procname,DBUSMALLINT line)
{
	sprintf (g_message,"SQL Server message %ld, state %d, "
		"severity %d: \n\t%s\n", msgno, msgstate, severity, msgtext);
	return (0);
}





void CSMDatabase::OnDeleteFixMsg(PTRMSG pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	LPFIXMSGINFO pFixMsg = (LPFIXMSGINFO)(pMsg + sizeof(smpp_Message_header));
	
	m_LastExeDBTime = GetTickCount();

	
	if (!GetDBLink())
	{
		PrintMessage("The m_pLink is null on OnDeleteFixMsg");
		return;
	}
	
	switch (m_pLink->GetSrcType())
	{
	case ADO_LIB:
		break;
	case MS_SQL_DB_LIB:
		break;
	default:
		break;
	}
	
	CAdoObj * pDbObj = (CAdoObj *)m_pLink;
	m_pDelFixMsgCmd->ActiveConnection = pDbObj->m_pConnection;
	
	
	long ColIndex = 0;
	m_pDelFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->FeeMonth));	
	m_pDelFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->SPID));
	m_pDelFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->ServiceID));
	m_pDelFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->FeeAddr));
	m_pDelFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->UserAddr));
	
	try
	{
		m_pDelFixMsgCmd->CommandText = (_bstr_t)(LPCSTR)m_strDelFixMsg;
        m_pDelFixMsgCmd->CommandType = adCmdStoredProc;
		m_pDelFixMsgCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	
	
	catch (_com_error)
	{
		PrintProviderError("Execute OnDelFixedMsg error", pDbObj->m_pConnection);	
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
		return;
	}		
}



void CSMDatabase::OnTime()
{
	if (GetTickCount() - m_LastExeDBTime > (m_NotExeDBTime * 60 * 1000))
	{
		if (m_pLink)
		{
			ReturnLink(m_pLink, FALSE);
			m_pLink = NULL;
		}

		m_LastExeDBTime = GetTickCount();
	}
}


BOOL CSMDatabase::OnMountMessage()
{
	CSmServiceProcApp *pApp = (CSmServiceProcApp*)AfxGetApp();
	CMainProcMng *pMainMng = (CMainProcMng *)pApp->pMainMng;	
	PDBPROCESS  pDB;
	
	if (!m_pLink)
	{
		pMainMng->GetDbProcess(m_pLink, pDB);
	}
	
	if (!m_pLink)
	{
		g_nConnDBTimeOutCount++;
		return FALSE;
	}
	else
	{
		g_nConnDBTimeOutCount = 0;
	}

	if(false == m_bLoadMonthFee)
	{
		//加载包月扣费记录
		COleDateTime tmCurTm = COleDateTime::GetCurrentTime();
		CString sTm = tmCurTm.Format("%Y/%m/%d %H:%M:%S");
		
		char FeeMonth[32];
		CalFixMsgFeeMonth(sTm, FeeMonth);
		m_bLoadMonthFee = LoadFixMsg(FeeMonth);

		if(m_bLoadMonthFee)
		{
			pApp->GetMonthFee()->SetLoadFromDBIsOK();
		}
	}

	return true;
}


void CSMDatabase::OnUpdateFeeMonthTable(LPCTSTR pMsg)
{
	ASSERT(pMsg);
	
	smpp_Message_header * pHeader = (smpp_Message_header *)pMsg;
    tagFeeMonthStatus * pInfo = (tagFeeMonthStatus *)(pMsg + sizeof(smpp_Message_header));
	
	m_LastExeDBTime = GetTickCount();
	
	// 获取数据库活动连接
	if (!GetDBLink())
	{
		PrintMessage("Link is NULL OnUpdateFeeMonthTable()");
		return;
	}
	
	
	CAdoObj * pDbObj = (CAdoObj*)m_pLink;
	m_pUpdateMonthFeeCmd->ActiveConnection = pDbObj->m_pConnection;
	long ColIndex = 0;
	_variant_t ret;
	
	
	// 传送存储过程参数值
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->FeeAddr));
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->UserAddr));
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->SPID));
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->ServiceID));
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->FeeDate));
	CString sStat;
	sStat.Format("%d", pInfo->Status);
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(sStat));
	// SMGW45-07, 2006-1-6, wzy add begin //
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->FixedFee));
	m_pUpdateMonthFeeCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pInfo->SendAccount));
	// SMGW45-07, 2006-1-6, wzy add end //	
	
	try
	{
		m_pUpdateMonthFeeCmd->CommandText = (_bstr_t)(LPCSTR)m_strUpdteMonthFee;
        m_pUpdateMonthFeeCmd->CommandType = adCmdStoredProc;
		m_pUpdateMonthFeeCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	
	catch (_com_error)
	{
		PrintProviderError("UpdteFeeMonthTable procedure error", pDbObj->m_pConnection);
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
	}	
}


void CSMDatabase::LoadConfig()
{
	char  InitName[]=".\\FeeConfig.ini";
	char  sTemp[100];
	int Ret;

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
	
	
	Ret = GetPrivateProfileString("ProcedureName", 
		"DelFixedMsg", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_DelFixedMsg");
		WritePrivateProfileString("ProcedureName",
			"DelFixedMsg",
			sTemp,
			InitName);
	}
	m_strDelFixMsg = CString(sTemp) ;
	
	
	
	Ret = GetPrivateProfileString("ProcedureName", 
		"SaveFixedMsg", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_SaveFixedMsg");
		WritePrivateProfileString("ProcedureName",
			"SaveFixMsg",
			sTemp,
			InitName);
	}
	m_strSaveFixMsg = CString(sTemp) ;
	
	
	Ret = GetPrivateProfileString("ProcedureName", 
		"LoadFixedMsg", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_LoadFixedMsg");
		WritePrivateProfileString("ProcedureName",
			"LoadFixedMsg",
			sTemp,
			InitName);
	}
	m_strLoadFixMsg = CString(sTemp) ;
	
	Ret = GetPrivateProfileString("ProcedureName", 
		"FindMsgInDBBuffer", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_FindMsgInDBBuffer");
		WritePrivateProfileString("ProcedureName",
			"FindMsgInDBBuffer",
			sTemp,
			InitName);
	}
	m_strFindSMBuff = CString(sTemp) ;
	
	
	Ret = GetPrivateProfileString("ProcedureName", 
		"AddToDBBuffer", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_AddToDBBuffer");
		WritePrivateProfileString("ProcedureName",
			"AddToDBBuffer",
			sTemp,
			InitName);
	}
	m_strAddSMBuff = CString(sTemp) ;
	

	Ret = GetPrivateProfileString("ProcedureName", 
		"UpdateFeeMonthTable", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_UpdateFeeStatus");
		WritePrivateProfileString("ProcedureName",
			"UpdateFeeMonthTable",
			sTemp,
			InitName);
	}
	m_strUpdteMonthFee = CString(sTemp) ;
	

	Ret = GetPrivateProfileString("ProcedureName",
		"NotExeDBTime",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
    if(Ret == 0)
	{
		//ASSERT(0);
		strcpy(sTemp, "10"); //缺省为空
		WritePrivateProfileString("ProcedureName",
			"NotExeDBTime",
			sTemp,
			InitName);
	}
	m_NotExeDBTime = atoi(sTemp);
}


bool CSMDatabase::LoadFixMsg(LPCTSTR pFeeMonth)
{
	ASSERT(pFeeMonth);


	m_LastExeDBTime = GetTickCount();

	if (!GetDBLink())
	{
		PrintMessage("The m_pLink is null on OnDeleteFixMsg");
		return false;
	}
	
	switch (m_pLink->GetSrcType())
	{
	case ADO_LIB:
		break;
	case MS_SQL_DB_LIB:
		break;
	default:
		break;
	}
	
	CAdoObj * pDbObj = (CAdoObj *)m_pLink;
	m_pLoadFixMsgCmd->ActiveConnection = pDbObj->m_pConnection;
	_RecordsetPtr rs;

	long ColIndex = 0;
	m_pLoadFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFeeMonth));
	
	try
	{
		m_pLoadFixMsgCmd->CommandText = (_bstr_t)(LPCSTR)m_strLoadFixMsg;
        m_pLoadFixMsgCmd->CommandType = adCmdStoredProc;
		rs = m_pLoadFixMsgCmd->Execute(NULL, NULL, adCmdStoredProc);
	}	
	catch (_com_error)
	{
		PrintProviderError("Execute LoadFixMsg fail", pDbObj->m_pConnection);
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
		return false;
	}
	
	
	PrintMessage("开始加载包月扣费表.....");
	DWORD tick = GetTickCount();
	DWORD count = 0;
	while (rs != NULL && rs->State == adStateOpen && rs->adoEOF == FALSE)
	{
		FIXMSGINFO FixMsgInfo;
		memset(&FixMsgInfo, 0, sizeof(FixMsgInfo));

		try
		{	
			CString strFieldValue;
			_variant_t varValue;
			
			varValue = rs->GetCollect("FFeeMonth");
			if (varValue.vt != VT_NULL)
			{
				strncpy(FixMsgInfo.FeeMonth, (LPCTSTR)(_bstr_t)varValue, sizeof(FixMsgInfo.FeeMonth));
			}
			
			varValue = rs->GetCollect("FSPID");
			if (varValue.vt != VT_NULL)
			{
				strncpy(FixMsgInfo.SPID, (LPCTSTR)(_bstr_t)varValue, sizeof(FixMsgInfo.SPID));
			}

			varValue = rs->GetCollect("FServiceID");
			if (varValue.vt != VT_NULL)
			{
				strncpy(FixMsgInfo.ServiceID, (LPCTSTR)(_bstr_t)varValue, sizeof(FixMsgInfo.ServiceID));
			}

			varValue = rs->GetCollect("FUserAddr");
			if (varValue.vt != VT_NULL)
			{
				strncpy(FixMsgInfo.UserAddr, (LPCTSTR)(_bstr_t)varValue, sizeof(FixMsgInfo.UserAddr));
			}

			varValue = rs->GetCollect("FFeeAddr");
			if (varValue.vt != VT_NULL)
			{
				strncpy(FixMsgInfo.FeeAddr, (LPCTSTR)(_bstr_t)varValue, sizeof(FixMsgInfo.FeeAddr));
			}

			//加入到内存结构中
			CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
			pApp->GetMonthFee()->Add(
				FixMsgInfo.FeeMonth, 
				FixMsgInfo.SPID,
				FixMsgInfo.ServiceID,
				FixMsgInfo.FeeAddr,
				FixMsgInfo.UserAddr
				);
			
			count++;
			rs->MoveNext();
		}
		catch (...)
		{
			PrintMessage("加载包月扣费表失败，可能是返回的字段名有误");
			return false;
		}
		
	}

	char log[128];
	sprintf(log, "加载包月扣费表完毕,共%d条记录,耗时%dms", count, GetTickCount() - tick);
	PrintMessage(log);

	return true;
}


void CSMDatabase::OnSaveFixMsg(PTRMSG pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	LPFIXMSGINFO pFixMsg = (LPFIXMSGINFO)(pMsg + sizeof(smpp_Message_header));
	
	m_LastExeDBTime = GetTickCount();

	
	if (!GetDBLink())
	{
		//数据库断连
		PrintMessage("The m_pLink is null on OnSaveFixMsg");
		return;
	}
	
	switch(m_pLink->GetSrcType())
	{
	case ADO_LIB:
		break;
	case MS_SQL_DB_LIB:
		break;
	default:
		break;
	}
	
	CAdoObj * pDbObj = (CAdoObj *)m_pLink;
	m_pSaveFixMsgCmd->ActiveConnection = pDbObj->m_pConnection;
	
	
	long ColIndex = 0;
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->FeeMonth));	
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->SPID));
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->ServiceID));
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->FeeAddr));
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->UserAddr));	
	m_pSaveFixMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(pFixMsg->Submittime));

	try
	{
		m_pSaveFixMsgCmd->CommandText = (_bstr_t)(LPCSTR)m_strSaveFixMsg;
        m_pSaveFixMsgCmd->CommandType = adCmdStoredProc;
		m_pSaveFixMsgCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	
	
	catch (_com_error e)
	{
		//TRACE(e.Description());
		PrintProviderError("Execute OnSaveFixMsg error", pDbObj->m_pConnection);
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;	
		return;
	}		
}


bool CSMDatabase::GetDBLink()
{
	// 获取数据库活动连接
	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
	CMainProcMng *pMainMng = (CMainProcMng *)pApp->pMainMng;
	PDBPROCESS pDB;

	if (!m_pLink) 
	{
		pMainMng->GetDbProcess(m_pLink, pDB);
	}

	return ((m_pLink == NULL) ? false : true);

}


bool CSMDatabase::SendMsgToSMSThread(PTRMSG pMsg)
{
	SaveShortMessage *pSms = (SaveShortMessage *)pMsg;
	char buff[SAVE_MESSAGE_LEN + 1];
	memset(buff, 0, sizeof(buff));
	
	smpp_Message_header head;
	head.Command_ID = CMD_FIND_DB_BUFFER_RESULT;
	head.Command_Length = SAVE_MESSAGE_LEN;
	
	memcpy(buff, &head, sizeof(smpp_Message_header));
	memcpy(buff + sizeof(smpp_Message_header), pSms, sizeof(SaveShortMessage));
	
	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
	CMainProcMng * pMainProc = pApp->pMainMng;
	
	int nRet = pMainProc->m_pSmsThread->SendMessage(buff, sizeof(buff));

	if (0 == nRet)
	{
		PrintMessage("SendMsgToSMSThread fail");
		return false;
	}

	return true;
}


