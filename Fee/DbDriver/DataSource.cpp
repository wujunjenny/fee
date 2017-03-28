// DataSource.cpp: implementation of the CDataSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_CONNECT_NUM 10
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataSource::CDataSource()
{
}

CDataSource::~CDataSource()
{
}

CString CDataSource::GetDataSrc()
{
	return m_DataSrc;
}

void CDataSource::SetDataSrc(const char *src)
{
	ASSERT(src);
	m_DataSrc = src ;
}

CString CDataSource::GetDBName()
{
	return m_DBName ;
}

void CDataSource::SetDBName(const char *db)
{
	ASSERT(db);
	m_DBName = db ;
}

CString CDataSource::GetPassword()
{
	return m_Password;
}

void CDataSource::SetPassword(const char *pass)
{
	ASSERT(pass);
	m_Password = pass ;
}

CString CDataSource::GetServerName()
{
	return m_ServerName ;
}

void CDataSource::SetServerName(const char *srv)
{
	ASSERT(srv);
	m_ServerName = srv ;
}

WORD CDataSource::GetConnectNumber()
{
	return m_ConnectNum ;
}

void CDataSource::SetConnectNumber(WORD num)
{
	if( num > MAX_CONNECT_NUM )
		m_ConnectNum = MAX_CONNECT_NUM ;
	else
		m_ConnectNum = num ;
}

CString CDataSource::GetUserID()
{
	return m_UserID ;
}

void CDataSource::SetUserID(const char *id)
{
	ASSERT(id);
	m_UserID = id ;
}

DWORD CDataSource::GetDataSrcType()
{
	return m_DBType ;
}

void CDataSource::SetDataSrcType(DWORD srctyp)
{
	ASSERT(srctyp>=MS_SQL_DB_LIB && srctyp < DBSRC_NULL);
	m_DBType = srctyp ;
}

void CDataSource::SetProvider(DWORD provider)
{
    m_DBProvider = provider;
}

DWORD CDataSource::GetProvider()
{
    return m_DBProvider;
}
