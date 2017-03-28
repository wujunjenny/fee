#if !defined(AFX_QUERYFIXMSGDLG_H__093E609C_22C6_4A52_8574_8049E9825065__INCLUDED_)
#define AFX_QUERYFIXMSGDLG_H__093E609C_22C6_4A52_8574_8049E9825065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryFixMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQueryFixMsgDlg dialog

class CQueryFixMsgDlg : public CDialog
{
// Construction
public:
	CQueryFixMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryFixMsgDlg)
	enum { IDD = IDD_DIG_FIXMSG };
	CString	m_ChargeTel;
	CString	m_ServiceID;
	CString	m_spid;
	CString	m_UserTel;
	CString	m_FeeMonth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryFixMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryFixMsgDlg)
	afx_msg void OnBtnQuery();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYFIXMSGDLG_H__093E609C_22C6_4A52_8574_8049E9825065__INCLUDED_)
