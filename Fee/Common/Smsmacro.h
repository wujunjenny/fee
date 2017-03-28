#ifndef __SMCMACRO_H__
#define __SMCMACRO_H__

typedef char* PTRMSG;

#define UC	unsigned char
#define US	unsigned short
#define UL	unsigned long

#define UDHI_TYPE_NO            0
#define UDHI_TYPE_SMPP          10
#define UDHI_TYPE_INNER         20
#define UDHI_TYPE_SMS           30


#define DIR_SOURCE				1			//����
#define DIR_DEST				2			//����

#define CKT_INVALID				0			//��Чֵ
#define CKT_USER				1			//�û���Ȩ
#define CKT_NUM					2			//�Ŷμ�Ȩ

/*
#define USERCHECK_YES			2			//��Ȩ
#define USERCHECK_NO			1			//����Ȩ
#define USERCHECK_INVALID		0			//��Чֵ
*/
//���ڶ�SMPPЭ�鵱�·�ʧ��ʱ�������ĵĻش���
#define DEFAULT_RESULT_FOR_DELIVER          0
#define USERCHECK_SOURCE        2           //�Է�����Ϣ��Ȩ
#define USERCHECK_DEST          1           //�Խ�����Ϣ��Ȩ
#define USERCHECK_NO            0           //����Ȩ

#define NUMCHECK_NOPASS			2			//û�д˺ŶΣ���ͨ��
#define NUMCHECK_PASS			1			//�д˺ŶΣ�ͨ��

#define MAX_ACCOUNTSECT_LEN         (20+1)  
#define MAX_CLIENTSECT_LEN          (20+1)
#define MAX_IF_NAME_LEN             (16+1)
#define MAX_ACCOUNTNAME_LEN			(10+1)
#define MAX_ACCOUNTPASSWORD_LEN		(8+1)

#define MAX_ACCOUNT_HAVE_IF_NUM		20
#define MAX_ACCESSCODE_NUM			200

#define MAX_IF_NUM				200
#define MAX_PID					255

#define MAX_INTERFACEADDR_LEN		(40+1)
#define MAX_CODEPARAM_LEN			(40+1)

#define AR_INTEFACEMANAGER		0X00000001
#define AR_USERMANAGER			0X00000002
#define AR_STATISTIC			0X00000004

#define AR_SUBMIT				0X00000010
#define AR_DELIVER				0X00000020

//#define AR_SOURCE				0X00000100
//#define AR_DEST				0X00000200
#define AR_NORIGHT              0X00000000
#define AR_CONTROLRIGHT		    0X00000001
#define ADMINISTRATOR_ID        "sa"

#define IFSTATEGY_FIRST			1
#define IFSTATEGY_ALTERNATE		2

enum ENUM_BIND{
	BIND_SUCCESS = 0 ,
		BIND_FAIL,    //עҪ��IF�Ƿ���
		BIND_INVALID_USER,  //��Ч�û�
		BIND_INVALID_PASSWORD,  //��Ч����
		BIND_USER_ALREADY_RIGISTERED,  //�û���ע��
		BIND_NEW_ERROR,
		BIND_INITIAL_FAIL,
		BIND_CONNECT_FAIL,
		BIND_NAME_ERROR,
		BIND_TYPE_ERROR,
		BIND_POCKET_ERROR,
		BIND_SEND_ERROR,
		BIND_TIMEOUT,
		BIND_ACK_ERROR,
		BIND_IF_REPEAT,
		BIND_OTHER_ERROR,   //����������������
		
};
enum ENUM_SUBMIT_ACK{
	SUBMIT_SUCCESS=0,
		SUBMIT_OVER_MO,   //�û��ύ��Ŀ��������ύ��
		SUBMIT_OVER_MT,	  //�û��·���Ŀ��������·���
		SUBMIT_INVALID_USER,  //�û���Ȩ���û�������,Ҳ���������������
		SUBMIT_INVALID_DATA_FORMAT,  //���ݸ�ʽ����
		SUBMIT_ADDR_ERROR,   //�û���ַ����
		SUBMIT_CREATE_MESSAGE_ERROR,  //������Ϣ����
		SUBMIT_OTHER_ERROR,  //��������������
		
};

enum E_ERROR_MSG
{
	E_SUCCESS = 0,
		
		//interfacemanager error code
		//E_BEYONDMAXACCESSNUM,
		//E_ACCESSHANDLEERR,
		E_ACCESSERR,			//���������
		E_CALLERERR,			//���д���			*
		E_ACCOUNTERR,			//�ʺ�ָ�벻��		*
		E_SMTYPEERR,			//SmType����		*
		E_NEWACCOUNTFAIL,		//�����ʺŶ���ʧ��		*
		E_ACCOUNTNAMEERR,		//�ʺ��������޴��ʺ�	*
		E_CODETYPEERR,			//�������ʹ�
		E_CREATEIFFAIL,			//����IF����ʧ��
		E_BEYONDMAXIFNUM,		//����INTERFACEMANAGER�����IF��
		E_INVALIDIFID,			//��Ч��IF��ID
		E_ACCOUNTPASSWORDERR,	//�ʺŵ��������
		E_NORIGHT,				//û��Ȩ��
		E_PIDERR,				//PID����
		E_INVALIDROUTETABLE,	//��Ч��ROUTETABLE	*
		
		E_CANNOTFINDIF,			//ͨ��HDRV�����ҵ�IF *
		E_PARAMERR,				//������
		E_STATUSERR,			//״̬����
		
		//routetable
		E_INVALIDACCOUNT,		//��Ч��ACCOUNT		*
		
		//account error code
		//E_BEYONDACCOUNTMAXIFNUM,
		E_INVALIDIF,
		//E_ROUTEERR,
		
		E_NOSUBMITRIGHT,
		E_NODELIVERRIGHT,
		E_CHECKTYPEERR,			//��Ȩ���ʹ���
		E_NUMREPEAT,			//�Ŷ��ظ�
		E_NOTHISNUM,			//û��ָ���ĺŶ�
		E_WATIACK,              //�ȴ�Ӧ�������Ϣ����
		E_DCS_ERROR,            //����ʱDCS�д���
		E_SUBMIT_TIME_OUT,      //Ӧ��ʱ
		E_SUBMIT_ERROR,         //�����Ϣ�����ύ��Ϣ����
		E_PARSER,               //�û�������Ϣ��������
		E_SERVICE_NOT_OPEN,     //ָ������δ����
		E_IF_STYLE,             //�ӿ����ʹ���
		E_ADDR_FOMRAT,          //��ַ��ʽ����
		E_PARAM_NO,             //����Ĳ�������
		E_BIND_SERVICEERR,      //  ����ʱ�������ʹ���
		E_BUFFERERR,            //��������
		E_ADD_TRACEERR,         //���Ӹ���ʧ��
		E_CANCEL_TRACEERR,      //��ֹ����ʧ��
		E_ADD_STATERR,          //����ͳ������ʧ��
		E_STAT_TYPEERR,         //ͳ�����ʹ���
		E_STAT_NUMERR,          //ͳ�Ƽ�¼����
		E_STAT_IDERR,           //ͳ��ID����
};

#define DIR_SOURCE				1			//����
#define DIR_DEST				2			//����

#define CKT_INVALID				0			//��Чֵ
#define CKT_USER				1			//�û���Ȩ
#define CKT_NUM					2			//�Ŷμ�Ȩ

#define NUMCHECK_NOPASS			2			//û�д˺ŶΣ���ͨ��
#define NUMCHECK_PASS			1			//�д˺ŶΣ�ͨ��

#define MAX_ACCOUNTNAME_LEN			(10+1)
#define MAX_ACCOUNTPASSWORD_LEN		(8+1)

#define MAX_ACCOUNT_HAVE_IF_NUM		20
#define MAX_ACCESSCODE_NUM			200

#define MAX_IF_NUM				200
#define MAX_PID					255

#define MAX_INTERFACEADDR_LEN		(40+1)
#define MAX_CODEPARAM_LEN			(40+1)


#define MAX_QUERY_SMS_COUNT	100
#define SMS_QUERY_DIRECTION_ORG	0
#define SMS_QUERY_DIRECTION_DES	1
#define SMS_ACK_OK	0
#define SMS_ACK_ERROR	1

#define SHAKE_HAND_ERROR -188

//ͳ�Ƶ������ַ�������
#define MAX_STAT_VALUE_LEN  32
//ͳ��������Ŀ�����Ƶĳ���
#define MAX_MOBILE_HEAD_LEN 8
#define MAX_STAT_ITEM_LEN   32
#define MAX_COUNTRYCODE_LEN 8
#define MAX_ADDR_LEN (20+1)
#define MAX_TIMESTAMP_LEN	(19+1)
#define MAX_SUB_ADDRESS (41+1)
//SMGW42-55, 2005-7-12, jdz, modi begin//
#define MAX_SM_LEN	(254+1)
//SMGW42-55, 2005-7-12, jdz, modi end//
#define MAX_UD_LEN  (140+1)
#define MAX_LONG_UD_LEN (40*1024)
#define MAX_LONG_SM_LEN	(40*1024)
#define UDH_LENGTH	6
#define MAX_CD_LEN	(156+1)
#define MAX_PASSWORD (9+1)
#define MAX_SM_INTERFACE_NAME_LEN_1 21
#define MAX_PARAM_NAME_LEN	30
#define MAX_MONITOR_PARAM_LEN 30
#define MAX_VALUE_LEN	1024
#define MAX_SERVICE	10
#define MAX_SM_NUM 100
#define MAX_IF_EXT_DATA 100
#define MAX_ACTIVE_TIME_LEN (8 + 1)

#define SMPP_ACK_OK		0	
#define SMPP_ACK_ERROR	1


#define IF_FLOW_CONTROL	3	// ����
#define IF_TEMP_ERROR	2	// ��ʱ����
#define IF_INVALID		1	// interface �Ƿ�
#define IF_OK			0	// ����

#define SM_STATE_FLOWCONTROL	0XFF	// ����Ϣ��������״̬

/////////////////////////////////////////////////////////////
//	ɾ����־
#define DELSM_CAUSE_DEFORMITY		5	// ����Ϣ�����������ֶ���Ϣ����Ҫ����
#define DELSM_CAUSE_USER			4	// �û�����ɾ��
#define DELSM_CAUSE_COUNT_FLOW		3	// ���ͳ����涨����
#define DELSM_CAUSE_TIMEOUT			2	// �����Ϣ��ʱ
#define DELSM_CAUSE_FATAL_ERROR		1	// ���ô���
#define DELSM_DELIVER_OK			0	// �������


/*---------------------------------------------------------
SMIMsg.h�ṹ�������		
------------------------------------------------------------*/



#define IS_SMEI_ACK(ncommand) (ncommand&0x8000)
#define IS_IF_INNER_MSG(ncmd) (ncmd>=300)&&(ncmd<=399)
#define IS_TASKPOOL_MSG(ncmd) (ncmd>=400)&&(ncmd<=499)
#define IS_SMS_ACK(ncommand)  (ncommand&0x8000)


/////////////////////////////////////////////////////
//	��Ϣ���Ͷ���
#define SMEI_SUBMIT_SM	0
#define SMEI_SUBMIT_SM_ACK	(SMEI_SUBMIT_SM|0x8000)
#define SMEI_SUBMIT_COMMAND	1
#define SMEI_DELIVER_SM	2
#define SMEI_DELIVER_SM_ACK	(SMEI_DELIVER_SM|0x8000)
#define SMEI_STATUS_REPORT	3
#define SMEI_ALERT_SC	4

// ��ѯ��Ϣ
#define SMEI_QUERY_SM_STATUS	6
#define SMEI_QUERY_SM_STATUS_ACK	(SMEI_QUERY_SM_STATUS|0x8000)
#define SMEI_CANCEL_SM	13
#define SMEI_CANCEL_SM_ACK	(SMEI_CANCEL_SM|0x8000)
#define SMEI_REPLACE_SM	15
#define SMEI_REPLACE_SM_ACK	(SMEI_REPLACE_SM|0x8000)
#define SMEI_QUERY_MSGS	17
#define SMEI_QUERY_MSGS_ACK	(SMEI_QUERY_MSGS|0x8000)
#define SMEI_QUERY_MSG_DETAILS	19
#define SMEI_QUERY_MSG_DETAILS_ACK	(SMEI_QUERY_MSG_DETAILS|0x8000)




// ����
#define SMEI_ADD_SUB	21
#define SMEI_ADD_SUB_ACK	(SMEI_ADD_SUB|0x8000)
#define SMEI_DEL_SUB	23
#define SMEI_DEL_SUB_ACK	(SMEI_DEL_SUB|0x8000)
#define SMEI_MOD_SUB	25
#define SMEI_MOD_SUB_ACK	(SMEI_MOD_SUB|0x8000)
#define SMEI_ENQUIRE_SUB	27
#define SMEI_ENQUIRE_SUB_ACK	(SMEI_ENQUIRE_SUB|0x8000)




//���ݹ���
#define SMEI_ADD_INTERFACE	300
#define SMEI_ADD_INTERFACE_ACK	SMEI_ADD_INTERFACE|0x8000
#define SMEI_DEL_INTERFACE	301
#define SMEI_DEL_INTERFACE_ACK	SMEI_DEL_INTERFACE|0x8000
#define SMEI_MOD_INTERFACE	302
#define SMEI_MOD_INTERFACE_ACK	SMEI_MOD_INTERFACE|0x8000
#define SMEI_ADD_SERVICE	303
#define SMEI_ADD_SERVICE_ACK	SMEI_ADD_SERVICE|0x8000
#define SMEI_DEL_SERVICE	304
#define SMEI_DEL_SERVICE_ACK	SMEI_DEL_SERVICE|0x8000

#define SMEI_QUERY_INTERFACE	305
#define SMEI_QUERY_INTERFACE_ACK	SMEI_QUERY_INTERFACE|0x8000
#define SMEI_QUERY_INTERFACES	306
#define SMEI_QUERY_INTERFACES_ACK	SMEI_QUERY_INTERFACES|0x8000
//add msg for IFManager
#define SMEI_IF_ERROR		307


#define SMEI_BIND_TRANSMITTER		308
#define SMEI_BIND_TRANSMITTER_ACK	(SMEI_BIND_TRANSMITTER|0x8000)
#define SMEI_BIND_RECEIVER			309
#define SMEI_BIND_RECEIVER_ACK		(SMEI_BIND_RECEIVER|0x8000)
#define SMEI_UNBIND		310
#define SMEI_UNBIND_ACK	SMEI_UNBIND|0x8000

#define SMEI_SET_IF_EXTINFO			322
#define SMEI_SET_IF_EXTINFO_ACK		SMEI_SET_IF_EXTINFO|0x8000
#define SMEI_GET_IF_EXTINFO			323
#define SMEI_GET_IF_EXTINFO_ACK		SMEI_SET_IF_EXTINFO|0x8000


#define SMEI_ENQUIRE_LINK 			324
#define SMEI_ENQUIRE_LINK_ACK	    (SMEI_ENQUIRE_LINK|0x8000)
#define SMEI_NACK					325

//#define SMEI_BIND		308
//#define SMEI_UNBIND		309
//#define SMEI_UNBIND_ACK	SMEI_UNBIND|0x8000
//#define SMEI_BIND_ACK	SMEI_BIND|0x8000




// ά�����
#define SMEI_SET_PARAM	400
#define SMEI_SET_PARAM_ACK	SMEI_SET_PARAM|0x8000
#define SMEI_QUERY_PARAM	401
#define SMEI_QUERY_PARAM_ACK	SMEI_QUERY_PARAM|0x8000
#define SMEI_MONITOR	402
#define SMEI_MONITOR_ACK	SMEI_MONITOR|0x8000
#define SMEI_MONITOR_INFO	403
#define SMEI_CANCEL_MONITOR	404
#define SMEI_CANCEL_MONITOR_ACK	SMEI_CANCEL_MONITOR|0x8000
#define SMEI_TRACE	405
#define SMEI_TRACE_ACK	SMEI_TRACE|0x8000
#define SMEI_TRACE_INFO	406
#define SMEI_CANCEL_TRACE	407
#define SMEI_CANCEL_TRACE_ACK	SMEI_CANCEL_TRACE|0x8000
#define SMEI_NEW_TASK	408
#define SMEI_NEW_TASK_ACK	SMEI_NEW_TASK|0x8000
#define SMEI_DEL_TASK	409
#define SMEI_DEL_TASK_ACK	SMEI_DEL_TASK|0x8000
#define SMEI_QUERY_TASK	410
#define SMEI_QUERY_TASK_ACK	SMEI_QUERY_TASK|0x8000
#define SMEI_GET_TASK_RECORD	411
#define SMEI_TASK_RECORD	412
#define SMEI_QUERY_TASKS	413
#define SMEI_QUERY_TASKS_ACK	SMEI_QUERY_TASKS|0x8000

//add by zq for gateway trace
#define SMEI_GATEWAY_MONITOR			414	//taskpool ------->Gateway
#define SMEI_GATEWAY_MONITOR_ACK		415 //Gateway----------->TaskPool //SMEI_GATEWAY_MONITOR|0x8000
#define SMEI_MONITORGATEWAY				416	//MT ----------->TaskPool
#define SMEI_MONITORGATEWAY_ACK			SMEI_MONITORGATEWAY|0x8000  //TaskPool------->MT
#define SMEI_CANCEL_MONITORGATEWAY		417  //MT ----------->TaskPool
#define SMEI_CANCEL_MONITORGATEWAY_ACK	SMEI_CANCEL_MONITORGATEWAY|0x8000  //TaskPool------->MT
#define SMEI_MONITORGATEWAY_INFO		418

#define SMEI_SET_ERRTIME				419
#define SMEI_SET_ERRTIME_ACK			SMEI_SET_ERRTIME|0x8000
#define SMEI_QUERY_ERRTIME				420
#define SMEI_QUERY_ERRTIME_ACK			SMEI_QUERY_ERRTIME|0x8000


//end add



//CODE <==> SMSMAP ��Ϣ����
#define SMEI_SMSMAP_MTDATA	MAKEWORD(204,0)
#define SMEI_SMSMAP_MTACK	MAKEWORD(204,1)
#define SMEI_SMSMAP_MTERROR	MAKEWORD(204,2)
#define SMEI_SMSMAP_MODATA	MAKEWORD(204,3)
#define SMEI_SMSMAP_MOACK	MAKEWORD(204,4)
#define SMEI_SMSMAP_MOERROR	MAKEWORD(204,5)
#define SMEI_SMSMAP_ALERTSC	MAKEWORD(204,6)
#define SMEI_SMSMAP_GATEWAY_MONITOR MAKEWORD(204,13)
#define SMEI_SMSMAP_GATEWAY_MONITOR_ACK MAKEWORD(204,14)

// ʱ����Ϣ
#define SMEI_TIME_TRIGGER	100
#define SMEI_DUMP_SM		101

enum ENUM_ADD_USER_STATUS
{
	USER_ALREADY_EXIST = 0,
		ADD_USER_SUCESS,
		ADD_USER_FAIL,
		ADD_USER_OTHER_ERROR,
};
enum ENUM_DEL_USER_STATUS
{
	USER_NOT_EXIST=0,
		USER_DEL_SUCCESS,
		USER_DEL_FAIL,
		DEL_USER_OTHER_ERROR,
};

#endif
