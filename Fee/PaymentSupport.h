
/////////////////////////////////////////////////////////////////////////////
//
//  FileName: PaymentSupport.h
//
//  Description:支持预付费的头文件
//
//  author: wenzhaoyang 2003-08-01
//
/////////////////////////////////////////////////////////////////////////////


#ifndef    __PaymentSupport__h__

#define    __PaymentSupport__h__



#define UC    unsigned char

struct Payment_Request_Sm
{
    char DestMsgID[65];
	char OrigMsgID[65];
	WORD   MsgType;
	char ChargeTermID[21];
	char CPID[10];
	char DestTermID[21];
	char ServiceID[10];
    char FeeType[2];
	char FeeCode[6];
	char FixedFee[6];
	UC   Priority;
	UC   MsgLen;
	char AreaCode[4];
	char SMGNo[6];
	char FwdSMGNo[6];
	char SMCNo[6];
	char SubmitTime[14];
	char DoneTime[14];
	UC CDRType;
	char szOrgAdd[21];
	UC SMType;
	UC FwdMsgType;
	char Smid[65];
	char PreNwkNd[6];
	char NxtNwkNd[6];	
};

// SMGW431-06, 2005-10-27, wzy modify begin //
//typedef struct Payment_Confirm_Sm
struct Payment_Confirm_Sm
// SMGW431-06, 2005-10-27, wzy modify end //

{
	char DestMsgID[40];
	UC   ResultNotifyCode;
	UL   Status;
};



#endif