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

//��ͨ��Ϣ
#define IS_COMMON_MSG(nCommand) (nCommand<100)&&(nCommand>=0)
#define IS_COMMAND_ACK(nCommand) (nCommand & 0x8000)
#define IS_SMPP_ACK(nCommand) (nCommand & 0x80000000)

//��Ʊ��Ϣ
#define IS_STOCK_MSG(nCommand)  ( (nCommand &= 0x7fff) < (SMS_ST_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_ST_BASE)
#define IS_STOCK_MSG_SEND(nCommand)  \
( (nCommand &= 0x7fff) < (SMS_ST_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_ST_BASE)

//������Ϣ
#define IS_FLIGHT_MSG(nCommand) ( (nCommand &= 0x7fff) < (SMS_FLIGHT_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_FLIGHT_BASE)
#define IS_FLIGHT_MSG_SEND(nCommand) \
( (nCommand &= 0x7fff) < (SMS_FLIGHT_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_FLIGHT_BASE)

//������Ϣ
#define IS_NEWS_MSG(nCommand)   ( (nCommand &= 0x7fff) < (SMS_NEWS_BASE + 100)) && ((nCommand &= 0x7fff) >= SMS_NEWS_BASE)
#define IS_NEWS_MSG_SEND(nCommand) \
( (nCommand &= 0x7fff) < (SMS_NEWS_BASE | 0x8000)) && ((nCommand &= 0x7fff) >= SMS_NEWS_BASE)

//��ͨ�ⲿ�ӿ���Ϣ
#define IS_COMM_MSG(nCommand)   ( (nCommand &= 0x7fff) < 500) && ((nCommand &= 0x7fff) >= 400)

//������Ϣ
#define IS_CONFIG_MSG(nCommand)	 ( (nCommand &= 0x7fff) < 1000) && ((nCommand &= 0x7fff) >= 999)


//����ִ��״̬
#define CMD_STATUS_SUCCESS	0
#define CMD_STATUS_FAIL		1

#define USER_CHECK_SUCCESS  0
#define USER_CHECK_FAIL     1


//��������ִ��״̬
#define NEWS_SUCCEED        0
#define NEWS_FAIL           0x60


//�����е����Ƿ��к���
//û�к���
#define FLIGHT_DAY_NOT_SET  0
//�к���
#define FLIGHT_DAY_SET      1



//�������͵ķ���
#define IS_COMMON_ERR(Err)  (Err>=ERR_COMMON_BASE && Err<= ERR_COMMON_MAX)
#define IS_SMCIF_ERR(Err)   (Err>=ERR_SMCIF_BASE && Err<= ERR_SMCIF_MAX)
#define IS_STOCK_ERR(Err)  (Err>=ERR_STOCK_BASE && Err<= ERR_STOCK_MAX)
#define IS_FLIGHT_ERR(Err)  (Err>=ERR_FLIGHT_BASE && Err<= ERR_FLIGHT_MAX)
#define IS_NEWS_ERR(Err)    (Err>=ERR_NEWS_BASE && Err<= ERR_NEWS_MAX)

//�����������
#define ERR_TYPE_IF			0
//��ͨ�����붨��
#define ERR_COMMON_BASE		0
#define ERR_NO_ERR			(ERR_COMMON_BASE+0)		//û�д���
#define ERR_BIND_REPEAT		(ERR_COMMON_BASE+1)		//�ظ���¼
#define ERR_IF_NOT_EXIST	(ERR_COMMON_BASE+2)		//�ӿڲ�����
#define ERR_IF_TCP			(ERR_COMMON_BASE+3)		//�����
#define ERR_COMMON_MAX		19
//smc�ӿڴ������
#define ERR_SMCIF_BASE		20
#define ERR_SUBMITSM		(ERR_SMCIF_BASE+0)		//�ύ����Ϣʧ��
#define ERR_PARSER			(ERR_SMCIF_BASE+1)		//������ʧ��
#define ERR_SMCIF_MAX		39
// ��Ʊ�������
#define ERR_STOCK_BASE		40
#define ERR_STOCK_CODE_NUMBER   (ERR_COMMON_BASE+0)     //��Ʊ�������
#define ERR_STOCK_CODE_LENGTH   (ERR_STOCK_BASE+1)		//��Ʊ���볤�ȴ���
#define ERR_STOCK_IS_TP             (ERR_STOCK_BASE+2)      //��Ʊ��ͣ��
#define ERR_STOCK_NOTITY_UNREASONABLE (ERR_STOCK_BASE+3)    //�澯���ò�����
#define ERR_STOCK_NO_NOTIFY			(ERR_STOCK_BASE+4)      //�޸澯����
#define ERR_STOCK_NOT_EXIST		(ERR_STOCK_BASE+5)	    //��Ʊ������
#define ERR_NO_THIS_ALERT		(ERR_STOCK_BASE+6)      //û�д˸��澯����
#define ERR_EXCEED_MAX_NOTIFY_COUNT (ERR_STOCK_BASE+7)  //���ø澯���������
#define ERR_NO_NOTIFY_SET		(ERR_STOCK_BASE+8)      //û�����ø澯
#define ERR_NO_CODE_NOTIFY_SET  (ERR_STOCK_BASE+9)      //û�����ò�ѯ��Ʊ�ľ���
#define ERR_STOCK_MAX		59

//����������
#define ERR_FLIGHT_BASE		60
#define ERR_FLIGHT_NOT_EXIT	(ERR_FLIGHT_BASE+0)     //���಻����
#define ERR_FLIGHT_DB_ERROR	(ERR_FLIGHT_BASE+1)     //�������ݿ����
#define ERR_FLIGHT_MAX		79

//���Ŵ������
#define ERR_NEWS_BASE		90
#define ERR_NEWS_DB_ERROR	(ERR_NEWS_BASE+0)       //�������ݿ����
#define ERR_NEWS_SET_ERROR  (ERR_NEWS_BASE+1)       //�������ŷ���ʧ��
#define ERR_NEWS_NEWSNO_ERROR (ERR_NEWS_BASE+2)     //���ŵı������
#define ERR_NEWS_CANCEL_ERROR (ERR_NEWS_BASE+3)     //��ֹ���ŷ���ʧ��
#define ERR_NEWS_CANCEL_ALL_ERROR (ERR_NEWS_BASE+4)  //��ֹ�������ŷ���ʧ��
#define ERR_NEWS_QUERY_ERROR    (ERR_NEWS_BASE+5)   //��ѯ���ŷ���ʧ��
#define ERR_NEWS_QUERY_ALL_ERROR (ERR_NEWS_BASE+6)  //��ѯ�������ŷ���ʧ��
#define ERR_NEWS_QUERY_NOT_SET_ERROR (ERR_NEWS_BASE+7)  //û���������ŷ���
#define ERR_NEWS_MAX		120

//����������
#define ERR_TRAFIC_BASE		120
#define ERR_TRAINSHIP_DB_ERROR	(ERR_TRAFIC_BASE+0)     //���಻����
#define ERR_TRAINSHIP_NOT_EXIT	(ERR_TRAFIC_BASE+1)     //�������ݿ����
#define ERR_TRAFIC_MAX		128

//���ŵ�����
#define MAX_NEWS_TYPE       32
//����Ԥ���У��������������ֵ
#define MAX_WT_ADDRESS      128

//��ѯ����ʱ����IDû��ʱ�����
#define NEWS_NO_SUB_ID      0XFF

//��С���ű���
#define MINI_NEWS_NO    0  
//������ű���
#define MAX_NEWS_NO     MAX_NEWS_TYPE
//��С�����ӱ���
#define MINI_NEWS_SUB_NO 1
//����Ԥ���ĵ���Ϣ���
#define NEWS_WT_ID      1
#define STR_NEWS_WT_ID   "1"
//�յ�����ID
#define NEWS_NO_ID      -1

#define MAX_NEWS_QUERY_TYPE 100


/******************************************************************************************/
/*							ͨ������Ϣ													  */	
/******************************************************************************************/

//�ͻ��˵�ע�ἰ������Ϣ
#define SMS_CLIENT_BIND			1       //��������
#define SMS_CLIENT_BIND_ACK		(SMS_CLIENT_BIND|0x8000)

#define SMS_CLIENT_BIND_REC		0       //��������
#define SMS_CLIENT_BIND_REC_ACK	(SMS_CLIENT_BIND_REC|0x8000)

#define SMS_CLIENT_UNBIND		2       //��������
#define SMS_CLIENT_UNBIND_ACK	(SMS_CLIENT_UNBIND|0x8000)

#define SMS_SHAKEHAND			3       //������Ϣ
#define SMS_SHAKEHAND_ACK		(SMS_SHAKEHAND|0x8000)

//********�й�·���ʺ��Լ����õ������Ϣ******************
#define SMS_ADD_ACCOUNT         30      //�����ʺ�
#define SMS_ADD_ACCOUNT_ACK     (SMS_ADD_ACCOUNT|0x8000)

#define SMS_QUERY_ACCOUNT       31      //��ѯ�ʺ�
#define SMS_QUERY_ACCOUNT_ACK   (SMS_QUERY_ACCOUNT|0x8000)

#define SMS_MODI_ACCOUNT        32      //�޸��ʺ�
#define SMS_MODI_ACCOUNT_ACK    (SMS_MODI_ACCOUNT|0x8000)

#define SMS_QUERY_ALL_ACCOUNT       33      //��ѯ�����ʺ���
#define SMS_QUERY_ALL_ACCOUNT_ACK   (SMS_QUERY_ALL_ACCOUNT|0x8000)

#define SMS_DEL_ACCOUNT         34      //ɾ���ʺ�
#define SMS_DEL_ACCOUNT_ACK     (SMS_DEL_ACCOUNT|0x8000)

#define SMS_QUERY_INTERFACE     35      //��ѯ�ӿ�
#define SMS_QUERY_INTERFACE_ACK (SMS_QUERY_INTERFACE|0x8000)

#define SMS_DEL_INTERFACE       36      //ɾ���ӿ�
#define SMS_DEL_INTERFACE_ACK   (SMS_DEL_INTERFACE|0x8000)

#define SMS_QUERY_ALL_INTERFACE       37      //��ѯ���нӿڵ�ID
#define SMS_QUERY_ALL_INTERFACE_ACK   (SMS_QUERY_ALL_INTERFACE|0x8000)

#define SMS_ADD_INTERFACE      38      //���ӿͻ��˽ӿ�
#define SMS_ADD_INTERFACE_ACK  (SMS_ADD_INTERFACE|0x8000)

#define SMS_ADD_DISTABLE       39      //�Ӽ�·��
#define SMS_ADD_DISTABLE_ACK   (SMS_ADD_DISTABLE|0x8000)

#define SMS_DEL_DISTABLE        40      //ɾ��·��
#define SMS_DEL_DISTABLE_ACK    (SMS_DEL_DISTABLE|0x8000)

#define SMS_QUERY_DISTABLE      41      //��ѯ·��
#define SMS_QUERY_DISTABLE_ACK  (SMS_QUERY_DISTABLE|0x8000)


#define SMS_SET_PARAM            42      //���ò���
#define SMS_SET_PARAM_ACK        (SMS_SET_PARAM|0x8000)

#define SMS_GET_PARAM            43      //���ò���
#define SMS_GET_PARAM_ACK        (SMS_GET_PARAM|0x8000)


#define SMS_QUERY_ACCOUNT_NUMCHECK  52      //��ѯ�ʺźŶα�
#define SMS_QUERY_ACCOUNT_NUMCHECK_ACK  (SMS_QUERY_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_ADD_ACCOUNT_NUMCHECK    53      //����ʺźŶα�
#define SMS_ADD_ACCOUNT_NUMCHECK_ACK    (SMS_ADD_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_DEL_ACCOUNT_NUMCHECK    54      //ɾ���ʺźŶα�
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

//����ID
#define SMS_DUMP_BILL               (SMS_OMC_BASE + 28)



//*****************����Ϣ***************************
#define SMS_SUBMIT_ADDR         60      //��Ҫ���ڶ���Ϣ���ķ��ͻ��ѯ
#define SMS_SUBMIT_ADDR_ACK     (SMS_SUBMIT_ADDR|0x8000)              
#define SMS_SUBMIT_SERVICE      61      //��Ҫ�����˹�̨�ķ��ͻ��ѯ
#define SMS_SUBMIT_SERVICE_ACK  (SMS_SUBMIT_SERVICE|0x8000)              
#define SMS_DELIVER_ADDR         62      //��Ҫ���ڶ���Ϣ�����·���Ϣ
#define SMS_DELIVER_ADDR_ACK     (SMS_DELIVER_ADDR|0x8000)              
#define SMS_STATUS_REPORT        63     

//�˹�̨�����йز���
#define SMS_OPR_OPEN_SERVICE		80  //�˹�̨��ҵ��        
#define	SMS_OPR_OPEN_SERVICE_ACK	(SMS_OPR_OPEN_SERVICE|0x8000)
#define	SMS_OPR_QUERY_USER_SERVICE	81  //�˹�̨��ѯ����
#define SMS_OPR_QUERY_USER_SERVICE_ACK (SMS_OPR_QUERY_USER_SERVICE|0x8000)
#define SMS_OPR_CANCEL_USER_SERVICE	82  //�˹�̨��ֹ����
#define SMS_OPR_CANCEL_USER_SERVICE_ACK	(SMS_OPR_CANCEL_USER_SERVICE|0x8000)
#define SMS_OPR_DEL_USER			83  //�˹�̨ɾ���û�
#define SMS_OPR_DEL_USER_ACK		(SMS_OPR_DEL_USER | 0x8000)
#define SMS_CHECK_USER              84
#define SMS_CHECK_USER_ACK          (SMS_CHECK_USER|0x8000)
#define	SMS_OPR_QUERY_USER_ALL_SERVICE	    85  //�˹�̨��ѯ���з���
#define SMS_OPR_QUERY_USER_ALL_SERVICE_ACK (SMS_OPR_QUERY_USER_ALL_SERVICE|0x8000)

#define	SMS_OPR_SET_MSTYPE	        86  //�˹�̨�����ֻ�����
#define SMS_OPR_SET_MSTYPE_ACK      (SMS_OPR_SET_MSTYPE|0x8000)
#define	SMS_OPR_QUERY_MSTYPE	    87  //�˹�̨��ѯ�ֻ�����
#define SMS_OPR_QUERY_MSTYPE_ACK    (SMS_OPR_QUERY_MSTYPE|0x8000)

#define SMS_USER_FLUSH_SERVICE      88 //�û����ݿ�ˢ����Ϣ
#define SMS_USER_FLUSH_SERVICE_ACK (SMS_USER_FLUSH_SERVICE|0x8000)
#define SMS_USER_FLUSH_START        89  //��ʼˢ��
#define SMS_USER_FLUSH_START_ACK    (SMS_USER_FLUSH_START|0x8000)
#define SMS_USER_FLUSH_END          90  //ˢ�½���
#define SMS_USER_FLUSH_END_ACK      (SMS_USER_FLUSH_END|0x8000)
#define SMS_USER_FLUSH_REFRESHSYN      91  //ͬ��ˢ��CHANGE
#define SMS_USER_FLUSH_REFRESHSYN_ACK  (SMS_USER_FLUSH_REFRESHSYN |0x8000)
#define SMS_USER_FLUSH_REFRESHSYN_END  92   //ͬ������
#define SMS_USER_FLUSH_REFRESHSYN_END_ACK  (SMS_USER_FLUSH_REFRESHSYN_END|0x8000)
//end add

//����4��IDΪ������
#define SMS_DEBUG_INFO_REQ			94      
#define SMS_DEBUG_INFO_DATA			95

#define SMS_DEBUG_INFO_REQ_IN		96
#define SMS_CLEAR_DEBUG_SCREEN		97



//SmcIF ͬParser֮�����Ϣ����
#define SMS_SMCIF_PARSER_MSG	98
//ͨ�ô�����֪ͨ��Ϣ
#define SMS_ERR_MSG				99			

//Stock����Ϣ

//��Ϣ������< 25
//#define CMD_RES_INTERVAL		25
#define SMS_ST_BASE                 100
#define	SMS_ST_QUERY			    (SMS_ST_BASE+0) //��ѯ��Ʊ
#define SMS_ST_QUERY_RES			(SMS_ST_QUERY| 0x8000)
#define	SMS_ST_SET_NOTIFY		    (SMS_ST_BASE+1) //���ø澯
#define	SMS_ST_SET_NOTIFY_RES		(SMS_ST_SET_NOTIFY| 0x8000)
#define	SMS_ST_CANCEL_NOTIFY	    (SMS_ST_BASE+2)     //��ֹ�澯����
#define SMS_ST_CANCEL_NOTIFY_RES	(SMS_ST_CANCEL_NOTIFY| 0x8000)
#define SMS_ST_CANCEL_ALL_NOTIFY	(SMS_ST_BASE+3) //��ֹ���и澯����
#define SMS_ST_CANCEL_ALL_NOTIFY_RES (SMS_ST_CANCEL_ALL_NOTIFY| 0x8000)
#define	SMS_ST_QUERY_NOTIFY		    (SMS_ST_BASE+4)      //��ѯ�澯����
#define	SMS_ST_QUERY_NOTIFY_RES		(SMS_ST_QUERY_NOTIFY| 0x8000)

#define	SMS_ST_NOTIFY			    (SMS_ST_BASE+5)     //��Ʊ�澯��Ϣ
#define SMS_ST_NOTIFY_ACK		    (SMS_ST_NOTIFY | 0x8000)
#define SMS_ST_FLUSH_STOCK		    (SMS_ST_BASE+6)     //��Ʊˢ����Ϣ
#define SMS_ST_DEL_STOCK		    (SMS_ST_BASE+7)     //ɾ����Ʊ
#define SMS_ST_FLUSH_HQ			    (SMS_ST_BASE+8)     //����ˢ����Ϣ


//�˹�̨ͬStockϵͳ֮��Ľ�����ϢЭ��
#define SMS_ST_OPR_BASE             150
#define	SMS_ST_OPR_SET_NOTIFY		(SMS_ST_OPR_BASE+0)     //�˹�̨���ø澯
#define	SMS_ST_OPR_SET_NOTIFY_ACK	(SMS_ST_OPR_SET_NOTIFY|0x8000)
#define SMS_ST_OPR_QUERY			(SMS_ST_OPR_BASE+1)     //�˹�̨��ѯ��Ʊ
#define SMS_ST_OPR_QUERY_ACK		(SMS_ST_OPR_QUERY|0x8000)
#define	SMS_ST_OPR_CANCEL_NOTIFY	(SMS_ST_OPR_BASE+2)     //�˹�̨��ֹ�澯
#define	SMS_ST_OPR_CANCEL_NOTIFY_ACK	(SMS_ST_OPR_CANCEL_NOTIFY|0x8000)
#define	SMS_ST_OPR_QUERY_NOTIFY		(SMS_ST_OPR_BASE+3)     //�˹�̨��ѯ�澯
#define	SMS_ST_OPR_QUERY_NOTIFY_ACK	(SMS_ST_OPR_QUERY_NOTIFY|0x8000)
#define	SMS_ST_OPR_CANCEL_ALL_NOTIFY	(SMS_ST_OPR_BASE+4)  //�˹�̨��ֹ���и澯
#define	SMS_ST_OPR_CANCEL_ALL_NOTIFY_ACK	(SMS_ST_OPR_CANCEL_ALL_NOTIFY|0x8000)

//������Ϣ
#define SMS_FLIGHT_BASE             200
#define SMS_FLIGHT_QUERY_BY_NUM		(SMS_FLIGHT_BASE+1)     //ʹ�ú���Ų�ѯ������Ϣ
#define SMS_FLIGHT_QUERY_BY_NUM_RES	(SMS_FLIGHT_QUERY_BY_NUM | 0x8000)
#define SMS_FLIGHT_QUERY_BY_ADDR	(SMS_FLIGHT_BASE+2)     //ʹ�õ�����д��ѯ������Ϣ
#define SMS_FLIGHT_QUERY_BY_ADDR_RES	(SMS_FLIGHT_QUERY_BY_ADDR | 0x8000)
#define SMS_FLIGHT_QUERY    	    (SMS_FLIGHT_BASE+3)     //ʹ���Զ���ϲ�ѯ������Ϣ
#define SMS_FLIGHT_QUERY_RES	    (SMS_FLIGHT_QUERY | 0x8000)


//�˹�̨ͬ����ϵͳ֮��Ľ�����ϢЭ��
#define SMS_FLIGHT_OPR_BASE                     250
#define SMS_FLIGHT_OPR_QUERY_BY_NUM		    (SMS_FLIGHT_OPR_BASE+0)     //�˹�̨ʹ�ú���Ų�ѯ������Ϣ
#define SMS_FLIGHT_OPR_QUERY_BY_NUM_ACK		(SMS_FLIGHT_OPR_QUERY_BY_NUM | 0x8000)
#define SMS_FLIGHT_OPR_QUERY_BY_ADDR		(SMS_FLIGHT_OPR_BASE+1)     //�˹�̨ʹ�õ�����д��ѯ������Ϣ
#define SMS_FLIGHT_OPR_QUERY_BY_ADDR_ACK	(SMS_FLIGHT_OPR_QUERY_BY_ADDR | 0x8000)

//**************������Ϣ************
#define SMS_NEWS_BASE                300
#define SMS_NEWS_SET_SERVICE        (SMS_NEWS_BASE+0)     //�������ö��ķ���
#define SMS_NEWS_SET_SERVICE_RES    (SMS_NEWS_SET_SERVICE | 0x8000)

#define SMS_NEWS_QUERY_SERVICE     (SMS_NEWS_BASE+1)      //��ѯ���Ŷ��ķ���
#define SMS_NEWS_QUERY_SERVICE_RES (SMS_NEWS_QUERY_SERVICE | 0x8000)

#define SMS_NEWS_CANCEL_SERVICE     (SMS_NEWS_BASE+2)     //������ֹ���ķ���
#define SMS_NEWS_CANCEL_SERVICE_RES (SMS_NEWS_CANCEL_SERVICE | 0x8000)

#define SMS_NEWS_QUERY_ALL_SERVICE      (SMS_NEWS_BASE+3)  //��ѯ���ж��ķ���
#define SMS_NEWS_QUERY_ALL_SERVICE_RES  (SMS_NEWS_QUERY_ALL_SERVICE | 0x8000)

#define SMS_NEWS_CANCEL_ALL_SERVICE     (SMS_NEWS_BASE+4) //��ֹ���ж��ķ���
#define SMS_NEWS_CANCEL_ALL_SERVICE_RES (SMS_NEWS_CANCEL_ALL_SERVICE | 0x8000)

#define SMS_NEWS_SUBMIT                 (SMS_NEWS_BASE+5)  //�����ύ��Ϣ������Ϣ����
#define SMS_NEWS_SUBMIT_ACK             (SMS_NEWS_SUBMIT | 0X8000)

#define SMS_NEWS_QUERY_NEWS				(SMS_NEWS_BASE+6) //��ѯ����
#define SMS_NEWS_QUERY_NEWS_RES			(SMS_NEWS_QUERY_NEWS | 0x8000)

//******������Ϣ
#define SMS_HELP_BASE                   320
#define SMS_HELP_QUERY_HELP				(SMS_HELP_BASE + 0) //��ѯ����
#define SMS_HELP_QUERY_HELP_RES			(SMS_HELP_QUERY_HELP | 0x8000)


//******�˹�̨������Ϣ
//���÷���
#define SMS_NEWS_OPR_BASE               350
#define SMS_NEWS_OPR_SET_SERVICE        (SMS_NEWS_OPR_BASE+0)
#define SMS_NEWS_OPR_SET_SERVICE_ACK    (SMS_NEWS_OPR_SET_SERVICE | 0X8000)

//��ѯ����
#define SMS_NEWS_OPR_QUERY_SERVICE     (SMS_NEWS_OPR_BASE+1)
#define SMS_NEWS_OPR_QUERY_SERVICE_ACK (SMS_NEWS_OPR_QUERY_SERVICE | 0X8000)

//ȡ������
#define SMS_NEWS_OPR_CANCEL_SERVICE     (SMS_NEWS_OPR_BASE+2)
#define SMS_NEWS_OPR_CANCEL_SERVICE_ACK (SMS_NEWS_OPR_CANCEL_SERVICE | 0X8000)

//��ѯ���з���
#define SMS_NEWS_OPR_QUERY_ALL_SERVICE     (SMS_NEWS_OPR_BASE+3)
#define SMS_NEWS_OPR_QUERY_ALL_SERVICE_ACK (SMS_NEWS_OPR_QUERY_ALL_SERVICE | 0X8000)

//ȡ�����з���
#define SMS_NEWS_OPR_CANCEL_ALL_SERVICE     (SMS_NEWS_OPR_BASE+4)
#define SMS_NEWS_OPR_CANCEL_ALL_SERVICE_ACK (SMS_NEWS_OPR_CANCEL_ALL_SERVICE | 0X8000)

//��ѯ����
#define SMS_NEWS_OPR_QUERY_NEWS				(SMS_NEWS_OPR_BASE+5)
#define SMS_NEWS_OPR_QUERY_NEWS_ACK			(SMS_NEWS_OPR_QUERY_NEWS | 0X8000)

//#define SMS_OMC_BASE               400  //��ʹ��

#define SMS_TRAFIC_BASE                 500
#define SMS_TRAINSHIP_QUERY             (SMS_TRAFIC_BASE)
#define SMS_TRAINSHIP_QUERY_RES         (SMS_TRAFIC_BASE | 0X8000)

//��Ϣ
#define SMS_DIC_BASE             550
#define SMS_DIC_QUERY		    (SMS_DIC_BASE+1)     //ʹ�ò�ѯ��Ϣ
//#define SMS_DIC_QUERY_RES		(SMS_DIC_BASE+2) 
#define SMS_DIC_QUERY_RES		(SMS_DIC_QUERY | 0X8000) 

//Ҫ������������¼�������
//#define SMS_CONFIG					999

/*
//��ͨ��ϢԴҪ���շ�����Ϣ����,��Ҫ����IDת��
//�ڲ��ӿ���ID
#define	SMS_COMM_BIND			    SMS_CLIENT_BIND     //������Ϣ������
#define SMS_COMM_BIND_ACK			SMS_CLIENT_BIND_ACK

  #define	SMS_COMM_UNBIND			    SMS_CLIENT_UNBIND   //������Ϣ������
  #define SMS_COMM_UNBIND_ACK			SMS_CLIENT_UNBIND_ACK
  
	#define	SMS_COMM_SUBMIT_SM			401         //�ύ����Ϣ
	#define SMS_COMM_SUBMIT_SM_ACK		(SMS_COMM_SUBMIT_SM| 0x8000)
	
	  #define	SMS_COMM_ENQUIRE_LINK		402         //ȷ��������Ϣ
	  #define SMS_COMM_ENQUIRE_LINK_ACK	(SMS_COMM_ENQUIRE_LINK| 0x8000)
*/
/*
//�ⲿ�ӿ���ID
#define	SMS_COMM_BIND_OUT			    0   //������Ϣ������
#define SMS_COMM_BIND_OUT_ACK			(SMS_COMM_BIND_OUT | 0x8000)

  #define	SMS_COMM_UNBIND_OUT			    1   //������Ϣ������
  #define SMS_COMM_UNBIND_OUT_ACK			(SMS_COMM_UNBIND_OUT | 0x8000)
  
	#define	SMS_COMM_SUBMIT_SM_OUT			3   //�ύ����Ϣ
	#define SMS_COMM_SUBMIT_SM_OUT_ACK		(SMS_COMM_SUBMIT_SM_OUT | 0x8000)
	
	  #define	SMS_COMM_ENQUIRE_LINK_OUT		2   //ȷ��������Ϣ
	  #define SMS_COMM_ENQUIRE_LINK_OUT_ACK	(SMS_COMM_ENQUIRE_LINK_OUT | 0x8000)
	  
*/
//�ڲ��ýڵ��
#define INVALID_CMD_ID  0xFFFF      //���������ID��

#endif