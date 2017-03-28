// SmppDrvMng.cpp: implementation of the CSmppDrvMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmServiceProc.h"

#include "SmppDrvMng.h"
#include "smppdef.h"
#include "SmppCodeAndDecode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmppDrvMng::CSmppDrvMng()
{
	m_hIF=NULL;
	m_hTempIF=NULL;
	
	//SMGW40-10, 2005-3-5, jdz change begin//
	m_nState = smpp_broken;
	//SMGW40-10, 2005-3-5, jdz change end//
}

CSmppDrvMng::~CSmppDrvMng()
{
	if(m_hIF!=NULL)
		KillIF(m_hIF);
	if(m_hTempIF!=NULL)
		KillIF(m_hTempIF);
	
	
}


int CSmppDrvMng::GetState()
{
	return m_nState;
}

void CSmppDrvMng::SetState(int state)
{
	m_nState = state;
}

BOOL CSmppDrvMng::Init(LPCTSTR pAddress,WORD port)
{
/*
	if(m_hIF !=NULL)
	{
		KillIF(m_hIF);
		m_hIF=NULL;
	}
	if(m_hTempIF!=NULL)
	{
		KillIF(m_hTempIF);
		m_hTempIF=NULL;
	}
	
	m_hTempIF=CreateTcpIF(pAddress,port,FALSE,4000);
	
	if(m_hTempIF)
	{
		SetState(smpp_connecting);
		return TRUE;
	}
	else
	{
		SetState(smpp_broken) ;
		return FALSE;
	}
	//SMGW40-10, 2005-3-5, jdz change end//	
*/	
	return TRUE;
}

BOOL CSmppDrvMng::Send(smpp_Message_header* pMsg)
{
	HANDLE hIF;
	if(pMsg->nSndID == 0)
	{
		if(m_hIF==NULL)
			return FALSE;
		else
		  hIF = m_hIF;

	}
	else
	{
		hIF = (HANDLE)pMsg->nSndID;
	}
	/*
	char tranMsg[SMPP_MESSAGE_LEN + 500];
	memset(tranMsg, 0, sizeof(tranMsg));

	org_Message_header* pMsgHeader = (org_Message_header*)tranMsg;
	char* pMsgSubdes = (char*)(tranMsg + sizeof(org_Message_header));

	pMsgHeader->Command_ID = pMsg->Command_ID;
	pMsgHeader->Command_Length = pMsg->Command_Length - 2*sizeof(long int);
	pMsgHeader->Command_Status = pMsg->Command_Status;
	pMsgHeader->Sequence_No = pMsg->Sequence_No;

	memcpy( pMsgSubdes,(char *)pMsg + sizeof(Message_header),pMsg->Command_Length - 2*sizeof(long int));
	*/
	char* pOutputMsg = new char[4016];
	int outputsize = 4016;
	
	BOOL ret;
	
	if(CODE_SUCCEED!=decode.Code(pMsg,\
		*(int*)&(pMsg->Command_Length),\
		pOutputMsg,\
		outputsize))
	{
		delete []pOutputMsg;
		return FALSE;
	}
	
	ret=CSockMng::Send(hIF,pOutputMsg,outputsize);
	delete []pOutputMsg;
	return ret;
}

void CSmppDrvMng::Destroy()
{
/*
	if(m_hIF!=NULL)
	{
		KillIF(m_hIF);
		m_hIF=NULL;
	}
	if(m_hTempIF!=NULL)
	{
		KillIF(m_hTempIF);
		m_hTempIF=NULL;
		
	}
*/
	CSockMng::Destroy();
}

int CSmppDrvMng::OnRead( HANDLE hIFHandle,	//接收接口句柄
						LPVOID pBuff,		//接收缓冲区
						int size			//接收缓冲区长度
						)
{
	//ASSERT(m_hIF==hIFHandle);
	//CSmppCodeAndDecode decode;
	char* pOutMsg =new char[4096];
	int outmsgsize=4096;
	int ret;
	
	ret = decode.Decode(pBuff,size,pOutMsg,outmsgsize);
	switch (ret)
	{
	case CODE_SUCCEED:
		{

            smpp_Message_header* pHeader = (smpp_Message_header*)pOutMsg;
			TRACE("CSmppDrvMng::OnRead,msg[%x]\r\n",pHeader->Command_ID);
//modify by gyx
			pHeader->nRcvID = (long int)hIFHandle;
			pHeader->nSndID = 0;
//end
            switch(pHeader->Command_ID)
            {
            case ESME_DELIVER_SM:
			case ESME_SHREPORT_SM:
			case ESME_PAYCONFIRM_SM:
			case ESME_PAYMENT_SM_RESP:
				// *** SMGW30-21,2004-09-03,wzy add begin *** //
			case ESME_CHECK_VALID_SM:
				// *** SMGW30-21,2004-09-03,wzy add end *** //
				if(((CSmServiceProcApp *)AfxGetApp())->pMainMng->m_pSmsThread)
				{
					
					((CSmServiceProcApp *)AfxGetApp())->pMainMng->m_pSmsThread->SendMessage(pOutMsg,outmsgsize + 2*sizeof(pHeader->nRcvID));
				}
				
                break;
				
            default:
				((CSmServiceProcApp*)AfxGetApp())->pMainMng->SendMessage(pOutMsg,outmsgsize+2*sizeof(pHeader->nRcvID));
                break;
            }
			delete []pOutMsg;
			return size;
		}
		break;

	case CODE_NO_ENOUGH_ORG_BUF:
		{
			delete []pOutMsg;
			return 0;
		}
		break;

	default:
		delete []pOutMsg;
		return size;
	}

	delete []pOutMsg;
	return 0;
}
						
						
int CSmppDrvMng::OnAccept( HANDLE hIFHandle ,		//接口句柄
						  LPCTSTR pRemoteAddress,	//对端地址
						  int AddrSize,				//对端地址缓冲区大小
						  DWORD dwCodeType			//编码协议，此为创建SERVER时设置的编码协议
						  )
{
	ASSERT(0);
	return 0;
}

int CSmppDrvMng::OnConnect( 
						   
						   
						   HANDLE hIFHandle //接口句柄
						   )
{
/*
	ASSERT(m_hIF==NULL);
	if(m_hTempIF==hIFHandle)
		m_hTempIF=NULL;
	m_hIF=hIFHandle;
	SetState(smpp_logining);
*/
	((CSmServiceProcApp*)AfxGetApp())->pMainMng->OnConnect(hIFHandle);
	// add by lzh
	((CSmServiceProcApp*)AfxGetApp())->GetDlg()->OnLogin();
	// add end	
	return 0;
}

int CSmppDrvMng::OnError( 
						 HANDLE hIFHandle ,	//接口句柄
						 int ErrorCode,		//错误原因
						 BOOL bRecover		//重连标志
						 )
{
/*
	if(m_hIF==hIFHandle)
	{
		//SMGW40-10, 2005-3-5, jdz change begin//
		if(m_hIF != NULL)
		{
			KillIF(m_hIF);
		}
		//SMGW40-10, 2005-3-5, jdz change end//
		
		m_hIF = NULL;
		SetState(smpp_broken);
		// add by lzh
		((CSmServiceProcApp*)AfxGetApp())->GetDlg()->OnBroken();
		// add end
		return 0;
	}
	//add by wj
	if(hIFHandle == m_hTempIF)
	{
		KillIF(hIFHandle);
		m_hTempIF = NULL;
		SetState(smpp_broken);
		// add by lzh
		((CSmServiceProcApp*)AfxGetApp())->GetDlg()->OnBroken();
		// add end
		return 0;
	}
*/    
	((CSmServiceProcApp*)AfxGetApp())->pMainMng->OnError(hIFHandle,ErrorCode);
	((CSmServiceProcApp*)AfxGetApp())->GetDlg()->OnBroken();

	//KillIF(hIFHandle);
	//end add
	return 0;
}

int CSmppDrvMng::OnTrace( 
						 HANDLE hIFHandle ,	//接口句柄
						 BOOL bIn ,			//消息方向
						 int size,			//消息长度
						 LPVOID pBuff		//消息体
						 )
{
	return 0;
}

VOID CSmppDrvMng::SendDeliverAck(smpp_Message_header *pMsg, char * mgid)
{
    smpp_Message_header* pHeader = (smpp_Message_header*)pMsg;
    smpp_Submit_Sm* pSub = (smpp_Submit_Sm*)((CHAR*)pMsg+sizeof(smpp_Message_header));
	
    smpp_Message_header ResHead;
    smpp_Submit_Sm_Resp  ResSub;
	
	ResHead.Command_ID =ESME_DELIVER_SM_RESP;
	ResHead.Command_Length = sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp);
	ResHead.Command_Status = E_SUCCESS;
	ResHead.Sequence_No =pHeader->Sequence_No; 
//modify by gyx
	ResHead.nRcvID = pHeader->nRcvID;
	ResHead.nSndID = pHeader->nSndID;
//end
	if ( mgid )
		sprintf(ResSub.Message_id ,"%s", mgid); 
	else
		sprintf(ResSub.Message_id ,"0"); 
    
	
	char* ResMsg = new char [sizeof(smpp_Message_header)+sizeof(smpp_Submit_Sm_Resp)];
    memset(ResMsg,0,sizeof(ResMsg)); 
    memcpy(ResMsg,&ResHead,sizeof(ResHead));
    memcpy(ResMsg+sizeof(smpp_Message_header),&ResSub,sizeof(ResSub));
	
	Send((smpp_Message_header*)ResMsg);
    delete []ResMsg;
	
}
