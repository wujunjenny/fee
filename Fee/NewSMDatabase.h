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
	//��Ϣ���պ���
	BOOL OnMessage(PTRMSG pMsg);
	//��Ϣ���ͺ���
	BOOL SendMessageToQueue(PTRMSG pMsg);

	//��Ϣ���浽���ݿ⻺�����
	void OnAddDBBuffer(LPCTSTR pMsg);
	//����״̬����������ݿ⻺����в���ԭ��Ϣ
	void OnFindMsgInDBBuffer(LPCTSTR pMsg);
	//����WEB���¿۷ѱ�(MonthFee)״̬
	void OnUpdateFeeMonthTable(LPCTSTR pMSG);
	//���ؼ�¼�����ݿ��еİ��¿۷���Ϣ
	bool LoadFixMsg(LPCTSTR pFeeMonth);
	//������¿۷Ѽ�¼�����ݿ���
	void OnSaveFixMsg(PTRMSG pMsg);
	//ɾ�����ݿ��еİ��¿۷Ѽ�¼
	void OnDeleteFixMsg(PTRMSG pMsg);
	//����Ϣ��SMS�߳�
	bool SendMsgToSMSThread(PTRMSG pMsg);

	//std::shared_ptr<COTLObj> CreateDBConnection();
protected:
	//����CNewMsg����ִ�еĲ�������
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

