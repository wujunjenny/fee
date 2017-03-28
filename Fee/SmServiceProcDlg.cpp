// SmServiceProcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmServiceProc.h"
#include "SmServiceProcDlg.h"
#include <string>
#include "MainProcMng.h"
#include "GlobalDef.h"
#include "QueryFixMsgDlg.h"
#include "FeeOmcManager.h"
#include "TestDBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
extern LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcDlg dialog

CSmServiceProcDlg::CSmServiceProcDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSmServiceProcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmServiceProcDlg)
	m_sAccount = _T("");
	m_sIPAddr = _T("");
	m_wPort = 0;
	m_sPass = _T("");
	m_sPID = _T("");
	m_sDataSource = _T("");
	m_sOrgAddr = _T("");
	m_sProcName = _T("");
	m_nCheckInterval = 0;
	m_sProcNameOne = _T("");
	m_sTitle = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAlarmHandMsgProc = NULL;
    m_bDBBroken = TRUE;
}

void CSmServiceProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmServiceProcDlg)
	//	DDX_Control(pDX, IDC_BUTTON1, m_asd);
	DDX_Control(pDX, IDC_STATIC_CONNLED, m_cConnLed);
	DDX_Control(pDX, IDC_STATIC_DBLED, m_cDBLed);
	DDX_Control(pDX, IDC_SENDSUCCOUNT, m_SendSucCount);
	DDX_Control(pDX, IDC_SENDFAILCOUNT, m_SendFailCount);
	DDX_Control(pDX, IDC_SENDCOUNT, m_SendCount);
	DDX_Control(pDX, IDC_STATIC_STATE, m_cltState);
	DDX_Control(pDX, IDC_STATIC_IP, m_cltIP);
	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_sAccount);
	DDX_Text(pDX, IDC_EDIT_IPADDR, m_sIPAddr);
	DDX_Text(pDX, IDC_EDIT_IPPORT, m_wPort);
	DDX_Text(pDX, IDC_EDIT_PASS, m_sPass);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST1, m_cConnectList);
}

BEGIN_MESSAGE_MAP(CSmServiceProcDlg, CDialog)
//{{AFX_MSG_MAP(CSmServiceProcDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnReConnect)
ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnUpdatePara)
ON_WM_TIMER()
ON_BN_CLICKED(ID_MYCANCEL, OnMyCancel)
ON_BN_CLICKED(IDC_ZERO, OnZero)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BUTTON_CATCH, OnButtonCatch)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBtnQuery)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATIC_STATE, &CSmServiceProcDlg::OnStnClickedStaticState)
	ON_BN_CLICKED(IDC_DB_BUTTON, &CSmServiceProcDlg::OnBnClickedDbButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcDlg message handlers

LPCTSTR CSmServiceProcDlg::GetConnectStatusStr(int nStatus)
{
	switch(nStatus)
	{
	case smpp_active:
		return "smpp_active";
	case smpp_broken:
		return "smpp_broken";
	case smpp_connecting:
		return "smpp_connecting";
	case smpp_logining:
		return "smpp_login";
	default:
		break;
	}
	return "unknown status";
}

void CSmServiceProcDlg::UpdateConnectStatus(void)
{
	CSmServiceProcApp *pApp = (CSmServiceProcApp*)AfxGetApp();

	auto list = pApp->pMainMng->GetConnections();

	auto itr = list.begin();
	int bConnect = 0;
	int bBroken = 0;
	while (itr!=list.end())
	{
		int i=0;
		for(i=0;i<m_cConnectList.GetItemCount();i++)
		{
			if(itr->nSeq==m_cConnectList.GetItemData(i))
			{
				CString sStatus;
				CString sID;
				sStatus.Format("%s",GetConnectStatusStr(itr->nStatus));
				m_cConnectList.SetItemText(i,2,(LPCTSTR)sStatus);
				//sID.Format("0x%x",itr->nDrvID);
				//m_cConnectList.SetItemText(i,2,(LPCTSTR)sID);
				if(itr->nStatus == smpp_active)
				{
					bConnect++;
				}
				else
				if(itr->nStatus == smpp_broken)
				{
					bBroken++;
				}
			};
		}
		itr++;
	}

	if(bConnect==list.size())
	{
		 SetConnLed(ID_LED_GREEN);
		 m_cltState.SetWindowText("已连");
	}
	else if (bBroken == list.size())
	{
		 SetConnLed(ID_LED_RED);
		 m_cltState.SetWindowText("未连");
	}
	else
	{
		 SetConnLed(ID_LED_YELLOW);
		 m_cltState.SetWindowText("故障");			
	}


}

BOOL CSmServiceProcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDestroyFlg = FALSE;
	SetTimer(100,1000,NULL);
	
	DWORD dwStyle = m_cConnectList.GetExtendedStyle();
	dwStyle |=LVS_EX_FULLROWSELECT;
	m_cConnectList.SetExtendedStyle(dwStyle);

	dwStyle = m_cConnectList.GetStyle();
	dwStyle |=LVS_SHOWSELALWAYS;
	SetWindowLong(m_cConnectList.m_hWnd,GWL_STYLE,dwStyle);
	m_cConnectList.InsertColumn(0,"Name",LVCFMT_LEFT,100);
	m_cConnectList.InsertColumn(1,"Ip",LVCFMT_LEFT,160);
	m_cConnectList.InsertColumn(2,"Status",LVCFMT_LEFT,120);


    ((CSmServiceProcApp*)AfxGetApp())->SetDlg(this);  
    char  InitName[]=".\\FeeConfig.ini";
	char  sTemp[100];
	char  sSession[]="SmConnection";
	
	int Ret = 0;
	
	//SMGW41-28, 2005-4-4, jdz, modi begin//
	// 数据库缓存参数配置
	Ret = GetPrivateProfileString("DBCacheSetting", 
		"MaxBuffSize", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "1000000");
		WritePrivateProfileString("DBCacheSetting",
			"MaxBuffSize",
			sTemp,
			InitName);
	}
	g_DBMaxBuffSize = atoi(sTemp);
	
	Ret = GetPrivateProfileString("DBCacheSetting", 
		"MaxBuffMsgCount", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "10000");
		WritePrivateProfileString("DBCacheSetting",
			"MaxBuffMsgCount",
			sTemp,
			InitName);
	}
	g_DBMaxMsgCount = atoi(sTemp);
	//SMGW41-28, 2005-4-4, jdz, modi begin//
	
	
	// *** SMGW25-30,2004-06-30,wzy add begin *** //
	Ret = GetPrivateProfileString("DBExeTimeout",
		"timeout",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
    if (Ret == 0)
	{
		strcpy(sTemp, "0");
		WritePrivateProfileString("DBExeTimeout",
			"timeout",
			sTemp,
			InitName);
	}
	
	g_Timeout = atoi(sTemp);
	// *** SMGW25-30,2004-06-30,wzy add end *** //		
	
	Ret = GetPrivateProfileString("SmConnection",
		"IPAddr", 
		"", 
		sTemp, 
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "127.0.0.1");
		WritePrivateProfileString("SmConnection",
			"IPAddr",
			sTemp,
			InitName);
	}
	m_sIPAddr=sTemp;
	
	Ret = GetPrivateProfileString("SmConnection",
		"Port", 
		"", 
		sTemp, 
		sizeof(sTemp), 
		InitName); 
	if (Ret == 0)
	{
		strcpy(sTemp, "5018");
		WritePrivateProfileString("SmConnection",
			"Port",
			sTemp,
			InitName);
	}
	m_wPort=atoi(sTemp);
	
	Ret = GetPrivateProfileString("Title",
		"title",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "网关计费");
		WritePrivateProfileString("Title",
			"title",
			sTemp,
			InitName);
	}
	m_sTitle = sTemp;
	
	Ret = GetPrivateProfileString("ScanDB", 
		"ScanDBTime", 
		"",
		sTemp,
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "30");
		WritePrivateProfileString("ScanDB",
			"ScanDBTime",
			sTemp,
			InitName);
	}
	
	g_ScanDBTime = atof(sTemp);
	
	Ret = GetPrivateProfileString("BufferSupport",
		"DataExistTime", 
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "7200");
		WritePrivateProfileString("BufferSupport",
			"DataExistTime",
			sTemp,
			InitName);
	}
	g_DataExistTime = atof(sTemp);
	
	Ret = GetPrivateProfileString("BufferSupport",
		"IsAskReport", 
		"",
		sTemp,
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "0");
		WritePrivateProfileString("BufferSupport",
			"IsAskReport",
			sTemp,
			InitName);
	}
	g_IsAskReport = atoi(sTemp);
	if(g_IsAskReport != 0 )
		g_IsAskReport = 1;
	
	Ret = GetPrivateProfileString("BufferSupport", 
		"AskReportTime", 
		"",
		sTemp,
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "2880");
		WritePrivateProfileString("BufferSupport",
			"AskReportTime",
			sTemp,
			InitName);
	}
	g_AskReportTime = atoi(sTemp);
	if (g_AskReportTime <= 0)
		g_AskReportTime = 2880;
	
	Ret = GetPrivateProfileString("Debug", 
		"OpenCache",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "1");
		WritePrivateProfileString("Debug",
			"OpenCache",
			sTemp,
			InitName);
	}
	g_OpenCache = atoi(sTemp);
	
	// *** SMGW30-08,2004-07-22,wzy add begin *** //
	Ret = GetPrivateProfileString("Version", 
		"VersionInfo",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	
	if (Ret == 0)
	{
		strcpy(sTemp, "版本信息：小灵通网关2.6");
		WritePrivateProfileString("Version",
			"VersionInfo",
			sTemp,
			InitName);
	}
	
	g_Version = sTemp;
	
	
	Ret = GetPrivateProfileString("Version", 
		"Copyright",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	
	if (Ret == 0)
	{
		strcpy(sTemp, "版权信息:Shenzhen share CO. LTD");
		WritePrivateProfileString("Version",
			"Copyright",
			sTemp,
			InitName);
	}
	
	g_Copyright = sTemp;
	// *** SMGW30-08,2004-07-22,wzy add end *** //
	
	
	Ret = GetPrivateProfileString("BillConfig",
		"SaveFileTime", 
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "3600");
		WritePrivateProfileString("BillConfig",
			"SaveFileTime",
			sTemp,
			InitName);
	}
	g_SaveFileTime = atof(sTemp);
	
	if (g_SaveFileTime <= 0)
	{
		g_SaveFileTime = 3600;
	}
	
	// *** SMGW25-66,2004-09-24,wzy add begin *** //
	Ret = GetPrivateProfileString("PaymentResultTimeOut",
		"TimeOut", 
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "120");
		WritePrivateProfileString("PaymentResultTimeOut",
			"TimeOut",
			sTemp,
			InitName);
	}
	g_PaymentAffirmTimeOut = atoi(sTemp);
	// *** SMGW25-66,2004-09-24,wzy add end *** //
	
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Jan",
		"",
		sTemp,
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "01/20");
		WritePrivateProfileString("FeeEndTime",
			"Jan",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Jan, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime", 
		"Feb", 
		"",
		sTemp, 
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "02/20");
		WritePrivateProfileString("FeeEndTime",
			"Feb",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Feb, sTemp);
	
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Mar", 
		"",
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "03/20");
		WritePrivateProfileString("FeeEndTime",
			"Mar",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Mar, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Apr",
		"",
		sTemp, 
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "04/20");
		WritePrivateProfileString("FeeEndTime",
			"Apr",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Apr, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"May",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "05/20");
		WritePrivateProfileString("FeeEndTime",
			"May",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.May, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Jun",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "06/20");
		WritePrivateProfileString("FeeEndTime",
			"Jun",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Jun, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Jul",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "07/20");
		WritePrivateProfileString("FeeEndTime",
			"Jul",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Jul, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime", 
		"Agu",
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "08/20");
		WritePrivateProfileString("FeeEndTime",
			"Agu",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Aug, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Sep", 
		"",
		sTemp, 
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "09/20");
		WritePrivateProfileString("FeeEndTime",
			"Sep",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Sep, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Oct", 
		"",
		sTemp,
		sizeof(sTemp), 
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "10/20");
		WritePrivateProfileString("FeeEndTime",
			"Oct",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Oct, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Nov", 
		"",
		sTemp,
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "11/20");
		WritePrivateProfileString("FeeEndTime",
			"Nov",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Nov, sTemp);
	
	Ret = GetPrivateProfileString("FeeEndTime",
		"Dec",
		"",
		sTemp, 
		sizeof(sTemp),
		InitName);
	if (Ret == 0)
	{
		strcpy(sTemp, "12/20");
		WritePrivateProfileString("FeeEndTime",
			"Dec",
			sTemp,
			InitName);
	}
	strcpy(g_TimeTab.Dec, sTemp);
	
	
	
    SetWindowText(m_sTitle);
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CSmServiceProcApp *pApp = ((CSmServiceProcApp*)AfxGetApp());
	
	pApp->pDrv = new CSmppDrvMng;
	pApp->pMainMng = new CMainProcMng;
	pApp->pLinkMng = new CDataConeMng;
	pApp->pLinkMng->Init();
    pApp->pDBMng = new CNewSMDatabase;
	pApp->pDBMng->Init("DBCache", g_DBMaxBuffSize, 102400, g_DBMaxMsgCount);
	pApp->pDBMng->Start();
	
	//SMGW41-28, 2005-4-4, jdz, modi begin//
    //pApp->pCheckTimeOutDBSM = new CCheckTimeOutDBSM;
	//pApp->pCheckTimeOutDBSM->Start();
	//SMGW41-28, 2005-4-4, jdz, modi end//
	
	pApp->pBillFileMng = new CBillFileMng;
	pApp->pBillFileMng->Init("BillFile", g_DBMaxBuffSize, 102400, g_DBMaxMsgCount);
	pApp->pBillFileMng->Start();
	pApp->pDrv->Init(m_sIPAddr, m_wPort);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_sPID=pApp->pMainMng->GetPID();
	m_nCheckInterval=pApp->pMainMng->GetCheckDbInterval(); 
    m_sDataSource=pApp->pMainMng->GetDataSrcName(); 
	m_sAccount=pApp->pMainMng->GetAccount();
	m_sPass=pApp->pMainMng->GetPass(); 
	
    //点LED灯
    SetConnLed(ID_LED_RED);
    SetDBLed(ID_LED_RED);
	
   	UpdateData(FALSE); 
	
	//***SMGW40-05, 2005-1-27, zf, add begin***//
	CString strTitle = "" ;
	GetWindowText(strTitle);
	m_pAlarmHandMsgProc = new CAlarmHandMsgProc( strTitle, CString(InitName) );
	if (!m_pAlarmHandMsgProc || !m_pAlarmHandMsgProc->Start())
	{
		CString log;
		log.Format("告警系统初始化失败!");
		PrintMessage("%s", log);
	}
	//***SMGW40-05, 2005-1-27, zf, add end***//


	auto list = pApp->pMainMng->GetConnections();

	auto itr = list.begin();
	int bConnect = 0;
	int bBroken = 0;
	while (itr!=list.end())
	{
		int index = m_cConnectList.InsertItem(m_cConnectList.GetItemCount(),itr->sGWName.c_str());
		m_cConnectList.SetItemData(index,itr->nSeq);
		char portStr[10];
		sprintf(portStr,"%ld",itr->nPort);
		std::string temstr = itr->sIPAddr + ":"+ portStr;
		m_cConnectList.SetItemText(index,1,temstr.c_str());

		itr++;
	}

	m_omc = std::make_shared<FeeOmcManager>();

	return TRUE;
}

void CSmServiceProcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CSmServiceProcDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSmServiceProcDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CSmServiceProcDlg::DestroyWindow() 
{
	
	m_bDestroyFlg = TRUE;
	CSmServiceProcApp *pApp = (CSmServiceProcApp*)AfxGetApp();
	
	pApp->pMainMng->Destroy();
	pApp->pDrv->Destroy();
	pApp->pDBMng->Destroy();
	
	
	//SMGW41-28, 2005-4-4, jdz, modi begin//
	//pApp->pCheckTimeOutDBSM->End();
	//delete pApp->pCheckTimeOutDBSM;
	//SMGW41-28, 2005-4-4, jdz, modi end//
	
	//-----SMGW41-35、-43,2005-04-12,zxh,modi begin----//
	
	pApp->pBillFileMng->Destroy();
	
	if(m_pAlarmHandMsgProc)
	{
		m_pAlarmHandMsgProc->End();
		delete m_pAlarmHandMsgProc;
		m_pAlarmHandMsgProc = NULL;
	}
	
	
	//-----SMGW41-35、-43,2005-04-12,zxh,modi end----//
	
	delete pApp->pDBMng;	
	delete pApp->pMainMng;
	delete pApp->pLinkMng;
	delete pApp->pDrv;
	delete pApp->pFileLinkMng;
	delete pApp->pBillFileMng;
	
	return CDialog::DestroyWindow();
}

void CSmServiceProcDlg::OnReConnect() 
{
	// TODO: Add your control notification handler code here
	this->UpdateData(TRUE);
	OnUpdatePara();
	
	((CSmServiceProcApp*)AfxGetApp())->pMainMng->SetPass(m_sPass);
	((CSmServiceProcApp*)AfxGetApp())->pMainMng->SetAccount(m_sAccount);
	((CSmServiceProcApp*)AfxGetApp())->pDrv->Init(m_sIPAddr,m_wPort);
	SetConnLed(ID_LED_RED);
	m_cltState.SetWindowText("未连");
	//MessageBox("gw fail 1111");
	
}

//***SMGW25-69, 2004-10-08, JDZ, ADD BEGIN***//
void CSmServiceProcDlg::ReConnect() 
{
	((CSmServiceProcApp*)AfxGetApp())->pDrv->Init(m_sIPAddr,m_wPort);
	try
	{
		SetConnLed(ID_LED_RED);
		m_cltState.SetWindowText("未连");
	}
	catch(...)
	{
	}
}
//***SMGW25-69, 2004-10-08, JDZ, ADD end***//


void CSmServiceProcDlg::OnUpdatePara() 
{
    UpdateData(TRUE);
	
	char  InitName[]=".\\FeeConfig.ini";
    char  sSession[100]="para";
	char sTemp[20];
	memset(sTemp,0,20);
	sprintf(sTemp,"%d",m_nCheckInterval);
	
    strcpy(sSession,"SmConnection");
    
	
	WritePrivateProfileString(sSession, "IPAddr", m_sIPAddr, InitName); 
    WritePrivateProfileString(sSession, "Account", m_sAccount,  InitName); 
    WritePrivateProfileString(sSession, "Possword", m_sPass,InitName); 
	
	memset(sTemp,0,20);
	sprintf(sTemp,"%d",m_wPort);
    WritePrivateProfileString(sSession, "Port", sTemp,InitName); 
    
	
}
void CSmServiceProcDlg::OnConnect()
{
	m_cltState.SetWindowText("已连");
	SetConnLed(ID_LED_GREEN);
	
	//告警接口
	if(m_bGWBroken)
	{
		m_bGWBroken = false;
		TRACE("\n发出网关接口告警恢复\n");
	}
	//告警接口end
	
	if (this->m_pAlarmHandMsgProc)
	{	
		m_pAlarmHandMsgProc->SendMessage(0,'S',0,1,"已和网关建立连接") ;
	}
	
	return;
}


void  CSmServiceProcDlg::OnBroken()
{
	
	SetConnLed(ID_LED_RED);
	m_cltState.SetWindowText("断连" );
	
	//告警接口
	if (!m_bGWBroken)
	{
		m_bGWBroken = true;
		TRACE("发出网关接口告警\n");
	}
	if (this->m_pAlarmHandMsgProc)
	{
		m_pAlarmHandMsgProc->SendMessage(2, 'S', 0, 0, "未和网关建立连接") ;
	}
	//告警接口end
	
	return;
}


void CSmServiceProcDlg::OnLogin()
{
	
	SetConnLed(ID_LED_RED);
	m_cltState.SetWindowText("登录" );
	return;
}

void CSmServiceProcDlg::OnSend(int nSendCount)
{
    
	CString str;
	str.Format("%d", nSendCount);
	GetDlgItem(IDC_SENDCOUNT)->SetWindowText(str);
}


void  CSmServiceProcDlg::OnSendSuc(int Count)
{   
	CString str;
	str.Format("%d", Count);
	GetDlgItem(IDC_SENDSUCCOUNT)->SetWindowText(str);
}


void  CSmServiceProcDlg::OnSendFAIL(int Count)
{
    CString str;
	str.Format("%d", Count);
	GetDlgItem(IDC_SENDFAILCOUNT)->SetWindowText(str);
}


void CSmServiceProcDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if( m_bDestroyFlg == TRUE )
		return;
	OnSend(SendCount);
	OnSendSuc(SendSucCount);
	OnSendFAIL(SendFailCount);
	
	if(g_nConnDBTimeOutCount > 5)
	{
        //大于5次连不上，数据库告警
		if(m_bDBBroken != TRUE)
		{
            m_bDBBroken = TRUE;
   	        SetDBLed(ID_LED_RED);
        }
		
		if (this->m_pAlarmHandMsgProc)
		{
			m_pAlarmHandMsgProc->SendMessage(2,'D',1,0,"数据库连接失败") ;
		}
		
		TRACE("发出数据库告信息");
    }
	else
	{
        //数据库完好
        if(m_bDBBroken == TRUE)
		{
            m_bDBBroken = FALSE;
   	        SetDBLed(ID_LED_GREEN);
			
			if (this->m_pAlarmHandMsgProc)
			{
				m_pAlarmHandMsgProc->SendMessage(0,'D',1,1,"数据库连接成功") ;
			}
			
			TRACE("发出数据库告警恢复");
        }
    }
	
    CString str;
	str.Format("%d",g_nRecvDeliverCount);
	GetDlgItem(IDC_RECV_DELIVER)->SetWindowText(str);
	
	
	str.Format("%d",g_recvbuff);
	GetDlgItem(IDC_RECVBUFF)->SetWindowText(str);
	
	str.Format("%d",g_messagequesize);
	GetDlgItem(IDC_QUESIZE)->SetWindowText(str);
	
	str.Format("%d",g_timermsg);
	GetDlgItem(IDC_TIMEMSG)->SetWindowText(str);
	
	str.Format("%d",g_msg);
	GetDlgItem(IDC_MSG)->SetWindowText(str);
	
	str.Format("%d",g_nExecTimeOutCount);
	GetDlgItem(IDC_DBTIMEOUT)->SetWindowText(str);
	
	str.Format("%d",g_MsgMapCount);
	GetDlgItem(IDC_MAP_COUNT)->SetWindowText(str);
	
	str.Format("%d",g_PreMsgMapCount);
	GetDlgItem(IDC_PREMAP_COUNT)->SetWindowText(str);
    
	UpdateConnectStatus();

	CDialog::OnTimer(nIDEvent);
}


BOOL CSmServiceProcDlg::PreTranslateMessage(MSG* pMsg) 
{
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CSmServiceProcDlg::OnCancel() 
{
}


void CSmServiceProcDlg::OnMyCancel() 
{

	CDialog::OnCancel();
}


void CSmServiceProcDlg::OnOK() 
{
	
	//CDialog::OnOK();
}


void CSmServiceProcDlg::OnZero() 
{
    SendCount=0;
    SendSucCount=0;
    SendFailCount=0;
    g_nRecvDeliverCount = 0;
    g_recvdata=0;
    g_recvbuff=0;
    g_messagequesize=0;
    g_timermsg=0;
    g_msg=0;
    g_nExecTimeOutCount=0;
	
	GetDlgItem(IDC_RECV_DELIVER)->SetWindowText("0");
	GetDlgItem(IDC_RECVBUFF)->SetWindowText("0");
	GetDlgItem(IDC_QUESIZE)->SetWindowText("0");
	GetDlgItem(IDC_TIMEMSG)->SetWindowText("0");
	GetDlgItem(IDC_MSG)->SetWindowText("0");
	GetDlgItem(IDC_DBTIMEOUT)->SetWindowText("0");
}


VOID CSmServiceProcDlg::SetDBLed(int Led)
{
	CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_DBLED);
    if (pWnd)
		m_cDBLed.SetLED(pWnd, Led, ID_SHAPE_ROUND,800);
	
}


void CSmServiceProcDlg::SetConnLed(int Led)
{
	CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_CONNLED);
    if (pWnd)
		m_cConnLed.SetLED(pWnd,Led,ID_SHAPE_ROUND,800);
	
}


void CSmServiceProcDlg::OnButton1() 
{
	
}


void CSmServiceProcDlg::OnClose() 
{
	this->OnMyCancel();
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_STATIC_VERSION, g_Version);
	SetDlgItemText(IDC_STATIC_COPYRIGHT, g_Copyright);
	return TRUE;
}


void CSmServiceProcDlg::OnButtonCatch() 
{
	CSmServiceProcApp* pApp = (CSmServiceProcApp*)AfxGetApp();
	
	// 手工将内存中的数据刷到数据库中
	char *pOutMsg =new char[65];
	int outmsgsize = 65;
	memset(pOutMsg, 0, outmsgsize);
	smpp_Message_header* pHeader = (smpp_Message_header*)pOutMsg;
	pHeader->Command_ID = CMD_CLEAR_MEMORY_MSG;
	pHeader->Command_Status = 0;
	pApp->pMainMng->m_pSmsThread->SendMessage(pOutMsg,outmsgsize);
}

void CSmServiceProcDlg::OnBtnQuery() 
{
	CQueryFixMsgDlg dlg ;	
	int nReponse = dlg.DoModal() ;

	if (nReponse == IDOK)
	{
	}
	else if (nReponse == IDCANCEL)
	{
	}
	
}


void CSmServiceProcDlg::OnStnClickedStaticState()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSmServiceProcDlg::OnBnClickedDbButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//CTestDBDlg dlg;
	//dlg.DoModal();
	throw(1);
}
