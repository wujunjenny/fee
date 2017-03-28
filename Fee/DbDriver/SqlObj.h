// SqlObj.h: interface for the CSqlObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__sqlObj__h__)
#define __sqlObj__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LinkObject.h"

#ifndef DBNTWIN32
#define DBNTWIN32
#endif

#include "sqlfront.h"
#include "sqldb.h"

#define DBLOGIN_TIMEOUT 5

class CSqlObj : public CLinkObject  
{
public:
	CSqlObj(CDataSource* father);
	virtual ~CSqlObj();
	virtual BOOL IsLinkOk();
	virtual void DestroyLink();
	virtual BOOL CreateLink();
	
private:
	
	int LoadDbExecTimeOutSet();
	int m_iDbExecTimeOutSec;
};

#endif // !defined(__sqlObj__h__)
