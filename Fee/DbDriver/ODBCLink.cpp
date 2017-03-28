// ODBCLink.cpp: implementation of the CODBCObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ODBCLink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CODBCObj::CODBCObj(CDataSource* father)
{
	ASSERT(father);
	m_pFather = father ;
	m_DataSrc = m_pFather->GetDataSrc();
	SetConnect(m_pFather->GetServerName(),m_pFather->GetUserID(),
		m_pFather->GetDBName(),m_pFather->GetPassword()) ;
	
	m_nLinkState = LINK_STATE_IDLE ;
	
	CreateLink();
}

CODBCObj::~CODBCObj()
{
	DestroyLink();
}


BOOL CODBCObj::ConnectDB(void)
{
	
	if(m_nLinkState != LINK_STATE_IDLE)
		return FALSE ;
	
	try
	{
		m_dbCust.OpenEx(m_StrConnect,CDatabase::noOdbcDialog);
		SQLSetConnectOption(m_dbCust.m_hdbc, 
			SQL_CURRENT_QUALIFIER, 
			(unsigned long)LPCSTR(m_Database));
	}
	catch(...)
	{
		TRACE("连接发生异常！\n");
		return FALSE ;
	}
	
	TRACE("	连接成功!");	
	RETCODE  nRetCode;
	
	AFX_ODBC_CALL(::SQLAllocStmt(m_dbCust.m_hdbc, &(m_hstmt)));
	if (nRetCode!=SQL_SUCCESS&&nRetCode!=SQL_SUCCESS_WITH_INFO)
	{
		RETCODE  nRetCode;
		UCHAR	szErrorMsg[256];
		SDWORD   NativeError;
		UCHAR FAR	szSqlState[16];
		SWORD    cbErrorMsg,cbErrorMsgMax=255 ;
		
		AFX_ODBC_CALL(::SQLError(SQL_NULL_HENV,
			m_dbCust.m_hdbc,
			SQL_NULL_HSTMT, 
			szSqlState, 
			&NativeError, 
			szErrorMsg, 
			cbErrorMsgMax, 
			&cbErrorMsg)); 
		if(m_dbCust.IsOpen())
			m_dbCust.Close() ;
		return FALSE ;
	}
	
	AFX_ODBC_CALL(::SQLSetStmtOption(m_hstmt, SQL_QUERY_TIMEOUT, 5)); 
	m_pProc = m_hstmt ;
	m_nLinkState = LINK_STATE_AVAIL ;
	return TRUE ;
	
}

BOOL CODBCObj::DisConnect(void) 
{
	ASSERT(m_nLinkState == LINK_STATE_AVAIL ) ;
	if(m_nLinkState != LINK_STATE_AVAIL )
		return FALSE ;
	
	RETCODE	nRetCode;
	AFX_ODBC_CALL(::SQLFreeStmt(m_hstmt,SQL_DROP));
	if (m_dbCust.IsOpen())
		m_dbCust.Close();
	
	m_nLinkState = LINK_STATE_IDLE ;
	return TRUE ;
}


BOOL CODBCObj::ErrProc(void) 
{
	ASSERT(m_nLinkState == LINK_STATE_AVAIL) ;
	RETCODE	nRetCode;
	AFX_ODBC_CALL(::SQLFreeStmt(m_hstmt,SQL_DROP));
	if (m_dbCust.IsOpen())
		m_dbCust.Close();
	
	m_nLinkState = LINK_STATE_IDLE ;
	return TRUE ;
}

BOOL CODBCObj::DoTestSel(HSTMT hstmt) 
//select nID,Name from t_web 
{
	ASSERT(m_nLinkState == LINK_STATE_AVAIL) ;
	RETCODE	nRetCode;
	CString	strCmd="select Max(sm_ID),Min(sm_ID),count(sm_ID) from SM_HisTable1" ;
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS) ;
	AFX_ODBC_CALL(::SQLExecDirect(hstmt,(unsigned char *)LPCSTR(strCmd),SQL_NTS)) ;
	if (nRetCode==SQL_SUCCESS)
	{
		//SQLINTEGER   cbName,cbID ;
        SQLLEN   cbID ;
		
		SQLLEN   sID1;
		SQLLEN   sID2 ;
		SQLLEN   sID3 ;
		
		SQLBindCol(hstmt, 1, SQL_C_ULONG, &sID1,	0, &cbID);
		SQLBindCol(hstmt, 2, SQL_C_ULONG, &sID2,	0, &cbID);
		SQLBindCol(hstmt, 3, SQL_C_ULONG, &sID3,	0, &cbID);
		
		while (TRUE)
		{
			AFX_ODBC_CALL(::SQLFetch(hstmt));
			if (nRetCode==SQL_SUCCESS)
			{
				TRACE("nID=%d \n",sID1) ;
			}
			else
				break;
		}
		AFX_ODBC_CALL(::SQLFreeStmt(hstmt,SQL_CLOSE));
	}
	return TRUE ;
}

BOOL CODBCObj::DoTestProc(HSTMT hstmt) 
{
	ASSERT(m_nLinkState == LINK_STATE_AVAIL) ;
	
	RETCODE	nRetCode = 1;
	SQLLEN	cbID = SQL_NTS ;
	SQLLEN cbName = SQL_NTS ;
	SQLLEN cbRet = SQL_NTS ;
	
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	nRetCode = SQLPrepare(hstmt,
		(unsigned char *)"{call xp_TestProc(?,?,?)}" , 
		SQL_NTS); 
	
    if(nRetCode == SQL_ERROR )
		Diagnosis() ;	
	unsigned short usnID  = 1 ;
	AFX_ODBC_CALL(::SQLBindParameter(hstmt, 
		1,  
		SQL_PARAM_INPUT, 
		SQL_C_USHORT, 
		SQL_SMALLINT,
		0,  
		0, 
		&usnID,
		0, 
		&cbID)); 
    if(nRetCode == SQL_ERROR )
		Diagnosis() ;
	
	char csName[10] = "abcde" ;
	AFX_ODBC_CALL(::SQLBindParameter(hstmt, 
		2,  
		SQL_PARAM_INPUT, 
		SQL_C_CHAR, 
		SQL_VARCHAR,
		11 ,  
		0, 
		csName,
		11, 
		&cbName)); 
	
    if(nRetCode == SQL_ERROR )
		Diagnosis() ;
	
	unsigned short usRetCode = 9 ;
	AFX_ODBC_CALL(::SQLBindParameter(hstmt, 
		3,  
		SQL_PARAM_OUTPUT, 
		SQL_C_USHORT,
		SQL_SMALLINT,
		0, 
		0,
		&usRetCode ,	
		0, 
		&cbRet));
    if(nRetCode == SQL_ERROR )
		Diagnosis() ;
	
	
	int nRet = SQLExecute(hstmt);
	if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
	{
		//			ErrorDetect( pMsg );
		Diagnosis() ;
		
		if (nRetCode!=SQL_SUCCESS&&nRetCode!=SQL_SUCCESS_WITH_INFO)
		{
			RETCODE  nRetCode;
			UCHAR	szErrorMsg[256];
			SDWORD   NativeError;
			UCHAR FAR	szSqlState[16];
			SWORD    cbErrorMsg,cbErrorMsgMax=255 ;
			
			AFX_ODBC_CALL(::SQLError(SQL_NULL_HENV,
				m_dbCust.m_hdbc,
				SQL_NULL_HSTMT, 
				szSqlState, 
				&NativeError, 
				szErrorMsg, 
				cbErrorMsgMax, 
				&cbErrorMsg)); 
			return FALSE ;
		}
	}
	
	nRetCode = nRet ;
	
	if (nRetCode!=SQL_SUCCESS&&nRetCode!=SQL_SUCCESS_WITH_INFO)
	{
		ASSERT(0) ;
		return FALSE ;
	}
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	
	return TRUE ;
}


BOOL CODBCObj::DoBoth(HSTMT hstmt) 
{
	DoTestProc(hstmt) ;
	return TRUE ;
}


BOOL CODBCObj::IsLinkOk()
{
	if(m_nLinkState == LINK_STATE_AVAIL)
		return TRUE ;
	return FALSE ;
}

void CODBCObj::DestroyLink()
{
	DisConnect() ;
}

BOOL CODBCObj::CreateLink()
{
	ConnectDB();
    return TRUE;
}


void CODBCObj::SetConnect(CString DBAlias,CString UID,CString Database,CString PWD)
{
	m_Uid	=  UID;
	m_DbAlias= DBAlias;
	m_Database = Database;
	m_Pwd	 = PWD;
	
	m_StrConnect= _T("");
	m_StrConnect+=_T("DSN=");
	m_StrConnect+=m_DbAlias;
	
	m_StrConnect+=_T(";UID=");
	m_StrConnect+=m_Uid;
	m_StrConnect+=_T(";PWD=");
	m_StrConnect+=m_Pwd;
	m_StrConnect+=_T(";");
}

void CODBCObj::Diagnosis(void)
{
	SQLRETURN Ret ;
	SQLCHAR Sqlstate[6] ;
	SQLINTEGER NativeErrorPtr ;
	SQLCHAR MessageText[100 ];
	SQLSMALLINT TextLengthPtr ;
	
	Ret = SQLGetDiagRec(
		SQL_HANDLE_STMT,
		m_hstmt,
		1,
		Sqlstate,
		&NativeErrorPtr,
		MessageText,
		100,
		&TextLengthPtr ) ;
	TRACE("SQLSTATE:%s; Message:%s \n",Sqlstate ,MessageText ) ;
}
