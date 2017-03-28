// BillFileMng.h: interface for the CBillFileMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BILLFILEMNG_H__F7F373DA_3C36_430E_9A17_65A28918B430__INCLUDED_)
#define AFX_BILLFILEMNG_H__F7F373DA_3C36_430E_9A17_65A28918B430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "COMMON\ThreadMng.h"
#include "DataConeMng.h"
#include "Globaldef.h"
#include "NewMng.h"



class CBillFileMng : public CNewMng
{
public:
	BOOL OnMessage(PTRMSG pMsg);
	bool SendMessageToQueue(PTRMSG pMsg);


	CBillFileMng();
	virtual ~CBillFileMng();
	virtual BOOL OnMountMessage();
	virtual void OnTime();
	
protected:
	VOID FormatBill(tagSmsSave *pSM, CString& sLine);
	CDataConeMng * m_LinkMng;
	CFile m_BillDataFile;
	BOOL SaveBillData(char *pSM);
	VOID ClearTempFile();
};

#endif
