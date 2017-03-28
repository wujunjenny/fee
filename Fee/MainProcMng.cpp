// MainProcMng.cpp: implementation of the CMainProcMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SmServiceProc.h"
#include "MainProcMng.h"
#include "AdoObj.h"
#include "eventlog.h"
#include "GlobalDef.h"
#include "SmppDrvMng.h"


#ifndef DBINITCONSTANTS
#define DBINITCONSTANTS
#endif



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define INNERMSG_SUM  0X00000091

#define JSON_READ_INT(obj,name,_value)		{ auto _itr = (obj).FindMember(name); \
											  if(_itr!=(obj).MemberEnd())  \
											  { _value = _itr->value.GetInt();}; }

#define JSON_READ_STR(obj,name,_value)		{ auto _itr = (obj).FindMember(name);if(_itr!=(obj).MemberEnd()){ _value = _itr->value.GetString(); }; }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::string gProxyGwName;
std::string gDefaultGwName;

inline void TESTHR(HRESULT _hr) 
{ 
	if FAILED(_hr) _com_issue_error(_hr);
}


CMainProcMng::CMainProcMng()
{
//modify by gyx
	::InitializeCriticalSection(&m_CriticalSection);

	FILE* fp = fopen("FeeConnection.json", "rb"); // 非Windows平台使用"r"
	if(fp==NULL)
	{
		//fp =  fopen("AuthConnection.json", "wb");
		//char writeBuffer[65536];
		//rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		//rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		//writer.StartObject();
		//writer.Key("http-port");
		//writer.Int(6789);
		//writer.EndObject();
		errno;
	}
	else
	{
		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		rapidjson::Document reader;
		reader.ParseStream(is);
		if(reader.HasParseError())
		{
		}
		else
		{
			auto itr = reader.FindMember("ProxyGwName");
			if( itr != reader.MemberEnd())
			{
				if(itr->value.IsString())
					gProxyGwName = itr->value.GetString();					
			}
			itr = reader.FindMember("DefaultGwName");
			if( itr != reader.MemberEnd())
			{
				if(itr->value.IsString())
					gDefaultGwName = itr->value.GetString();					
			}

			itr = reader.FindMember("ConnectionArray");
			if(itr != reader.MemberEnd())
			{
				//nPort= itr->value.GetInt();
				if(itr->value.IsArray())
				{
					auto con_itr = itr->value.Begin();
					while(con_itr!=itr->value.End())
					{
						CONNECT_INFO info;
						JSON_READ_STR(*con_itr,"Name",info.sGWName);
						JSON_READ_STR(*con_itr,"Account",info.sAccount);
						JSON_READ_STR(*con_itr,"Password",info.sPassword);
						JSON_READ_STR(*con_itr,"IP",info.sIPAddr);
						JSON_READ_INT(*con_itr,"Port",info.nPort);
						AddConnection(info);
						con_itr++;
					}
				}

			}
		}
		fclose(fp);
	}
//end

	m_nSeq = 0;	
	char  InitName[]=".\\FeeConfig.ini";
	char  sTemp[100];
	char  sSession[100]="para";
	
	
	int Ret = 0;
	
	Ret = GetPrivateProfileString("DataSrc0",
		"DataSrcName", 
		"", 
		sTemp, 
		sizeof(sTemp),
		InitName); 
	if(Ret == 0)
	{
		strcpy(sTemp, "DBSMSERVICE");
		WritePrivateProfileString("DataSrc0",
			"DataSrcName",
			sTemp,
			InitName);
	}
	m_sDataSrcName=sTemp;
    
	Ret = GetPrivateProfileString("SmConnection",
		"Account", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName); 
	if(Ret == 0)
	{
		strcpy(sTemp, "LocalFee");
		WritePrivateProfileString("SmConnection",
			"Account",
			sTemp,
			InitName);
	}
	m_sAccount=sTemp;
	
	
	Ret = GetPrivateProfileString("SmConnection",
		"bindtype",
		"", 
		sTemp,
		sizeof(sTemp),
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "1");
		WritePrivateProfileString("SmConnection",
			"bindtype",
			sTemp,
			InitName);
	}
	m_nBindType=atoi(sTemp);
	
	
	GetPrivateProfileString(sSession,"SMSTHREAD", "1", sTemp, sizeof(sTemp), InitName); 
	m_nThreadCount=atoi(sTemp);
	if (m_nThreadCount > MAX_MULTI_SMS_THREAD)
	{
		m_nThreadCount = MAX_MULTI_SMS_THREAD;
	}
	
	
	Ret = GetPrivateProfileString("SmConnection",
		"MaxShakeHandCount", 
		"",
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "10");
		WritePrivateProfileString("SmConnection",
			"MaxShakeHandCount",
			sTemp,
			InitName);
	}
	m_iMaxShakeHandCount = atoi(sTemp);
	
	m_iShakeHandCount = m_iMaxShakeHandCount;
	
	
	m_pSmsThread = new CSMFeeThread();
	m_pSmsThread->Init("Fee", g_MaxBuffSize, 100 * 1024, g_MaxMsgCount);
	m_pSmsThread->InitTimer(g_MsgMapMaxCount * 2);
	m_pSmsThread->Start();

	
	m_nPri=atoi(sTemp);
	if (m_nBindType==1)
	{
        m_nBindType=ESME_BNDRCV;
	}
	else
	{
        m_nBindType=ESME_BNDTRN;
	}
	
	
	m_nShakehandTimerID = SetTimer(1000,0,0);
	m_nCheckDbTimerID = SetTimer(m_nCheckDbInterval,0,0);	
	m_nLastExecTime = GetTickCount();
	g_savefilecount =GetTickCount();
	
}
void CMainProcMng::Destroy()
{
	CCommThreadMng::Destroy();
	CAutoLock lock(&m_CriticalSection);

	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		if(itr->nDrvID!=0)
		{
			((CSmServiceProcApp*)AfxGetApp())->pDrv->KillIF((HANDLE)itr->nDrvID);
		}
		itr++;
	}
	Sleep(2000);

}
CMainProcMng::~CMainProcMng()
{
	m_pSmsThread->Destroy();
	delete m_pSmsThread;
}


BOOL CMainProcMng::OnMessage(PTRMSG pMsg)
{	
	smpp_Message_header* pHeader=(smpp_Message_header*)pMsg;
	TRACE("CMainProcMng::OnMessage msgtype[%x]\r\n",pHeader->Command_ID);
	switch(pHeader->Command_ID)
	{
	case ESME_BNDRCV:
		ASSERT(0);
		break;
	case ESME_BNDRCV_RESP:
		m_iShakeHandCount = m_iMaxShakeHandCount;
		OnBindResp(pMsg);
		break;
	case ESME_BNDTRN:
		ASSERT(0);
		break;
	case ESME_BNDTRN_RESP:
		m_iShakeHandCount = m_iMaxShakeHandCount;
		OnBindResp(pMsg);
		break;
	case ESME_UBD:
	case ESME_UBD_RESP:
		break;
	case ESME_SUB_SM:
		break;
	case ESME_SUB_SM_RESP:
		break;
	case ESME_DELIVER_SM:
        {	
        }
		break;
	case ESME_DELIVER_SM_RESP:
		break;
		
	case ESME_QRYLINK:
		break;
		
	case ESME_QRYLINK_RESP:		
		//m_iShakeHandCount = m_iMaxShakeHandCount;
		{
			CAutoLock lock(&m_CriticalSection);
			smpp_Message_header* pHeader = (smpp_Message_header*)pMsg;
			smpp_Bind_Receiver_Resp* pResp = (smpp_Bind_Receiver_Resp*)(pMsg+sizeof(smpp_Message_header));
			long int hDrv = pHeader->nRcvID;

			auto itr = m_connectlist.begin();
			while(itr!=m_connectlist.end())
			{
				if(itr->nDrvID==hDrv)
				{
					if(itr->nStatus==smpp_active)
						itr->nShakeHandCount = m_iMaxShakeHandCount;
					break;
				}
				itr++;
			}

		}
		break;
		
	case ESME_NACK:
	default:	
		break;		
	}
	
	return TRUE;
}
//modify by gyx 
long int CMainProcMng::RequireRoutIf( long int id,char *name)
{
	CAutoLock lock(&m_CriticalSection);
	long int defaultIf = -1;
	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		int status = itr->nStatus;
		if( status == smpp_active )
		{
			if( itr->nDrvID == id )
			{
				if( itr->sGWName != gProxyGwName)
					return id;
			}
			std::string destname = name;
			if( name == itr->sGWName && itr->sGWName != gProxyGwName )
			{
				return itr->nDrvID;
			}
			if( itr->sGWName == gDefaultGwName)
				defaultIf = itr->nDrvID;

		}
		itr++;
	}
	return defaultIf;
}

void CMainProcMng::MountConnetion(void)
{
	CAutoLock lock(&m_CriticalSection);
	int count = 0;
	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		int status = itr->nStatus;
		switch(status)
		{
		case smpp_active:
			{
				
				if(itr->nShakeHandCount< 0)
				{
					PrintMessage("connect shakehand out connect break,[%s],[%d]",itr->sGWName.c_str(),itr->nShakeHandCount) ;
					//((CSmServiceProcApp*)AfxGetApp())->m_pDrv->SetState(smpp_broken);
					itr->nStatus = smpp_broken;
				}
				else
				{
					count ++;
					--itr->nShakeHandCount;

					//PrintMessage("connect shakehand [%s],[%d]",itr->sGWName.c_str(),itr->nShakeHandCount) ;
					smpp_Message_header rsp;
					rsp.Command_ID = ESME_QRYLINK;
					rsp.Command_Length = sizeof(smpp_Message_header);
					rsp.Command_Status =0;
					rsp.Sequence_No = GetSeq();
					rsp.nSndID = itr->nDrvID;
 					((CSmServiceProcApp*)AfxGetApp())->pDrv->Send(&rsp);	


				}	

			}
			break;
		case smpp_broken:
			{
				itr->nShakeHandCount = m_iMaxShakeHandCount;
				//((CSmServiceProcApp*)AfxGetApp())->GetDlg()->ReConnect();
				if(itr->nDrvID!=0)
				{
					((CSmServiceProcApp*)AfxGetApp())->pDrv->KillIF((HANDLE)itr->nDrvID);
				}

				itr->nDrvID = (int)((CSmServiceProcApp*)AfxGetApp())->pDrv->CreateTcpIF(itr->sIPAddr.c_str(),itr->nPort,FALSE,4000);
				itr->nStatus = smpp_connecting;
			}
			break;
		case smpp_logining:
			{

				if(itr->nShakeHandCount< 0)
				{
					PrintMessage("%d:0x%x   smpp_logining ,ShakeHandCount<0 ,broken",itr->nSeq,itr->nDrvID) ;
					itr->nStatus = smpp_broken;
				}
				else
				{
					--itr->nShakeHandCount;
					
					char * pMsg = new char[1024];
					memset(pMsg,0,1024);
					smpp_Message_header* pHeader = (smpp_Message_header*)pMsg;
					smpp_Bind_Receiver* pBind = (smpp_Bind_Receiver*)(pMsg+sizeof(smpp_Message_header));
					pHeader->Command_ID =m_nBindType; //ESME_BNDRCV; 
					pHeader->Command_Length = sizeof(smpp_Message_header)+sizeof(smpp_Bind_Receiver);
					pHeader->Command_Status = 0;
					pHeader->Sequence_No = GetSeq();
					pHeader->nSndID = itr->nDrvID;
					strcpy(pBind->system_id,itr->sAccount.c_str());
					strcpy(pBind->password,itr->sPassword.c_str());
					pBind->interface_version = 0;
					pBind->system_type[0] = 0;
					((CSmServiceProcApp*)AfxGetApp())->pDrv->Send((smpp_Message_header*)pMsg);				
					delete [] pMsg;
				}

			}
			break;
		case smpp_connecting:
			{
				if(itr->nShakeHandCount< 0)
				{
					PrintMessage("%d:0x%x   smpp_connecting ,ShakeHandCount<0 ,broken",itr->nSeq,itr->nDrvID) ;
					itr->nStatus = smpp_broken;
				}
				else
				{
					--itr->nShakeHandCount;
				}
	
			}
		default:
			break;
		}
		itr++;
	}
	if( count > 0 )
		((CSmServiceProcApp*)AfxGetApp())->pDrv->SetState(smpp_active);
	
}
//end
BOOL CMainProcMng::OnTimer(DWORD timeID, DWORD dwPara)
{
	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
	
	if (timeID == m_nShakehandTimerID)
	{
		MountConnetion();
/*
		int status = pApp->pDrv->GetState();
		switch (status)
		{
		case smpp_active:
			{
				if (m_iShakeHandCount < 0)
				{
					pApp->pDrv->SetState(smpp_broken);
				}
				else
				{
					--m_iShakeHandCount;
					
					smpp_Message_header rsp;
					rsp.Command_ID = ESME_QRYLINK;
					rsp.Command_Length = sizeof(smpp_Message_header);
					rsp.Command_Status =0;
					rsp.Sequence_No = GetSeq();
					pApp->pDrv->Send(&rsp);					
				}	
				
				break;
			}
			
		case smpp_broken:
			{
				m_iShakeHandCount = m_iMaxShakeHandCount;
				pApp->GetDlg()->ReConnect();
				
				break;
			}			
			
		case smpp_logining:
			{				
				if(m_iShakeHandCount < 0)
				{
					pApp->pDrv->SetState(smpp_broken);
				}
				else
				{
					--m_iShakeHandCount;
					
					char *pMsg = new char[1024];
					memset(pMsg,0,1024);
					smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
					smpp_Bind_Receiver *pBind = 
						(smpp_Bind_Receiver *)(pMsg + sizeof(smpp_Message_header));
					pHeader->Command_ID = m_nBindType; 
					pHeader->Command_Length = sizeof(smpp_Message_header) + sizeof(smpp_Bind_Receiver);
					pHeader->Command_Status = 0;
					pHeader->Sequence_No = GetSeq();
					strcpy(pBind->system_id, m_sAccount);
					strcpy(pBind->password, m_sPass);
					pBind->interface_version = 0;
					pBind->system_type[0] = 0;
					
					pApp->pDrv->Send((smpp_Message_header *)pMsg);
					
					delete []pMsg;
				}
				
				break;
			}		
			
		case smpp_connecting:
			{
				if (m_iShakeHandCount < 0)
				{
					pApp->pDrv->SetState(smpp_broken);
				}
				else
				{
					m_iShakeHandCount--;
				}
				break;
			}
		}
*/

	}
	else if (timeID	==	m_nCheckDbTimerID)
	{
		int status = pApp->pDrv->GetState();
		switch (status)
		{
		case smpp_active:
			{
				ProcessDb();
				break;
			}
		case smpp_broken:
			break;
		case smpp_logining:
			{
				break;
			}
		case smpp_connecting:
			break;
		default:
			break;
		}
	}
	
	return TRUE;
}

void CMainProcMng::OnBindResp(LPCTSTR pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	smpp_Bind_Receiver_Resp* pResp = 
		(smpp_Bind_Receiver_Resp *)(pMsg + sizeof(smpp_Message_header));
/*	
	CSmServiceProcApp *pApp = (CSmServiceProcApp*)AfxGetApp();
	if (pHeader->Command_Status == ESME_ROK)
	{
		if (smpp_logining == pApp->pDrv->GetState())
		{
			pApp->pDrv->SetState(smpp_active);
			pApp->GetDlg()->OnConnect();
		}
		
		m_iShakeHandCount = m_iMaxShakeHandCount;
		
	}
	else
	{
		pApp->pDrv->SetState(smpp_broken);
	}
*/
	CAutoLock lock(&m_CriticalSection);

	if(pHeader->Command_Status == ESME_ROK)
	{
		long int hDrv = pHeader->nRcvID;

		auto itr = m_connectlist.begin();
		while(itr!=m_connectlist.end())
		{
			if(itr->nDrvID==hDrv)
			{
				itr->nStatus = smpp_active;
				itr->nShakeHandCount = m_iMaxShakeHandCount;
				PrintMessage("OnBindResp ok,  connect[%s]",itr->sGWName) ;
				break;
			}
			itr++;
		}

		//((CSmServiceProcApp*)AfxGetApp())->GetDlg()->OnConnect();
	
	}
	else
	{
		long int hDrv = pHeader->nRcvID;
		auto itr = m_connectlist.begin();
		while(itr!=m_connectlist.end())
		{
			if(itr->nDrvID==hDrv)
			{
				itr->nStatus = smpp_broken;
				itr->nShakeHandCount = m_iMaxShakeHandCount;
				
				((CSmServiceProcApp*)AfxGetApp())->pDrv->KillIF((HANDLE)itr->nDrvID);
				PrintMessage("OnBindResp error,  connect[%s]",itr->sGWName) ;
				itr->nDrvID = 0;
				break;
			}
			itr++;
		}

	}
}

DWORD CMainProcMng::GetSeq()
{
	return m_nSeq++;
}

void hex2bin(unsigned char *dest, const unsigned char *src, unsigned int len)
{
	unsigned int i;
	
	if (!dest) return;
	
	for (i = 0; i < len; i++)
	{
		unsigned aux;
		
		if (src[2 * i] >= '0' && src[2 * i] <= '9') aux = src[2 * i] - '0';
		else if (src[2 * i] >= 'a' && src[2 * i] <= 'f') aux = src[2 * i] - 'a' + 10;
		else if (src[2 * i] >= 'A' && src[2 * i] <= 'F') aux = src[2 * i] - 'A' + 10;
		else 
		{
			dest[0] = 0;
			return;
		}
		dest[i] = aux << 4;
		if (src[2 * i + 1] >= '0' && src[2 * i + 1] <= '9') aux = src[2 * i + 1] - '0';
		else if (src[2 * i + 1] >= 'a' && src[2 * i + 1] <= 'f') aux = src[2 * i + 1] - 'a' + 10;
		else if (src[2 * i + 1] >= 'A' && src[2 * i + 1] <= 'F') aux = src[2 * i + 1] - 'A' + 10;
		else 
		{
			dest[0] = 0;
			return;
		}
		dest[i] |= aux;
	}
}

void bin2hex(unsigned char *dest, const unsigned char *src, unsigned int len)
{
	unsigned int i;
	
	if (!dest) return;
	
	for (i = 0; i < len; i++) 
	{
		dest[2 * i] = (src[i] & 0xf0) >> 4;
		if (dest[2 * i] < 10) dest[2 * i] += '0';
		else dest[2 * i] += ('A' - 10);
		dest[2 * i + 1] = src[i] & 0x0f;
		if (dest[2 * i + 1] < 10) dest[2 * i + 1] += '0';
		else dest[2 * i + 1] += ('A' - 10);
	}
}

void CMainProcMng::ProcessDb()
{
	if(smpp_active!=((CSmServiceProcApp*)AfxGetApp())->pDrv->GetState())
	{
		return;//没有连接成功
	}
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//PDBPROCESS  pDB;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
    DWORD nExecTime = GetTickCount();
	
    if( (nExecTime - m_nLastExecTime) > 0
        && (nExecTime - m_nLastExecTime) < (m_nCheckDbInterval*8/10) )
	{
        return;
    }
    m_nLastExecTime = nExecTime;
	
	
	return;
}


void CMainProcMng::GetDbProcess(CLinkObject *&pLink, PDBPROCESS &pDB)
{
	pLink = NULL;
    pDB = NULL;
	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
	pLink = pApp->pLinkMng->GetLink("DBSMSERVICE");
	if (!pLink)
		return ;
	
	if (!pLink->IsLinkOk())
	{
		pApp->pLinkMng->ReturnLink(pLink,FALSE);
        pLink = NULL;
		return  ;
	}
	
	DWORD type= pLink->GetSrcType();
	switch( type )
	{
	case MS_SQL_DB_LIB:
		{
			//pDB = (DBPROCESS*)pLink->GetProc();
   //         if( pDB != NULL )
   //         {
   //             dbcancel(pDB) ;
   //         }
			break;
		}
	case SYBASE_CT_LIB :
		return ;
		break;
	case ODBC_LIB:
		return ;
		break;
	case ADO_LIB:
		return;
		break;
	default:
		ASSERT(0);
		break;
	}
	
    return;	
}
//midify by gyx
int CMainProcMng::DeleteConnection(int nSeq)
{
	CAutoLock lock(&m_CriticalSection);
	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		if(itr->nSeq==nSeq)
		{
			if(itr->nDrvID!=0)
			{
				((CSmServiceProcApp*)AfxGetApp())->pDrv->KillIF((HANDLE)itr->nDrvID);
			}
			m_connectlist.erase(itr);
			return nSeq;
		}
		itr++;
	}

	return 0;
}

void CMainProcMng::OnConnect(HANDLE hHandle)
{
	CAutoLock lock(&m_CriticalSection);
	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		if(itr->nDrvID == (int)hHandle)
		{
			itr->nStatus = smpp_logining;
			PrintMessage("connected [%s][%s] [%s]",itr->sGWName.c_str(),itr->sAccount.c_str(),itr->sIPAddr.c_str());
			break;
		}
		itr++;
	}
}


void CMainProcMng::OnError(HANDLE hDrv, int nError)
{

	CAutoLock lock(&m_CriticalSection);
	auto itr = m_connectlist.begin();
	while(itr!=m_connectlist.end())
	{
		if(itr->nDrvID == (int)hDrv)
		{
			itr->nStatus = smpp_broken;
			PrintMessage("OnError, lingbroken,name[%s],error[%d]",itr->sGWName.c_str(),nError);
			((CSmServiceProcApp*)AfxGetApp())->pDrv->KillIF((HANDLE)itr->nDrvID);
			itr->nDrvID = NULL;
			break;
		}
		itr++;
	}

}


//end

void CMainProcMng::ReturnLink(CLinkObject *pLink,BOOL bOK)
{
	((CSmServiceProcApp*)AfxGetApp())->pLinkMng->ReturnLink(pLink,bOK);
}


