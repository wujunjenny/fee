#ifndef __CMDDEF__ 
#define __CMDDEF__ 

enum varServiceType
{
    SERVICE_NO = 0,
		SERVICE_STOCK = 1,
		SERVICE_NEWS = 2,
		SERVICE_FLIGHT = 4,
		SERVICE_QUERY = 5,
		SERVICE_REVERSE1 = 6,
		SERVICE_WH = 7,
		SERVICE_SMS = 8,
		SERVICE_CHARGE_QUERY = 10,
		SERVICE_EMAIL_GATE = 11,
		SERVICE_EMAIL_SEND = 12,
		SERVICE_WEB = 15,
		SERVICE_TRAIN = 16,
		SERVICE_SHIP = 17,
		SERVICE_DICT = 18,
		SERVICE_OPR = 120,
		SERVICE_MT = 121,
		SERVICE_SMC_TRANS =127,
		SERVICE_SMC_RECV =128,
		
		SERVICE_NO_BLANK = 9999,
};

//普通消息
#define IS_COMMON_MSG(nCommand) (nCommand<100)&&(nCommand>=0)
#define IS_COMMAND_ACK(nCommand) (nCommand & 0x8000)
#define IS_SMPP_ACK(nCommand) (nCommand & 0x80000000)

//股票消息
#define IS_STOCK_MSG(nCommand)  ( (nCommand &= 0x7fff) < (SMS_ST_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_ST_BASE)
#define IS_STOCK_MSG_SEND(nCommand)  \
( (nCommand &= 0x7fff) < (SMS_ST_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_ST_BASE)

//航班消息
#define IS_FLIGHT_MSG(nCommand) ( (nCommand &= 0x7fff) < (SMS_FLIGHT_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_FLIGHT_BASE)
#define IS_FLIGHT_MSG_SEND(nCommand) \
( (nCommand &= 0x7fff) < (SMS_FLIGHT_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_FLIGHT_BASE)

//新闻消息
#define IS_NEWS_MSG(nCommand)   ( (nCommand &= 0x7fff) < (SMS_NEWS_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_NEWS_BASE)
#define IS_NEWS_MSG_SEND(nCommand) \
( (nCommand &= 0x7fff) < (SMS_NEWS_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_NEWS_BASE)

//普通外部接口消息
#define IS_COMM_MSG(nCommand)   ( (nCommand &= 0x7fff) < 500) && ((nCommand &= 0x7fff) >= 400)

//配置消息
#define IS_CONFIG_MSG(nCommand)	 ( (nCommand &= 0x7fff) < 1000) && ((nCommand &= 0x7fff) >= 999)


//命令执行状态
#define CMD_STATUS_SUCCESS	0
#define CMD_STATUS_FAIL		1

#define USER_CHECK_SUCCESS  0
#define USER_CHECK_FAIL     1


//新闻命令执行状态
#define NEWS_SUCCEED        0
#define NEWS_FAIL           0x60


//航班中当天是否有航班
//没有航班
#define FLIGHT_DAY_NOT_SET  0
//有航班
#define FLIGHT_DAY_SET      1



//错误类型的分类
#define IS_COMMON_ERR(Err)  (Err>=ERR_COMMON_BASE && Err<= ERR_COMMON_MAX)
#define IS_SMCIF_ERR(Err)   (Err>=ERR_SMCIF_BASE && Err<= ERR_SMCIF_MAX)
#define IS_STOCK_ERR(Err)  (Err>=ERR_STOCK_BASE && Err<= ERR_STOCK_MAX)
#define IS_FLIGHT_ERR(Err)  (Err>=ERR_FLIGHT_BASE && Err<= ERR_FLIGHT_MAX)
#define IS_NEWS_ERR(Err)    (Err>=ERR_NEWS_BASE && Err<= ERR_NEWS_MAX)

//定义错误类型
#define ERR_TYPE_IF			0
//普通错误码定义
#define ERR_COMMON_BASE		0
#define ERR_NO_ERR			(ERR_COMMON_BASE+0)		//没有错误
#define ERR_BIND_REPEAT		(ERR_COMMON_BASE+1)		//重复登录
#define ERR_IF_NOT_EXIST	(ERR_COMMON_BASE+2)		//接口不存在
#define ERR_IF_TCP			(ERR_COMMON_BASE+3)		//网络错
#define ERR_COMMON_MAX		19
//smc接口错误代码
#define ERR_SMCIF_BASE		20
#define ERR_SUBMITSM		(ERR_SMCIF_BASE+0)		//提交短消息失败
#define ERR_PARSER			(ERR_SMCIF_BASE+1)		//解释器失败
#define ERR_SMCIF_MAX		39
// 股票错误代码
#define ERR_STOCK_BASE		40
#define ERR_STOCK_CODE_NUMBER   (ERR_COMMON_BASE+0)     //股票代码错误
#define ERR_STOCK_CODE_LENGTH   (ERR_STOCK_BASE+1)		//股票代码长度错误
#define ERR_STOCK_IS_TP             (ERR_STOCK_BASE+2)      //股票正停牌
#define ERR_STOCK_NOTITY_UNREASONABLE (ERR_STOCK_BASE+3)    //告警设置不合理
#define ERR_STOCK_NO_NOTIFY			(ERR_STOCK_BASE+4)      //无告警设置
#define ERR_STOCK_NOT_EXIST		(ERR_STOCK_BASE+5)	    //股票不存在
#define ERR_NO_THIS_ALERT		(ERR_STOCK_BASE+6)      //没有此个告警设置
#define ERR_EXCEED_MAX_NOTIFY_COUNT (ERR_STOCK_BASE+7)  //设置告警超过最大数
#define ERR_NO_NOTIFY_SET		(ERR_STOCK_BASE+8)      //没有设置告警
#define ERR_NO_CODE_NOTIFY_SET  (ERR_STOCK_BASE+9)      //没有设置查询股票的警告
#define ERR_STOCK_MAX		59

//航班错误代码
#define ERR_FLIGHT_BASE		60
#define ERR_FLIGHT_NOT_EXIT	(ERR_FLIGHT_BASE+0)     //航班不存在
#define ERR_FLIGHT_DB_ERROR	(ERR_FLIGHT_BASE+1)     //航班数据库错误
#define ERR_FLIGHT_MAX		79

//新闻错误代码
#define ERR_NEWS_BASE		90
#define ERR_NEWS_DB_ERROR	(ERR_NEWS_BASE+0)       //新闻数据库错误
#define ERR_NEWS_SET_ERROR  (ERR_NEWS_BASE+1)       //设置新闻服务失败
#define ERR_NEWS_NEWSNO_ERROR (ERR_NEWS_BASE+2)     //新闻的编码错误
#define ERR_NEWS_CANCEL_ERROR (ERR_NEWS_BASE+3)     //终止新闻服务失败
#define ERR_NEWS_CANCEL_ALL_ERROR (ERR_NEWS_BASE+4)  //终止所有新闻服务失败
#define ERR_NEWS_QUERY_ERROR    (ERR_NEWS_BASE+5)   //查询新闻服务失败
#define ERR_NEWS_QUERY_ALL_ERROR (ERR_NEWS_BASE+6)  //查询所有新闻服务失败
#define ERR_NEWS_QUERY_NOT_SET_ERROR (ERR_NEWS_BASE+7)  //没有设置新闻服务
#define ERR_NEWS_MAX		120

//航班错误代码
#define ERR_TRAFIC_BASE		120
#define ERR_TRAINSHIP_DB_ERROR	(ERR_TRAFIC_BASE+0)     //航班不存在
#define ERR_TRAINSHIP_NOT_EXIT	(ERR_TRAFIC_BASE+1)     //航班数据库错误
#define ERR_TRAFIC_MAX		128

//新闻的种类
#define MAX_NEWS_TYPE       32
//天气预报中，地名个数的最大值
#define MAX_WT_ADDRESS      128

//查询标题时，子ID没有时的填充
#define NEWS_NO_SUB_ID      0XFF

//最小新闻编码
#define MINI_NEWS_NO    0  
//最大新闻编码
#define MAX_NEWS_NO     MAX_NEWS_TYPE
//最小新闻子编码
#define MINI_NEWS_SUB_NO 1
//天气预报的的消息序号
#define NEWS_WT_ID      1
#define STR_NEWS_WT_ID   "1"
//空的新闻ID
#define NEWS_NO_ID      -1

#define MAX_NEWS_QUERY_TYPE 100


/******************************************************************************************/
/*							通用类消息													  */	
/******************************************************************************************/

//客户端的注册及握手消息
#define SMS_CLIENT_BIND			1       //连接命令
#define SMS_CLIENT_BIND_ACK		(SMS_CLIENT_BIND|0x8000)

#define SMS_CLIENT_BIND_REC		0       //连接命令
#define SMS_CLIENT_BIND_REC_ACK	(SMS_CLIENT_BIND_REC|0x8000)

#define SMS_CLIENT_UNBIND		2       //断连命令
#define SMS_CLIENT_UNBIND_ACK	(SMS_CLIENT_UNBIND|0x8000)

#define SMS_SHAKEHAND			3       //握手信息
#define SMS_SHAKEHAND_ACK		(SMS_SHAKEHAND|0x8000)

//********有关路由帐号以及配置的相关信息******************
#define SMS_ADD_ACCOUNT         30      //增加帐号
#define SMS_ADD_ACCOUNT_ACK     (SMS_ADD_ACCOUNT|0x8000)

#define SMS_QUERY_ACCOUNT       31      //查询帐号
#define SMS_QUERY_ACCOUNT_ACK   (SMS_QUERY_ACCOUNT|0x8000)

#define SMS_MODI_ACCOUNT        32      //修改帐号
#define SMS_MODI_ACCOUNT_ACK    (SMS_MODI_ACCOUNT|0x8000)

#define SMS_QUERY_ALL_ACCOUNT       33      //查询所有帐号名
#define SMS_QUERY_ALL_ACCOUNT_ACK   (SMS_QUERY_ALL_ACCOUNT|0x8000)

#define SMS_DEL_ACCOUNT         34      //删除帐号
#define SMS_DEL_ACCOUNT_ACK     (SMS_DEL_ACCOUNT|0x8000)

#define SMS_QUERY_INTERFACE     35      //查询接口
#define SMS_QUERY_INTERFACE_ACK (SMS_QUERY_INTERFACE|0x8000)

#define SMS_DEL_INTERFACE       36      //删除接口
#define SMS_DEL_INTERFACE_ACK   (SMS_DEL_INTERFACE|0x8000)

#define SMS_QUERY_ALL_INTERFACE       37      //查询所有接口的ID
#define SMS_QUERY_ALL_INTERFACE_ACK   (SMS_QUERY_ALL_INTERFACE|0x8000)

#define SMS_ADD_INTERFACE      38      //增加客户端接口
#define SMS_ADD_INTERFACE_ACK  (SMS_ADD_INTERFACE|0x8000)

#define SMS_ADD_DISTABLE       39      //接加路由
#define SMS_ADD_DISTABLE_ACK   (SMS_ADD_DISTABLE|0x8000)

#define SMS_DEL_DISTABLE        40      //删除路由
#define SMS_DEL_DISTABLE_ACK    (SMS_DEL_DISTABLE|0x8000)

#define SMS_QUERY_DISTABLE      41      //查询路由
#define SMS_QUERY_DISTABLE_ACK  (SMS_QUERY_DISTABLE|0x8000)


#define SMS_SET_PARAM            42      //设置参数
#define SMS_SET_PARAM_ACK        (SMS_SET_PARAM|0x8000)

#define SMS_GET_PARAM            43      //设置参数
#define SMS_GET_PARAM_ACK        (SMS_GET_PARAM|0x8000)


#define SMS_QUERY_ACCOUNT_NUMCHECK  52      //查询帐号号段表
#define SMS_QUERY_ACCOUNT_NUMCHECK_ACK  (SMS_QUERY_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_ADD_ACCOUNT_NUMCHECK    53      //添加帐号号段表
#define SMS_ADD_ACCOUNT_NUMCHECK_ACK    (SMS_ADD_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_DEL_ACCOUNT_NUMCHECK    54      //删除帐号号段表
#define SMS_DEL_ACCOUNT_NUMCHECK_ACK    (SMS_DEL_ACCOUNT_NUMCHECK | 0X8000)


#define SMS_OMC_BASE                400
#define SMS_ADD_TRACE               (SMS_OMC_BASE + 0)
#define SMS_ADD_TRACE_ACK	        (SMS_ADD_TRACE|0x8000)
#define SMS_TRACE_INFO	            (SMS_OMC_BASE + 1)
#define SMS_CANCEL_TRACE	        (SMS_OMC_BASE + 2)
#define SMS_CANCEL_TRACE_ACK        (SMS_CANCEL_TRACE|0x8000)
//#define SMS_OMC_TASK_FAIL           (SMS_OMC_BASE + 3)
#define SMS_ADD_STAT                (SMS_OMC_BASE + 4)
#define SMS_ADD_STAT_ACK            (SMS_ADD_STAT|0x8000)
#define SMS_DEL_STAT                (SMS_OMC_BASE + 5)
#define SMS_DEL_STAT_ACK            (SMS_DEL_STAT|0x8000)
#define SMS_QUERY_STAT              (SMS_OMC_BASE + 6)
#define SMS_QUERY_STAT_ACK          (SMS_QUERY_STAT|0x8000)
#define SMS_GET_STAT_RECORD         (SMS_OMC_BASE + 7)
#define SMS_GET_STAT_RECORD_ACK     (SMS_GET_STAT_RECORD|0x8000)
#define SMS_QUERY_STATS             (SMS_OMC_BASE + 8)
#define SMS_QUERY_STATS_ACK         (SMS_QUERY_STATS|0x8000)
#define SMS_QUERY_SYSTEMTIME        (SMS_OMC_BASE + 9)
#define SMS_QUERY_SYSTEMTIME_ACK    (SMS_QUERY_SYSTEMTIME|0x8000)
#define SMS_MT_GETTASK_RESNUM       (SMS_OMC_BASE + 10)
#define SMS_MT_GETTASK_RESNUM_ACK   (SMS_MT_GETTASK_RESNUM|0x8000)
#define SMS_CLEAR_TASKRES           (SMS_OMC_BASE + 11)
#define SMS_CLEAR_TASKRES_ACK       (SMS_CLEAR_TASKRES|0x8000)
#define SMS_TASK_FAIL               (SMS_OMC_BASE + 12)
#define SMS_TASK_FAIL_ACK           (SMS_TASK_FAIL|0x8000)
#define SMS_FILTER                  (SMS_OMC_BASE + 13)
//#define SMS_FILTER_ACK              (SMS_FILTER|0x8000)
//#define SMS_STAT_ACK                (SMS_OMC_BASE + 14)
#define SMS_OMC_TIMER               (SMS_OMC_BASE + 15)
#define SMS_OMC_IF_DELETE           (SMS_OMC_BASE + 16)

//话单ID
#define SMS_DUMP_BILL               (SMS_OMC_BASE + 28)



//*****************短消息***************************
#define SMS_SUBMIT_ADDR         60      //主要用于短消息中心发送或查询
#define SMS_SUBMIT_ADDR_ACK     (SMS_SUBMIT_ADDR|0x8000)              
#define SMS_SUBMIT_SERVICE      61      //主要用于人工台的发送或查询
#define SMS_SUBMIT_SERVICE_ACK  (SMS_SUBMIT_SERVICE|0x8000)              
#define SMS_DELIVER_ADDR         62      //主要用于短消息中心下发消息
#define SMS_DELIVER_ADDR_ACK     (SMS_DELIVER_ADDR|0x8000)              
#define SMS_STATUS_REPORT        63     

//人工台开户有关操作
#define SMS_OPR_OPEN_SERVICE		80  //人工台开业务        
#define	SMS_OPR_OPEN_SERVICE_ACK	(SMS_OPR_OPEN_SERVICE|0x8000)
#define	SMS_OPR_QUERY_USER_SERVICE	81  //人工台查询服务
#define SMS_OPR_QUERY_USER_SERVICE_ACK (SMS_OPR_QUERY_USER_SERVICE|0x8000)
#define SMS_OPR_CANCEL_USER_SERVICE	82  //人工台终止服务
#define SMS_OPR_CANCEL_USER_SERVICE_ACK	(SMS_OPR_CANCEL_USER_SERVICE|0x8000)
#define SMS_OPR_DEL_USER			83  //人工台删除用户
#define SMS_OPR_DEL_USER_ACK		(SMS_OPR_DEL_USER | 0x8000)
#define SMS_CHECK_USER              84
#define SMS_CHECK_USER_ACK          (SMS_CHECK_USER|0x8000)
#define	SMS_OPR_QUERY_USER_ALL_SERVICE	    85  //人工台查询所有服务
#define SMS_OPR_QUERY_USER_ALL_SERVICE_ACK (SMS_OPR_QUERY_USER_ALL_SERVICE|0x8000)

#define	SMS_OPR_SET_MSTYPE	        86  //人工台设置手机类型
#define SMS_OPR_SET_MSTYPE_ACK      (SMS_OPR_SET_MSTYPE|0x8000)
#define	SMS_OPR_QUERY_MSTYPE	    87  //人工台查询手机类型
#define SMS_OPR_QUERY_MSTYPE_ACK    (SMS_OPR_QUERY_MSTYPE|0x8000)

#define SMS_USER_FLUSH_SERVICE      88 //用户数据库刷新消息
#define SMS_USER_FLUSH_SERVICE_ACK (SMS_USER_FLUSH_SERVICE|0x8000)
#define SMS_USER_FLUSH_START        89  //开始刷新
#define SMS_USER_FLUSH_START_ACK    (SMS_USER_FLUSH_START|0x8000)
#define SMS_USER_FLUSH_END          90  //刷新结束
#define SMS_USER_FLUSH_END_ACK      (SMS_USER_FLUSH_END|0x8000)
#define SMS_USER_FLUSH_REFRESHSYN      91  //同步刷新CHANGE
#define SMS_USER_FLUSH_REFRESHSYN_ACK  (SMS_USER_FLUSH_REFRESHSYN |0x8000)
#define SMS_USER_FLUSH_REFRESHSYN_END  92   //同步结束
#define SMS_USER_FLUSH_REFRESHSYN_END_ACK  (SMS_USER_FLUSH_REFRESHSYN_END|0x8000)
//end add

//以下4个ID为测试用
#define SMS_DEBUG_INFO_REQ			94      
#define SMS_DEBUG_INFO_DATA			95

#define SMS_DEBUG_INFO_REQ_IN		96
#define SMS_CLEAR_DEBUG_SCREEN		97



//SmcIF 同Parser之间的消息交互
#define SMS_SMCIF_PARSER_MSG	98
//通用错误处理通知消息
#define SMS_ERR_MSG				99			

//Stock类消息

//消息的条数< 25
//#define CMD_RES_INTERVAL		25
#define SMS_ST_BASE                 100
#define	SMS_ST_QUERY			    (SMS_ST_BASE+0) //查询股票
#define SMS_ST_QUERY_RES			(SMS_ST_QUERY| 0x8000)
#define	SMS_ST_SET_NOTIFY		    (SMS_ST_BASE+1) //设置告警
#define	SMS_ST_SET_NOTIFY_RES		(SMS_ST_SET_NOTIFY| 0x8000)
#define	SMS_ST_CANCEL_NOTIFY	    (SMS_ST_BASE+2)     //终止告警设置
#define SMS_ST_CANCEL_NOTIFY_RES	(SMS_ST_CANCEL_NOTIFY| 0x8000)
#define SMS_ST_CANCEL_ALL_NOTIFY	(SMS_ST_BASE+3) //终止所有告警设置
#define SMS_ST_CANCEL_ALL_NOTIFY_RES (SMS_ST_CANCEL_ALL_NOTIFY| 0x8000)
#define	SMS_ST_QUERY_NOTIFY		    (SMS_ST_BASE+4)      //查询告警设置
#define	SMS_ST_QUERY_NOTIFY_RES		(SMS_ST_QUERY_NOTIFY| 0x8000)

#define	SMS_ST_NOTIFY			    (SMS_ST_BASE+5)     //股票告警消息
#define SMS_ST_NOTIFY_ACK		    (SMS_ST_NOTIFY | 0x8000)
#define SMS_ST_FLUSH_STOCK		    (SMS_ST_BASE+6)     //股票刷新消息
#define SMS_ST_DEL_STOCK		    (SMS_ST_BASE+7)     //删除股票
#define SMS_ST_FLUSH_HQ			    (SMS_ST_BASE+8)     //行情刷新消息


//人工台同Stock系统之间的交互消息协议
#define SMS_ST_OPR_BASE             150
#define	SMS_ST_OPR_SET_NOTIFY		(SMS_ST_OPR_BASE+0)     //人工台设置告警
#define	SMS_ST_OPR_SET_NOTIFY_ACK	(SMS_ST_OPR_SET_NOTIFY|0x8000)
#define SMS_ST_OPR_QUERY			(SMS_ST_OPR_BASE+1)     //人工台查询股票
#define SMS_ST_OPR_QUERY_ACK		(SMS_ST_OPR_QUERY|0x8000)
#define	SMS_ST_OPR_CANCEL_NOTIFY	(SMS_ST_OPR_BASE+2)     //人工台终止告警
#define	SMS_ST_OPR_CANCEL_NOTIFY_ACK	(SMS_ST_OPR_CANCEL_NOTIFY|0x8000)
#define	SMS_ST_OPR_QUERY_NOTIFY		(SMS_ST_OPR_BASE+3)     //人工台查询告警
#define	SMS_ST_OPR_QUERY_NOTIFY_ACK	(SMS_ST_OPR_QUERY_NOTIFY|0x8000)
#define	SMS_ST_OPR_CANCEL_ALL_NOTIFY	(SMS_ST_OPR_BASE+4)  //人工台终止所有告警
#define	SMS_ST_OPR_CANCEL_ALL_NOTIFY_ACK	(SMS_ST_OPR_CANCEL_ALL_NOTIFY|0x8000)

//航班消息
#define SMS_FLIGHT_BASE             200
#define SMS_FLIGHT_QUERY_BY_NUM		(SMS_FLIGHT_BASE+1)     //使用航班号查询航班信息
#define SMS_FLIGHT_QUERY_BY_NUM_RES	(SMS_FLIGHT_QUERY_BY_NUM | 0x8000)
#define SMS_FLIGHT_QUERY_BY_ADDR	(SMS_FLIGHT_BASE+2)     //使用地名缩写查询航班信息
#define SMS_FLIGHT_QUERY_BY_ADDR_RES	(SMS_FLIGHT_QUERY_BY_ADDR | 0x8000)
#define SMS_FLIGHT_QUERY    	    (SMS_FLIGHT_BASE+3)     //使用自动组合查询航班信息
#define SMS_FLIGHT_QUERY_RES	    (SMS_FLIGHT_QUERY | 0x8000)


//人工台同航班系统之间的交互消息协议
#define SMS_FLIGHT_OPR_BASE                     250
#define SMS_FLIGHT_OPR_QUERY_BY_NUM		    (SMS_FLIGHT_OPR_BASE+0)     //人工台使用航班号查询航班信息
#define SMS_FLIGHT_OPR_QUERY_BY_NUM_ACK		(SMS_FLIGHT_OPR_QUERY_BY_NUM | 0x8000)
#define SMS_FLIGHT_OPR_QUERY_BY_ADDR		(SMS_FLIGHT_OPR_BASE+1)     //人工台使用地名缩写查询航班信息
#define SMS_FLIGHT_OPR_QUERY_BY_ADDR_ACK	(SMS_FLIGHT_OPR_QUERY_BY_ADDR | 0x8000)

//**************新闻消息************
#define SMS_NEWS_BASE                300
#define SMS_NEWS_SET_SERVICE        (SMS_NEWS_BASE+0)     //新闻设置定阅服务
#define SMS_NEWS_SET_SERVICE_RES    (SMS_NEWS_SET_SERVICE | 0x8000)

#define SMS_NEWS_QUERY_SERVICE     (SMS_NEWS_BASE+1)      //查询新闻定阅服务
#define SMS_NEWS_QUERY_SERVICE_RES (SMS_NEWS_QUERY_SERVICE | 0x8000)

#define SMS_NEWS_CANCEL_SERVICE     (SMS_NEWS_BASE+2)     //新闻终止定阅服务
#define SMS_NEWS_CANCEL_SERVICE_RES (SMS_NEWS_CANCEL_SERVICE | 0x8000)

#define SMS_NEWS_QUERY_ALL_SERVICE      (SMS_NEWS_BASE+3)  //查询所有定阅服务
#define SMS_NEWS_QUERY_ALL_SERVICE_RES  (SMS_NEWS_QUERY_ALL_SERVICE | 0x8000)

#define SMS_NEWS_CANCEL_ALL_SERVICE     (SMS_NEWS_BASE+4) //终止所有定阅服务
#define SMS_NEWS_CANCEL_ALL_SERVICE_RES (SMS_NEWS_CANCEL_ALL_SERVICE | 0x8000)

#define SMS_NEWS_SUBMIT                 (SMS_NEWS_BASE+5)  //新闻提交消息到短消息中心
#define SMS_NEWS_SUBMIT_ACK             (SMS_NEWS_SUBMIT | 0X8000)

#define SMS_NEWS_QUERY_NEWS				(SMS_NEWS_BASE+6) //查询新闻
#define SMS_NEWS_QUERY_NEWS_RES			(SMS_NEWS_QUERY_NEWS | 0x8000)

//******帮助信息
#define SMS_HELP_BASE                   320
#define SMS_HELP_QUERY_HELP				(SMS_HELP_BASE + 0) //查询帮助
#define SMS_HELP_QUERY_HELP_RES			(SMS_HELP_QUERY_HELP | 0x8000)


//******人工台新闻消息
//设置服务
#define SMS_NEWS_OPR_BASE               350
#define SMS_NEWS_OPR_SET_SERVICE        (SMS_NEWS_OPR_BASE+0)
#define SMS_NEWS_OPR_SET_SERVICE_ACK    (SMS_NEWS_OPR_SET_SERVICE | 0X8000)

//查询服务
#define SMS_NEWS_OPR_QUERY_SERVICE     (SMS_NEWS_OPR_BASE+1)
#define SMS_NEWS_OPR_QUERY_SERVICE_ACK (SMS_NEWS_OPR_QUERY_SERVICE | 0X8000)

//取消服务
#define SMS_NEWS_OPR_CANCEL_SERVICE     (SMS_NEWS_OPR_BASE+2)
#define SMS_NEWS_OPR_CANCEL_SERVICE_ACK (SMS_NEWS_OPR_CANCEL_SERVICE | 0X8000)

//查询所有服务
#define SMS_NEWS_OPR_QUERY_ALL_SERVICE     (SMS_NEWS_OPR_BASE+3)
#define SMS_NEWS_OPR_QUERY_ALL_SERVICE_ACK (SMS_NEWS_OPR_QUERY_ALL_SERVICE | 0X8000)

//取消所有服务
#define SMS_NEWS_OPR_CANCEL_ALL_SERVICE     (SMS_NEWS_OPR_BASE+4)
#define SMS_NEWS_OPR_CANCEL_ALL_SERVICE_ACK (SMS_NEWS_OPR_CANCEL_ALL_SERVICE | 0X8000)

//查询新闻
#define SMS_NEWS_OPR_QUERY_NEWS				(SMS_NEWS_OPR_BASE+5)
#define SMS_NEWS_OPR_QUERY_NEWS_ACK			(SMS_NEWS_OPR_QUERY_NEWS | 0X8000)

//#define SMS_OMC_BASE               400  //已使用

#define SMS_TRAFIC_BASE                 500
#define SMS_TRAINSHIP_QUERY             (SMS_TRAFIC_BASE)
#define SMS_TRAINSHIP_QUERY_RES         (SMS_TRAFIC_BASE | 0X8000)

//消息
#define SMS_DIC_BASE             550
#define SMS_DIC_QUERY		    (SMS_DIC_BASE+1)     //使用查询信息
//#define SMS_DIC_QUERY_RES		(SMS_DIC_BASE+2) 
#define SMS_DIC_QUERY_RES		(SMS_DIC_QUERY | 0X8000) 

//要求各管理器重新加载配置
//#define SMS_CONFIG					999

/*
//普通信息源要求收发短消息命令,需要进入ID转换
//内部接口用ID
#define	SMS_COMM_BIND			    SMS_CLIENT_BIND     //连接信息服务器
#define SMS_COMM_BIND_ACK			SMS_CLIENT_BIND_ACK

  #define	SMS_COMM_UNBIND			    SMS_CLIENT_UNBIND   //断连信息服务器
  #define SMS_COMM_UNBIND_ACK			SMS_CLIENT_UNBIND_ACK
  
	#define	SMS_COMM_SUBMIT_SM			401         //提交短消息
	#define SMS_COMM_SUBMIT_SM_ACK		(SMS_COMM_SUBMIT_SM| 0x8000)
	
	  #define	SMS_COMM_ENQUIRE_LINK		402         //确认连接信息
	  #define SMS_COMM_ENQUIRE_LINK_ACK	(SMS_COMM_ENQUIRE_LINK| 0x8000)
*/
/*
//外部接口用ID
#define	SMS_COMM_BIND_OUT			    0   //连接信息服务器
#define SMS_COMM_BIND_OUT_ACK			(SMS_COMM_BIND_OUT | 0x8000)

  #define	SMS_COMM_UNBIND_OUT			    1   //断连信息服务器
  #define SMS_COMM_UNBIND_OUT_ACK			(SMS_COMM_UNBIND_OUT | 0x8000)
  
	#define	SMS_COMM_SUBMIT_SM_OUT			3   //提交短消息
	#define SMS_COMM_SUBMIT_SM_OUT_ACK		(SMS_COMM_SUBMIT_SM_OUT | 0x8000)
	
	  #define	SMS_COMM_ENQUIRE_LINK_OUT		2   //确认连接信息
	  #define SMS_COMM_ENQUIRE_LINK_OUT_ACK	(SMS_COMM_ENQUIRE_LINK_OUT | 0x8000)
	  
*/
//内部用节点号
#define INVALID_CMD_ID  0xFFFF      //错误的命令ID号

#endif