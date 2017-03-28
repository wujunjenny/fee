// SmServiceProcDlg.h : header file
//

#if !defined(__SmServiceProcDlg__h__)
#define __SmServiceProcDlg__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcDlg dialog
#include "DynamicLED.h"
#include <string>
//#include "AlarmServerDrvMng.h"
#include "AlarmHandMsgProc.h"
#include "afxcmn.h"

class FeeOmcManager;
class CSmServiceProcDlg : public CDialog
{
	// Construction
public:
	CAlarmHandMsgProc *m_pAlarmHandMsgProc;
	void ReConnect(); 
	
	BOOL m_bDBBroken;
	BOOL m_bGWBroken;
	BOOL m_bIsAlarmEnable;
	void SetConnLed(int Led);
	VOID SetDBLed(int Led);
	void OnLogin();
	void  OnConnect();
	void  OnBroken();
	void OnSend(int nSendCount);
	void OnSendSuc(int Count);
	void     OnSendFAIL(int Count);
	CSmServiceProcDlg(CWnd* pParent = NULL);

	void UpdateConnectStatus(void);
	LPCTSTR GetConnectStatusStr(int nStatus);
	
	//告警参数
	//监听端口
	WORD m_wAlarmListenPort;
	//握手超时
	DWORD m_dwActiveTimeout;
	
	//告警消息
	DWORD m_dwMsgTimeout;
	DWORD m_dwMsgRetry;

	BOOL m_bDestroyFlg;
	
	std::shared_ptr<FeeOmcManager> m_omc;
	
	// Dialog Data
	//{{AFX_DATA(CSmServiceProcDlg)
	enum { IDD = IDD_SMSERVICEPROC_DIALOG };
	CButton	m_asd;
	CDynamicLED	m_cConnLed;
	CDynamicLED	m_cDBLed;
	CButton	m_SendSucCount;
	CButton	m_SendFailCount;
	CButton	m_SendCount;
	CStatic	m_cltState;
	CStatic	m_cltIP;
	CString	m_sAccount;
	CString	m_sIPAddr;
	short	m_wPort;
	CString	m_sPass;
	CString	m_sPID;
	CString	m_sDataSource;
	CString	m_sOrgAddr;
	CString	m_sProcName;
	DWORD	m_nCheckInterval;
	CString	m_sProcNameOne;
	CString	m_sTitle;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmServiceProcDlg)
public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CSmServiceProcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReConnect();
	afx_msg void OnUpdatePara();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnCancel();
	afx_msg void OnMyCancel();
	virtual void OnOK();
	afx_msg void OnZero();
	afx_msg void OnButton1();
	afx_msg void OnClose();
	afx_msg void OnButtonCatch();
	afx_msg void OnBtnQuery();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_cConnectList;
	afx_msg void OnStnClickedStaticState();
	afx_msg void OnBnClickedDbButton();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__SmServiceProcDlg__h__)
