#include "stdafx.h"
#include  "smppdef.h"
#include "CodeAndDecode.h"
#include "PaymentSupport.h"
#include "smppCodeAndDecode.h"
#include "GlobalDef.h"






CSmppCodeAndDecode::CSmppCodeAndDecode()
{
	
}

char * G_Bin2HStr( char * inbuf, int inlen )
{
	static char stbuf[ 1024 * 10 ];
	char buf[ 20 ];
	unsigned char a;
	
	memset( stbuf, 0, sizeof( stbuf ) );
	sprintf( stbuf, "%06d: ", 0 );
	
	for ( int i = 0; i < inlen; i ++ )
	{
		a = (unsigned char)(*(inbuf + i));
		sprintf( buf, "%02X ", a );
		strcat( stbuf, buf );
		
		if ( !(i % 16) && i )
		{
			strcat( stbuf, "\n" );
			sprintf( buf, "%06d: ", i );
			strcat( stbuf, buf );
		}
	}
	strcat( stbuf, "\n" );
	
	return stbuf;
}

int CSmppCodeAndDecode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	char **cppOrgBuff;
	char **cppDestBuff;
	Message_header *spMH;
	int nEN,nDeCnt,nPreDestSize;
	
	spMH=( Message_header *)pDestBuff;
	
	if(nOrgSize<(int)(sizeof( org_Message_header))) 
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	cppOrgBuff=(char **)(&pOrgBuff);
	cppDestBuff=(char **)(&pDestBuff);
	// add by lzh
	LPVOID  pOrgStartPos=pOrgBuff;
    LPVOID  pDestStartPos=pDestBuff;
	//end 
	nDeCnt=nOrgSize;
	nPreDestSize=nDestSize;
	
	nEN=fnGetMessageHeader(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
	if(nEN!=DECODE_SUCCEED)
		return(nEN);
	
    if (nOrgSize < spMH->Command_Length)
    {
        ASSERT(0);
        return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
    }
    nOrgSize = spMH->Command_Length;
	
	
	switch(spMH->Command_ID)
	{
	case ESME_BNDRCV:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDRCV_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN:
		nEN=fnGetBindReceiver(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_BNDTRN_RESP:
		nEN=fnGetBindReceiverResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_UBD:
		break;
	case ESME_UBD_RESP:
		break;
	case ESME_SUB_SM:
		//changed by wujun
		{
			nEN=fnGetSubmitSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			
			ASSERT(extlen >= 0);
			smpp_Submit_Sm* pSm = (smpp_Submit_Sm*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			
			
			if(pSm->ext_data_len>0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				nDestSize-=pSm->ext_data_len;
				
			}
			
			
		}
		break;
	case ESME_SUB_SM_RESP:
		nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DELIVER_SM:
		//changed by wujun
		{
			//	PrintMessage("DELIVER DECODE!\n");
			nEN=fnPutDelvrdSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			//changed by lzh
			//	int extlen = spMH->Command_Length-(*cppOrgBuff-pOrgBuff);  //old
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			//end
			ASSERT(extlen >= 0);
			smpp_Submit_Sm* pSm = (smpp_Submit_Sm*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len >0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				
				nDestSize-=pSm->ext_data_len;
				
			}
		}
		
		break;
	case ESME_DELIVER_SM_RESP:
		{
			nEN=fnGetSubmitSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			break;
		}
		
		
	case ESME_SHREPORT_SM:
		{
            nEN=fnGetShReportSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos); //new 
			//end
			ASSERT(extlen >= 0);
			smpp_Submit_Sm* pSm = (smpp_Submit_Sm*)((char*)pDestStartPos+sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len >0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				
				nDestSize-=pSm->ext_data_len;
				
			}
		}
		break;
	case  ESME_SHREPORT_SM_RESP:
		nEN=fnGetShReportSmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		
		if (E_SUCCESS == spMH->Command_Status)
			SendSucCount++;
		else
			SendFailCount++;
		
		break;
	case ESME_PAYCONFIRM_SM:
		nDestSize -=  sizeof(Payment_Confirm_Sm);
		nDeCnt += sizeof(Payment_Confirm_Sm); 
		memcpy(*cppDestBuff, *cppOrgBuff, sizeof(Payment_Confirm_Sm));
		break;
		
	case ESME_PAYMENT_SM_RESP:
		break;
		
		//SMGW42-55, 2005-7-12, jdz, modi begin//
	case ESME_CHECK_VALID_SM:
		{
			nEN=fnPutDelvrdSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
			if(nEN!=DECODE_SUCCEED)
				return(nEN);
			
			int extlen = spMH->Command_Length - (*cppOrgBuff - (char *)pOrgStartPos);
			ASSERT(extlen >= 0);
			smpp_Submit_Sm *pSm = (smpp_Submit_Sm *)((char *)pDestStartPos + sizeof(smpp_Message_header));
			pSm->ext_data_len = extlen;
			if (pSm->ext_data_len > 0)
			{
				memcpy(pSm->ext_data,*cppOrgBuff,pSm->ext_data_len);
				nDestSize-=pSm->ext_data_len;
			}
		}
		break;
		
	case ESME_RESUME_SM_RESP:
		break;
		//SMGW42-55, 2005-7-12, jdz, modi end//
		
	case ESME_QUERY_SM:
		nEN=fnGetQuerySm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_RESP:
		nEN=fnGetQuerySmResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM:
		nEN=fnGetCancelSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_CANCEL_SM_RESP:
		break;
	case ESME_REPLACE_SM:
		nEN=fnGetReplaceSm(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_RESP:
		break;
	case ESME_QRYLINK:
		break;
	case ESME_QRYLINK_RESP:
		break;
	case ESME_NACK:
		break;
		
	case ESME_ADD_SUB:
		nEN=fnGetAddSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_ADD_SUB_RESP:
		break;
	case ESME_DEL_SUB:
		nEN=fnGetDelSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_DEL_SUB_RESP:
		break;
	case ESME_MOD_SUB:
		nEN=fnGetAddSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_MOD_SUB_RESP:
		break;
	case ESME_QRY_SUB:
		nEN=fnGetDelSub(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QRY_SUB_RESP:
		nEN=fnGetEnquireSubResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_REPLACE_SM_EX_RESP:
		break;
	case ESME_REPLACE_SM_EX:
		nEN=fnGetReplaceSmEx(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS:
		nEN=fnGetQuerySms(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SMS_RESP:
		nEN=fnGetQuerySmsResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS:
		nEN=fnGetQuerySmDetails(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_QUERY_SM_DETAILS_RESP:
		nEN=fnGetQuerySmDetailsResp(cppOrgBuff,nDeCnt,cppDestBuff,nDestSize);
		if(nEN!=DECODE_SUCCEED)
			return(nEN);
		break;
		
        
	case ESME_PPS_USER_CHECK:
		nEN=fnGetPpsUserCheck(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_USER_CHECK_RESP:
		nEN=fnGetPpsUserCheckResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY:
		nEN=fnGetPpsResultNotify(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
	case ESME_PPS_RESULT_NOTIFY_RESP:
		nEN=fnGetPpsResultNotifyResp(cppOrgBuff, nDeCnt, cppDestBuff, nDestSize);
		if(nEN != DECODE_SUCCEED)
			return(nEN);
		break;
        ///////////////////////////////////////////////////
        //end add
        ///////////////////////////////////////////////////
	default:
		//协议中没有此类型的命令,返回协议不支持
		return(DECODE_PROTOCOL_ERROR);
	}
	
	//nOrgSize=nDeCnt;
    nOrgSize = spMH->Command_Length;
    //END CHANGE
	nDestSize=nPreDestSize-nDestSize;
	//changed by wujun
	spMH->Command_Length=nDestSize;
	//end chang
	
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Replace_Sm_Ex *pRS;
	nDestSize-=sizeof( Replace_Sm_Ex);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Replace_Sm_Ex *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->original_message_id,sizeof(pRS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->data_coding,sizeof(pRS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Replace_Sm_Ex);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Query_Sms *pRS;
	nDestSize-=sizeof( Query_Sms);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sms *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->direction,sizeof(pRS->direction))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->ton,sizeof(pRS->ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->npi,sizeof(pRS->npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->addr,sizeof(pRS->addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->start_time,sizeof(pRS->start_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->final_time,sizeof(pRS->final_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->number,sizeof(pRS->number))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sms);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)

{
	Query_Sms_Resp *pRS;
	nDestSize-=sizeof( Query_Sms_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sms_Resp *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->direction,sizeof(pRS->direction))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->number,sizeof(pRS->number))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(pRS->number>MAX_SM_NUM)
		return(DECODE_PROTOCOL_ERROR);
	for(int i=0;i<pRS->number;i++)
	{
		if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_ID[i],sizeof(pRS->sm_ID[i]))==DECODE_PROTOCOL_ERROR)
			return(DECODE_PROTOCOL_ERROR);
	}
	
	(*cppDestValid)+=sizeof( Query_Sms_Resp);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Query_Sm_Details *pRS;
	nDestSize-=sizeof( Query_Sm_Details);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sm_Details *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_ID,sizeof(pRS->sm_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_ton,sizeof(pRS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_npi,sizeof(pRS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sm_Details);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Query_Sm_Details_Resp *pRS;
	nDestSize-=sizeof( Query_Sm_Details_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Query_Sm_Details_Resp *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->service_type,sizeof(pRS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_ton,sizeof(pRS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_npi,sizeof(pRS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_ton,sizeof(pRS->dest_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_npi,sizeof(pRS->dest_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->dest_addr,sizeof(pRS->dest_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->priority_flag,sizeof(pRS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->protocol_ID,sizeof(pRS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->data_coding,sizeof(pRS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->final_date,sizeof(pRS->final_date))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_id,sizeof(pRS->message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_status,sizeof(pRS->message_status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->GSM_code,sizeof(pRS->GSM_code))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->message_length,sizeof(pRS->message_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->message_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Query_Sm_Details_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	int nOrgSize;
	nOrgSize=nDeCnt;
	nDeCnt=0;
	Message_header *pMH;
	nDestSize-=sizeof( org_Message_header);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pMH=( Message_header *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Length ,sizeof(pMH->Command_Length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_ID,sizeof(pMH->Command_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Command_Status,sizeof(pMH->Command_Status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pMH->Sequence_No,sizeof(pMH->Sequence_No))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if((pMH->Command_Length)>MAX_SMPP_PACKET_LENGTH||(pMH->Command_Length)<(int)(sizeof(org_Message_header)))
		return (DECODE_SMPP_HEADER_ERROR);
	
	if((pMH->Command_Length)>nOrgSize)
		return(ESME_DECODE_NO_ENOUGHT_ORG_BUF);
	
	(*cppDestValid)+=sizeof( Message_header);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Bind_Receiver *pBR;
	nDestSize-=sizeof( Bind_Receiver);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBR=( Bind_Receiver *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->system_id,sizeof(pBR->system_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->password,sizeof(pBR->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->system_type,sizeof(pBR->system_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->interface_version,sizeof(pBR->interface_version))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->addr_ton,sizeof(pBR->addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->addr_npi,sizeof(pBR->addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBR->address_range,sizeof(pBR->address_range))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Bind_Receiver);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Bind_Receiver_Resp *pBRR;
	nDestSize-=sizeof( Bind_Receiver_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pBRR=( Bind_Receiver_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pBRR->system_id,sizeof(pBRR->system_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Bind_Receiver_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Submit_Sm_Resp *pSSR;
	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSSR->Message_id,sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)

{
	Query_Sm *pQS;
	nDestSize-=sizeof( Query_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pQS=( Query_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->original_message_id,sizeof(pQS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_ton,sizeof(pQS->originating_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_npi,sizeof(pQS->originating_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQS->originating_addr,sizeof(pQS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Query_Sm);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Query_Sm_Resp *pQSR;
	nDestSize-=sizeof( Query_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pQSR=( Query_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->original_message_id,sizeof(pQSR->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->final_date,sizeof(pQSR->final_date))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->message_status,sizeof(pQSR->message_status))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pQSR->GSM_code,sizeof(pQSR->GSM_code))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Query_Sm_Resp);	
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)

{
	Cancel_Sm *pCS;
	nDestSize-=sizeof( Cancel_Sm);
	pCS=( Cancel_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->service_type,sizeof(pCS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->original_message_id,sizeof(pCS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr_ton,sizeof(pCS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr_npi,sizeof(pCS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->source_addr,sizeof(pCS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->dest_addr_ton,sizeof(pCS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->dest_addr_npi,sizeof(pCS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pCS->destination_addr,sizeof(pCS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Cancel_Sm);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)

{
	Replace_Sm *pRS;
	nDestSize-=sizeof( Replace_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pRS=( Replace_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->original_message_id,sizeof(pRS->original_message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_ton,sizeof(pRS->orig_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->orig_addr_npi,sizeof(pRS->orig_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->originating_addr,sizeof(pRS->originating_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->schedule_delivery_time,sizeof(pRS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->validity_period,sizeof(pRS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->registered_delivery_flag,sizeof(pRS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_default_msg_id,sizeof(pRS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pRS->sm_length,sizeof(pRS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	//if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,sizeof(pRS->short_message))==DECODE_PROTOCOL_ERROR)
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pRS->short_message,(UC)pRS->sm_length)==DECODE_PROTOCOL_ERROR)
		/////////end change 
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Replace_Sm);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)

{
	Enquire_Sub_Resp *pAS;
	nDestSize-=sizeof( Enquire_Sub_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pAS=( Enquire_Sub_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ID,sizeof(pAS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_name,sizeof(pAS->sub_name))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_address,sizeof(pAS->sub_address))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->MS_type,sizeof(pAS->MS_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ton,sizeof(pAS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_npi,sizeof(pAS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->service_level,sizeof(pAS->service_level))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->SM_allowed,sizeof(pAS->SM_allowed))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->OCOS,sizeof(pAS->OCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->TCOS,sizeof(pAS->TCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->password,sizeof(pAS->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->reg_datetime,sizeof(pAS->reg_datetime))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Add_Sub);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Add_Sub *pAS;
	nDestSize-=sizeof( Add_Sub);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pAS=( Add_Sub *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ID,sizeof(pAS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_name,sizeof(pAS->sub_name))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_address,sizeof(pAS->sub_address))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->MS_type,sizeof(pAS->MS_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_ton,sizeof(pAS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->sub_npi,sizeof(pAS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->service_level,sizeof(pAS->service_level))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->SM_allowed,sizeof(pAS->SM_allowed))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->OCOS,sizeof(pAS->OCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->TCOS,sizeof(pAS->TCOS))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pAS->password,sizeof(pAS->password))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Add_Sub);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Del_Sub *pDS;
	nDestSize-=sizeof( Del_Sub);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pDS=( Del_Sub *)(*cppDestValid);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_ton,sizeof(pDS->sub_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_npi,sizeof(pDS->sub_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pDS->sub_ID,sizeof(pDS->sub_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid)+=sizeof( Del_Sub);
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,long int &nDestValid,int size)
{
	int i;
	char *cpDestValid;
	cpDestValid=(char *)(&nDestValid);
	size=sizeof(nDestValid);
	cpDestValid+=size-1;
	nDeCnt+=size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
	}
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,UL &nDestValid,int size)
{
	int i;
	char *cpDestValid;
	cpDestValid=(char *)(&nDestValid);
	size=sizeof(nDestValid);
	cpDestValid+=size-1;
	nDeCnt+=size;
	for(i=0;i<size;i++)
	{
		*cpDestValid=**cppOrgBuff;
		(*cppOrgBuff)++;
		cpDestValid--;
	}
	return(DECODE_SUCCEED);
}


int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int &nDeCnt,unsigned char &cDestValid,int size)
{
	nDeCnt+=sizeof(cDestValid);
	cDestValid=**cppOrgBuff;
	(*cppOrgBuff)+=sizeof(cDestValid);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetFromCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=**cppOrgBuff;
		*cpDestValid=c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if(c=='\0')
			break;
	}
	if(i>=size)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}
int CSmppCodeAndDecode::fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
{
	int i;
	char c1,c2;
	for(i=0;i<size/2;i++)
	{
		c1=**cppOrgBuff;
		*cpDestValid=c1;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		c2=**cppOrgBuff;
		*cpDestValid=c2;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
		if(c1=='\0' && c2=='\0')
			break;
	}
	if(i>=size/2)
		return(DECODE_PROTOCOL_ERROR);
	else
		return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size)
{
	int i;
	char c;
	for(i=0;i<size;i++)
	{
		c=**cppOrgBuff;
		*cpDestValid=c;
		cpDestValid++;
		(*cppOrgBuff)++;
		nDeCnt++;
	}
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnPutPpsUserCheck(\
										  char **cppDestBuff, int& nDestSize,\
										  char **cppOrgValid, int& nOrgSize)
{
    Pps_User_Check *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_User_Check);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_User_Check *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulVersion, sizeof(pDS->ulVersion));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSMSCAddress, sizeof(pDS->sSMSCAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSourceAddress, sizeof(pDS->sSourceAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sDestinationAddress, sizeof(pDS->sDestinationAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byMOMTFlag, sizeof(pDS->byMOMTFlag));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmID, sizeof(pDS->ulSmID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmLength, sizeof(pDS->ulSmLength));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulServiceID, sizeof(pDS->ulServiceID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof(Pps_User_Check);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsUserCheckResp(\
											  char **cppDestBuff, int& nDestSize,\
											  char **cppOrgValid, int& nOrgSize)
{
    Pps_User_Check_Resp *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_User_Check_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_User_Check_Resp *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulOperationResult, sizeof(pDS->ulOperationResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byNotifyMode, sizeof(pDS->byNotifyMode));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	(*cppOrgValid) += sizeof(Pps_User_Check_Resp);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsResultNotify(\
											 char **cppDestBuff, int& nDestSize,\
											 char **cppOrgValid, int& nOrgSize)
{
    Pps_Result_Notify *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_Result_Notify);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_Result_Notify *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulVersion, sizeof(pDS->ulVersion));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSMSCAddress, sizeof(pDS->sSMSCAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sSourceAddress, sizeof(pDS->sSourceAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->sDestinationAddress, sizeof(pDS->sDestinationAddress));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->byMOMTFlag, sizeof(pDS->byMOMTFlag));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmID, sizeof(pDS->ulSmID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSmLength, sizeof(pDS->ulSmLength));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulSendResult, sizeof(pDS->ulSendResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
    
    nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulServiceID, sizeof(pDS->ulServiceID));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof(Pps_Result_Notify);
	
    return( CODE_SUCCEED );
}
int CSmppCodeAndDecode::fnPutPpsResultNotifyResp(\
												 char **cppDestBuff, int& nDestSize,\
												 char **cppOrgValid, int& nOrgSize)
{
    Pps_Result_Notify_Resp *pDS;
	int nEN;
	nOrgSize -= sizeof( Pps_Result_Notify_Resp);
	if (nOrgSize < 0)
		return(ESME_CODE_NO_ENOUGHT_ORG_BUF);
	pDS = ( Pps_Result_Notify_Resp *)(*cppOrgValid);
	
	
	nEN = fnPutToCode( cppDestBuff, nDestSize, \
        pDS->ulOperationResult, sizeof(pDS->ulOperationResult));
	if (nEN != CODE_SUCCEED) 
		return( nEN );
	
	(*cppOrgValid) += sizeof( Pps_Result_Notify_Resp);
	
    return( CODE_SUCCEED );
}


///////////////////////////////////////////////
//GetFun
int CSmppCodeAndDecode::fnGetPpsUserCheck(char **cppOrgBuff, int& nDeCnt,\
										  char **cppDestValid, int& nDestSize)
{
	
    Pps_User_Check *pOutput;
	nDestSize -= sizeof(Pps_User_Check);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_User_Check *)(*cppDestValid);
	
	if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulVersion, sizeof(pOutput->ulVersion)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSMSCAddress, \
        sizeof(pOutput->sSMSCAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSourceAddress, \
        sizeof(pOutput->sSourceAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sDestinationAddress, \
        sizeof(pOutput->sDestinationAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byMOMTFlag, \
        sizeof(pOutput->byMOMTFlag)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmID, \
        sizeof(pOutput->ulSmID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmLength, \
        sizeof(pOutput->ulSmLength)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulServiceID, \
        sizeof(pOutput->ulServiceID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_User_Check );
	return(DECODE_SUCCEED);
	
}

int CSmppCodeAndDecode::fnGetPpsUserCheckResp(char **cppOrgBuff, int& nDeCnt,\
											  char **cppDestValid, int& nDestSize)
{
    Pps_User_Check_Resp *pOutput;
	nDestSize -= sizeof(Pps_User_Check_Resp);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_User_Check_Resp *)(*cppDestValid);
	
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulOperationResult, \
        sizeof(pOutput->ulOperationResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byNotifyMode, \
        sizeof(pOutput->byNotifyMode)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
	(*cppDestValid) += sizeof( Pps_User_Check_Resp );
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetPpsResultNotify(char **cppOrgBuff, int& nDeCnt,\
											 char **cppDestValid, int& nDestSize)
{
    Pps_Result_Notify *pOutput;
	nDestSize -= sizeof(Pps_Result_Notify);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_Result_Notify *)(*cppDestValid);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulVersion, \
        sizeof(pOutput->ulVersion)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSMSCAddress, \
        sizeof(pOutput->sSMSCAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sSourceAddress, \
        sizeof(pOutput->sSourceAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->sDestinationAddress, \
        sizeof(pOutput->sDestinationAddress)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->byMOMTFlag, \
        sizeof(pOutput->byMOMTFlag)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmID, \
        sizeof(pOutput->ulSmID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSmLength, \
        sizeof(pOutput->ulSmLength)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulSendResult, \
        sizeof(pOutput->ulSendResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
    
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulServiceID, \
        sizeof(pOutput->ulServiceID)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_Result_Notify );
	return(DECODE_SUCCEED);
}
int CSmppCodeAndDecode::fnGetPpsResultNotifyResp(char **cppOrgBuff, int& nDeCnt,\
												 char **cppDestValid, int& nDestSize)
{
    Pps_Result_Notify_Resp *pOutput;
	nDestSize -= sizeof(Pps_Result_Notify_Resp);
	
	if(nDestSize < 0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	
	pOutput=( Pps_Result_Notify_Resp *)(*cppDestValid);
	
	
    if (fnGetFromCode(cppOrgBuff, nDeCnt, \
        pOutput->ulOperationResult, \
        sizeof(pOutput->ulOperationResult)) == DECODE_PROTOCOL_ERROR)
		return (DECODE_PROTOCOL_ERROR);
	
	(*cppDestValid) += sizeof( Pps_Result_Notify_Resp );
	return(DECODE_SUCCEED);
}



int CSmppCodeAndDecode::fnPutDelvrdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	
	
	
	
	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}
int CSmppCodeAndDecode::fnGetShReportSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Submit_Sm_Resp *pSSR;
	nDestSize-=sizeof( Submit_Sm_Resp);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSSR=( Submit_Sm_Resp *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSSR->Message_id,sizeof(pSSR->Message_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm_Resp);
	return(DECODE_SUCCEED);
}

int CSmppCodeAndDecode::fnGetShReportSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize)
{
	Submit_Sm *pSS;
	nDestSize-=sizeof( Submit_Sm);
	if(nDestSize<0)
		return(ESME_DECODE_NO_ENOUGHT_DEST_BUF);
	pSS=( Submit_Sm *)(*cppDestValid);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->service_type,sizeof(pSS->service_type))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_ton,sizeof(pSS->source_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr_npi,sizeof(pSS->source_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->source_addr,sizeof(pSS->source_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_ton,sizeof(pSS->dest_addr_ton))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->dest_addr_npi,sizeof(pSS->dest_addr_npi))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->destination_addr,sizeof(pSS->destination_addr))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->esm_class,sizeof(pSS->esm_class))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->protocol_ID,sizeof(pSS->protocol_ID))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->priority_flag,sizeof(pSS->priority_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->schedule_delivery_time,sizeof(pSS->schedule_delivery_time))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->validity_period,sizeof(pSS->validity_period))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->registered_delivery_flag,sizeof(pSS->registered_delivery_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->replace_if_present_flag,sizeof(pSS->replace_if_present_flag))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->data_coding,sizeof(pSS->data_coding))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_default_msg_id,sizeof(pSS->sm_default_msg_id))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	if(fnGetFromCode(cppOrgBuff,nDeCnt,pSS->sm_length,sizeof(pSS->sm_length))==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	
	DWORD unicodeflag=pSS->data_coding;
	unicodeflag=unicodeflag&DCSMASK;
	
	if(fnGetFromOctCode(cppOrgBuff,nDeCnt,pSS->short_message,(UC)pSS->sm_length)==DECODE_PROTOCOL_ERROR)
		return(DECODE_PROTOCOL_ERROR);
	(*cppDestValid)+=sizeof( Submit_Sm);
	return(DECODE_SUCCEED);
}



