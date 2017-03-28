// DataConeMng.h: interface for the CLinkMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DataConeMng__h__)
#define __DataConeMng__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "StdAfx.h"
//#include <afxtempl.h>

#define MAX_DATA_SRC  20
#include "LinkObject.h"
#include "datasource.h"
#include "smcmacro.h"
#include "ErrorLevel.h"
//#include "MainApp.h"
//#include "msgdef.h"
#include "ODBCLink.h"


#define RegKeyDBRoot   ".Default\\software\\HWDBMng"
#define RegKeyDBCount  "DataSrcCount"

//#define MAX_DBNAME_LEN 20


#ifndef _DATASOURCE_DEFINE
#define _DATASOURCE_DEFINE

typedef	struct tag_DATASOURCE
{
	UC SourceID[MAX_DBNAME_LEN];
	UC Server[MAX_DBNAME_LEN];
	UC Database[MAX_DBNAME_LEN];
	UC UserID[MAX_DBNAME_LEN];
	UC Pwd[MAX_DBNAME_LEN];
	int	 Connects;
	int  Type;
	int Provider;
}DATASOURCE;
#endif

class CDataSource ;
class CLinkObject ;

class CDataConeMng  
{
public:
	virtual void WriteSysLogRecord(LPCSTR szPmt, LPCSTR pBuff, \
		int iBuffSize, int iWarnLevel = LOG_WARN_PRMT);
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
    int GetDataSourceCount();
    int GetDataSourceValue(DATASOURCE* pBuff,int count);
	CDataSource* GetDataSrc(const char* src);
	void ReturnLink(CLinkObject* obj,BOOL flag = TRUE);
	CLinkObject* GetLink(const char* src);
	CArray<CLinkObject*,CLinkObject*> m_DBList;
	CArray<CLinkObject*,CLinkObject*> m_BadList;
	CDataConeMng();
	virtual ~CDataConeMng();
	void Init();
    int InitDataSourceInfo(DATASOURCE* pBuff, int count);
	void ReadDataSrcInfo();
	
public:
	CDataSource m_DataSrc[MAX_DATA_SRC];
protected:
	void DestroyLink();
	void InitLink();
private:
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
    CString m_sIniFileName;
	
private:
	CRITICAL_SECTION m_Lock;
	CRITICAL_SECTION m_LockBad;
	DWORD m_DataSrcCount;
	BOOL m_SQlFirst ;
};

#endif
