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


#define DIR_SOURCE				1			//主叫
#define DIR_DEST				2			//被叫

#define CKT_INVALID				0			//无效值
#define CKT_USER				1			//用户鉴权
#define CKT_NUM					2			//号段鉴权

/*
#define USERCHECK_YES			2			//鉴权
#define USERCHECK_NO			1			//不鉴权
#define USERCHECK_INVALID		0			//无效值
*/
//用于对SMPP协议当下发失败时调度中心的回答结果
#define DEFAULT_RESULT_FOR_DELIVER          0
#define USERCHECK_SOURCE        2           //对发送消息鉴权
#define USERCHECK_DEST          1           //对接收消息鉴权
#define USERCHECK_NO            0           //不鉴权

#define NUMCHECK_NOPASS			2			//没有此号段，不通过
#define NUMCHECK_PASS			1			//有此号段，通过

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
		BIND_FAIL,    //注要是IF非法等
		BIND_INVALID_USER,  //无效用户
		BIND_INVALID_PASSWORD,  //无效密码
		BIND_USER_ALREADY_RIGISTERED,  //用户已注册
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
		BIND_OTHER_ERROR,   //连接其它错误类型
		
};
enum ENUM_SUBMIT_ACK{
	SUBMIT_SUCCESS=0,
		SUBMIT_OVER_MO,   //用户提交数目大于最大提交数
		SUBMIT_OVER_MT,	  //用户下发数目大于最大下发数
		SUBMIT_INVALID_USER,  //用户无权或用户不存在,也包括上面两种情况
		SUBMIT_INVALID_DATA_FORMAT,  //数据格式错误
		SUBMIT_ADDR_ERROR,   //用户地址错误
		SUBMIT_CREATE_MESSAGE_ERROR,  //建立消息错误
		SUBMIT_OTHER_ERROR,  //其其它错误类型
		
};

enum E_ERROR_MSG
{
	E_SUCCESS = 0,
		
		//interfacemanager error code
		//E_BEYONDMAXACCESSNUM,
		//E_ACCESSHANDLEERR,
		E_ACCESSERR,			//接入码错误
		E_CALLERERR,			//主叫错误			*
		E_ACCOUNTERR,			//帐号指针不符		*
		E_SMTYPEERR,			//SmType错误		*
		E_NEWACCOUNTFAIL,		//生成帐号对象失败		*
		E_ACCOUNTNAMEERR,		//帐号名错，查无此帐号	*
		E_CODETYPEERR,			//编码类型错
		E_CREATEIFFAIL,			//生成IF对象失败
		E_BEYONDMAXIFNUM,		//超过INTERFACEMANAGER的最大IF数
		E_INVALIDIFID,			//无效的IF的ID
		E_ACCOUNTPASSWORDERR,	//帐号的密码错误
		E_NORIGHT,				//没有权限
		E_PIDERR,				//PID错误
		E_INVALIDROUTETABLE,	//无效的ROUTETABLE	*
		
		E_CANNOTFINDIF,			//通过HDRV不能找到IF *
		E_PARAMERR,				//参数错
		E_STATUSERR,			//状态错误
		
		//routetable
		E_INVALIDACCOUNT,		//无效的ACCOUNT		*
		
		//account error code
		//E_BEYONDACCOUNTMAXIFNUM,
		E_INVALIDIF,
		//E_ROUTEERR,
		
		E_NOSUBMITRIGHT,
		E_NODELIVERRIGHT,
		E_CHECKTYPEERR,			//鉴权类型错误
		E_NUMREPEAT,			//号段重复
		E_NOTHISNUM,			//没有指定的号段
		E_WATIACK,              //等待应答队列消息错误
		E_DCS_ERROR,            //解码时DCS有错误
		E_SUBMIT_TIME_OUT,      //应答超时
		E_SUBMIT_ERROR,         //向短消息中心提交消息错误
		E_PARSER,               //用户输入消息解析错误
		E_SERVICE_NOT_OPEN,     //指定服务未启动
		E_IF_STYLE,             //接口类型错误
		E_ADDR_FOMRAT,          //地址格式错误
		E_PARAM_NO,             //错误的参数类型
		E_BIND_SERVICEERR,      //  连接时服务类型错误
		E_BUFFERERR,            //缓区错误
		E_ADD_TRACEERR,         //增加跟踪失败
		E_CANCEL_TRACEERR,      //终止跟踪失败
		E_ADD_STATERR,          //增加统计任务失败
		E_STAT_TYPEERR,         //统计类型错误
		E_STAT_NUMERR,          //统计记录错误
		E_STAT_IDERR,           //统计ID错误
};

#define DIR_SOURCE				1			//主叫
#define DIR_DEST				2			//被叫

#define CKT_INVALID				0			//无效值
#define CKT_USER				1			//用户鉴权
#define CKT_NUM					2			//号段鉴权

#define NUMCHECK_NOPASS			2			//没有此号段，不通过
#define NUMCHECK_PASS			1			//有此号段，通过

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

//统计的条件字符串长度
#define MAX_STAT_VALUE_LEN  32
//统计任务项目的名称的长度
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


#define IF_FLOW_CONTROL	3	// 流控
#define IF_TEMP_ERROR	2	// 临时错误
#define IF_INVALID		1	// interface 非法
#define IF_OK			0	// 正常

#define SM_STATE_FLOWCONTROL	0XFF	// 短消息处于流控状态

/////////////////////////////////////////////////////////////
//	删除标志
#define DELSM_CAUSE_DEFORMITY		5	// 短消息不完整，这种短消息不需要保存
#define DELSM_CAUSE_USER			4	// 用户主动删除
#define DELSM_CAUSE_COUNT_FLOW		3	// 发送超过规定次数
#define DELSM_CAUSE_TIMEOUT			2	// 因短消息超时
#define DELSM_CAUSE_FATAL_ERROR		1	// 永久错误
#define DELSM_DELIVER_OK			0	// 发送完毕


/*---------------------------------------------------------
SMIMsg.h结构定义结束		
------------------------------------------------------------*/



#define IS_SMEI_ACK(ncommand) (ncommand&0x8000)
#define IS_IF_INNER_MSG(ncmd) (ncmd>=300)&&(ncmd<=399)
#define IS_TASKPOOL_MSG(ncmd) (ncmd>=400)&&(ncmd<=499)
#define IS_SMS_ACK(ncommand)  (ncommand&0x8000)


/////////////////////////////////////////////////////
//	消息类型定义
#define SMEI_SUBMIT_SM	0
#define SMEI_SUBMIT_SM_ACK	(SMEI_SUBMIT_SM|0x8000)
#define SMEI_SUBMIT_COMMAND	1
#define SMEI_DELIVER_SM	2
#define SMEI_DELIVER_SM_ACK	(SMEI_DELIVER_SM|0x8000)
#define SMEI_STATUS_REPORT	3
#define SMEI_ALERT_SC	4

// 查询消息
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




// 受理
#define SMEI_ADD_SUB	21
#define SMEI_ADD_SUB_ACK	(SMEI_ADD_SUB|0x8000)
#define SMEI_DEL_SUB	23
#define SMEI_DEL_SUB_ACK	(SMEI_DEL_SUB|0x8000)
#define SMEI_MOD_SUB	25
#define SMEI_MOD_SUB_ACK	(SMEI_MOD_SUB|0x8000)
#define SMEI_ENQUIRE_SUB	27
#define SMEI_ENQUIRE_SUB_ACK	(SMEI_ENQUIRE_SUB|0x8000)




//数据管理
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




// 维测管理
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



//CODE <==> SMSMAP 消息定义
#define SMEI_SMSMAP_MTDATA	MAKEWORD(204,0)
#define SMEI_SMSMAP_MTACK	MAKEWORD(204,1)
#define SMEI_SMSMAP_MTERROR	MAKEWORD(204,2)
#define SMEI_SMSMAP_MODATA	MAKEWORD(204,3)
#define SMEI_SMSMAP_MOACK	MAKEWORD(204,4)
#define SMEI_SMSMAP_MOERROR	MAKEWORD(204,5)
#define SMEI_SMSMAP_ALERTSC	MAKEWORD(204,6)
#define SMEI_SMSMAP_GATEWAY_MONITOR MAKEWORD(204,13)
#define SMEI_SMSMAP_GATEWAY_MONITOR_ACK MAKEWORD(204,14)

// 时钟消息
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
