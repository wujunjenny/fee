// SmServiceProc.h : main header file for the SMSERVICEPROC application
//

#if !defined(__SmServiceProc__h__)
#define __SmServiceProc__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MainProcMng.h"
#include "DataConeMng.h"
#include "SMDatabase.h"
#include "NewSMDatabase.h"
#include "SmppDrvMng.h"
#include "SmServiceProcDlg.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include "SockMsgQueue.h"
#include "BillFileMng.h"
#include "CheckTimeOutDBSM.h"
#include "LogManager.h"
#include "MonthFee.h"
#include <iostream>
//#include "strstrea.h"

/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcApp:
// See SmServiceProc.cpp for the implementation of this class
//

#define MAX_BUFF_BYTES (1024 * 1024 * 2)

class CSmServiceProcApp : public CWinApp
{
public:
	CSockMsgQueue * m_pglbuf;
	CSmServiceProcApp();
	CSmServiceProcDlg * GetDlg()
	{ 
		return (CSmServiceProcDlg *)m_pDlg;
	}
	
	void SetDlg(CSmServiceProcDlg * pDlg)
	{ 
		m_pDlg = pDlg;
	}
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmServiceProcApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	
	// Implementation
	CMainProcMng * pMainMng;
	CDataConeMng * pLinkMng;
	//CSMDatabase * pDBMng;
	CNewSMDatabase * pDBMng;
	//CCheckTimeOutDBSM * pCheckTimeOutDBSM;
	CDataConeMng *pFileLinkMng;
	CBillFileMng *pBillFileMng;
	CSmppDrvMng* pDrv;
	CMonthFee m_MonthFee;
	CMonthFee * GetMonthFee()
	{
		return &m_MonthFee;
	};
	
	
	// *** SMGW25-32,2004-07-05,wzy modify begin *** //
	BOOL SendMsgToDBMng(LPVOID pMsg)
    {
		ASSERT(pMsg);
		//	ASSERT(m_pDBMng);
		if(!pDBMng)
			return FALSE;
		return pDBMng->SendMessageToQueue((char*)pMsg);
    }	
	// *** SMGW25-32,2004-07-05,wzy modify end *** //
	
	//-----SMGW41-36,2005-04-12,zxh,add begin----//
	FileLogWriter * m_fw;
	LogManager * m_lm;
	//-----SMGW41-36,2005-04-12,zxh,add end----//
	
private:
    CDialog * m_pDlg;
	//{{AFX_MSG(CSmServiceProcApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(__SmServiceProc__h__)
