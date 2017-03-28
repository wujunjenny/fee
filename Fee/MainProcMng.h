// MainProcMng.h: interface for the CMainProcMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__MainProcMng__h__)
#define __MainProcMng__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "COMMON\CommThreadMng.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
#include "AdoObj.h"
#include "SMFeeThread.h"
#include "smppdef.h"
#include "RouteTable.h"
#include "SmppDrvMng.h"
#include <rapidjson\document.h>
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson\writer.h>
#include <rapidjson\reader.h>
#include <string>
#include <list>

const int MAX_MULTI_SMS_THREAD = 10;
//modify by gyx 
typedef struct tagConnectInfo
{
	int			nSeq;
	std::string sGWName;
	std::string sIPAddr;
	int			nPort;
	std::string sAccount;
	std::string sPassword;
	int			nShakeHandCount;
	int			nDrvID;//tcpdrvid
	int			nStatus;//connect status
	tagConnectInfo()
	{
		nDrvID = 0;
		nStatus = smpp_broken;
		nSeq = 0;
		nPort = 0;
		nShakeHandCount = 0;
	}
}CONNECT_INFO;

class CAutoLock
{
	public:
		CAutoLock(CRITICAL_SECTION* pLock,BOOL bLock=TRUE){ 
			m_pLock = NULL;
			m_bLock = 0;
			if(pLock)
			{
				m_pLock =pLock; 
				if(bLock)
				{
					::EnterCriticalSection(pLock);
					m_bLock = 1;
				}
			}
		};
		~CAutoLock(){
			if(m_bLock)
			{
				if(m_pLock) ::LeaveCriticalSection(m_pLock);
			}
		};
		int UnLock(){ 
			if(m_pLock)
			{
				::LeaveCriticalSection(m_pLock);
				m_bLock = 0;
			}
			return 0;
		};
		int Lock()
		{ 
			if(m_pLock)
			{
				::EnterCriticalSection(m_pLock);
				m_bLock = 1;
			}
			return 0;
		};
	protected:
		CRITICAL_SECTION* m_pLock;
		int m_bLock;
};

//end

class CMainProcMng : public CCommThreadMng  
{
public:
	
	CSMFeeThread *m_pSmsThread;
	int m_nThreadCount;
	
	
	CMainProcMng();
	virtual ~CMainProcMng();
	void SetAccount(LPCTSTR pAccount)
	{ 
		m_sAccount = pAccount;
	};
	
	LPCTSTR  GetAccount()
	{ 
		return  m_sAccount;
	};
	
	void SetPass(LPCTSTR pPass)
	{ 
		m_sPass = pPass; 
	};
	
	LPCTSTR  GetPass()
	{
		return m_sPass;
	};
	
    
	void SetPID(LPCTSTR pPid )
	{ 
		m_sPID=pPid; 
	}
	
	LPCTSTR GetPID()
	{
		return  m_sPID; 
	} 
	
	void SetCheckDbInterval(DWORD nInterval)
	{
		m_nCheckDbInterval=nInterval;
	}
	
	DWORD GetCheckDbInterval() 
	{
		return m_nCheckDbInterval;
	} 
	
	
	CString GetDataSrcName()
	{
		return m_sDataSrcName;
	}

//modufy by gyx
	int GetConnection(int nSeq,CONNECT_INFO& con)
	{
		CAutoLock lock(&m_CriticalSection);
		auto itr = m_connectlist.begin();
		while(itr!=m_connectlist.end())
		{
			if(itr->nSeq==nSeq)
			{
				con = *itr;
				return itr->nSeq;
			}
			itr++;
		}
		return 0;
	}

	int	AddConnection(CONNECT_INFO& con)
	{
		CAutoLock lock(&m_CriticalSection);
		int nSeq=0;
		auto itr = m_connectlist.begin();
		while(itr!=m_connectlist.end())
		{
			nSeq = max(nSeq,itr->nSeq);
			itr++;
		}
		nSeq++;
		con.nSeq = nSeq;
		m_connectlist.push_back(con);
		return nSeq;
	};

	int DeleteConnection(int nSeq);

	int ModiConnection(CONNECT_INFO& con)
	{
		CAutoLock lock(&m_CriticalSection);
		auto itr = m_connectlist.begin();
		while(itr!=m_connectlist.end())
		{
			if(itr->nSeq==con.nSeq)
			{
				*itr = con;
				return itr->nSeq;
			}
			itr++;
		}
		return 0;
	}

	std::list<CONNECT_INFO> GetConnections()
	{
		CAutoLock lock(&m_CriticalSection);
		std::list<CONNECT_INFO> tmplist(m_connectlist);
		return tmplist;
	}

	void MountConnetion(void);
	long int RequireRoutIf( long int id,char *name);

	void OnConnect(HANDLE hHandle);
	void OnError(HANDLE hDrv, int nError);
//end
	
public:
	DWORD m_nCheckDbTimerID;
	DWORD m_nCheckDbInterval; 
    DWORD m_nLastExecTime;
	
	void ReturnLink(CLinkObject* pLink, BOOL bOK =TRUE);
	void GetDbProcess(CLinkObject * & pLink, PDBPROCESS & pDB);

	virtual void Destroy();
protected:
	
	//***SMGW25-69, 2004-10-08, JDZ, ADD BEGIN***//
	int m_iShakeHandCount;
	int m_iMaxShakeHandCount;
	//***SMGW25-69, 2004-10-08, JDZ, ADD end***//
	DWORD GetSeq();
	void ProcessDb();
	void OnBindResp(LPCTSTR pMsg);
	
	CString m_sAccount;
	CString m_sPass;
	CString m_SourceAddr;
	DWORD	m_nSeq;
	
	int     m_nBindType;
	int     m_nPri;
	
	
	
	DWORD m_nShakehandTimerID;
	
	
	CString m_sPID;
	CString m_sDataSrcName; 	 
//modify by gyx
	std::list<CONNECT_INFO> m_connectlist;
	CRITICAL_SECTION m_CriticalSection;
//end	
	virtual BOOL OnMessage(PTRMSG pMsg);
	BOOL OnTimer(DWORD timeID, DWORD dwPara);


	
};


#endif // !defined(__MainProcMng__h__)
