#include "stdafx.h"
#include <string.h>
//#include <winsock2.h>
#include "tlv.h"

//SMGW42-55, 2005-7-12, jdz, modi begin//
#define STREAM_LEN_MAX 2048
//SMGW42-55, 2005-7-12, jdz, modi end//

CTLV::CTLV() : m_pSTLV(NULL)
{
	
}

CTLV::~CTLV()
{
	STLV* pSTLV = m_pSTLV;
	while(pSTLV)
	{
		m_pSTLV = m_pSTLV ->pNext;
		delete[] pSTLV ->bstrValue;
		delete pSTLV;
		pSTLV = m_pSTLV;
	}
}

int CTLV::AddItem(DWORD dwTag, DWORD dwLen, const BYTE* bstrValue)
{
	STLV* plastSTLV = m_pSTLV;
	STLV* pnewSTLV = new STLV;
	
	if( pnewSTLV == NULL )
		return -1;
	
	pnewSTLV ->dwTag = dwTag;
	pnewSTLV ->dwLen = dwLen;
	pnewSTLV ->bstrValue = new BYTE[dwLen+1];
	memset(pnewSTLV ->bstrValue,0,dwLen+1);
	memcpy(pnewSTLV ->bstrValue, bstrValue, dwLen);
	pnewSTLV ->pNext = NULL;
	
	while(plastSTLV && plastSTLV ->pNext )
		plastSTLV = plastSTLV ->pNext;
	
	if( plastSTLV == NULL)
		m_pSTLV = pnewSTLV;
	else
		plastSTLV ->pNext = pnewSTLV;
	
	return 0;
}

//��������: ȡ��һ��TLV�ֶ�
//
//��ڲ���: dwTag: TLV�ֶε�Tagֵ
//
//����ֵ: value�ĵ�ַ
//        dwLen: ���ö��� ����TLV�ֶε�value�ĳ���  

const BYTE* CTLV::GetItem(DWORD dwTag, DWORD& dwLen)
{
	STLV* pSTLV = m_pSTLV;
	while(1)
	{
		if(pSTLV==NULL)
		{
			return NULL;
		}
		if(pSTLV->dwTag == dwTag)
			break;
		pSTLV = pSTLV->pNext;
	}
	dwLen = pSTLV->dwLen;
	return pSTLV->bstrValue;
}

//�������سɹ���� 0 �ɹ�
//������Ŀ�껺�����㹻������·��سɹ� 0
//���򷵻� -1 ʧ�� �����ı�ԭ���Ļ���������
int CTLV::ToStream(BYTE *pStream, DWORD& dwStreamLength)
{
	WORD wPosTemp = (WORD)dwStreamLength;
	WORD	wTemp;
	STLV* pSTLV = m_pSTLV;
	while(pSTLV!=NULL)
	{
		//�ж��Ƿ񳬳�Ŀ�껺����
		if( wPosTemp + pSTLV->dwLen + 2*sizeof(wTemp) > STREAM_LEN_MAX) 
			return -1;
		
		wTemp = htons((WORD)pSTLV ->dwTag);
		memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
		wPosTemp += sizeof(wTemp);
		
		wTemp = htons((WORD)pSTLV ->dwLen);
		memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
		wPosTemp += sizeof(wTemp);
		
		memcpy(pStream + wPosTemp, pSTLV->bstrValue, pSTLV->dwLen);
		wPosTemp += (WORD)(pSTLV->dwLen);
		
		pSTLV = pSTLV ->pNext;
	}
	
	dwStreamLength = wPosTemp;
	return 0;
}

//�������سɹ���� 0 �ɹ�

//�����ڻ��������Ⱥ�TLV����ƥ������·���0
//���򷵻� -1 ʧ�� ����CTLV�м���ƥ��ĳ�Ա
int CTLV::FromStream(const BYTE *pStream, const DWORD &dwStreamLength, DWORD &dwStreamPosition)
{
	WORD wTagTemp=0, wLenTemp=0;
	while(dwStreamPosition < dwStreamLength)
	{
		memcpy(&wTagTemp, pStream +dwStreamPosition, sizeof(wTagTemp));
		memcpy(&wLenTemp, pStream +dwStreamPosition+ sizeof(wTagTemp) , sizeof(wLenTemp));
		wTagTemp = ntohs(wTagTemp);
		wLenTemp = ntohs(wLenTemp);
		
		if( dwStreamPosition + wLenTemp + sizeof(wTagTemp) + sizeof(wLenTemp) > dwStreamLength ) 
			return -1;
		
		AddItem((DWORD)wTagTemp, (DWORD)wLenTemp, pStream +dwStreamPosition +sizeof(wLenTemp) +sizeof(wTagTemp) );
		
		dwStreamPosition += wLenTemp + sizeof(wLenTemp) + sizeof(wTagTemp);
	}
	return 0;
}



//***SMGW40-01, 2004-12-23, jdz, modi begin***//	  

//��TLV�л�ȡһ��long int �����ݵ�ֵ
long int CTLV::GetLongIntItemValue(DWORD dwTag)
{
	STLV* pSTLV = m_pSTLV;
	while(1)
	{
		if(pSTLV == NULL)
		{
			return 0;
		}
		
		if(pSTLV->dwTag == dwTag)
		{
			break;
		}
		
		pSTLV = pSTLV->pNext;
	}
	
	long int temp;
	memcpy(&temp, pSTLV->bstrValue, sizeof(long int));
	long int temp2 = ntohl(temp);
	
	return temp2;
}


//����һ��long int �����ݵ�TLV��
bool CTLV::AddLongIntItem(DWORD dwTag, const long int bstrValue)
{
	STLV* plastSTLV = m_pSTLV;
	STLV* pnewSTLV = new STLV;
	
	if( pnewSTLV == NULL )
		return false;
	
	pnewSTLV->dwTag = dwTag;
	pnewSTLV->dwLen = sizeof(long int);
	pnewSTLV->bstrValue = new BYTE[sizeof(long int)+1];
	memset(pnewSTLV->bstrValue, 0, sizeof(long int)+1);
	long int temp = htonl(bstrValue);
	//(*(long int*)pnewSTLV->bstrValue) = temp;
	memcpy(pnewSTLV->bstrValue, &temp, sizeof(long int));
	pnewSTLV ->pNext = NULL;
	
	while(plastSTLV && plastSTLV ->pNext )
		plastSTLV = plastSTLV ->pNext;
	
	if( plastSTLV == NULL)
		m_pSTLV = pnewSTLV;
	else
		plastSTLV ->pNext = pnewSTLV;
	
	return true;
}	  


//��������һ��long int �����ݵ�TLV��
bool CTLV::SetLongIntItem(DWORD dwTag, const long int bstrValue)
{
	STLV* pSTLV = m_pSTLV;
	STLV* pPreSTLV = NULL;
	while(1)
	{
		if(pSTLV == NULL)
		{
			//if search fail,add new item
			STLV* pnewSTLV = new STLV;
			if( pnewSTLV == NULL )
				return false;
			
			pnewSTLV->dwTag = dwTag;
			pnewSTLV->dwLen = sizeof(long int);
			pnewSTLV->bstrValue = new BYTE[sizeof(long int)+1];
			memset(pnewSTLV->bstrValue, 0, sizeof(long int)+1);
			long int temp = htonl(bstrValue);
			//(*(long int*)pnewSTLV->bstrValue) = temp;
			memcpy(pnewSTLV->bstrValue, &temp, sizeof(long int));
			pnewSTLV ->pNext = NULL;
			
			if(pPreSTLV == NULL)
			{
				m_pSTLV = pnewSTLV;
			}
			else
			{
				pPreSTLV->pNext = pnewSTLV;
			}
			
			return true;
		}
		
		if(pSTLV->dwTag == dwTag)
		{
			break;
		}
		
		pPreSTLV = pSTLV;
		pSTLV = pSTLV->pNext;
	}
	
	if(sizeof(long int) <= pSTLV->dwLen)
	{
		pSTLV->dwLen = sizeof(long int);
		
		memset(pSTLV ->bstrValue, 0, sizeof(long int)+1);
		long int temp = htonl(bstrValue);
		memcpy(pSTLV ->bstrValue, &temp, sizeof(long int));
	}
	else
	{
		pSTLV->dwLen = sizeof(long int);
		delete[] pSTLV->bstrValue;
		pSTLV->bstrValue = new BYTE[sizeof(long int)+1];
		memset(pSTLV->bstrValue, 0, sizeof(long int)+1);
		long int temp = htonl(bstrValue);
		memcpy(pSTLV ->bstrValue, &temp, sizeof(long int));
	}
	
	return true;
}

//***SMGW40-01, 2004-12-23, jdz, modi end***//
