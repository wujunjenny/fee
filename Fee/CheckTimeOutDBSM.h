// CheckTimeOutDBSM.h: interface for the CCheckTimeOutDBSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CheckTimeOutDBSM__H__)
#define __CheckTimeOutDBSM__H__

#import "msado15.dll" no_namespace rename("EOF","adoEOF")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommThreadMng.h"
#include "smcmacro.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
//#include "SqlFront.h"
#include "CmdDef.h"
#include "SmppDef.h"
#include <string>
//#include "MultiThreadMng.h"
#include "newmng.h"


class CSmsAppLinkMng ;
class CLinkObject;
class CSmsAppApp;

class CCheckTimeOutDBSM
{
	
public:
	CCheckTimeOutDBSM();
	virtual ~CCheckTimeOutDBSM();
	
	bool Start();
	void End();
	
	
protected:
	
	CLinkObject* m_pLink;
	BOOL RecordDBData(char *pSM, int opType);
	//对数据库缓存表中超时未回状态报告的消息进行检查
	int CheckTimeOutMsg();
	void InitCmd();
	void PrintProviderError(LPCTSTR pszMsg,_ConnectionPtr pConnection);
	
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hKillEvent;
	HANDLE m_Thread;	
	
	
	CString m_strCheckTimeOutMsg;
	bool m_bChecking;
	DWORD m_LastCheckTime; 
	
	void ReturnLink(CLinkObject *pLink,BOOL bOK=TRUE);
	
	_CommandPtr m_pCheckExpiredMsgCmd;
	
};

#endif // !defined(__CheckTimeOutDBSM__H__)
