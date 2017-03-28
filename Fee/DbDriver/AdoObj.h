// SqlObj.h: interface for the CSqlObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__ADOObj__h__)
#define __ADOObj__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// please copy the msado15.dll to your currect directory

#include <comdef.h>
#import ".\\msado15.dll" \
	no_namespace \
rename("EOF", "adoEOF")

#include "LinkObject.h"


#define MAX_SQL_LEN     2048

#define DBLOGIN_TIMEOUT 5

typedef enum{
    ADO_SQL_SERVER = 1,
		ADO_ORACLE = 2
}ADO_DB_TYPE;


class CAdoObj : public CLinkObject  
{
public:
	CAdoObj(CDataSource* father);
	virtual ~CAdoObj();
	virtual BOOL IsLinkOk();
	virtual void DestroyLink();
	virtual BOOL CreateLink();
	
	//test
	_ConnectionPtr m_pConnection;
private:
	BOOL m_IsConnectionOpen;
	CString m_ConnectStr;
	
	int LoadDbExecTimeOutSet();
	int m_iDbExecTimeOutSec;
};

#endif // !defined(__ADOObj__h__)

