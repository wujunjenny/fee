// DBMng.cpp: implementation of the CCheckTimeOutDBSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckTimeOutDBSM.h"
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

//const int size = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
//const int len = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int err_handler(DBPROCESS   *dbproc, int severity, int dberr,int oserr,const char* dberrstr,const char* oserrstr);
extern int msg_handler (DBPROCESS *dbproc,DBINT msgno,int msgstate,int severity,const char *msgtext,const char *srvname,const char   *procname,DBUSMALLINT line);

CCheckTimeOutDBSM::CCheckTimeOutDBSM()
{
	m_pLink = NULL;
	m_bChecking = false;
	m_LastCheckTime = GetTickCount();
	
	InitCmd();
	
	//dberrhandle(err_handler);
	//dbmsghandle (msg_handler );
	
    char  InitName[]=".\\FeeConfig.ini";
	char  sTemp[100];
	int Ret;
	
	//存储过程名可配置
	Ret = GetPrivateProfileString("ProcedureName", 
		"CheckTimeOutMsg", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "xp_CheckTimeOutMsg");
		WritePrivateProfileString("ProcedureName",
			"CheckTimeOutMsg",
			sTemp,
			InitName);
	}
	m_strCheckTimeOutMsg = CString(sTemp) ;
}

CCheckTimeOutDBSM::~CCheckTimeOutDBSM()
{
	CMainProcMng* pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);
	if(m_pLink && pMainMng)
	{
		pMainMng->ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
	}
}

bool CCheckTimeOutDBSM::Start()
{
	//创建线程
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hKillEvent)
	{
		PrintMessage("in CCheckTimeOutDBSM() CreateEvent Fail");
		return false;
	}
	
	DWORD ThreadId;
   	m_Thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &ThreadId);
	if(!m_Thread)
	{
		PrintMessage("in CCheckTimeOutDBSM() CreateThread Fail");
		return false;
	}
	//-----SMGW41-34,2005-04-12,zxh,modi begin----//
	return true;
	//-----SMGW41-34,2005-04-12,zxh,modi end----//
}


void CCheckTimeOutDBSM::End()
{
	//结束线程
	m_bChecking = false;
	
	SetEvent(m_hKillEvent);
	if( WaitForSingleObject(m_Thread, 10000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_Thread, 4);
	}
	CloseHandle(m_hKillEvent);
	CloseHandle(m_Thread);
}


DWORD WINAPI CCheckTimeOutDBSM::ThreadProc(LPVOID lpParam)
{
	CCheckTimeOutDBSM* pMng = (CCheckTimeOutDBSM*)lpParam;
	
	CMainProcMng *pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);	
	
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//PDBPROCESS  pDB;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	//SMGW41-62, 2005-6-17, jdz, modi begin//
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	bool bCheck = false;
	
	while(WaitForSingleObject(pMng->m_hKillEvent, 0) != WAIT_OBJECT_0)
	{		
		//SMGW42-62, 2005-7-15, jdz, modi begin//
		DWORD time = GetTickCount();
		if(bCheck //需要立即进行检查
			|| (time - pMng->m_LastCheckTime) > (g_ScanDBTime * 60 * 1000))//超过检查时间周期，则进行检查
		{						
			//获取与网关的连接状态
			int status = ((CSmServiceProcApp*)AfxGetApp())->pDrv->GetState();
			if(smpp_active == status)
			{//如果与网关已连接，则进行检查
				if(-1 == pMng->CheckTimeOutMsg())
				{//如果检查时，是数据库未连接错误则不断重试
					bCheck = true;
				}
				else
				{
					bCheck = false;
				}
			}
			else
			{//如果与网关未连接，则不断重试
				bCheck = true;
			}
		}
		//SMGW42-62, 2005-7-15, jdz, modi end//
		
		Sleep(10000);
	}
	
	::CoUninitialize();
	//SMGW41-62, 2005-6-17, jdz, modi end//
	
	return 0;
}



void CCheckTimeOutDBSM::ReturnLink(CLinkObject *pLink,BOOL bOK)
{
	CMainProcMng *pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);
	pMainMng->ReturnLink(pLink, bOK);
}



void CCheckTimeOutDBSM::PrintProviderError(LPCTSTR pszMsg, _ConnectionPtr pConnection)
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
    catch(_com_error &e)
    {
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
    }
};

void CCheckTimeOutDBSM::InitCmd()
{
	m_pCheckExpiredMsgCmd.CreateInstance( "ADODB.Command");
	m_pCheckExpiredMsgCmd->GetParameters()->Append(m_pCheckExpiredMsgCmd->CreateParameter(_bstr_t("ExpiredTime"), adInteger, adParamInput, 4, _variant_t(1234L)));
	m_pCheckExpiredMsgCmd->GetParameters()->Append(m_pCheckExpiredMsgCmd->CreateParameter(_bstr_t("sCurTime"), adBSTR, adParamInput, 100, _variant_t(1234L)));
}


//对数据库缓存表中超时未回状态报告的消息进行检查
//返回值：－1＝数据库连接错误，或存储过程执行错误；0＝其他错误；1＝执行成功
int CCheckTimeOutDBSM::CheckTimeOutMsg()
{
	m_bChecking = true;
	
	CMainProcMng *pMainMng = (CMainProcMng*)(((CSmServiceProcApp*)AfxGetApp())->pMainMng);
	PDBPROCESS  pDB;
	
	if (!m_pLink) 
		pMainMng->GetDbProcess(m_pLink, pDB);
	
	if (!m_pLink)
	{
		ASSERT(0);
		m_bChecking = false;
		PrintMessage("The m_pLink is null on call OnCheckTimeOutMsg function");
		return -1;
	}
	
	CAdoObj *pDbObj = (CAdoObj *)m_pLink;
	_RecordsetPtr pRecord;
	long ColIndex = 0;
	
	m_pCheckExpiredMsgCmd->ActiveConnection = pDbObj->m_pConnection;
	m_pCheckExpiredMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue((long)g_AskReportTime);
	COleDateTime tm = COleDateTime::GetCurrentTime();
	CString sTm = tm.Format("%Y/%m/%d %H:%M:%S");
	m_pCheckExpiredMsgCmd->GetParameters()->GetItem(_variant_t(ColIndex++))->PutValue(_bstr_t(sTm));
	
	//SMGW41-62, 2005-6-17, jdz, modi begin//
	CString log;
	log.Format("开始将超时消息发送状态报告....");
	PrintMessage((char*)(LPCTSTR)log);
	DWORD time = GetTickCount();
	//SMGW41-62, 2005-6-17, jdz, modi end//
	
	try
	{
		m_pCheckExpiredMsgCmd->CommandText = (_bstr_t)(LPCSTR)m_strCheckTimeOutMsg;
        m_pCheckExpiredMsgCmd->CommandType = adCmdStoredProc;
		if ( g_Timeout >= 0)
			m_pCheckExpiredMsgCmd->CommandTimeout = g_Timeout;
		
		pRecord = m_pCheckExpiredMsgCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch (_com_error)
	{
		ReturnLink(m_pLink, FALSE);
		m_pLink = NULL;
		m_bChecking = false;
		PrintProviderError( "Execute xp_CheckTimeOutMsg fail", pDbObj->m_pConnection);
		return -1;
	}
	
	//SMGW42-62, 2005-7-15, jdz, modi begin//
	m_LastCheckTime = GetTickCount();
	//SMGW42-62, 2005-7-15, jdz, modi end//
	
	
	if (pRecord != NULL && pRecord->State == adStateOpen)
	{
		int count = 0;
		
		while (!pRecord->adoEOF)
		{
			SaveShortMessage data;
			memset(&data, 0, sizeof(data));
			CString strFieldValue;
			
			try
			{
				_variant_t varValue = pRecord->GetCollect("FSmid");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.smid, strFieldValue, sizeof(data.smid) - 1);
				
				varValue = pRecord->GetCollect("FUnikey");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.unikey, strFieldValue, sizeof(data.unikey) - 1);
				
				varValue = pRecord->GetCollect("FSourceID");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.sourceID, strFieldValue, sizeof(data.sourceID) - 1);
				
				varValue = pRecord->GetCollect("FOrgAddr");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.orgAddr, strFieldValue, sizeof(data.orgAddr) - 1);
				
				varValue = pRecord->GetCollect("FDestAddr");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.destAddr, strFieldValue, sizeof(data.destAddr) - 1);
				
				varValue = pRecord->GetCollect("FSubmitTime");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.submitTime, strFieldValue, sizeof(data.submitTime) - 1);
				
				//	varValue = pRecord->GetCollect("FDeliverTime");
				//	strFieldValue.Empty();
				//	if (varValue.vt != VT_NULL)
				//		strFieldValue = varValue.bstrVal;
				//	strFieldValue.TrimRight();
				COleDateTime tmTime = COleDateTime::GetCurrentTime();
				CString sTime = tmTime.Format("%Y/%m/%d %H:%M:%S");
				strncpy(data.deliverTime, sTime, sizeof(data.deliverTime) -1);
				
				
				varValue = pRecord->GetCollect("FFeeType");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.feeType, strFieldValue, sizeof(data.feeType) - 1);
				
				varValue = pRecord->GetCollect("FFeeAddr");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.feeAddr, strFieldValue, sizeof(data.feeAddr) - 1);
				
				varValue = pRecord->GetCollect("FMessageType");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.messageType, strFieldValue, sizeof(data.messageType) - 1);
				
				varValue = pRecord->GetCollect("FMsgType");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.msgType, strFieldValue, sizeof(data.msgType) - 1);
				
				varValue = pRecord->GetCollect("FServiceID");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.serviceID, strFieldValue, sizeof(data.serviceID) - 1);
				
				
				strncpy(data.serviceUpID, "EXPIRED", sizeof(data.serviceUpID) - 1);
				varValue = pRecord->GetCollect("FCPID");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.spcode, strFieldValue, sizeof(data.spcode) - 1);
				
				varValue = pRecord->GetCollect("FFeeCode");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.feeCode, strFieldValue, sizeof(data.feeCode) - 1);
				
				varValue = pRecord->GetCollect("FFixedFee");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.fixedFee, strFieldValue, sizeof(data.fixedFee) - 1);
				
				varValue = pRecord->GetCollect("FPriority");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.priority, strFieldValue, sizeof(data.priority) - 1);
				
				varValue = pRecord->GetCollect("FAreaCode");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.areaCode, strFieldValue, sizeof(data.areaCode) - 1);
				
				varValue = pRecord->GetCollect("FSMGNo");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.smgno, strFieldValue, sizeof(data.smgno) - 1);
				
				varValue = pRecord->GetCollect("FSendAccount");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.sendAccount, strFieldValue, sizeof(data.sendAccount) - 1);
				
				varValue = pRecord->GetCollect("FRcvAccount");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.rcvAccount, strFieldValue, sizeof(data.rcvAccount) - 1);
				
				varValue = pRecord->GetCollect("FFwdSMGNo");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.fwdsmgno, strFieldValue, sizeof(data.fwdsmgno) - 1);
				
				varValue = pRecord->GetCollect("FSMCNo");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.smcCode, strFieldValue, sizeof(data.smcCode) - 1);
				
				varValue = pRecord->GetCollect("FMsgLen");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				data.msgLen = atoi(strFieldValue);
				
				varValue = pRecord->GetCollect("FPayUserType");
				if (varValue.vt != VT_NULL)
					data.payUserType = varValue.lVal;
				
				varValue = pRecord->GetCollect("FPayFlow");
				if (varValue.vt != VT_NULL)
					data.payFlow = varValue.lVal;
				
				varValue = pRecord->GetCollect("FPayBefore");
				if (varValue.vt != VT_NULL)
					data.payBeforeSupport = varValue.lVal;
				
				varValue = pRecord->GetCollect("FPaymntFlag");
				if (varValue.vt != VT_NULL)
					data.paymntFlag = varValue.lVal;
				
				varValue = pRecord->GetCollect("FSMType");
				if (varValue.vt != VT_NULL)
					data.SMType = varValue.lVal;
				
				varValue = pRecord->GetCollect("FFromNo");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.FromNo, strFieldValue, sizeof(data.FromNo) - 1);
				
				varValue = pRecord->GetCollect("FToNo");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.ToNo, strFieldValue, sizeof(data.ToNo) - 1);
				
				varValue = pRecord->GetCollect("FAuthPriceID");
				if (varValue.vt != VT_NULL)
				{
					//strFieldValue.Format("%s", varValue.bstrVal);
					strFieldValue = varValue.bstrVal;
				}
				strFieldValue.TrimRight();
				strncpy(data.AuthPriceID, strFieldValue, sizeof(data.AuthPriceID) - 1);
				
				varValue = pRecord->GetCollect("FOperatorType");
				if (varValue.vt != VT_NULL)
					data.OperatorType = varValue.lVal;

				//*** SMGW43-03, 2005-09-09, ZF add begin
				varValue = pRecord->GetCollect("FGiftAddr");
				if (varValue.vt != VT_NULL)
				{
					strFieldValue = varValue.bstrVal;
				}
				strFieldValue.TrimRight();
				strncpy(data.GiftAddr, strFieldValue, sizeof(data.GiftAddr) - 1);
				//*** SMGW43-03, 2005-09-09, ZF add end			
				
				strncpy(data.status, "560", sizeof(data.status));
				
				varValue = pRecord->GetCollect("FReportFlag");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.reportFlag, strFieldValue, sizeof(data.reportFlag) - 1);
				
				varValue = pRecord->GetCollect("FUD");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.UD, strFieldValue, sizeof(data.UD) - 1);
				
				varValue = pRecord->GetCollect("FGivenCode");
				strFieldValue.Empty();
				if (varValue.vt != VT_NULL)
					strFieldValue = varValue.bstrVal;
				strFieldValue.TrimRight();
				strncpy(data.givenCode, strFieldValue, sizeof(data.givenCode) - 1);
			}
			catch(...)
			{
				m_bChecking = false;
				//SMGW41-62, 2005-6-17, jdz, modi begin//
				//SMGW41-62, 2005-6-17, jdz, modi begin//
				PrintMessage("在从数据库中获取超时消息时异常" );
				//SMGW41-62, 2005-6-17, jdz, modi end//
				ReturnLink(m_pLink, FALSE);
				m_pLink = NULL;
				//SMGW41-62, 2005-6-17, jdz, modi end//
				
				return 0;
			}
			
			//(5.1)包月扣费消息超时处理
			if((atoi(data.feeType) == 2 && atoi(data.messageType) == 8) && 
				(atoi(data.msgType) == MTF || atoi(data.msgType) == MTFF))
			{//MTF/MTFF类型包月扣费消息扣费超时
				smpp_Message_header Header;
				memset(&Header, 0, sizeof(smpp_Message_header));
				
				Header.Command_ID = CMD_DELFIXEDMSG;
				Header.Command_Length = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
				Header.Command_Status = 0;
				
				char DBMsg[SAVE_MESSAGE_LEN + 1];
				memset(DBMsg, 0, sizeof(DBMsg));
				
				memcpy(DBMsg, &Header, sizeof(smpp_Message_header));
				memcpy(DBMsg + sizeof(smpp_Message_header), &data, sizeof(SaveShortMessage));
				
				//SMGW41-62, 2005-6-17, jdz, modi begin//
				if(!((CSmServiceProcApp*)AfxGetApp())->pDBMng->SendMessageToQueue(DBMsg))
				{
					PrintMessage("In CheckTimeOutMsg() 发送消息 CMD_DELFIXEDMSG 失败, SPID=%s, ServiceID=%s, FeeAddr=%s",
						data.spcode, data.serviceID, data.feeAddr);
				}
				//SMGW41-62, 2005-6-17, jdz, modi end//				
			}
			
			char ResMsg[SAVE_MESSAGE_LEN + 1];
			memset(ResMsg, 0, sizeof(ResMsg));
			
			smpp_Message_header Header;
			memset(&Header, 0, sizeof(smpp_Message_header));
			Header.Command_ID = CMD_CHECK_EXPIRE_RESULT;
			Header.Command_Length = SAVE_MESSAGE_LEN;
			
			memcpy(ResMsg, &Header, sizeof(smpp_Message_header));
			memcpy(ResMsg + sizeof(smpp_Message_header), &data, sizeof(SaveShortMessage));
			
			//SMGW41-62, 2005-6-17, jdz, modi begin//
			BOOL ret = ((CSmServiceProcApp *)AfxGetApp())->pMainMng->m_pSmsThread->SendMessage(ResMsg, sizeof(ResMsg));
			if(!ret)
			{
				PrintMessage("In CheckTimeOutMsg() 发送消息 CMD_CHECK_EXPIRE_RESULT 失败, SndAcnt=%s, RcvAcnt=%s, OrgAddr=%s, DestAddr=%s",
					data.sendAccount, data.rcvAccount, data.orgAddr, data.destAddr);
			}
			//SMGW41-62, 2005-6-17, jdz, modi end//				
			
			count ++;
			if(count >= 20)
			{
				Sleep(300);
				count = 0;
			}
			
			if(false == m_bChecking)
			{
				break;
			}
			
			pRecord->MoveNext();
		}
	}
	
	//SMGW41-62, 2005-6-17, jdz, modi begin//
	log.Format("将超时消息发送状态报告结束，耗时%u", GetTickCount() - time);
	PrintMessage((char*)(LPCTSTR)log);
	//SMGW41-62, 2005-6-17, jdz, modi end//
	
	m_bChecking = false;
	
	//SMGW41-62, 2005-6-17, jdz, modi begin//
	ReturnLink(m_pLink, FALSE);
	m_pLink = NULL;
	//SMGW41-62, 2005-6-17, jdz, modi end//
	
	return 1;
}



BOOL CCheckTimeOutDBSM::RecordDBData(char *pSM, int opType)
{
	char buf[4000];
	Message_header mh;
	
	switch (opType)
	{
	case CMD_CHECK_TIMEOUT_TAB:
		mh.Command_ID = CMD_CHECK_TIMEOUT_TAB;
		mh.Command_Length = sizeof(Message_header) + sizeof(tagSmsCheckExpiredDB);
		memcpy(buf, &mh, sizeof(Message_header));
		memcpy(buf + sizeof(Message_header), pSM, sizeof(tagSmsCheckExpiredDB));
		break;
		
	default:
		return FALSE;
	}
	
	mh.Command_Status = 0;
	((CSmServiceProcApp*)AfxGetApp())->pBillFileMng->SendMessageToQueue(buf);
	
	return TRUE;
}




