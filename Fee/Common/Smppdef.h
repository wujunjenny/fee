#ifndef __DEF_H__
#define __DEF_H__
#include "CodeAndDecode.h"
#include "tlv.h"
#include "SmppExDef.h"

//SMGW42-55, 2005-7-12, jdz, modi begin//
#define MAX_SMPP_PACKET_LENGTH  (2048)
//SMGW42-55, 2005-7-12, jdz, modi end//
#define DCSMASK	0XCC
#define DCS_UNICODE	0X08





#define ESME_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
#define ESME_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
#define ESME_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
#define ESME_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF



#define ESME_ROK  			0X00000000
#define ESME_RINVMSGLEN 	0X40000001
#define ESME_RINVCMDLEN		0X40000002
#define ESME_RINVCMDID     	0X40000003
#define ESME_RINVBNDSTS		0X40000004
#define ESME_RALYBND   		0X40000005
#define ESME_RINVPRTFLG		0X40000006
#define ESME_RINVREGDLVFLG	0X40000007
#define ESME_RSYSERR    	0X40000008
#define ESME_RINVPAR   		0X40000009
#define ESME_RINVSRCADR     0X4000000A
#define ESME_RINVDSTADR		0X4000000B
#define ESME_RINVMSGID		0X4000000C
#define ESME_RINVPASWD		0X4000000D
#define ESME_RINVPASWDLEN  	0X4000000E
#define ESME_RINVSYSIDSRV	0X4000000F
#define ESME_RCNTCANMSG  	0X40000010
#define ESME_RINVDATFMT 	0X40000011
#define ESME_RCNTREPMSG 	0X40000012 
#define ESME_RMSGQFUL		0X40000013
#define ESME_RSERNOTSUP  	0X40000014
#define ESME_RINVREPADDR	0X40000015
#define ESME_RUNKNOWNERR 	0X40000016



#define ESME_RCNTADDCUST	0X00000000
#define ESME_RCNTDELCUST	0X10000001
#define ESME_RCNTMODCUST	0X10000002
#define ESME_RCNTQRYCUST   	0X10000003
#define ESME_RINVCUSTID		0X10000004
#define ESME_RINVCUSTIDLEN	0X10000005
#define ESME_RINVCUSTNAMLEN 0X10000006
#define ESME_RINVADRLEN   	0X10000007
#define ESME_RCUSTEXIST 	0X10000008
#define ESME_RCUSTNOTEXIST  0X10000009




#define ERROR_NONE			RP_ACK
#define P_UNKNOWN           RP_UNKNOWN_SUBSCRIBER
#define p_PROVISION  		RP_TELESERVICE_NOT_PROVISIONED
#define T_BARRED          	RP_CALL_BARRED
#define T_SUPPORT     		RP_FACILITY_NOT_SUPPORTED
#define T_ABSENT          	RP_ABSENT_SUBSCRIBER
#define T_MSSUPPORT		 	RP_MS_NOT_EQUIPED  
#define T_MSERROR			RP_ERROR_IN_MS
#define P_ILLEGAL_SUB		RP_ILLEGAL_MS
#define P_ILLEGAL_EQUIP 	RP_ILLEGAL_EQUIPMENT
#define T_SYSTEM			RP_SYSTEM_FAILURE
#define T_MEMCAP			RP_MEMORY_CAPACITY_EXCEEDED



#define EN_ROUTE  			0X30000001
#define DELIVERED           0X30000002
#define EXPIRED      		0X30000003
#define DELETED           	0X30000004
#define UNDELIVERABLE 		0X30000005
#define ACCEPTED          	0X30000006
#define INVALID    		 	0X30000007



#define SM_STATE_EN_ROUTE               1       
#define SM_STATE_DELIVERED              2       
#define SM_STATE_EXPIRED                3       
#define SM_STATE_DELETED                4      
#define SM_STATE_UNDELIVERABLE          5     
#define SM_STATE_ACCEPTED               6      
#define SM_STATE_INVALID                7   




#define ESME_BNDRCV			0X00000001
#define ESME_BNDRCV_RESP	0X80000001
#define ESME_BNDTRN			0X00000002
#define ESME_BNDTRN_RESP	0X80000002
#define ESME_UBD   			0X00000006 
#define ESME_UBD_RESP		0X80000006
#define ESME_SUB_SM			0X00000004
#define ESME_SUB_SM_RESP	0X80000004
#define ESME_DELIVER_SM		0X00000005
#define ESME_DELIVER_SM_RESP 0X80000005
#define ESME_QUERY_SM		0X00000003
#define ESME_QUERY_SM_RESP	0X80000003
#define ESME_CANCEL_SM		0X00000008
#define ESME_CANCEL_SM_RESP	0X80000008
#define ESME_REPLACE_SM		0X00000007
#define ESME_REPLACE_SM_RESP 0X80000007
#define ESME_QRYLINK		0X00000015
#define ESME_QRYLINK_RESP	0X80000015
#define ESME_NACK			0X80000000

#define ESME_SHREPORT_SM		0X00000017
#define ESME_SHREPORT_SM_RESP   0X80000017
#define ESME_PAYMENT_SM			0X00000022
#define ESME_PAYMENT_SM_RESP    0X80000022
#define ESME_PAYCONFIRM_SM      0X00000023
#define ESME_PAYCONFIRM_SM_RESP 0X80000023
#define ESME_CHECK_VALID_SM			0X00000027
#define ESME_CHECK_VALID_SM_RESP	0X80000027
//SMGW42-55, 2005-7-12, jdz, modi begin//
#define ESME_RESUME_SM				0X00000026
#define ESME_RESUME_SM_RESP			0X80000026
//SMGW42-55, 2005-7-12, jdz, modi end//


// 线程内部的消息
#define CMD_SAVE_FILE_BILL			0X00000901
#define CMD_ADD_DB_BUFFER			0X00000101
#define CMD_REPEAT_CHECK			0X00000103
#define CMD_REPEAT_CHECK_RESULT		0X80000103
#define CMD_FIND_DB_BUFFER			0X00000104
#define CMD_FIND_DB_BUFFER_RESULT	0X80000104
#define CMD_CLEAR_TEMP_FILE			0X00000105
#define CMD_CHECK_EXPIRE_RESULT     0X00000108
#define CMD_CHECK_EXPIRE			0X00000109
#define CMD_CLEAR_MEMORY_MSG		0X00000110
#define CMD_DELFIXEDMSG				0X00000112
#define CMD_UPDATE_FEEMONTH_STATE	0X00000113
#define CMD_SAVEFIXEDMSG			0X00000114





#define ESME_REPLACE_SM_EX			0X00000090	
#define	ESME_REPLACE_SM_EX_RESP		0X80000090	
#define ESME_QUERY_SMS				0X00000091	
#define ESME_QUERY_SMS_RESP			0X80000091	
#define ESME_QUERY_SM_DETAILS		0X00000092	
#define ESME_QUERY_SM_DETAILS_RESP	0X80000092	
#define ESME_ADD_SUB				0X000000A0	
#define ESME_ADD_SUB_RESP			0X800000A0	
#define ESME_DEL_SUB				0X000000A1	
#define ESME_DEL_SUB_RESP			0X800000A1	
#define ESME_MOD_SUB				0X000000A2	
#define ESME_MOD_SUB_RESP			0X800000A2	
#define ESME_QRY_SUB				0X000000A3	
#define ESME_QRY_SUB_RESP			0X800080A3	





#define SYSTEM_ID_LEN					16
#define PASSWORD_LEN					 9
#define SYSTEM_TYPE_LEN					13
#define ADDRESS_RANGE_LEN				41 
#define SERVICE_TYPE_LEN				11
#define SOURCE_ADDR_LEN					21
#define ADDR_LEN						21
#define DESTINATION_ADDR_LEN			21
#define	SMPP_TIME_LEN					17 
#define SCHEDULE_DELIVERY_TIME_LEN		SMPP_TIME_LEN
#define VALIDITY_PERIOD_LEN				SMPP_TIME_LEN 
#define REG_TIME_LEN					SMPP_TIME_LEN
#define SHORT_MESSAGE_LEN			   255 
#define MESSAGE_ID_LEN					 65
#define ORIGINAL_MESSAGE_ID_LEN			 9
#define ORIGINATING_ADDR_LEN			21	
#define FINAL_DATE_LEN					17
#define REGISTERED_DELIVERY_FLAG_LEN    17

#define CUSTOMER_ID_LEN					21
#define CUSTOMER_NAME_LEN				21
#define CUSTOMER_ADDRESS_LEN			31
#define SMSC_PROVISIONING_RECORD

typedef struct org_smpp_Message_header
{
	long int Command_Length;
	long int Command_ID;
	long int Command_Status;
	long int Sequence_No;

} org_Message_header;

typedef struct smpp_Message_header
{
	long int Command_Length;
	long int Command_ID;
	long int Command_Status;
	long int Sequence_No;
//add by wj
	long int nRcvID;
	long int nSndID;
//end
} Message_header;

typedef struct smpp_Bind_Receiver
{
	char system_id[SYSTEM_ID_LEN];
	char password[PASSWORD_LEN];
	char system_type[SYSTEM_TYPE_LEN];
	UC interface_version;
	UC addr_ton;
	UC addr_npi;
	char address_range[ADDRESS_RANGE_LEN];
} Bind_Receiver;

typedef struct smpp_Bind_Receiver_Resp
{
	char system_id[SYSTEM_ID_LEN];
}Bind_Receiver_Resp;

typedef struct smpp_Submit_Sm
{
	char service_type[SERVICE_TYPE_LEN];
	UC source_addr_ton;
	UC source_addr_npi;
	char source_addr[SOURCE_ADDR_LEN];
	UC dest_addr_ton;
	UC dest_addr_npi;
	char destination_addr[DESTINATION_ADDR_LEN];
	UC esm_class;
	UC protocol_ID;
	UC priority_flag;
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC replace_if_present_flag;
	UC data_coding;
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
	UL ext_data_len;	
	char ext_data[];
}Submit_Sm;


struct Sm_Report
{
	UC        Msg_ID[40]; //12个字节的消息号
	UC        Protocol;// 协议类型　0　SMPP　1　CMPP　2　SGIP
	char      StateCode[10];//原始状态码
	char      ErrCode[10];//错误码
	char      Submit_Time[11];//提交时间
	char      Done_Time[11];//下发时间
	DWORD     state;//分析后的状态码
	char	  Text[21]; 
	UC		  MSG_SRC_ID[30];
	UC        MOFlag;
	UC        SMType;
	UC        ReportType;
	UC        SubType;
	//SMGW40-11, 2005-3-7, jdz, modi begin//
	char      SndActName[21];	//状态报告消息的发送帐号
	char      RcvActName[21];	//状态报告消息的接收帐号
	//SMGW40-11, 2005-3-7, jdz, modi end//
};

typedef struct smpp_Submit_Sm_Resp
{
	char Message_id[MESSAGE_ID_LEN];
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	UL ext_data_len;
	char ext_data[];
	//SMGW42-55, 2005-7-12, jdz, modi end//
}Submit_Sm_Resp;

typedef struct smpp_Query_Sm
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC originating_ton;
	UC originating_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
}Query_Sm;

typedef struct smpp_Query_Sm_Resp
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	char final_date[FINAL_DATE_LEN];
	UC message_status;
	UC GSM_code;
}Query_Sm_Resp;

typedef struct smpp_Cancel_Sm
{
	char service_type[SERVICE_TYPE_LEN];
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC source_addr_ton;
	UC source_addr_npi;
	char source_addr[SOURCE_ADDR_LEN];
	UC dest_addr_ton;
	UC dest_addr_npi;
	char destination_addr[DESTINATION_ADDR_LEN];
}Cancel_Sm;

typedef struct smpp_Replace_Sm
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC orig_addr_ton;
	UC orig_addr_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
}Replace_Sm;

typedef struct smpp_Replace_Sm_ex
{
	char original_message_id[ORIGINAL_MESSAGE_ID_LEN];
	UC orig_addr_ton;
	UC orig_addr_npi;
	char originating_addr[ORIGINATING_ADDR_LEN];
    UC esm_class;  
	char schedule_delivery_time[SCHEDULE_DELIVERY_TIME_LEN];
	char validity_period[VALIDITY_PERIOD_LEN];
	UC registered_delivery_flag;
	UC data_coding;   
	UC sm_default_msg_id;
	UC sm_length;
	char short_message[SHORT_MESSAGE_LEN];
}Replace_Sm_Ex;

typedef struct smpp_Query_Sms
{
	UC	direction;		
	UC	ton; 
	UC	npi; 
	char	addr[MAX_ADDR_LEN];	
	char	start_time[MAX_TIMESTAMP_LEN]; 
	char	final_time[MAX_TIMESTAMP_LEN]; 
	UC	number;	
}Query_Sms;

typedef struct smpp_Query_Sms_Resp
{
	UC	direction;	
	UC	number; 
    
    char	sm_ID[MAX_SM_NUM][MESSAGE_ID_LEN];
}Query_Sms_Resp;

typedef struct	smpp_Query_Sm_Details
{
	char	sm_ID[MESSAGE_ID_LEN];
	UC	originating_ton; 
	UC	originating_npi; 
	char	originating_addr[ORIGINATING_ADDR_LEN];	
	UC	sm_length;
}	Query_Sm_Details;

typedef struct	smpp_Query_Sm_Details_Resp
{
	char	service_type[SERVICE_TYPE_LEN];
	UC	originating_ton;
	UC	originating_npi;
	char	originating_addr[MAX_ADDR_LEN];
	UC	dest_ton;
	UC	dest_npi;
	char	dest_addr[MAX_ADDR_LEN];
	UC	priority_flag;
    UC  RD;         
    UC  RP;         
    UC  UDHI;
    UC  registered_delivery_flag;
    UC  MR;
	UC	protocol_ID;
	UC	data_coding;
	char	schedule_delivery_time[SMPP_TIME_LEN];
	char	validity_period[SMPP_TIME_LEN];
	char	final_date[SMPP_TIME_LEN];
	char	message_id[MESSAGE_ID_LEN];
	UC	message_status;
	UC	GSM_code;
	UC	message_length;
	char	short_message[SHORT_MESSAGE_LEN];
}Query_Sm_Details_Resp;



typedef struct smpp_Add_Sub
{
	
	char	sub_ID[MAX_ADDR_LEN];	
	char	sub_name[MAX_ADDR_LEN];			
	char	sub_address[MAX_SUB_ADDRESS];		
	UC	MS_type;					
	UC	sub_ton;				
	UC	sub_npi;				
	long	service_level;			
	UC	SM_allowed;				
	UC	OCOS;					
	UC	TCOS;					
	char	password[MAX_PASSWORD];	
	
} Add_Sub;

typedef struct smpp_Del_Sub
{
	UC sub_ton;						
	UC sub_npi;						
	char sub_ID[MAX_ADDR_LEN];				
}Del_Sub;

typedef struct smpp_Mod_Sub
{
	
	char	sub_ID[MAX_ADDR_LEN];	
	char	sub_name[MAX_ADDR_LEN];				
	char	sub_address[MAX_SUB_ADDRESS];			
	UC	MS_type;				
	UC	sub_ton;				
	UC	sub_npi;					
	long	service_level;			
	UC	SM_allowed;				
	UC	OCOS;					
	UC	TCOS;					
	char	password[MAX_PASSWORD];	
	
}Mod_Sub;

typedef struct smpp_Enquire_Sub
{
	UC sub_ton;	
	UC sub_npi;	
	char sub_ID[MAX_ADDR_LEN];
}Enquire_Sub;

typedef struct smpp_Enquire_Sub_Resp
{
	
	char	sub_ID[MAX_ADDR_LEN];
	char	sub_name[MAX_ADDR_LEN];	 
	char	sub_address[MAX_SUB_ADDRESS];	
	UC	MS_type;
	UC	sub_ton;
	UC	sub_npi;	
	long	service_level;	
	UC	SM_allowed;	
	UC	OCOS;
	UC	TCOS;
	char	password[MAX_PASSWORD];	
	char	reg_datetime[REG_TIME_LEN];	
}Enquire_Sub_Resp;




#define ESME_PPS_USER_CHECK         0X01000001	
#define ESME_PPS_USER_CHECK_RESP    0X81000001	
#define ESME_PPS_RESULT_NOTIFY      0X01000002	
#define ESME_PPS_RESULT_NOTIFY_RESP	0X81000002


typedef struct smpp_Pps_UserCheck
{
    long ulVersion;	                        
    char sSMSCAddress[MAX_ADDR_LEN];	   
    char sSourceAddress[MAX_ADDR_LEN];	   
    char sDestinationAddress[MAX_ADDR_LEN];	
    UC   byMOMTFlag;	                  
    long   ulSmID;                           
    long  ulSmLength;                       
    long ulServiceID;                       
}Pps_User_Check;


typedef struct smpp_Pps_User_Check_Resp
{
    long  ulOperationResult;     
	
    UC byNotifyMode; 	         
	
	
	
}Pps_User_Check_Resp;


typedef struct smpp_Pps_Result_Notify
{
    long ulVersion;	                       
    char sSMSCAddress[MAX_ADDR_LEN];	    
    char sSourceAddress[MAX_ADDR_LEN];	    
    char sDestinationAddress[MAX_ADDR_LEN];	
    UC   byMOMTFlag;	                    
    long   ulSmID;                          
    long  ulSmLength;                       
    long  ulSendResult;                                        
    long ulServiceID;                        
}Pps_Result_Notify;

typedef struct smpp_Pps_Result_Notify_Resp
{
    long ulOperationResult;                 
}Pps_Result_Notify_Resp;


typedef struct tagFixMsgInfo
{
	char FeeMonth[7];
	char SPID[11];
	char ServiceID[11];
	char UserAddr[22];
	char FeeAddr[22];
	char Submittime[30];
}  FIXMSGINFO, *LPFIXMSGINFO;

#endif



