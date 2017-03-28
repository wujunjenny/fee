///////////////////////////////////////////////////////////////////
//
// ISMP管理系统--ISMGW
//
// Copyright(c) 2005 深圳市协尔信息技术有限公司
//
//-----------------------------------------------------------------
//
// 工　程: SmServiceProc
// 文件名: SMFeeThread.h
// 作　者: 
// 日　期: 2005-4-11
// 描　述: SMFeeThread文件主要包含CSMFeeThread类，此CSMFeeThread主要
//		   处理所有从网关模块发送过来的消息，OnMessage为消息入口函数 
// 版　本: V4.1
//-----------------------------------------------------------------
// 
///////////////////////////////////////////////////////////////////

// CSMFeeThread.h: interface for the CSMFeeThread class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__SMFeeThread__h__)
#define __SMFeeThread__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#import "msado15.dll" no_namespace rename("EOF","adoEOF")
#include "COMMON\CommThreadMng.h"
#include "COMMON\MultiThreadMng.h"
#include "DataConeMng.h"
//#include "sqlObj.h"
#include "AdoObj.h"
#include <string>
#include "tlv.h"
#include "MainProcMng.h"
#include "smppdef.h"
#include "BaseMng.h"
//#include <map>
#include "globaldef.h"
#include "NewMap.h"

#include "icrsint.h"
#include <unordered_map>
#include <memory>
#include <list>


const int NORMAL_DELIVER_MSG = 0;       // 正常的DELIVER消息
const int MONTH_SERVICE_MSG = 2;        // 包月服务的消息
const int MONTH_CHARGE_MSG = 2;         // 包月扣费的消息
const int MONTH_ONLEY_CHARGE_MSG = 8;   // 包月仅用于扣费的消息
const int SP_CHARGE_MSG = 4;            // SP实现计费的消息
const int PAYMEMT_BEFORE_USER = 0;      // 预付费用户
const int PAYMENT_BEHIAND_USER = 1;     // 后付费用户
const int PAYMENT_BEFORE_FLOW = 0;      // 预付费流程
const int PAYMENT_BEHIND_FLOW = 1;      // 后付费流程

const int MO_CHARGE = 1;
const int MT_CHARGE = 2;
const int INFO_CAHRGE = 3;


class CSMFeeThread : public CBaseMng  
{
public:
	CSMFeeThread();
	virtual ~CSMFeeThread();

public:

//////////////////////////////////////////////////////////////////////
// 函数名: OnMessage
// 功　能: 接收所有从网关模块发来的消息并且将消息分发到各个消息处理函数
//		   中
// 返　回: BOOL  - 
// 参　数: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	virtual BOOL OnMessage(PTRMSG pMsg);

//////////////////////////////////////////////////////////////////////
// 函数名: nDeliverSM
// 功　能: 处理网关转发消息成功或失败后发送过来的消息，如果转发成功，则
//		   可能需要将收到的消息缓存到内存或数据库中等待状态报告回执.
// 返　回: O - 
// 参　数: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	void OnDeliverSM(LPCTSTR pMsg);

//////////////////////////////////////////////////////////////////////
// 函数名: OnReportSM
// 功　能: 处理状态报告消息函数，此函数主要功能：
//		   1.根据状态报告找回原来对应的消息，转换消息ID；
//		   2.进行计费处理；
//		   3.根据设置和状态报告是否成功，发送预付费请求
// 返　回: VOID  - 
// 参　数: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnReportSM(LPCTSTR pMsg);
	


//////////////////////////////////////////////////////////////////////
// 函数名: OnCheckFixMsg
// 功　能: 处理包月扣费请求消息
// 返　回: void  - 
// 参　数: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	void OnCheckFixMsg(PTRMSG pMsg);

public:


//////////////////////////////////////////////////////////////////////
// 函数名: OnExpireReport
// 功　能: 接收超时未回状态报告的消息并发送状态报告
// 返　回: VOID  - 
// 参　数: PTRMSG pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnExpireReport(PTRMSG pMsg);

//////////////////////////////////////////////////////////////////////
// 函数名: OnPayConfirmSM
// 功　能: 处理预付费平台扣费结果消息，主要是计费处理并发送状态报告
// 返　回: VOID  - 
// 参　数: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnPayConfirmSM(LPCTSTR pMsg);

//////////////////////////////////////////////////////////////////////
// 函数名: OnLookupRes
// 功　能: 将从数据库缓存中找回的原消息后的处理流程函数
// 返　回: VOID  - 
// 参　数: LPCTSTR pMsg - 
//////////////////////////////////////////////////////////////////////
	VOID OnLookupRes(LPCTSTR pMsg);


	// 定时器函数
	BOOL OnTimer(DWORD timeID, DWORD dwPara);


	// 存储需要回状态报告的数据
	//CNewMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_BufferMap;
	//CMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_BufferMap1;
	std::unordered_map<std::string,std::shared_ptr<SaveShortMessage>> m_BufferMap;
	std::list<std::weak_ptr<SaveShortMessage>> m_time_index;
	// 存储需要等扣费结果的数据
	//CNewMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_PrePayMap;
	CMap<CString, LPCSTR, SaveShortMessage, SaveShortMessage&> m_PrePayMap;
	
protected:
	CPtrList m_CacheReportList;	
	int		 m_SwitchPos[48];
	bool	 m_DoneTimeSrc;
	bool	 m_bconvertUTF8;

//-----SMGW41-39,2005-04-12,zxh,modi begin----//
	DWORD    m_dwSequence_NO;  /*协议序列号*/
//-----SMGW41-39,2005-04-12,zxh,modi end----//


//-----SMGW41-48,2005-04-14,zxh,modi begin----//
	int		m_iMTFCheck;	/*MTF/MTFF类型的包月扣费消息是否进行重单剔除*/
//-----SMGW41-48,2005-04-14,zxh,modi end----//
//-----tianhy add 20051114 begin----//
	//对于网关返回的付费用户类型的判断
	int m_iPayResultErrCode ; 
//-----tianhy add 20051114 end----//
	// 转发状态报告到数据库线程中
	void SendReportToDB(LPCTSTR pMsg);

	//-----SMGW41-39,2005-04-12,zxh,modi begin----//

	// 内存消息超时后转发到数据库缓存
	VOID SendDBBuffer(SaveShortMessage data);
	// 发送写文件话单的方法
	VOID SendWriteFileBill( SaveShortMessage *pData);
	// 包月扣费失败后发送更新包月用户状态消息
	VOID SendDelFixedMsg(SaveShortMessage data);
	// SMGW42-26, 2005-06-01, wzy add begin //
	VOID SendUpdateFeeMonthInfo(tagFeeMonthStatus *pInfo);
	// SMGW42-26, 2005-06-01, wzy add end //



	// 主动发送状态报告到网关模块
	VOID SendStatusReport( SaveShortMessage data );
	// 发送预付费请求到网关模块
	VOID  SendPaymntRequest(SaveShortMessage data);

	//-----SMGW41-39,2005-04-12,zxh,modi end----//

	// 转发状态报告到网关模块
	VOID TranslateStatusReport(smpp_Message_header *pHeader, smpp_Submit_Sm *pSub, SaveShortMessage data);	

	/*
	// 发送包月扣费通知
	VOID SendDeliver(SaveShortMessage data);
	// 校验包月用户状态后转发包月扣费消息到网关模块
	*/

//	VOID ResumeMsg(SaveShortMessage data);


	// 校验状态报告正确的方法
	BOOL CheckStatusReport(SaveShortMessage *pData);
	BOOL CheckStatusReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);
	
	// 取TLV数据函数
	VOID GetTLVItem(CString &strDestString, CTLV &tlv, DWORD tag, int DataType);
	// 取消息内容函数
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	void GetMsgContent(smpp_Submit_Sm* pSub, SaveShortMessage &msg,long RecvID);
	void GetMsgContent(smpp_Submit_Sm* pSub, FEECHECKMSG &msg);
	void SendCheckFixMsgResp(unsigned long int SequenceNo, int ErrCode,long RecevId);
	//SMGW42-55, 2005-7-12, jdz, modi end//

	// 缓存状态报告函数，避免先回状态报告后回ACK的问题
	int GetCacheReport(LPVOID pMsg,int& size);
	void CacheReport(LPCTSTR pMsg);
	BOOL ProcessorReport(LPCTSTR pMsg);

	// 将内存缓存消息转到数据库缓存的消息处理函数，接收界面线程发送的消息
	void OnClearMemoryMsg(PTRMSG pMsg);

	virtual BOOL OnMountMessage();

	// 缓存状态报告的方法
	BOOL CaheNormaldata(SaveShortMessage& data);
	// 发送预付请求后缓存状态报告的方法
	BOOL CahePrepaydata(SaveShortMessage& data);
	// 定时器超时的消息处理函数
	void OnNormalTimeOut(BYTE*pParam2,DWORD param2len);
	void OnPrepayTimeOut(BYTE*pParam2,DWORD param2len);

	// 定时器消息
	virtual void OnTickTime(DWORD nTimeID,DWORD param1,BYTE*pParam2,DWORD param2len);
	virtual void OnTime();	
	
	//-----SMGW42-109,2005-11-19,thy add begin-----//
	//////////////////////////////////////////////////////////////////////
	// 函数名: CacheShortMessage
	// 功　能: 缓存短消息，等待状态报告
	// 返　回: void  - 
	// 参　数:	SaveShortMessage 引用 
	//////////////////////////////////////////////////////////////////////
	//缓存，等待状态报告
	void CacheShortMessage(SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// 函数名: NeedSendPrePay
	// 功　能: 是否发起预付费流程
	// 返　回: TRUE 发起预付费流程；FASLSE 不发起预付费流程
	// 参　数: SaveShortMessage 引用 
	//////////////////////////////////////////////////////////////////////
	BOOL NeedSendPrePay(const SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// 函数名: SendPrePayMsg
	// 功　能: 发起预付费流程
	// 返　回: BOOL  TRUE:  先缓存消息然后 发起预付费流程成功
	//				FALSE:  缓存消息失败 没有发起预付费流程成功
	// 参　数:  SaveShortMessage 引用 
	//////////////////////////////////////////////////////////////////////
	BOOL SendPrePayMsg(SaveShortMessage &data);


	//////////////////////////////////////////////////////////////////////
	// 函数名	:	CheckFixMsg
	// 功　能	:	发起包月扣费重单检查
	// 返　回	:	int --错误码
	//////////////////////////////////////////////////////////////////////
	int CheckFixMsg(LPCSTR pSPID, LPCSTR pServiceID, LPCSTR pFeeAddr, LPCSTR pDestAddr);
	//////////////////////////////////////////////////////////////////////
	// 函数名: SendDeliverResp
	// 功　能: 返回消息返回包
	// 返　回: void
	// 参　数: long int Sequence_No  
	//////////////////////////////////////////////////////////////////////
	void SendDeliverResp(long int Sequence_No,long lSendId);

	//////////////////////////////////////////////////////////////////////
	// 函数名: DeliverStatusFail
	// 功　能: DELIVER消息状态为失败时的处理
	// 返　回: void
	// 参　数:  SaveShortMessage 
	//////////////////////////////////////////////////////////////////////
	void DeliverStatusFail(SaveShortMessage &data);
	
	//////////////////////////////////////////////////////////////////////
	// 函数名: DeliverStatusSuc
	// 功　能: DELIVER消息状态为成功时的处理
	// 返　回: void
	// 参　数:  SaveShortMessage   
	//////////////////////////////////////////////////////////////////////
 	void DeliverStatusSuc(SaveShortMessage &data);
	
	//////////////////////////////////////////////////////////////////////
	// 函数名: ReportDeal
	// 功　能: 状态报告消息处理
	// 返　回: void
	// 参　数:  SaveShortMessage 引用  
	//			smpp_Message_header 指针引用
	//			smpp_Submit_Sm 指针引用
	//////////////////////////////////////////////////////////////////////
	void ReportDeal(smpp_Submit_Sm *&pSub,smpp_Message_header *&pHeader,SaveShortMessage &data);
	void ReportDeal(SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// 函数名: GetSubmitFeeType
	// 功　能: 判断下行的记费类型
	// 返　回:  0:免费
	//			1:按条
	//			2:包月免费
	//			3:封顶
	//			4:包月扣费
	//			5:CDR话单
	//			-1:未知道
	// 参　数:  SaveShortMessage 引用  
	//////////////////////////////////////////////////////////////////////
	int GetSubmitFeeType(const SaveShortMessage &data);

	//////////////////////////////////////////////////////////////////////
	// 函数名: DeliverStatusSucc
	// 功　能: 获取网关转发业务类型
	// 返　回:  int
	//			0:点对点
	//			1：上行
	//			2：下行
	//			-1：未知道类型
	// 参　数:  int 
	//////////////////////////////////////////////////////////////////////
	int GetGWTransferMsgType(int MsgType);

	
	//判断是否下行消息
	BOOL IsMTMsg(int MsgType);		
	//判断上行消息是否是发送给本地SP的
	BOOL MOSMIsLocalSP(int MsgType);
	//从状态报告中获取提交时间，并更新到话单中
	void UpdateSubmitTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);
	//从状态报告中获取处理结束时间，并更新到话单中
	void UpdateDoneTimeFromReport(smpp_Submit_Sm *pSub, SaveShortMessage &data);

	bool SendSaveFixMsgToDB(const LPFIXMSGINFO pSM);

	//-----SMGW42-109,2005-11-19,thy add end-----//
};


//计算包月扣费消息所在的计费月
void CalFixMsgFeeMonth(const char * MsgSubmitTime, char * pFeeMonth);

#endif // !defined(__SMFeeThread__h__)
