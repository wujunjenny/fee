
// tag ����ֵ
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
#define Tag_FromNo           0x011A              //�ϼ���Ԫ����
#define Tag_ToNo             0x011B                //�¼���Ԫ����



#define Tag_EX_Content			 0X0112 // ��ΪSMGP����Ϣ���ݳ��ȶ���140
#define Tag_FixedFee             0X0113  
#define Tag_MsgType              0x0114
#define Tag_LoginMode            0x0115
#define Tag_DestSMGNo            0x0116
#define Tag_SrcSMGNo             0x0117
#define Tag_MSGID                0X0118
#define Tag_SmcNo                0X0119

//SMGW42-55, 2005-7-12, jdz, modi begin//
//CShortMsg����ת��Ϊ������������
#define Tag_EX_ShorMsg				0x0199
//SMGW42-55, 2005-7-12, jdz, modi end//


//***SMGW40-01, 2004-12-3, jdz, add begin***//
//��Ϣ������
#define Tag_DealStep				0x0200

//���������в�������
#define Tag_OperatorType			0x0201
//���������б���Ƿ�ת����Ϣ
#define Tag_SendFlag				0x0202
//����������ҵ��ID
#define Tag_ServiceID				0x0203
//���������м�Ȩģ������ļ�ȨID
#define Tag_AuthPriceID				0x0205
//���������ж�����ϵ�ܸ���
#define Tag_OrderPriceCount			0x0206
//���������ж�����ϵ�б�
#define Tag_OrderPriceBuf			0x0207
//����������SP�����ö�
#define Tag_SPCredit				0x0208
//����������ҵ���������
#define Tag_ServiceValidTime		0x0209
//����������ҵ�����·����������
#define Tag_MaxMTCount				0x0210
//�������������õļƷ�����
#define Tag_ServiceFeeType			0x0211
//�������������õ��ʷ�
#define Tag_ServiceFeeCode			0x0212
//�������������õİ��·�/�ⶥ��
#define Tag_ServiceFixedFee			0x0213

//���״̬�����Ƿ���Ҫת��
#define Tag_ReportSendFlag			0x0214

//�����Ϣת��״̬
#define Tag_Status					0x0215

//***SMGW40-01, 2004-12-3, jdz, add end***//

//SMGW42-65, 2005-7-22, jdz, modi begin//
//��Ȩ������
#define Tag_AuthErrCode           	0x0223
//SMGW42-65, 2005-7-22, jdz, modi end//

//*** SMGW43-01, 2005-09-09, ZF add begin ***//
//SP�·�ʱ�����
#define Tag_TimeControl				0x0224
//*** SMGW43-01, 2005-09-09, ZF add end ***//

//SMGW43-10, 2005-09-15, zhangtao add begin//
//ҵ�����ö�
#define Tag_ServiceCredit            0x0225
//SMGW43-10, 2005-09-15, zhangtao add end//

//ɾ�����
#define  Tag_QFFlag					 0x0226

//�������û�����
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

// ����
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


// �Ʒ��û������ֶ� tlv
typedef struct tagFee_UserType
{
	DWORD Tag;
	DWORD Length;
	UC Value;
	
} Fee_UserType;
// ���Ʒ��û��ĺ��� tlv
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
// ��Ϣ������Դ tlv
typedef struct tagMsg_src
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_MSG_SRC_LEN];
} Msg_src;
// �ʷ���� tlv
typedef  struct  tagFeeType
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_FEETYPE_LEN];                  
}  FeeType;

// �ʷѴ���  tlv
typedef struct tagFeeCode
{
	DWORD Tag;
	DWORD Length;
    char  Value[CMPP_FEECODE_LEN];                  
} FeeCode;
// ����  tlv
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

// �ύʱ�� tlv
typedef struct tagEX_SubTime
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_TIMESTAMP_LEN];
} EX_SubTime;

// Ͷ��ʱ�� tlv
typedef struct tagEX_DlrTime
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_TIMESTAMP_LEN];
} EX_DlrTime;

// ������Ϣ�ʻ� tlv
typedef struct tagEX_RecvAccount
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_ACCOUNTNAME_LEN];
} EX_RecvAccount;

// ������Ϣ�ʻ� tlv
typedef struct tagEX_SendAccount
{
	DWORD Tag;
	DWORD Length;
	char Value[MAX_ACCOUNTNAME_LEN];
} EX_SendAccount;

// Դ��ϢID tlv
typedef struct tagEX_OrigMessageID
{
	DWORD Tag;
	DWORD Length;
	char Value[MESSAGE_ID_LEN];
} EX_OrigMessageID;

// Ŀ����ϢID tlv
typedef struct tagEX_DestMessageID
{
	DWORD Tag;
	DWORD Length;
	char Value[MESSAGE_ID_LEN];
} EX_DestMessageID;

// ״̬����ֵ tlv
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












