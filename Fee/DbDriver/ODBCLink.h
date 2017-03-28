// ODBCLink.h: interface for the CODBCLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__ODBCLink_h__)
#define __ODBCLink_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdb.h>
#include "LinkObject.h"
#include "DataSource.h"

#define LINK_STATE_IDLE		0 
//表示处于未使用状态
#define LINK_STATE_AVAIL	1
//可用状态
#define LINK_STATE_INUSE	2
//被占用状态
#define LINK_STATE_ERR		3
//连接错误

class CDataSource ;
class CODBCObj : public CLinkObject
{
public:
	CODBCObj(CDataSource* father);
	virtual ~CODBCObj();
	
	CDatabase m_dbCust ;
	HSTMT	 m_hstmt ;
private:
	CString m_Uid	;
	CString m_DbAlias ;
	CString m_Database ;
	CString m_Pwd ;
	CString m_StrConnect ;
	
private:
	int m_nLinkState ;
	
	BOOL IsLinkAvail(void){return FALSE;} ; 
	
	
public:
	BOOL ConnectDB(void) ;
	BOOL DisConnect(void) ;
	BOOL ErrProc(void) ;
	//	BOOL DOVerb(void) ;
	BOOL DoTestSel(HSTMT hstmt) ;
	BOOL DoTestProc(HSTMT hstmt) ;
	BOOL DoBoth(HSTMT hstmt) ;
	void SetConnect(CString DBAlias,CString UID,CString Database,CString PWD) ;
	
	void Diagnosis(void) ;
public:
	//	void* GetProc();
	//	DWORD GetSrcType();
	//	CString GetDataSrc();
	virtual BOOL IsLinkOk();
	virtual void DestroyLink();
	virtual BOOL CreateLink();
};

#endif // !defined(__ODBCLink_h__)
