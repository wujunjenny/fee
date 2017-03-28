// SqlObj.cpp: implementation of the CSqlObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "smcapp.h"
#include "SqlObj.h"
#include "datasource.h"
#include "DataConeMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqlObj::CSqlObj(CDataSource* father)
{
	ASSERT(father);
	m_pFather = father ;
	m_DataSrc = m_pFather->GetDataSrc();
	
    m_iDbExecTimeOutSec = LoadDbExecTimeOutSet();
    //end add
	
	CreateLink();
}

CSqlObj::~CSqlObj()
{
	DestroyLink();
}

BOOL CSqlObj::IsLinkOk()
{
	if((DBPROCESS*)m_pProc == (DBPROCESS*)NULL)
		return FALSE ;
	if(dbdead((DBPROCESS*)m_pProc))
		return FALSE;
	return TRUE;
	
}
void CSqlObj::DestroyLink()
{
	if((DBPROCESS*)m_pProc)
	{
		dbclose((DBPROCESS*)m_pProc);
		m_pProc = NULL ;
	}
}
BOOL CSqlObj::CreateLink()
{
	char HostName[256];
	DWORD size =256;
	GetComputerName(HostName,&size);
	
	LOGINREC* pLogin ;
	BOOL vInitRet = FALSE;
	
	DBLOCKLIB();
	pLogin = dblogin() ;
	
	ASSERT(pLogin !=(LOGINREC*) NULL);
	
	if(pLogin == NULL)
		return  vInitRet;
	
	DBSETLUSER(pLogin , (LPCTSTR)m_pFather->GetUserID());
	
	DBSETLAPP(pLogin , AfxGetAppName());
	
	DBSETLPWD(pLogin , (LPCTSTR)m_pFather->GetPassword());
	
	DBSETLHOST(pLogin , (LPCTSTR)HostName);
	DBSETLVERSION(pLogin,DBVER60) ;
	dbsetlogintime(DBLOGIN_TIMEOUT);
	(DBPROCESS*)m_pProc = dbopen(pLogin ,(LPCTSTR)m_pFather->GetServerName() );
	CString DefaultName = m_pFather->GetDBName();
	
	if((DBPROCESS*)m_pProc !=(DBPROCESS*)NULL)
LABEL1:
	if(dbuse(m_pProc , (LPCTSTR)DefaultName)==FAIL)
		vInitRet = FALSE;
	else
	{
		CString yy = dbname((DBPROCESS*)m_pProc);
		VERIFY(!yy.IsEmpty());
		if(DefaultName.CompareNoCase(yy) == 0)
		{
			BOOL bRet;
			bRet = dbsettime(m_iDbExecTimeOutSec);
			ASSERT(bRet);
			//end add
			vInitRet = TRUE;
		}
		else
		{
			DefaultName = _T("master");
			Sleep(100);
			goto LABEL1;
		}
	}
	else
		vInitRet = FALSE;
	DBUNLOCKLIB();
	return vInitRet;
}

int CSqlObj::LoadDbExecTimeOutSet()
{
    int iRet = 0;
    char pBuff[128];
    int iTimeOut = 0;
    memset(pBuff, 0, sizeof(pBuff));
    iRet = GetPrivateProfileString("SqlConfig", \
        "DbExecTimeOutSec", \
        "", pBuff, sizeof(pBuff), \
        ".\\LinkMng.ini");
    if (0 == iRet)
    {
        ASSERT(0);
        strcpy(pBuff, "0");
        WritePrivateProfileString("SqlConfig", \
            "DbExecTimeOutSec", pBuff, \
            ".\\LinkMng.ini");
    }
    
    iTimeOut = atoi(pBuff);
	
    if (iTimeOut < 0)
    {
        ASSERT(0);
        strcpy(pBuff, "0"); 
        WritePrivateProfileString("SqlConfig", \
            "DbExecTimeOutSec", pBuff, \
            ".\\LinkMng.ini");
        iTimeOut = atoi(pBuff);
    }
	
    return iTimeOut;
}
