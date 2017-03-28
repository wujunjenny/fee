// QueryFixMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smserviceproc.h"
#include "QueryFixMsgDlg.h"
#include "SmServiceProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFixMsgDlg dialog


CQueryFixMsgDlg::CQueryFixMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryFixMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQueryFixMsgDlg)
	m_ChargeTel = _T("");
	m_ServiceID = _T("");
	m_spid = _T("");
	m_UserTel = _T("");
	//}}AFX_DATA_INIT
}


void CQueryFixMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFixMsgDlg)
	DDX_Text(pDX, IDC_EDT_CHARGETEL, m_ChargeTel);
	DDX_Text(pDX, IDC_EDT_SERVICEID, m_ServiceID);
	DDX_Text(pDX, IDC_EDT_SPID, m_spid);
	DDX_Text(pDX, IDC_EDT_USERTEL, m_UserTel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFixMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CQueryFixMsgDlg)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBtnQuery)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryFixMsgDlg message handlers

void CQueryFixMsgDlg::OnBtnQuery() 
{
	UpdateData(TRUE);

	CSmServiceProcApp *pApp = (CSmServiceProcApp *)AfxGetApp();

	DWORD count = pApp->GetMonthFee()->GetCount();
    CString str;
	str.Format("%u", count);
	GetDlgItem(IDC_COUNT)->SetWindowText(str);
	
	CString FeeMonth = pApp->GetMonthFee()->GetFeeMonth();
	GetDlgItem(IDC_FEEMONTH_SHOW)->SetWindowText(FeeMonth);

	if(m_spid.IsEmpty() || m_ServiceID.IsEmpty() || m_ChargeTel.IsEmpty() || m_UserTel.IsEmpty())
	{
		return;
	}

	BOOL bRet =  pApp->GetMonthFee()->Find(FeeMonth, m_spid, m_ServiceID, m_ChargeTel, m_UserTel);
	if (bRet)
	{
		AfxMessageBox("存在该包月扣费记录！");
	}
	else
	{
		AfxMessageBox("不存在该包月扣费记录！");
	}	
}

