// BillFileMng.cpp: implementation of the CBillFileMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BillFileMng.h"
#include "SmServiceProc.h"
#include "smppdef.h"
#include "Shlwapi.h"
#include "GlobalDef.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBillFileMng::CBillFileMng()
{
	CString Path = ".\\OrgBillPath";
	if (!PathFileExists(Path))
		CreateDirectory(Path, NULL);
	Path = ".\\LocalBillPath";
	if (!PathFileExists(Path))
		CreateDirectory(Path, NULL);
	Path = ".\\RemoteBillPath";
	if (!PathFileExists(Path))
		CreateDirectory(Path, NULL);
}

CBillFileMng::~CBillFileMng()
{
	ClearTempFile();
	Destroy();
}

bool CBillFileMng::SendMessageToQueue(PTRMSG pMsg)
{
	ASSERT(pMsg);
	if(NULL == pMsg)
	{	return false;	}
	
	smpp_Message_header *pMsgHead=(smpp_Message_header*)pMsg;
	if( !SendMessage(pMsg, pMsgHead->Command_Length) )
	{
		return false;	
	}
	
	return true;
}


BOOL CBillFileMng::OnMessage(PTRMSG pMsg)
{
	ASSERT(pMsg);
	smpp_Message_header *pMsgHead = (smpp_Message_header*)pMsg;
	
	switch(pMsgHead->Command_ID)
	{
	case CMD_CLEAR_TEMP_FILE:			// 清除临时目录话单命令
		ClearTempFile();
		break;
	case CMD_SAVE_FILE_BILL:			// 保存话单命令
		SaveBillData(pMsg);
		break;
	default:
		ASSERT(0);
		break;
	}
	
	return TRUE;	
}



BOOL CBillFileMng::SaveBillData(char *pSM)
{
	CString DBDataPath = ".\\";
	CString FileName = ".\\BillData.txt";
	CFileException e;
	
	if (m_BillDataFile.m_hFile == CFile::hFileNull)
	{
		BOOL result = m_BillDataFile.Open(
			FileName,
			CFile::modeCreate |
			CFile::shareDenyNone |
			CFile::modeNoTruncate |
			CFile::modeWrite,
			&e
			);
		
		if (FALSE == result) 
		{
			PrintMessage("The BillData.txt File not opened");
			return FALSE;
		}
	}
	
	
	Message_header *pMH = (Message_header *)pSM;
	m_BillDataFile.SeekToEnd();
	
	switch (pMH->Command_ID)
	{
	case CMD_SAVE_FILE_BILL:
		{
			tagSmsSave *pSaveSM = (tagSmsSave *)(pSM + sizeof(Message_header));
			CString sBill;
			FormatBill(pSaveSM, sBill);
			m_BillDataFile.Write(sBill, sBill.GetLength());
		}
		break;
	default:
		ASSERT(0);
		break;
	}
	
	
	// 判断文件容量
	if (m_BillDataFile.GetLength() >= 2 * 1024 * 1024)
	{
		m_BillDataFile.Close();
		SYSTEMTIME cft;
		char szExt[30];
		GetLocalTime(& cft );
		sprintf(szExt, 
			"%04d%02d%02d%02d%02d%02d%03d", 
			cft.wYear, 
			cft.wMonth, 
			cft.wDay, 
			cft.wHour,
			cft.wMinute,
			cft.wSecond, 
			cft.wMilliseconds);
		
		
		CString BakFileName = "";
		BakFileName.Format(".\\OrgBillPath\\BillData%s.txt", szExt);
		MoveFile(FileName, BakFileName);
	}
	
	return TRUE;
}



VOID CBillFileMng::ClearTempFile()
{		
	if (m_BillDataFile.m_hFile != CFile::hFileNull)
	{
		CString FileName = ".\\BillData.txt";
		m_BillDataFile.Close();
		SYSTEMTIME cft;
		char szExt[30];
		GetLocalTime(& cft );
		sprintf(szExt, 
			"%04d%02d%02d%02d%02d%02d%03d", 
			cft.wYear, 
			cft.wMonth, 
			cft.wDay, 
			cft.wHour,
			cft.wMinute,
			cft.wSecond, 
			cft.wMilliseconds);
		CString BakFileName = "";
		BakFileName.Format(".\\OrgBillPath\\BillData%s.txt", szExt);
		MoveFile(FileName, BakFileName);
	}
}

VOID CBillFileMng::FormatBill(tagSmsSave *pSM, CString& sLine)
{
	CString str;
	CString sToken = "|";
	
	sLine = pSM->Unikey;
	sLine += sToken;
	sLine += pSM->Smid;
	sLine += sToken;
	sLine += pSM->OrgAddr;
	sLine += sToken;
	sLine += pSM->DestAddr;
	sLine += sToken;
	sLine += pSM->submitTime;
	sLine += sToken;
	sLine += pSM->deliverTime;
	sLine += sToken;
	sLine += pSM->UD;
	sLine += sToken;
	str.Format("%d", pSM->MsgType);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->MessageType);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->Status);
	sLine += str;
	sLine += sToken;
	sLine += pSM->serviceID;
	sLine += sToken;
	str.Format("%d", pSM->FeeUserType);
	sLine += str;
	sLine += sToken;
	sLine += pSM->FeeType;
	sLine += sToken;
	sLine += pSM->FeeCode;
	sLine += sToken;
	sLine += pSM->FeeAddr;
	sLine += sToken;
	sLine += pSM->RcvAccount;
	sLine += sToken;
	sLine += pSM->SendAccount;
	sLine += sToken;
	str.Format("%d", pSM->ReportFlag);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->MoFlag);
	sLine += str;
	sLine += sToken;
	sLine += pSM->GivenCode;
	sLine += sToken;
	sLine += pSM->ServiceUpID;
	sLine += sToken;
	sLine += pSM->SpCode;
	sLine += sToken;
	sLine += pSM->FixedFee;
	sLine += sToken;
	sLine += pSM->SMGNo;
	sLine += sToken;
	sLine += pSM->FwdSMGNO;
	sLine += sToken;
	sLine += pSM->SMCNo;
	sLine += sToken;
	sLine += pSM->AreaCode;
	sLine += sToken;
	sLine += pSM->SourceID;
	sLine += sToken;
	str.Format("%d", pSM->Priority);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->MsgLen);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->PayUserType);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->PayFlow);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->PayBefore);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->AuthErrCode);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->PaymntFlag);
	sLine += str;
	sLine += sToken;
	str.Format("%d", pSM->SMType);
	sLine += str;
	sLine += sToken;
	sLine += pSM->FromNo;
	sLine += sToken;
	sLine += pSM->ToNo;
	//add by wj
	sLine += sToken;
	sLine += pSM->SourceIpAdd;

	sLine += sToken;
	str.Format("%d", pSM->SourceCodeType);
	sLine += str;

	sLine += sToken;
	sLine += pSM->SendOrgAddr;
	sLine += sToken;
	sLine += pSM->SendDestAddr;
	sLine += sToken;
	sLine += pSM->SendServiceID;
	sLine += sToken;
	sLine += pSM->SendCPID;

	sLine += CString("|\r\n");
}


BOOL CBillFileMng::OnMountMessage()
{
	return TRUE;
}

void CBillFileMng::OnTime()
{
	
}
