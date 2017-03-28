

#if !defined(__GLOBALDEF_H__)

#define __GLOBALDEF_H__

#include "smppdef.h"



extern double g_ScanDBTime;             // 扫描数据库的时间
extern double g_DataExistTime;		    // 缓冲区中数据的生存周期  单位：小时
extern double g_SaveFileTime;		    // 保存话单的时间周期 单位：小时
extern int g_IsAskReport;				//超时是否回状态报告
extern int g_AskReportTime;             // 回复状态报告的时间
extern int g_AccountStatus;
extern int g_AccountSeq;
extern CString g_FilePath;	    		// 话单文件路径	
extern CString g_BakFilePath;		// 话单文件备份路径

struct tagTimeTable
{
	char Jan[12];
    char Feb[12];
    char Mar[12];
    char Apr[12];
    char May[12];
    char Jun[12];
    char Jul[12];
    char Aug[12];
    char Sep[12];
    char Oct[12];
    char Nov[12];
    char Dec[12];
};

typedef  struct tagSaveShortMessage
{
	unsigned long int saveTime;
	char unikey[65];
	char smid[65];
	char sourceID[65];
	char orgAddr[22];
	char destAddr[22];
	char submitTime[21];
	char deliverTime[21];
	char UD[300];
	char msgType[3];
	char status[6];
	char serviceID[11];
	char feeUserType[2];
	char feeCode[7];
	char feeType[3];
	char feeAddr[22];
	char fixedFee[7];
	char rcvAccount[22];
	char sendAccount[22];
	char reportFlag[2];
	char moFlag[2];
	char givenCode[6];
	char serviceUpID[11];
	char spcode[11];
	char smcCode[7];
	char messageType[3];
	char smgno[7];
	char fwdsmgno[7];
	char areaCode[7];
	char priority[2];
	int  msgLen;
	unsigned char payUserType;
	unsigned char payFlow;
	unsigned char payBeforeSupport;
	unsigned char AuthErrCode;
	unsigned char paymntFlag;
	unsigned char SMType;
	char FromNo[7];
	char ToNo[7];
	DWORD TimerID;
	char AuthPriceID[65];
	int OperatorType;
	char GiftAddr[22];
//add by wj
//OrgAddr maybe convert so record org info
	char SendOrgAddr[22];	//#define Tag_Ex_SendOrgAddr			 0x0304
	char SendDestAddr[22];	//#define Tag_Ex_SendDestAddr			 0x030A
	char SendServiceID[20]; //#define Tag_Ex_SendServiceCode		 0x0305
	char SendCPID[20];		//#define Tag_Ex_SendCPID				 0x0306
	char SourceIpAdd[50];	//#define Tag_Ex_SourceIpAddr			 0x0307
	int SourceCodeType;		//#define Tag_Ex_SourceCodeType		     0x0308
	int ReturnFirst;		//#define Tag_Ex_ReturnFirst			 0x0309
//end 
//modify by gyx
	char SendGWName[30];    //#define Tag_Ex_SendGWName			     0x030B
	long RecvID;
	tagSaveShortMessage()
	{
		RecvID = 0;
		memset(SendGWName,0,30);
	
	};
//end

} SaveShortMessage;

//SMGW42-55, 2005-7-12, jdz, modi begin//
typedef struct tagFeeCheckMsg
{
	char SPID[11];		//企业代码
	char FeeAddr[22];	//计费号码
	char SubmitTime[30];//提交时间
	char OrgAdd[22];
	char DestAddr[22];
	char ServiceID[11];//业务代码
}FEECHECKMSG;

#define SMID_LENTH   (14+1)

struct SM_STRUCT
{
	
	char	EmsClass;
	char	SndTon;
	char   SndNpi;
	//源地址
	char   SndMSISDN[MAX_ADDR_LEN];
	char	RcvTon;
	char	RcvNpi;
	//目的地址
	char   RcvMSISDN[MAX_ADDR_LEN];
	char	pri;
	char	pid;
	char	ScheduleTime[MAX_TIMESTAMP_LEN];
	char	ExpireTime[MAX_TIMESTAMP_LEN];
	char   ServiceID[MAX_SERVICE+1];
	char   ReportType;
	//短消息内容
	char	DCS;
	int	SMLength;
	char	SMUD[MAX_SM_LEN+1];
	
};


struct structShortMsg2
{
	char DestMsgID[40];
	int  FeeFlow;
	char m_CalledNo[22];
	char m_CallingNo[22];
	int m_DealStep;
	char m_DeliverTime[MAX_TIMESTAMP_LEN];
	char m_FeeCallNo[22];
	int  m_IsReport;
	int	 m_MsgType;
	int	 m_nErr;
	int	 m_nExtDataSize;
	int	 m_nPayBeforeSupport;
	int  m_nReSubmitCount;
	int  m_nState;
	DWORD      m_nSubmitTime;
	bool       bHaveSMData;
	SM_STRUCT  m_pSMData;
	int m_SendMsgFlag;
	int m_SendToGWMonthMsg;
	char    m_sNewCode[MAX_SERVICE+1];
	char	m_sRcverAccount[MAX_ACCOUNTNAME_LEN];
	char	m_sSenderAccount[MAX_ACCOUNTNAME_LEN];
	char    m_sSMID[SMID_LENTH];
	char	m_SubmitTime[MAX_TIMESTAMP_LEN];
	UC      m_TLVMsgType;
	UL		m_ulCmdID;
	UL      m_ulRecverID;
	UL      m_ulSenderID;
	UL m_ulSequenceID;
	UL nSequenceID;
	char sAccountName[MAX_ACCOUNTNAME_LEN]; 
};
//SMGW42-55, 2005-7-12, jdz, modi end//


struct tagFeeMonthStatus
{
	char FeeAddr[22];		//计费号码
	char UserAddr[22];		//下发用户号码
    char SPID[11];			//企业代码
    char ServiceID[11];		//业务代码
    char FeeDate[21];		//日期
    int Status;				//状态
	char FixedFee[7];		//包月费用
	char SendAccount[30];	//发送帐号
};



struct tagSmsSave
{ 
	char Unikey[65];
	char Smid[65];	
	char OrgAddr[22];	
	char DestAddr[22];	
	char submitTime[30];	
	char deliverTime[30];	
	char UD[300];	
	WORD MsgType;	
	WORD MessageType;	
	WORD Status;	
	char serviceID[12];	
	WORD FeeUserType;	
	char FeeType[3];	
	char FeeCode[7];	
	char FeeAddr[22];	
	char RcvAccount[17];	
	char SendAccount[17];	
	WORD ReportFlag;	
	WORD MoFlag;	
	char GivenCode[7];	
	char ServiceUpID[12];	
	char SpCode[17];	
	char FixedFee[7];	
	char SMGNo[7];	
	char FwdSMGNO[7];	
	char SMCNo[7];	
	char AreaCode[7];	
	char SourceID[65];	
	WORD Priority;	
	WORD MsgLen;	
	WORD PayUserType;
	WORD PayFlow;	
	WORD PayBefore;	
	WORD AuthErrCode;	
	WORD PaymntFlag;	
	WORD SMType;	
	char FromNo[7];	
	char ToNo[7];	
//add by wj
//OrgAddr maybe convert so record org info
	char SendOrgAddr[22];	//#define Tag_Ex_SendOrgAddr			 0x0304
	char SendDestAddr[22];	//#define Tag_Ex_SendDestAddr			 0x030A
	char SendServiceID[20]; //#define Tag_Ex_SendServiceCode		 0x0305
	char SendCPID[20];		//#define Tag_Ex_SendCPID				 0x0306
	char SourceIpAdd[50];	//#define Tag_Ex_SourceIpAddr			 0x0307
	int SourceCodeType;		//#define Tag_Ex_SourceCodeType		 0x0308
	int ReturnFirst;			//#define Tag_Ex_ReturnFirst			 0x0309
//end 
};


struct tagSmsSaveDB
{ 
	char SPType;
	char Unikey[65];
	char Smid[65];	
	char OrgAddr[22];	
	char DestAddr[22];	
	char submitTime[30];	
	char deliverTime[30];	
	char UD[300];	
	WORD MsgType;	
	WORD MessageType;	
	WORD Status;	
	char serviceID[12];	
	WORD FeeUserType;	
	char FeeType[3];	
	char FeeCode[7];	
	char FeeAddr[22];	
	char RcvAccount[17];	
	char SendAccount[17];	
	WORD ReportFlag;	
	WORD MoFlag;	
	char GivenCode[7];	
	char ServiceUpID[12];	
	char SpCode[17];	
	char FixedFee[7];	
	char SMGNo[7];	
	char FwdSMGNO[7];	
	char SMCNo[7];	
	char AreaCode[7];	
	char SourceID[65];	
	WORD Priority;	
	WORD MsgLen;	
	WORD PayUserType;
	WORD PayFlow;	
	WORD PayBefore;	
	WORD AuthErrCode;	
	WORD PaymntFlag;	
	WORD SMType;	
	char FromNo[7];	
	char ToNo[7];
	char AuthPriceID[65];
	int OperatorType;
	char GiftAddr[22];
};


struct tagDelDBBufferMsg
{ 
	char SPType;
	char Unikey[65];
};

struct tagSmsCheckExpiredDB
{
	char SPType;
	char ExpiredTime[10];
	char CurTime[20];
};


//***SMGW40-05, 2005-01-15, jdz, modi begin***//
struct tagSmsCheckRepeatFixedMsg
{
	char SPType;
	char FeeMonth[6];
	char spid[10];
	char ServiceID[10];
	char Terminal_id[21];
	char SubmitTime[30];
	char FixFlag;
	//SMGW43-03, 2005-09-09, ZF add begin//
	char DestAddr[21];
	//SMGW43-03, 2005-09-09, ZF add end//
};

struct tagSmsDelFixedMsg
{
	char SPType;
	char FeeMonth[6];
	char spid[10];
	char ServiceID[10];
	char Terminal_id[21];
	//SMGW43-03, 2005-09-09, ZF add begin//
	char DestAddr[21];
	//SMGW43-03, 2005-09-09, ZF add end//
};

struct tagSmsSaveFixUserState
{
	char SPType;
	char FeeMonth[6];
	char spid[10];
	char ServiceID[10];
	char Terminal_id[21];
	char SubmitTime[30];
};
//***SMGW40-05, 2005-01-15, jdz, modi end***//

// *** SMGW30-14,2004-08-06,wzy add begin *** //
// *** SMGW30-14,2004-08-06,wzy add end *** //

// *** SMGW25-24,2000-06-22,wzy add end *** //
extern tagTimeTable g_TimeTab;
extern int SendCount;
extern int SendSucCount;
extern int SendFailCount;
extern int g_nRecvDeliverCount;
extern int g_nExecTimeOutCount;
extern ULONG g_savefilecount;
extern int SendCount;
extern ULONG g_nConnDBTimeOutCount;
extern int g_messagequesize;
extern int g_timermsg;
extern int g_msg;
extern int g_recvdata;
extern int g_recvbuff;
extern char g_message[2000];
extern char g_error[2000];
extern int g_OpenCache;
// *** SMGW25-30,2004-06-30,wzy add begin *** //
extern int g_Timeout;
// *** SMGW25-30,2004-06-30,wzy add end *** //
// *** SMGW25-32,2004-07-05,wzy add begin *** //
extern DWORD g_MsgMapCount;
extern DWORD g_PreMsgMapCount;
// *** SMGW25-32,2004-07-05,wzy add end *** //

// *** wujun *** //
extern DWORD g_MsgMapMaxCount;
extern DWORD g_PreMsgMaxMapCount;
// *** wujun end *** //

extern DWORD g_MaxBuffSize;
extern DWORD g_MaxMsgCount;
extern DWORD g_DBMaxBuffSize;
extern DWORD g_DBMaxMsgCount;
extern WORD g_NetMode;
extern WORD g_FeeMonthMsgNeedReport;


extern DWORD g_PaymentAffirmTimeOut;


extern CString g_Version;
extern CString g_Copyright;


extern void PrintMessage(LPSTR lpszFormat, ...);

#define MAX_LOG_FILE_LEN	5242880

const int SMPP_MESSAGE_LEN = sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm);
const int SAVE_MESSAGE_LEN = sizeof(smpp_Message_header) + sizeof(SaveShortMessage);
#define SUCCESS_DELIVER_MESSAGE		0

#define NEED_SAVETO_DATABASE		1
#define NEED_SAVETO_DISKFILE		1

//支持预付费流程
#define SUPPORT_PREPAY				1
//不支持预付费流程
#define NOT_SUPPORT_PREPAY			0

//计费点不在SP管理平台
#define FEEDEFINE_NOTIN_SPMS		0
//计费点在SP管理平台
#define FEEDEFINE_IN_SPMS			1

//后付费用户不走预付费流程
#define NOTPREPAY_USER_NOT_SUPPORT_PREPAY		0
//后付费用户走预付费流程
#define NOTPREPAY_USER_SUPPORT_PREPAY			1

//预付费用户
#define PREPAY_USER			0
//后付费用户
#define NOT_PREPAY_USER		2
#define NORMAL_PREPAY_USER			0
#define NORMAL_BEHINDPAY_USER		2

#define MO_NEED_FEE					1
#define MT_NEED_FEE					1
#define MC_NEED_FEE					1

#define NO_PRE_PAY					0
#define NO_STANDARD_METHOD			0

#define SUCCESS_REPORT				0
#define FAIL_REPORT					1
#define EXPIRED_REPORT				2

#define CMD_REPEAT_CHECK_TAB			0
#define CMD_CHECK_TIMEOUT_TAB			2
#define CMD_ADD_DBBUFFERMSG_TAB			3
#define CMD_DEL_DBBUFFERMSG_TAB			4
#define CMD_DELFIXEDMSG_TAB				5




#define NEED_RESPONSED_REPORT		1


#endif