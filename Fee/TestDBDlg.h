#pragma once
#include "afxwin.h"


// CTestDBDlg 对话框

class CTestDBDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestDBDlg)

public:
	CTestDBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDBDlg();

// 对话框数据
	enum { IDD = IDD_DB_TEST_DIALOG };

	std::shared_ptr<COTLObj> m_pcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonConnect();
	CString m_log_param;
	afx_msg void OnBnClickedQueryProcButton();
	CString m_proc;
	CString m_stmt;
	afx_msg void OnBnClickedExecButton();
	afx_msg void OnBnClickedExec2Button();
	CStatic m_InputListPos;
	CMFCListCtrl m_OutList;
	CMFCPropertyGridCtrl m_InputParamList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonQryParam();
};
