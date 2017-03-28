// CDataConeMng.cpp: implementation of the CLinkMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataConeMng.h"
//#include "SqlObj.h"
#include "AdoObj.h"

#include "regkey.h"
#include "eventlog.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataConeMng::CDataConeMng()
{
	WriteEventLog(1, EVENTLOG_INFORMATION_TYPE, "ReLinkMng", "Prmt", "");
	
    m_sIniFileName = ".\\FeeConfig.ini";
	m_SQlFirst = TRUE ;
	m_DataSrcCount = 0 ;
	
    ReadDataSrcInfo();
	
	InitializeCriticalSection(&m_Lock);
	InitializeCriticalSection(&m_LockBad);
}

void CDataConeMng::Init()
{
	InitLink();
	
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	
	DWORD type = m_DataSrc[0].GetDataSrcType();
	
	m_hThread = CreateThread(
		NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);						
}


CDataConeMng::~CDataConeMng()
{
	SetEvent(m_hKillEvent);
	DWORD type = m_DataSrc[0].GetDataSrcType();
	
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
   	CloseHandle(m_hThread);
	
	CloseHandle(m_hKillEvent);
	
	DestroyLink();
	DeleteCriticalSection(&m_Lock);
	DeleteCriticalSection(&m_LockBad);
	//if(!m_SQlFirst)
	//	dbexit();
}

CLinkObject* CDataConeMng::GetLink(const char *src)
{
	ASSERT(src);
    if( NULL == src)
        return NULL;
	EnterCriticalSection(&m_Lock);
	for(int i =0; i<m_DBList.GetSize();i++)
	{
		CLinkObject* p = m_DBList.GetAt(i);
		CString src1 = p->GetDataSrc() ;
		if(src1.CompareNoCase(src) == 0)
		{
			m_DBList.RemoveAt(i);
			LeaveCriticalSection(&m_Lock);
			
			if(!(p->IsLinkOk()))
			{ 
				p->DestroyLink() ;
				p->CreateLink() ;
				
				if (!(p->IsLinkOk()))
				{
					//如果这时依然没有连接上,则加入到坏队列
					p->DestroyLink() ;
					
					EnterCriticalSection(&m_LockBad);
					m_BadList.Add(p);
					LeaveCriticalSection(&m_LockBad);
					return NULL;
				}
				
			}
			return p ;
		}
	}
	LeaveCriticalSection(&m_Lock);
    TRACE("Db src error \n");
	
    WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, \
		"Db src error", "ServerError", \
		src);
	
    //系统信息
    CString sStr;
	sStr.Format("数据源%s错误", src);
    WriteSysLogRecord((LPCSTR)sStr, \
        NULL, 0, LOG_WARN_GRAVITY_ERROR);    
	
	return NULL ;
}

void CDataConeMng::ReturnLink(CLinkObject *obj,BOOL flag)
{
	if(!obj)
		return ;
	if(flag && obj->IsLinkOk() )
	{
		EnterCriticalSection(&m_Lock);
		m_DBList.Add(obj);
		LeaveCriticalSection(&m_Lock);
		return ;
	}
	
	// SMGW41-60, 2005-06-13, wzy add begin //
	obj->DestroyLink();
	
	EnterCriticalSection(&m_LockBad);	
	m_BadList.Add(obj);	
	LeaveCriticalSection(&m_LockBad);
	// SMGW41-60, 2005-06-13, wzy add end //	
	
	WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, \
		"LinkMngSql", "Bad Link", \
		obj->GetDataSrc());
}

void CDataConeMng::InitLink()
{
	for(int i = 0 ;i<(int)m_DataSrcCount ; i++)
	{
		DWORD type = m_DataSrc[i].GetDataSrcType();
		int count = m_DataSrc[i].GetConnectNumber();
		switch(type)
		{
		case ADO_LIB:
			{
				for(int j = 0 ;j<count; j++)
				{
					CAdoObj* p = new CAdoObj(&m_DataSrc[i]) ;
					ASSERT(p) ;
                    if(p){
						m_DBList.Add(p) ;
                        if(!p->IsLinkOk())
                        {
                            WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, \
                                "LinkMngADO", m_DataSrc[i].GetServerName(), \
                                m_DataSrc[i].GetDataSrc());
							
                            CString sStr;
							sStr.Format("数据库%s,%s连接初始化失败", \
                                m_DataSrc[i].GetServerName(), m_DataSrc[i].GetDataSrc());
                            WriteSysLogRecord((LPCSTR)sStr, \
                                NULL, 0, LOG_WARN_GRAVITY_ERROR);
                        }
                    }
                    else
                    {
                        TRACE("ADO DataSrc Error \n");
						
                        //系统信息
                        CString sStr;
						sStr.Format("数据库%s,%s初始化失败", \
                            m_DataSrc[i].GetServerName(), m_DataSrc[i].GetDataSrc());
                        WriteSysLogRecord((LPCSTR)sStr, \
                            NULL, 0, LOG_WARN_GRAVITY_ERROR);
                    }
				}
			}
			break;
		case MS_SQL_DB_LIB:
			{
				//if(m_SQlFirst)
				//{
				//	dbinit();
				//	m_SQlFirst = FALSE ;
				//}
				//
				//for(int j = 0 ;j<count; j++)
				//{
				//	CSqlObj* p = new CSqlObj(&m_DataSrc[i]);
				//	ASSERT(p);
				//	if(p)
    //                {
				//		m_DBList.Add(p);
    //                    if(!p->IsLinkOk())
    //                    {
    //                        WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, \
    //                            "LinkMngSql", m_DataSrc[i].GetServerName(), \
    //                            m_DataSrc[i].GetDataSrc());
    //                        
    //                        CString sStr;
				//			sStr.Format("数据库%s,%s连接初始化失败", \
    //                            m_DataSrc[i].GetServerName(), m_DataSrc[i].GetDataSrc());
    //                        WriteSysLogRecord((LPCSTR)sStr, \
    //                            NULL, 0, LOG_WARN_GRAVITY_ERROR);
    //                    }
    //                }
    //                else
    //                {
    //                    TRACE("Sql DataSrc Error \n");
				//		
    //                    CString sStr;
				//		sStr.Format("数据库%s,%s初始化失败", \
    //                        m_DataSrc[i].GetServerName(), m_DataSrc[i].GetDataSrc());
    //                    WriteSysLogRecord((LPCSTR)sStr, \
    //                        NULL, 0, LOG_WARN_GRAVITY_ERROR);
    //                }
				//	
				//}
			}
			break;
		case SYBASE_CT_LIB:
            WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, "LinkMngSybase", "Error", "");
			break;
		case ODBC_LIB:
            {
				for(int j = 0 ;j<count; j++)
				{
					CODBCObj* p = new CODBCObj(&m_DataSrc[i]) ;
					ASSERT(p) ;
					if(p)
						m_DBList.Add(p) ;
				}
			}
			break;
			
		default:
            WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, "LinkMngOther", "Error", "");
			break;
		}
	}
}

void CDataConeMng::DestroyLink()
{
	EnterCriticalSection(&m_Lock);
	int i;
	for(i = 0 ;i <m_DBList.GetSize();i++)
	{
		delete m_DBList.GetAt(i);
	}
	m_DBList.RemoveAll();
	LeaveCriticalSection(&m_Lock);
	
	EnterCriticalSection(&m_LockBad);
	for(i = 0 ;i <m_BadList.GetSize();i++)
	{
		delete m_BadList.GetAt(i);
	}
	m_BadList.RemoveAll();
	LeaveCriticalSection(&m_LockBad);
}


int CDataConeMng::InitDataSourceInfo(DATASOURCE* pBuff, int count)
{
	int i;
	if(pBuff == NULL){
		return 0;
	}
	
	if(count>MAX_DATA_SRC)
		count = MAX_DATA_SRC ;
	
	DATASOURCE* pSrc = pBuff;
	
	for(i = 0 ; i<(int)count ; i++)
	{
		m_DataSrc[i].SetDataSrcType(pSrc[i].Type);
		m_DataSrc[i].SetUserID((LPCTSTR)pSrc[i].UserID);
		m_DataSrc[i].SetConnectNumber(pSrc[i].Connects);
		m_DataSrc[i].SetServerName((LPCTSTR)pSrc[i].Server);
		m_DataSrc[i].SetPassword((LPCTSTR)pSrc[i].Pwd);
		m_DataSrc[i].SetDBName((LPCTSTR)pSrc[i].Database);
		m_DataSrc[i].SetDataSrc((LPCTSTR)pSrc[i].SourceID);
        m_DataSrc[i].SetProvider(pSrc[i].Provider);
		m_DataSrcCount ++ ;
	}
	
	return 1;
}

void CDataConeMng::ReadDataSrcInfo()
{
	DWORD count = GetDataSourceCount();
	if(!count)
	{
		ASSERT(0);
		return ;
	}
	if(count>MAX_DATA_SRC)
		count = MAX_DATA_SRC ;
	DATASOURCE* pSrc = new DATASOURCE[count] ;
	int i;
	for(i=0;i<(int)count;i++)
		GetDataSourceValue(&(pSrc[i]),i);
	
	for(i = 0 ; i<(int)count ; i++)
	{
		m_DataSrc[i].SetDataSrcType(pSrc[i].Type);
		m_DataSrc[i].SetUserID((LPCTSTR)pSrc[i].UserID);
		m_DataSrc[i].SetConnectNumber(pSrc[i].Connects);
		m_DataSrc[i].SetServerName((LPCTSTR)pSrc[i].Server);
		m_DataSrc[i].SetPassword((LPCTSTR)pSrc[i].Pwd);
		m_DataSrc[i].SetDBName((LPCTSTR)pSrc[i].Database);
		m_DataSrc[i].SetDataSrc((LPCTSTR)pSrc[i].SourceID);
        m_DataSrc[i].SetProvider(pSrc[i].Provider);
		m_DataSrcCount ++ ;
	}
	delete[] pSrc ;
	return ;
}

CDataSource* CDataConeMng::GetDataSrc(const char *src)
{
	ASSERT(src);
	for(int i = 0 ;i<(int)m_DataSrcCount ;i++)
	{
		CDataSource* p = &m_DataSrc[i] ;
		if((p->GetDataSrc()).CompareNoCase(src) == 0)
			return p ;
	}
	return NULL ;
}

DWORD WINAPI CDataConeMng::ThreadProc(LPVOID lpParam)
{
	CDataConeMng* pVM = (CDataConeMng *)lpParam;
	ASSERT(pVM);
	while( WaitForSingleObject(pVM->m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		EnterCriticalSection(&pVM->m_LockBad);
		if((pVM->m_BadList).GetSize()>0)
		{
			CLinkObject* p = pVM->m_BadList.GetAt(0);
			ASSERT(p);
			p->DestroyLink();
			//p->CreateLink();
			WriteEventLog(1, EVENTLOG_INFORMATION_TYPE, "ReLinkMng", "Error", "");
            
            //if (p->IsLinkOk())
            //{
			pVM->m_BadList.RemoveAt(0);
			
			EnterCriticalSection(&pVM->m_Lock);
			pVM->m_DBList.Add(p);
			LeaveCriticalSection(&pVM->m_Lock);
			
			WriteEventLog(0, EVENTLOG_INFORMATION_TYPE, \
				"LinkMngSql", "ReLink Success", \
				p->GetDataSrc());
            //}
            //else
            //{
            //    pVM->m_BadList.RemoveAt(0);
            //    pVM->m_BadList.Add(p);
            //}
			
		}
		LeaveCriticalSection(&pVM->m_LockBad);
		Sleep(10);
	}
	return 0L;
}


int CDataConeMng::GetDataSourceCount()
{
    int iCount;
	int Ret = 0;
	char pBuff[256];
	Ret = GetPrivateProfileString("DataSrcCount",
		"DataSrcCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "1");
		WritePrivateProfileString("DataSrcCount",
			"DataSrcCount",
			pBuff,
			m_sIniFileName);
	}
	iCount = atoi(pBuff);
	return iCount;
}

int CDataConeMng::GetDataSourceValue(DATASOURCE* pDataSrc, int nIndex)
{
	char sDataSrcSect[100];
	sprintf(sDataSrcSect,"DataSrc%d", nIndex);
	
	int Ret = 0;
	char pBuff[256];
	Ret = GetPrivateProfileString(sDataSrcSect,
		"ServerName",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "(local)");
		WritePrivateProfileString(sDataSrcSect,
			"ServerName",
			pBuff,
			m_sIniFileName);
	}
    strcpy((char*)&(pDataSrc->Server),pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"DataSrcName",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "DBSMSERVICE");
		WritePrivateProfileString(sDataSrcSect,
			"DataSrcName",
			pBuff,
			m_sIniFileName);
	}
    strcpy((char*)&(pDataSrc->SourceID),pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"DBName",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "SMGWDB");
		WritePrivateProfileString(sDataSrcSect,
			"DBName",
			pBuff,
			m_sIniFileName);
	}
    strcpy((char*)&(pDataSrc->Database), pBuff);
	
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"DBType",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		ASSERT(0);
		strcpy(pBuff, "3");
		WritePrivateProfileString(sDataSrcSect,
			"DBType",
			pBuff,
			m_sIniFileName);
	}
    pDataSrc->Type = atoi(pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"Provider",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		sprintf(pBuff, "%d", ADO_SQL_SERVER);
		WritePrivateProfileString(sDataSrcSect,
			"Provider",
			pBuff,
			m_sIniFileName);
	}
    pDataSrc->Provider = atoi(pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"UserID",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "sa");
		WritePrivateProfileString(sDataSrcSect,
			"UserID",
			pBuff,
			m_sIniFileName);
	}
	strcpy((char*)&(pDataSrc->UserID), pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"UserPass",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
	if(Ret == 0)
	{
		strcpy(pBuff, "");
		WritePrivateProfileString(sDataSrcSect,
			"UserPass",
			pBuff,
			m_sIniFileName);
	}
	strcpy((char*)&(pDataSrc->Pwd),pBuff);
	
    Ret = GetPrivateProfileString(sDataSrcSect,
		"ConnectCount",
		"",
		pBuff,
		sizeof(pBuff),
		m_sIniFileName);
    if(Ret == 0)
	{
		strcpy(pBuff, "5");
		WritePrivateProfileString(sDataSrcSect,
			"ConnectCount",
			pBuff,
			m_sIniFileName);
	}
	pDataSrc->Connects = atoi(pBuff);
	if(pDataSrc->Connects < 3)
		pDataSrc->Connects = 3;
    ASSERT(pDataSrc->Connects != 0);
	return 0;
}


void CDataConeMng::WriteSysLogRecord(LPCSTR szPmt, LPCSTR pBuff, int iBuffSize, int iWarnLevel)
{
    TRACE("请重载此警告记录消息\n");
    return;
}
