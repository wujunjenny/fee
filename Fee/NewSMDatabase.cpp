#include "stdafx.h"
#include "NewSMDatabase.h"
#include "smcmacro.h"
#include "SmppDef.h"
#include "globaldef.h"
//#include "OTLObj.h"
#include "sqllite\sqlite3.h"
#include <ctime>
#include "SmServiceProc.h"

CNewSMDatabase::CNewSMDatabase(void)
{
	//m_pconnection = COTLObj::CreateObj("COciObj");
	m_pdb = nullptr;
	m_btrans = false;
	m_addbuffer_stmt = nullptr;
	m_findbuffer_stmt = nullptr;
	m_deletebuffer_stmt = nullptr;
	init();
}


CNewSMDatabase::~CNewSMDatabase(void)
{
	if(m_pdb)
	{
		sqlite3_finalize(m_addbuffer_stmt);
		sqlite3_finalize(m_findbuffer_stmt);
		sqlite3_finalize(m_deletebuffer_stmt);
		sqlite3_db_cacheflush(m_pdb);
		sqlite3_close(m_pdb);
		m_pdb=nullptr;
		sqlite3_shutdown();
	}
}



BOOL CNewSMDatabase::SendMessageToQueue(PTRMSG pMsg) // 消息进入函数
{
	
	ASSERT(pMsg);
	if(!pMsg)
		return FALSE;
	smpp_Message_header* pMsgHead = (smpp_Message_header*)pMsg;

	return SendMessage(pMsg, pMsgHead->Command_Length);
	
}

BOOL CNewSMDatabase::OnMessage(PTRMSG pMsg)
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



void CNewSMDatabase::OnTime()
{
	//if (GetTickCount() - m_LastExeDBTime > (m_NotExeDBTime * 60 * 1000))
	//{
	//	if (m_pLink)
	//	{
	//		ReturnLink(m_pLink, FALSE);
	//		m_pLink = NULL;
	//	}

	//	m_LastExeDBTime = GetTickCount();
	//}
	static time_t last_time;
	if(time(nullptr)-last_time> m_timecheck)
	{
		last_time = time(nullptr);
		if(m_pdb)
		{
			AutoTrans _tran(this);
			char tmp[2000];
			sprintf(tmp,"select * from sm_cache_table where savetime < datetime('now','-%d minute','localtime')",m_timeout_minutes);

			PrintMessage("check db timeout sql=[%s]",tmp);
			std::string  sql = tmp;
			sqlite3_stmt* stmt1 = nullptr;

			std::list<std::string> unikeylist;
			//get now time str;
			char time_buff[30];
			struct tm *timeinfo;
			time_t rawtime = time(nullptr);
			timeinfo = localtime (&rawtime);
			strftime(time_buff,sizeof(time_buff),"%Y-%m-%d %H:%M:%S",timeinfo);


			auto rc = sqlite3_prepare_v2(m_pdb,sql.c_str(),-1,&stmt1,nullptr);
			if(rc != SQLITE_OK )
			{
			}

			while(sqlite3_step(stmt1)==SQLITE_ROW)
			{

				char* punikey = (char*)sqlite3_column_text(stmt1, 0);
				char* psavetime = (char*)sqlite3_column_text(stmt1, 1);
				SaveShortMessage *poutdata = (SaveShortMessage*)sqlite3_column_blob(stmt1,2);
				int sz =sqlite3_column_bytes(stmt1,2);
				//copy data
				unikeylist.push_back(punikey);
				char buff[SAVE_MESSAGE_LEN + 1];
				memset(buff, 0, sizeof(buff));
				SaveShortMessage* pdata = (SaveShortMessage*)(buff+sizeof(smpp_Message_header));
				SaveShortMessage& data = *pdata;
				memset(&data, 0, sizeof(data));
				memcpy(&data,poutdata,sz);	
				strncpy(data.serviceUpID,"DBEXPR", sizeof(data.serviceUpID) - 1);
				if(m_DoneTimeSrc)
				{
						//取文本话单格式中处理结束时间字段的值取状态报告中的done_time时间
						strncpy(data.deliverTime, time_buff, sizeof(data.deliverTime) - 1);
				}
				strncpy(data.status,"560",sizeof(data.status) - 1);
				data.RecvID = 0;
				smpp_Message_header& head = *((smpp_Message_header*)buff);
				head.Command_ID = CMD_CHECK_EXPIRE_RESULT;
				head.Command_Length = SAVE_MESSAGE_LEN;	

				CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
				CMainProcMng * pMainProc = pApp->pMainMng;	
				int nRet = pMainProc->m_pSmsThread->SendMessage(buff, sizeof(buff));
				if (0 == nRet)
				{
					PrintMessage("Timeout SendMsgToSMSThread fail unikey[%s]",punikey);
				}

			}		

			sqlite3_finalize(stmt1);

			while(unikeylist.size())
			{
				m_ntrans_count++;
				char* perro;
				sql = "delete from sm_cache_table where unikey ='";
				sql += *unikeylist.begin();
				sql +="';";
				auto rt = sqlite3_exec(m_pdb,sql.c_str(),0,0,&perro);
				if(rt!=SQLITE_OK)
				{
					PrintMessage("Timeout delete from db error unikey[%s]",unikeylist.begin()->c_str());
				}
				else
				{
					PrintMessage("Timeout delete from db ok unikey[%s]",unikeylist.begin()->c_str());
				}
				unikeylist.pop_front();
			}

		}

	}

}


BOOL CNewSMDatabase::OnMountMessage()
{
	//if(m_pconnection==nullptr)
	//{
	//	m_pconnection = COTLObj::CreateObj("COciObj");

	//}

	if(m_pdb==nullptr)
	{
		
		//m_db_file_name = "c:/sqlite/testdb.db";
		//m_db_tmp_dir = "c:/sqlite/tmp";
		//sqlite3_temp_directory = (char*)m_db_tmp_dir.c_str();
		int rc = sqlite3_open(m_db_file_name.c_str(),&m_pdb);
		if( rc == SQLITE_OK)
		{

			std::string  sql = "create table if not exists sm_cache_table "
				"( unikey char(60)  PRIMARY KEY not null,"
				"savetime char(30) not null,"
				"data  blob not null );";

			rc = sqlite3_exec(m_pdb,sql.c_str(),0,0,0);  

			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("[%s] error  code:[%d] msg:[%s]",sql.c_str(),nerror,perror);
			}
			else
			{
				PrintMessage("[%s] ok",sql.c_str());

			}


			sql = "CREATE INDEX if not exists index_sm_cache_unikey"
				 " ON sm_cache_table (unikey);";

			rc = sqlite3_exec(m_pdb,sql.c_str(),0,0,0);  
			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("[%s] error  code:[%d] msg:[%s]",sql.c_str(),nerror,perror);
			}
			else
			{
				PrintMessage("[%s] ok",sql.c_str());

			}
 
			sql = "CREATE INDEX if not exists index_sm_cache_time"
				 " ON sm_cache_table (savetime);";

			rc = sqlite3_exec(m_pdb,sql.c_str(),0,0,0);
			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("[%s] error  code:[%d] msg:[%s]",sql.c_str(),nerror,perror);
			}
			else
			{
				PrintMessage("[%s] ok",sql.c_str());

			}


			rc = sqlite3_prepare_v2(m_pdb,"insert into sm_cache_table values(?,?,?);",-1,&m_addbuffer_stmt,nullptr);
			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("sqlite3_prepare_v2[%s] error  code:[%d] msg:[%s]","insert into sm_cache_table values(?,?,?);",nerror,perror);
			}
			else
			{
				PrintMessage("sqlite3_prepare_v2[%s] ok","insert into sm_cache_table values(?,?,?);");

			}
			rc = sqlite3_prepare_v2(m_pdb,"select * from sm_cache_table where unikey = ?;",-1,&m_findbuffer_stmt,nullptr);
			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("sqlite3_prepare_v2[%s] error  code:[%d] msg:[%s]","select * from sm_cache_table where unikey = ?;",nerror,perror);
			}
			else
			{
				PrintMessage("sqlite3_prepare_v2[%s] ok","select * from sm_cache_table where unikey = ?;");

			}
			rc = sqlite3_prepare_v2(m_pdb,"delete from sm_cache_table where unikey = ?;",-1,&m_deletebuffer_stmt,nullptr);
			if(rc !=SQLITE_OK)
			{
				auto perror = sqlite3_errmsg(m_pdb);
				auto nerror = sqlite3_errcode(m_pdb);
				PrintMessage("sqlite3_prepare_v2[%s] error  code:[%d] msg:[%s]","delete from sm_cache_table where unikey = ?;",nerror,perror);
			}
			else
			{
				PrintMessage("sqlite3_prepare_v2[%s] ok","delete from sm_cache_table where unikey = ?;");

			}

			PrintMessage("init sqlite db ok ");

		}
		else
		{
			auto perror = sqlite3_errmsg(m_pdb);
			auto nerror = sqlite3_errcode(m_pdb);

			PrintMessage("open db error  code:[%d] msg:[%s]",nerror,perror);
			if(m_pdb)
				sqlite3_close(m_pdb);
			m_pdb = nullptr;

		}
	}



	//if(m_pconnection == nullptr )
	//	return false;

	//auto rt = m_pconnection->init("");

	//if(rt!=0)
	//	return false;
	g_nConnDBTimeOutCount = 0;
	//CSmServiceProcApp *pApp = (CSmServiceProcApp*)AfxGetApp();
	//CMainProcMng *pMainMng = (CMainProcMng *)pApp->pMainMng;	
	//PDBPROCESS  pDB;
	//
	//if (!m_pLink)
	//{
	//	pMainMng->GetDbProcess(m_pLink, pDB);
	//}
	//
	//if (!m_pLink)
	//{
	//	g_nConnDBTimeOutCount++;
	//	return FALSE;
	//}
	//else
	//{
	//	g_nConnDBTimeOutCount = 0;
	//}

	//if(false == m_bLoadMonthFee)
	//{
	//	//加载包月扣费记录
	//	COleDateTime tmCurTm = COleDateTime::GetCurrentTime();
	//	CString sTm = tmCurTm.Format("%Y/%m/%d %H:%M:%S");
	//	
	//	char FeeMonth[32];
	//	CalFixMsgFeeMonth(sTm, FeeMonth);
	//	m_bLoadMonthFee = LoadFixMsg(FeeMonth);

	//	if(m_bLoadMonthFee)
	//	{
	//		pApp->GetMonthFee()->SetLoadFromDBIsOK();
	//	}
	//}

	return true;
}

//
//std::shared_ptr<COTLObj> CNewSMDatabase::CreateDBConnection()
//{
//	return COTLObj::CreateObj("COciObj");
//}


void CNewSMDatabase::OnAddDBBuffer(LPCTSTR pMsg)
{
	smpp_Message_header * pHeader = (smpp_Message_header *)pMsg;
	SaveShortMessage  *pSaveSm = (SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	

	//if(m_pconnection)
	//{
	//	COTLObj::input_params  in;
	//	COTLObj::output_params  out;

	//	{
	//		std::shared_ptr<COTLObj::in_param_ref> pparam = std::make_shared<COTLObj::in_param_ref>();
	//		pparam->ref = false;
	//		pparam->type = COTLObj::param_type_var_char;
	//		pparam->valueRef = nullptr;
	//		pparam->psvalue = std::make_shared<std::string>(pSaveSm->unikey);
	//		pparam->len = pparam->psvalue->size();
	//		in.push_back(pparam);
	//	}


	//	m_pconnection->ExcuteSQL(m_strAddSMBuff.c_str(),in,out);


	//}
	if(m_pdb)
	{
		int rt;
		char time_buff[30];
		struct tm *timeinfo;
		
		AutoTrans _tran(this);
		m_ntrans_count++;
		sqlite3_reset(m_addbuffer_stmt);
	
		rt = sqlite3_bind_text(m_addbuffer_stmt,1,pSaveSm->unikey,min(strlen(pSaveSm->unikey),sizeof(pSaveSm->unikey)),nullptr);
		
		time_t rawtime = time(nullptr);
		timeinfo = localtime (&rawtime);
		strftime(time_buff,sizeof(time_buff),"%Y-%m-%d %H:%M:%S",timeinfo);

		rt = sqlite3_bind_text(m_addbuffer_stmt,2,time_buff,min(strlen(time_buff),sizeof(time_buff)),nullptr);

		rt = sqlite3_bind_blob(m_addbuffer_stmt,3,pSaveSm,sizeof(SaveShortMessage),SQLITE_STATIC);

		rt = sqlite3_step(m_addbuffer_stmt);

		if(rt!=SQLITE_DONE)
		{
			auto perror = sqlite3_errmsg(m_pdb);
			auto nerror = sqlite3_errcode(m_pdb);
		    //insert error
			PrintMessage("OnAddDBBuffer error  code:[%d] msg:[%s]  unikey[%s]",nerror,perror,pSaveSm->unikey);
			return;
		}
		else
		{
			PrintMessage("OnAddDBBuffer ok  unikey[%s]",pSaveSm->unikey);
			//Sleep(1);
		}
	}

	return; 
}

void CNewSMDatabase::OnFindMsgInDBBuffer(LPCTSTR pMsg)
{
	
    smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	SaveShortMessage *pData = (SaveShortMessage *)(pMsg + sizeof(smpp_Message_header));
	
	CString sub_time, done_time, err_code;
	sub_time = pData->submitTime;
	done_time = pData->deliverTime;
	err_code = pData->serviceUpID;


	if(m_pdb)
	{
		int rt;
		char time_buff[30];
		struct tm *timeinfo;
		
		AutoTrans _tran(this);

		sqlite3_reset(m_findbuffer_stmt);
	
		rt = sqlite3_bind_text(m_findbuffer_stmt,1,pData->unikey,min(strlen(pData->unikey),sizeof(pData->unikey)),nullptr);
		if(rt!=SQLITE_OK)
		{
			const char* errmsg = sqlite3_errmsg(m_pdb);
		}


		rt = sqlite3_step(m_findbuffer_stmt);
		if(SQLITE_ROW!=rt) //rt!=SQLITE_OK&&SQLITE_DONE!=rt&&
		{
			//find error

			auto perror = sqlite3_errmsg(m_pdb);
			auto nerror = sqlite3_errcode(m_pdb);
		    //insert error
			PrintMessage("OnFindMsgInDBBuffer error  code:[%d] msg:[%s]  unikey[%s]",nerror,perror,pData->unikey);
			return;
		}


		//get data
		char* punikey = (char*)sqlite3_column_text(m_findbuffer_stmt, 0);
		char* psavetime = (char*)sqlite3_column_text(m_findbuffer_stmt, 1);
		SaveShortMessage *poutdata = (SaveShortMessage*)sqlite3_column_blob(m_findbuffer_stmt,2);
		int sz =sqlite3_column_bytes(m_findbuffer_stmt,2);
		//copy data

		char buff[SAVE_MESSAGE_LEN + 1];
		memset(buff, 0, sizeof(buff));
		SaveShortMessage* pdata = (SaveShortMessage*)(buff+sizeof(smpp_Message_header));

		SaveShortMessage& data = *pdata;
		memset(&data, 0, sizeof(data));
		memcpy(&data,poutdata,sz);	

		strncpy(data.serviceUpID, (LPCTSTR)err_code, sizeof(data.serviceUpID) - 1);
		if(m_DoneTimeSrc)
		{
				//取文本话单格式中处理结束时间字段的值取状态报告中的done_time时间
				strncpy(data.deliverTime, done_time, sizeof(data.deliverTime) - 1);
		}
		strncpy(data.status,pData->status,sizeof(data.status) - 1);
		data.RecvID = 0;
	
		smpp_Message_header& head = *((smpp_Message_header*)buff);
		head.Command_ID = CMD_FIND_DB_BUFFER_RESULT;
		head.Command_Length = SAVE_MESSAGE_LEN;
	
		CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();
		CMainProcMng * pMainProc = pApp->pMainMng;
	
		int nRet = pMainProc->m_pSmsThread->SendMessage(buff, sizeof(buff));

		if (0 == nRet)
		{
			PrintMessage("SendMsgToSMSThread fail uniket:[%s]",pData->unikey);
		}

		m_ntrans_count++;
		sqlite3_reset(m_deletebuffer_stmt);
		rt = sqlite3_bind_text(m_deletebuffer_stmt,1,pData->unikey,min(strlen(pData->unikey),sizeof(pData->unikey)),nullptr);
		rt = sqlite3_step(m_deletebuffer_stmt);

		if(rt!=SQLITE_DONE)
		{
			//delete error
			auto perror = sqlite3_errmsg(m_pdb);
			auto nerror = sqlite3_errcode(m_pdb);
		    //insert error
			PrintMessage("OnFindMsgInDBBuffer delete error  code:[%d] msg:[%s]  unikey[%s]",nerror,perror,pData->unikey);
			return;
		}


		//time_t rawtime = time(nullptr);
		//timeinfo = localtime (&rawtime);
		//strftime(time_buff,sizeof(time_buff),"%Y/%m/%d %H:%M:%S",timeinfo);

		//rt = sqlite3_bind_text(m_addbuffer_stmt,2,time_buff,min(strlen(time_buff),sizeof(time_buff)),nullptr);

		//rt = sqlite3_bind_blob(m_addbuffer_stmt,3,pSaveSm,sizeof(SaveShortMessage),SQLITE_STATIC);

	
	}

			
	return;	
}


void CNewSMDatabase::OnDeleteFixMsg(PTRMSG pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	LPFIXMSGINFO pFixMsg = (LPFIXMSGINFO)(pMsg + sizeof(smpp_Message_header));
	return;		
}

void CNewSMDatabase::OnSaveFixMsg(PTRMSG pMsg)
{
	smpp_Message_header *pHeader = (smpp_Message_header *)pMsg;
	LPFIXMSGINFO pFixMsg = (LPFIXMSGINFO)(pMsg + sizeof(smpp_Message_header));
	return;
}

void CNewSMDatabase::OnUpdateFeeMonthTable(LPCTSTR pMsg)
{
	ASSERT(pMsg);
	
	smpp_Message_header * pHeader = (smpp_Message_header *)pMsg;
    tagFeeMonthStatus * pInfo = (tagFeeMonthStatus *)(pMsg + sizeof(smpp_Message_header));
	return;
}


bool CNewSMDatabase::init(void)
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
	
	
	Ret = GetPrivateProfileString("SQLITE_CONFIG", 
		"DBName", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "SM_CACHE_DB.DB");
		WritePrivateProfileString("SQLITE_CONFIG",
			"DBName",
			sTemp,
			InitName);
	}
	m_db_file_name = sTemp ;


	Ret = GetPrivateProfileString("SQLITE_CONFIG", 
		"TimeToCheck", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "100");
		WritePrivateProfileString("SQLITE_CONFIG",
			"TimeToCheck",
			sTemp,
			InitName);
	}
	m_timecheck = atoi(sTemp);

	Ret = GetPrivateProfileString("SQLITE_CONFIG", 
		"TimeoutMinute", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if(Ret == 0)
	{
		strcpy(sTemp, "2880");
		WritePrivateProfileString("SQLITE_CONFIG",
			"TimeoutMinute",
			sTemp,
			InitName);
	}
	m_timeout_minutes = atoi(sTemp);


	return true;
}


// try to begin a trans
void CNewSMDatabase::begin_trans(void)
{
	if(m_pdb)
	{
		if(!m_btrans)
		{
			auto rc = sqlite3_exec(m_pdb,"begin;",0,0,0);  
			//reset all tmp value
			m_btrans = true;
			m_ntrans_count = 0;
			m_trans_time = time(nullptr);
		}
	}
}


// try to end tran
void CNewSMDatabase::end_trans(void)
{
	if(m_pdb)
	{
		if(m_btrans)
		{
			if(time(nullptr)-m_trans_time>10)
			{
				auto rc = sqlite3_exec(m_pdb,"commit;",0,0,0); 
				m_btrans = false;
				PrintMessage("sql3 commit %d ",m_ntrans_count);
			}
			else
			{
				//m_ntrans_count++;
			}
		}
	}
}
