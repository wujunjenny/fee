#pragma once
#include "newmng.h"
#include <memory>
#include <string>
class COTLObj;
struct sqlite3;
struct sqlite3_stmt;
class CNewSMDatabase :
	public CNewMng
{
public:
	CNewSMDatabase(void);
	~CNewSMDatabase(void);

public :
	//消息接收函数
	BOOL OnMessage(PTRMSG pMsg);
	//消息发送函数
	BOOL SendMessageToQueue(PTRMSG pMsg);

	//消息保存到数据库缓存表中
	void OnAddDBBuffer(LPCTSTR pMsg);
	//接收状态报告后在数据库缓存表中查找原消息
	void OnFindMsgInDBBuffer(LPCTSTR pMsg);
	//更新WEB包月扣费表(MonthFee)状态
	void OnUpdateFeeMonthTable(LPCTSTR pMSG);
	//加载记录在数据库中的包月扣费信息
	bool LoadFixMsg(LPCTSTR pFeeMonth);
	//保存包月扣费记录到数据库中
	void OnSaveFixMsg(PTRMSG pMsg);
	//删除数据库中的包月扣费记录
	void OnDeleteFixMsg(PTRMSG pMsg);
	//发消息到SMS线程
	bool SendMsgToSMSThread(PTRMSG pMsg);

	//std::shared_ptr<COTLObj> CreateDBConnection();
protected:
	//重载CNewMsg定制执行的操作函数
	void OnTime();
	BOOL OnMountMessage();
	
	std::shared_ptr<COTLObj> m_pconnection;
	sqlite3 *m_pdb;
	sqlite3_stmt *m_addbuffer_stmt;
	sqlite3_stmt *m_findbuffer_stmt;
	sqlite3_stmt *m_deletebuffer_stmt;

	// path and file like this C:/Documents%20and%20Settings/fred/Desktop/data.db
	std::string m_db_file_name;
	// tmp_dir
	//std::string m_db_tmp_dir;
	std::string m_strAddSMBuff;
	std::string m_strFindSMBuff;
	bool m_DoneTimeSrc;
	int m_timecheck;
	int m_timeout_minutes;

public:
	bool init(void);
	// try to begin a trans
	void begin_trans(void);
	// try to end tran
	void end_trans(void);
protected:
	bool m_btrans;
	int m_ntrans_count;
	time_t m_trans_time;
	class AutoTrans
	{
	public:
		AutoTrans(CNewSMDatabase* p):pOwner(p) { pOwner->begin_trans();};
		~AutoTrans() {pOwner->end_trans(); };
	protected:
		CNewSMDatabase* pOwner;
	};
};

