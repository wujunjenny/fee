
// tag 类型值
#define Tag_Fee_UserType         0x0100
#define Tag_Fee_termial_id       0x0101
#define Tag_TP_Udhi              0x0102
#define Tag_Msg_src              0x0103
#define Tag_FeeType              0x0104
#define Tag_FeeCode              0x0105
#define Tag_Reserve              0x0106
#define Tag_GivenCode            0x0107
#define Tag_MtFlag               0x0108
#define Tag_ServiceU             0x0109
#define ESM_CLASS_UDHI           0X40
#define MAX_TRUE_MSG_LEN		160+1

// YinBiaozheng Add begin,2003-03-18
#define Tag_EX_SubTime			0X0FF9
#define Tag_EX_DlrTime			0X0FFA
#define Tag_EX_RecvAccount		0X0FFB
#define Tag_EX_SendAccount		0X0FFC
#define Tag_EX_Status			0X0FFD
#define Tag_UNIKEY				0X0FFE
#define Tag_SMID				0X0FFF

#define Tag_SOURCEID		 0X0118
#define Tag_Ex_MsgType		 0x1002
#define Tag_Ex_AreaCode      0x1003
#define Tag_Ex_CPID			 0X1004
#define Tag_PayUserType      0x1005
#define Tag_RemainMoney      0x1007
#define Tag_PayFlow          0x1008
#define Tag_PayBeforeSupport 0x1009
#define Tag_PaymntFlag       0X100A
#define Tag_SMType           0X100B
#define Tag_FromNo           0x011A              //上级网元代码
#define Tag_ToNo             0x011B                //下级网元代码



#define Tag_EX_Content			 0X0112 // 因为SMGP的消息内容长度多于140
#define Tag_FixedFee             0X0113  
#define Tag_MsgType              0x0114
#define Tag_LoginMode            0x0115
#define Tag_DestSMGNo            0x0116
#define Tag_SrcSMGNo             0x0117
#define Tag_MSGID                0X0118
#define Tag_SmcNo                0X0119

//SMGW42-55, 2005-7-12, jdz, modi begin//
//CShortMsg对象转换为二进制流保存
#define Tag_EX_ShorMsg				0x0199
//SMGW42-55, 2005-7-12, jdz, modi end//


//***SMGW40-01, 2004-12-3, jdz, add begin***//
//消息处理步骤
#define Tag_DealStep				0x0200

//定购处理中操作类型
#define Tag_OperatorType			0x0201
//定购处理中标记是否转发消息
#define Tag_SendFlag				0x0202
//定购处理中业务ID
#define Tag_ServiceID				0x0203
//定购处理中鉴权模块产生的鉴权ID
#define Tag_AuthPriceID				0x0205
//定购处理中定购关系总个数
#define Tag_OrderPriceCount			0x0206
//定购处理中定购关系列表
#define Tag_OrderPriceBuf			0x0207
//定购处理中SP的信用度
#define Tag_SPCredit				0x0208
//定购处理中业务的生存期
#define Tag_ServiceValidTime		0x0209
//定购处理中业务能下发的最大条数
#define Tag_MaxMTCount				0x0210
//定购处理中配置的计费类型
#define Tag_ServiceFeeType			0x0211
//定购处理中配置的资费
#define Tag_ServiceFeeCode			0x0212
//定购处理中配置的包月费/封顶费
#define Tag_ServiceFixedFee			0x0213

//标记状态报告是否需要转发
#define Tag_ReportSendFlag			0x0214

//标记消息转发状态
#define Tag_Status					0x0215

//***SMGW40-01, 2004-12-3, jdz, add end***//

//SMGW42-65, 2005-7-22, jdz, modi begin//
//鉴权错误码
#define Tag_AuthErrCode           	0x0223
//SMGW42-65, 2005-7-22, jdz, modi end//

//*** SMGW43-01, 2005-09-09, ZF add begin ***//
//SP下发时间控制
#define Tag_TimeControl				0x0224
//*** SMGW43-01, 2005-09-09, ZF add end ***//

//SMGW43-10, 2005-09-15, zhangtao add begin//
//业务信用度
#define Tag_ServiceCredit            0x0225
//SMGW43-10, 2005-09-15, zhangtao add end//

//删除标记
#define  Tag_QFFlag					 0x0226

//被赠送用户号码
#define Tag_GiftAddr				 0x0227

//add by wj for bill
//OrgAddr maybe convert so record org info
#define Tag_Ex_SendOrgAddr			 0x0304
#define Tag_Ex_SendServiceCode		 0x0305
#define Tag_Ex_SendCPID				 0x0306
#define Tag_Ex_SourceIpAddr			 0x0307
#define Tag_Ex_SourceCodeType		 0x0308
#define Tag_Ex_ReturnFirst			 0x0309
#define Tag_Ex_SendDestAddr			 0x030A
//end 
//modify by gyx
#define Tag_Ex_SendGWName			 0x030B
//end

// 长度
#define CMPP_SOURCE_ADDR_LEN			   22

#define CMPP_DESTINATION_ADDR_LEN		   22	
#define CMPP_FEECODE_LEN                    7
#define CMPP_FEETYPE_LEN                    3
#define CMPP_RESERVE_LEN                    9
#define CMPP_MSG_SRC_LEN                   7
#define CMPP_MTFLAG_LEN                    2



const int MT = 1;
const int MO = 2;
const int MC = 3;
const int MTF = 4;
const int MOF = 5;
const int MCF = 6;
const int MTC = 7;
const int MOC = 8;
const int MCFF = 9;
const int MTFF = 10;
const int MOFF = 11;
const int MCC = 12;
const int MCAF = 13;
const int MCA = 14;
const int MCAFF = 15;
const int MACFF = 16;
const int FWDPAYMSG = 18;
const int FWDQUERYMSG = 19;
const int UNKOWN = 17;

typedef struct tagTLV
{
	DWORD Tag;
	DWORD Length;
} TLV;


// 计费用户类型字段 tlv
typedef struct tagFee_UserType
{
	DWORD Tag;
	DWORD Length;
	UC Value;
	
} Fee_UserType;
// 被计费用户的号码 tlv
typedef struct tagFee_termial_id
{
	DWORD Tag;
	DWORD  Length;
    char   Value[CMPP_SOURCE_ADDR_LEN]; 
} Fee_termial_id;


typedef struct tagTP_Udhi
{
	DWORD Tag;
	DWORD Length;
	UC Value;
	
} TP_Udhi;
// 信息内容来源 tlv
typedef struct tagMsg_src
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_MSG_SRC_LEN];
} Msg_src;
// 资费类别 tlv
typedef  struct  tagFeeType
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_FEETYPE_LEN];                  
}  FeeType;

// 资费代码  tlv
typedef struct tagFeeCode
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_FEECODE_LEN];                  
} FeeCode;
// 保留  tlv
typedef struct tagReserve
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_RESERVE_LEN];                  
} Reserve;
typedef struct tagGivenCode
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_FEECODE_LEN];                  
} GivenCode;
typedef struct tagMtFlag
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_MTFLAG_LEN];                  
} MtFlag;


#ifndef MESSAGE_ID_LEN
#define MESSAGE_ID_LEN 65
#endif

// 提交时间 tlv
typedef struct tagEX_SubTime
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_TIMESTAMP_LEN];
} EX_SubTime;

// 投递时间 tlv
typedef struct tagEX_DlrTime
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_TIMESTAMP_LEN];
} EX_DlrTime;

// 接收消息帐户 tlv
typedef struct tagEX_RecvAccount
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_ACCOUNTNAME_LEN];
} EX_RecvAccount;

// 发送消息帐户 tlv
typedef struct tagEX_SendAccount
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_ACCOUNTNAME_LEN];
} EX_SendAccount;

// 源消息ID tlv
typedef struct tagEX_OrigMessageID
{
	DWORD Tag;
	DWORD Length;
	char Value[MESSAGE_ID_LEN];
} EX_OrigMessageID;

// 目的消息ID tlv
typedef struct tagEX_DestMessageID
{
	DWORD Tag;
	DWORD Length;
	char Value[MESSAGE_ID_LEN];
} EX_DestMessageID;

// 状态报告值 tlv
typedef struct tagTag_EX_Status
{
	DWORD Tag;
	DWORD Length;
	DWORD Value;
} EX_Status;

#define SUCCESS_REPORT	0
#define FAIL_REPORT		1
#define EXPIRE_REPORT	2
// YinBiaozheng Add begin,2003-03-18












