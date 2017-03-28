// TestDBDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmServiceProc.h"
#include "TestDBDlg.h"
#include "afxdialogex.h"
#include "OTLObj.h"
#include "NewSMDatabase.h"

// CTestDBDlg 对话框

IMPLEMENT_DYNAMIC(CTestDBDlg, CDialogEx)

CTestDBDlg::CTestDBDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDBDlg::IDD, pParent)
	, m_stmt(_T(""))
{
	m_log_param = "spmsbill/spmsbill@10.108.20.92/spms";
	m_proc ="xp_AddToDBBuffer";
}

CTestDBDlg::~CTestDBDlg()
{
}

void CTestDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN, m_log_param);
	DDX_Text(pDX, IDC_PROC_EDIT, m_proc);
	DDX_Text(pDX, IDC_STMT_EDIT, m_stmt);
	DDX_Control(pDX, IDC_STATIC_INPUT, m_InputListPos);
	DDX_Control(pDX, IDC_LIST1, m_OutList);
}


BEGIN_MESSAGE_MAP(CTestDBDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTestDBDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CTestDBDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_QUERY_PROC_BUTTON, &CTestDBDlg::OnBnClickedQueryProcButton)
	ON_BN_CLICKED(IDC_EXEC_BUTTON, &CTestDBDlg::OnBnClickedExecButton)
	ON_BN_CLICKED(IDC_EXEC2_BUTTON, &CTestDBDlg::OnBnClickedExec2Button)
	ON_BN_CLICKED(IDC_BUTTON_QRY_PARAM, &CTestDBDlg::OnBnClickedButtonQryParam)
END_MESSAGE_MAP()


// CTestDBDlg 消息处理程序


void CTestDBDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CTestDBDlg::OnBnClickedButtonConnect()
{

	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	//m_pcon = ((CSmServiceProcApp*)AfxGetApp())->pDBMng->CreateDBConnection();

	//if(m_pcon)
	//{
	//	auto rt = m_pcon->init(m_log_param);
	//	if(rt!=0)
	//	{
	//		std::string info;
	//		info += m_pcon->last_msg;
	//		info += "\r\n";
	//		info += m_pcon->error_sql;
	//		info += "\r\n";
	//		info += m_pcon->error_var_info;
	//		info += "\r\n";

	//		AfxMessageBox(info.c_str());

	//	}
	//	else
	//		AfxMessageBox("connect OK");

	//}
	//else
	//{
	//		AfxMessageBox("create db obj error");
	//}

}


void CTestDBDlg::OnBnClickedQueryProcButton()
{
	// TODO: 在此添加控件通知处理程序代码
		UpdateData();
		//if(!m_pcon)
		//{
		//	AfxMessageBox("No connect");
		//	return;
		//}


		//std::string proc = m_proc;
		//std::string stmt;
		//std::string cur;
		//auto rt = m_pcon->QueryProc(proc.c_str(),stmt,cur);
		//if(rt!=0)
		//{
		//	std::string info;
		//	info += m_pcon->last_msg;
		//	info += "\r\n";
		//	info += m_pcon->error_sql;
		//	info += "\r\n";
		//	info += m_pcon->error_var_info;
		//	info += "\r\n";

		//	AfxMessageBox(info.c_str());

		//}
		//else
		//{
		//	AfxMessageBox(stmt.c_str());
		//	m_stmt = stmt.c_str();
		//	UpdateData(false);
		//}
}


void CTestDBDlg::OnBnClickedExecButton()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	//if(m_pcon)
	//{
	//	auto ret = m_pcon->ExcuteSQL(m_stmt);
	//	if(ret!=0)
	//	{
	//		std::string info;
	//		info += m_pcon->last_msg;
	//		info += "\r\n";
	//		info += m_pcon->error_sql;
	//		info += "\r\n";
	//		info += m_pcon->error_var_info;
	//		info += "\r\n";

	//		AfxMessageBox(info.c_str());

	//	}
	//	else
	//	{
	//		AfxMessageBox("ok");
	//	}

	//}
	//else
	//{
	//		AfxMessageBox("No connect");
	//}
}


void CTestDBDlg::OnBnClickedExec2Button()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	//if(m_pcon)
	//{
	//	COTLObj::input_params  in;
	//	COTLObj::output_params  out;

	//	char inputs[100][100];

	//	//for(int i =0;i<1;i++)
	//	//{
	//	//	sprintf(inputs[i],"%d",i);
	//	//	std::shared_ptr<COTLObj::in_param_ref> pparam = std::make_shared<COTLObj::in_param_ref>();
	//	//	pparam->type = COTLObj::param_type_var_char;
	//	//	pparam->valueRef = inputs[i];
	//	//	pparam->len = strlen(inputs[i]);
	//	//	in.push_back(pparam);
	//	//}

	//	int index =0;
	//	for(;index<m_InputParamList.GetPropertyCount();index++)
	//	{
	//		auto pprop = m_InputParamList.GetProperty(index);
	//		std::shared_ptr<COTLObj::in_param_ref> pparam = std::make_shared<COTLObj::in_param_ref>();
	//		pparam->ref = false;
	//		pparam->type = COTLObj::param_type_var_char;
	//		pparam->valueRef = nullptr;
	//		pparam->psvalue = std::make_shared<std::string>( CString(pprop->GetValue().bstrVal));
	//		pparam->len = pparam->psvalue->size();
	//		in.push_back(pparam);
	//	}

	//	m_OutList.DeleteAllItems();

	//	auto ret = m_pcon->ExcuteSQL(m_stmt,in,out);











	//	if(ret!=0)
	//	{
	//		std::string info;
	//		info += m_pcon->last_msg;
	//		info += "\r\n";
	//		info += m_pcon->error_sql;
	//		info += "\r\n";
	//		info += m_pcon->error_var_info;
	//		info += "\r\n";

	//		AfxMessageBox(info.c_str());

	//	}
	//	else
	//	{
	//		AfxMessageBox("ok");
	//		int row_count = this->m_OutList.GetHeaderCtrl().GetItemCount();

	//		auto itr = out.begin();
	//		int idx=0;
	//		while(itr!=out.end())
	//		{
	//			m_OutList.InsertItem(idx,"");
	//			int  i = 0;
	//			while(i < row_count)
	//			{
	//				m_OutList.SetItemText(idx,i,(*itr)->value.c_str());
	//				i++;
	//				itr++;
	//			}
	//			idx++;
	//		}
	//	}

	//}
	//else
	//{
	//	AfxMessageBox("No connect");
	//}
}


BOOL CTestDBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//m_InputList.SetStandardButtons(AFX_VSLISTBOX_BTN_NEW|AFX_VSLISTBOX_BTN_DELETE);
	//m_InputList.SetWindowText(_T("input params"));
	//m_OutList.SetStandardButtons(AFX_VSLISTBOX_BTN_NEW|AFX_VSLISTBOX_BTN_DELETE);
	//m_OutList.SetWindowText(_T("input params"));
	m_OutList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_OutList.EnableMarkSortedColumn();



	CRect rectPropList;
	m_InputListPos.GetClientRect(&rectPropList);
	m_InputListPos.MapWindowPoints(this, &rectPropList);

	m_InputParamList.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectPropList, this, (UINT)-1);

	m_InputParamList.EnableHeaderCtrl();
	m_InputParamList.EnableDescriptionArea();
	m_InputParamList.SetVSDotNetLook(TRUE);
	m_InputParamList.MarkModifiedProperties(TRUE);
	m_InputParamList.SetCustomColors(RGB(228, 243, 254), RGB(46, 70, 165), RGB(200, 236, 209), RGB(33, 102, 49), RGB(255, 229, 216), RGB(128, 0, 0), RGB(159, 159, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTestDBDlg::OnBnClickedButtonQryParam()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	//if(m_pcon)
	//{
	//	COTLObj::paramdec_list  in;
	//	COTLObj::paramdec_list  out;

	//	m_pcon->QueryParam(m_stmt,in,out);


	//	/*while(m_InputParamList.GetPropertyCount())
	//	{
	//		m_InputParamList.re.RemoveItem(0);
	//	}*/
	//	m_InputParamList.RemoveAll();
	//	for(auto itr = in.begin();itr!=in.end();itr++)
	//	{
	//		//m_InputList.AddItem((*itr)->name.c_str());
	//		m_InputParamList.AddProperty(new CMFCPropertyGridProperty((*itr)->name.c_str(), (COleVariant) _T(""), _T("This is a description")));
	//	}

	//	m_InputParamList.Invalidate();

	//	/*while(m_OutList.GetCount())
	//	{
	//		m_OutList.RemoveItem(0);
	//	}*/
	//	m_OutList.DeleteAllItems();
	//	while(m_OutList.GetHeaderCtrl().GetItemCount())
	//	{
	//		m_OutList.DeleteColumn(0);
	//	}
	//	int indx = 0;
	//	for(auto itr = out.begin();itr!=out.end();itr++)
	//	{
	//		//m_OutList.AddItem((*itr)->name.c_str());
	//		m_OutList.InsertColumn(indx,(*itr)->name.c_str(), LVCFMT_LEFT, 60 );
	//		indx++;
	//	}

	//	m_OutList.ShowScrollBar(SB_BOTH);
	//}
}
