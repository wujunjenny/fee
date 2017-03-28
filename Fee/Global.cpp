
#include "stdAfx.h"
#include "Shlwapi.h"
#include "Globaldef.h"
#include "SmServiceProc.h"



double g_ScanDBTime;				// 扫描数据库的时间
double g_DataExistTime;				// 缓冲区中数据的生存周期  单位：小时
double g_SaveFileTime;				// 保存话单的时间周期 单位：小时
int g_IsAskReport;					//超时是否回状态报告
int g_AskReportTime;				// 回复状态报告的时间
int g_AccountStatus;
int g_AccountSeq;
CString g_FilePath;	    			// 话单文件路径	
CString g_BakFilePath;				// 话单文件备份路径
tagTimeTable g_TimeTab;
int SendCount;
int SendSucCount;
int SendFailCount;
int g_nRecvDeliverCount;
int g_nExecTimeOutCount;
ULONG g_savefilecount;
ULONG g_nConnDBTimeOutCount = 6;
int g_messagequesize;
int g_timermsg;
int g_msg;
int g_recvdata;
int g_recvbuff;
char g_message[2000];
char g_error[2000];
int g_OpenCache;
int g_Timeout;

//add by wujun
DWORD g_MsgMapMaxCount = 10000;
DWORD g_PreMsgMaxMapCount = 10000;

//end add


DWORD g_MaxBuffSize;
DWORD g_MaxMsgCount;
DWORD g_DBMaxBuffSize;
DWORD g_DBMaxMsgCount;


DWORD g_MsgMapCount;
DWORD g_PreMsgMapCount;
DWORD g_PaymentAffirmTimeOut;
WORD g_NetMode;
WORD g_FeeMonthMsgNeedReport;

int g_EnableSaveSQLData;

CString g_Version;
CString g_Copyright;

CFile LogFile;

#if TRACEDBG
void PrintMessage(LPSTR lpszFormat, ...)
{
	char	buf[520];
	va_list va;
	
	va_start(va, lpszFormat);
	vsprintf(buf, lpszFormat, va);
	va_end(va);
	
	strcat(buf,"\r\n");
	
	TRACE0(buf);
}
#elif FILEDBG	// 写入跟踪文件
void PrintMessage(LPSTR lpszFormat, ...)
{
	char	aBuffer[5000];
	va_list va;
	
	va_start( va, lpszFormat );
	vsprintf( aBuffer, lpszFormat, va );
	va_end( va );
	
	((CSmServiceProcApp*)AfxGetApp())->m_lm->WriteLine(aBuffer,5);
	
}
#else		// 不做操作
void PrintMessage(LPSTR lpszFormat, ...)
{
}
#endif





