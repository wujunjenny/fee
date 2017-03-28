// SmServiceProc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SmServiceProc.h"
#include <objbase.h>
#include "GlobalDef.h"
#include <Shlwapi.h>
#include <sstream>
#include <vector>

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

const int MAX_ADDRESS_LENGTH = 32;
const int MAX_NAME_LENGTH = 1024;

// 崩溃信息
// 
struct CrashInfo
{
	CHAR ErrorCode[MAX_ADDRESS_LENGTH];
	CHAR Address[MAX_ADDRESS_LENGTH];
	CHAR Flags[MAX_ADDRESS_LENGTH];
};

// CallStack信息
// 
struct CallStackInfo
{
	DWORD64 Addr;
	DWORD64 ReturnAddr;
	DWORD64 StackAddr;
	DWORD64 FrameAddr;
	CHAR ModuleName[MAX_NAME_LENGTH];
	CHAR MethodName[MAX_NAME_LENGTH];
	CHAR FileName[MAX_NAME_LENGTH];
	CHAR LineNumber[MAX_NAME_LENGTH];
};

// 安全拷贝字符串函数
//
void SafeStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)
{
	if (nMaxDestSize <= 0) return;
	if (strlen(szSrc) < nMaxDestSize)
	{
		strcpy_s(szDest, nMaxDestSize, szSrc);
	}
	else
	{
		strncpy_s(szDest, nMaxDestSize, szSrc, nMaxDestSize);
		szDest[nMaxDestSize-1] = '\0';
	}
}  

// 得到程序崩溃信息
//
CrashInfo GetCrashInfo(const EXCEPTION_RECORD *pRecord)
{
	CrashInfo crashinfo;
	SafeStrCpy(crashinfo.Address, MAX_ADDRESS_LENGTH, "N/A");
	SafeStrCpy(crashinfo.ErrorCode, MAX_ADDRESS_LENGTH, "N/A");
	SafeStrCpy(crashinfo.Flags, MAX_ADDRESS_LENGTH, "N/A");

	sprintf_s(crashinfo.Address, "%08X", pRecord->ExceptionAddress);
	sprintf_s(crashinfo.ErrorCode, "%08X", pRecord->ExceptionCode);
	sprintf_s(crashinfo.Flags, "%08X", pRecord->ExceptionFlags);

	return crashinfo;
}

// 得到CallStack信息
//
vector<CallStackInfo> GetCallStack(const CONTEXT *pContext)
{
	HANDLE hProcess = GetCurrentProcess();

	//SymInitialize(hProcess, NULL, TRUE);

	vector<CallStackInfo> arrCallStackInfo;

	CONTEXT c = *pContext;

	STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));
	DWORD dwImageType = IMAGE_FILE_MACHINE_I386;

	// 不同的CPU类型，具体信息可查询MSDN
	//
#ifdef _M_IX86
	sf.AddrPC.Offset = c.Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
	dwImageType = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = c.Rip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Rsp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Rsp;
	sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	dwImageType = IMAGE_FILE_MACHINE_IA64;
	sf.AddrPC.Offset = c.StIIP;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.IntSp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrBStore.Offset = c.RsBSP;
	sf.AddrBStore.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.IntSp;
	sf.AddrStack.Mode = AddrModeFlat;
#else
	#error "Platform not supported!"
#endif

	HANDLE hThread = GetCurrentThread();

	while (true)
	{
		// 该函数是实现这个功能的最重要的一个函数
		// 函数的用法以及参数和返回值的具体解释可以查询MSDN
		//
		if (!StackWalk64(dwImageType, hProcess, hThread, &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			break;
		}

		if (sf.AddrFrame.Offset == 0)
		{
			break;
		}
				
		CallStackInfo callstackinfo;
		SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, "N/A");
		SafeStrCpy(callstackinfo.LineNumber, MAX_NAME_LENGTH, "N/A");
		callstackinfo.Addr = sf.AddrPC.Offset;
		callstackinfo.ReturnAddr = sf.AddrReturn.Offset;
		callstackinfo.StackAddr = sf.AddrStack.Offset;

		HMODULE	hModule = NULL;  
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)sf.AddrPC.Offset, &hModule);  

		callstackinfo.FrameAddr = (DWORD64)hModule;//sf.AddrFrame.Offset;
		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = MAX_NAME_LENGTH;

		DWORD symDisplacement = 0;
		
		// 得到函数名
		//
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))
		{
			SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, pSymbol->Name);
		}

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;

		// 得到文件名和所在的代码行
		//
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, lineInfo.FileName);
			sprintf_s(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		// 得到模块名
		//
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))
		{
			SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, moduleInfo.ModuleName);
		}

		arrCallStackInfo.push_back(callstackinfo);
	}

	//SymCleanup(hProcess);

	return arrCallStackInfo;
}



LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)  
{
	//APP_BEGIN_LOG(0);
	time_t rawtime;
	struct tm* timeinfo;
	char timE[80];
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(timE,80,"%Y-%m-%d-%I-%M-%S",timeinfo);
	CString sfilename,sfilename2;
	sfilename.Format("fee%s.dmp",timE);
	sfilename2.Format("fee%s.dmp.log",timE);




	std::stringstream slog;
	// 确保有足够的栈空间
	//
#ifdef _M_IX86
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		static char TempStack[1024 * 128];
		__asm mov eax,offset TempStack[1024 * 128];
		__asm mov esp,eax;
	}
#endif	

	CrashInfo crashinfo = GetCrashInfo(pExceptionInfo->ExceptionRecord);

	// 输出Crash信息
	//
	slog << "system Exception" << endl;
	slog << "\tThreadID: [" << ::GetCurrentThreadId() <<"]"<< endl;
	slog << "\tErrorCode: " << crashinfo.ErrorCode << endl;
	slog << "\tAddress: " << crashinfo.Address << endl;
	slog << "\tFlags: " << crashinfo.Flags << endl;
	
	vector<CallStackInfo> arrCallStackInfo = GetCallStack(pExceptionInfo->ContextRecord);

	// 输出CallStack
	//
	slog << "\tCallStack: " << endl;
	for (vector<CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)
	{
		CallStackInfo callstackinfo = (*i);
		
		slog<<"\t["<<std::hex<<callstackinfo.Addr<<"]";
		slog<<"\t["<<std::hex<<callstackinfo.ReturnAddr<<"]";
		slog<<"\t["<<std::hex<<callstackinfo.StackAddr<<"]";
		slog<<"\t["<<std::hex<<callstackinfo.FrameAddr<<"]\t";
		slog<< callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << callstackinfo.LineNumber << ")" << endl;
	}

	//APP_BEGIN_LOG(0)
	//APP_DEBUG_LOG(slog.str().c_str());
	//APP_END_LOG;
	//GetSmsApp()->CloseLog();


	DWORD dwsize;
	HANDLE hFile2 = CreateFile( sfilename2,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	::WriteFile(hFile2,slog.str().c_str(),slog.str().size(),&dwsize,NULL);
	CloseHandle(hFile2);  


	//	cout << "Enter TopLevelExceptionFilter Function" << endl;  
	HANDLE hFile = CreateFile( sfilename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
	MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;  
	stExceptionParam.ThreadId    = GetCurrentThreadId();  
	stExceptionParam.ExceptionPointers = pExceptionInfo;  
	stExceptionParam.ClientPointers    = FALSE;  
	MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);  
	CloseHandle(hFile);  
	//	getchar();


	return EXCEPTION_EXECUTE_HANDLER;
}

/*
/////////////////////////////////////////////////////////////////////////////
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)  
{
	//	cout << "Enter TopLevelExceptionFilter Function" << endl;
	time_t rawtime;
	struct tm* timeinfo;
	char timE[80];
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(timE,80,"%Y-%m-%d-%I-%M-%S",timeinfo);
	CString sfilename;
	sfilename.Format("Fee%s.dmp",timE);

	HANDLE hFile = CreateFile( sfilename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
	MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;  
	stExceptionParam.ThreadId    = GetCurrentThreadId();  
	stExceptionParam.ExceptionPointers = pExceptionInfo;  
	stExceptionParam.ClientPointers    = FALSE;  
	MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);  
	CloseHandle(hFile);  
	//	getchar();


	return EXCEPTION_EXECUTE_HANDLER;
}

*/
/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcApp

BEGIN_MESSAGE_MAP(CSmServiceProcApp, CWinApp)
//{{AFX_MSG_MAP(CSmServiceProcApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcApp construction

CSmServiceProcApp::CSmServiceProcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmServiceProcApp object

CSmServiceProcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmServiceProcApp initialization

BOOL CSmServiceProcApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
	CMFCButton::EnableWindowsTheming();
	if (!AfxSocketInit())
	{
		return FALSE;
	}
	
	m_pglbuf = new CSockMsgQueue(MAX_BUFF_BYTES);
	if (!m_pglbuf )
	{
		ASSERT(0);
		return FALSE;
	}
	
	

	////add wj
	//LPWSADATA lpwsaData = NULL;
	//WSADATA wsaData;
	//lpwsaData = &wsaData;
	//WORD wVersionRequested = MAKEWORD(1, 1);
	//WSAStartup(wVersionRequested, lpwsaData);
	////end wj

	/*if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}*/
	
	//COM 初始化，为了使用 ADO
    ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
	HANDLE hProcess = GetCurrentProcess();

	SymInitialize(hProcess, NULL, TRUE);

	LPTOP_LEVEL_EXCEPTION_FILTER pPrevFilter =  SetUnhandledExceptionFilter(TopLevelExceptionFilter);
	
	//----------------------SMGW41-36,2005-04-12,zxh,add begin---------------//
	char szCurrentDir[500];
	memset(szCurrentDir, 0, 500);
	GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	
	CString Path;
	
	// 初始化日志文件信息
	
	Path.Format("%s\\LogFile", szCurrentDir);
	if(!PathFileExists(Path))
	{
		CreateDirectory(Path, NULL);
	}
	
	CString LogFileName;
	LogFileName.Format("%s\\LogFile\\FeeLog.log", szCurrentDir);
	
	// 创建日志输出设备
	m_fw = new FileLogWriter(LogFileName);
	// 常见日志管理类
	m_lm = new LogManager(m_fw, (DWORD)5000);
	// 设置日志输出级别，5为默认MSG级别，数字越小级别越高
	m_lm->SetLogLevel(5);
	
	
	CSmServiceProcDlg dlg;
	
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	
	
	// 关闭日志管理器
	m_lm->Close();
	
	// 释放内存
	delete m_lm;
	delete m_fw;
	
	
    ::CoUninitialize();
	delete m_pglbuf;
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
	
}
