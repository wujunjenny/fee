// SMDatabase.h: interface for the CSMDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SMDatabase__H__)
#define __SMDatabase__H__

#import "msado15.dll" no_namespace rename("EOF","adoEOF")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "smcmacro.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
#include "SmppDef.h"
#include "globaldef.h"
#include "newmng.h"


const int Tag_Integer = 0;
const int Tag_String = 1;
const int Tag_LongInt = 2;


class CSMDatabase : public CNewMng
{
	
public:
	CSMDatabase();
	virtual ~CSMDatabase();
	
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
	
private:
	//数据库连接指针
	CLinkObject * m_pLink;

	//存储过程名称
	CString m_strLoadFixMsg;
	CString m_strSaveFixMsg;
	CString m_strDelFixMsg;
	CString m_strUpdteMonthFee;
	CString m_strAddSMBuff;
	CString m_strFindSMBuff;
	

	//ADO的COMMAND对象
	_CommandPtr m_pLoadFixMsgCmd;
	_CommandPtr m_pSaveFixMsgCmd;
	_CommandPtr m_pDelFixMsgCmd;
	_CommandPtr m_pUpdateMonthFeeCmd;
	_CommandPtr m_pAddSMBuffCmd;
	_CommandPtr m_pFindSMBuffCmd;	

	//从数据库中提取DoneTime时间的方式 true:状态报告接收的时间false:消息接收的时间
	bool m_DoneTimeSrc;

	//最后一次执行数据库操作的时间
	DWORD m_LastExeDBTime;
	//数据库空闲的最大时间，超过该最大时间，就断开与数据库的连接
	DWORD m_NotExeDBTime;
	
	bool m_bLoadMonthFee;

protected:


	//重载CNewMsg定制执行的操作函数
	void OnTime();
	BOOL OnMountMessage();

	//返回数据连接
	void ReturnLink(CLinkObject *pLink, BOOL bOK = TRUE);
	//打印数据库错误的日志
	void PrintProviderError(LPCTSTR pszMsg, _ConnectionPtr pConnection);
	//初始化ADO COMMAND对象
	void InitCommand();
	//获取DBLink指针
	bool GetDBLink(); 
	
	//保存消息或状态报告为日志
	void DumpSM(LPSTR pData, int len, SaveShortMessage * pSM);
	
	//DB Library方式操作数据库

	//加载配置参数
	void LoadConfig();

};

#endif
