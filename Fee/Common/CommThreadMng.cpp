///////////////////////////////////////////////////////////////////////////////////
//	���ļ�ʵ��һ���������ϢЭ��ṹ���������Ա
#include "stdafx.h"
#include "CommThreadMng.h"
//#include "msginner.h"
// temp define:
#define MAX_EXT_DATA_LEN	8


#define MSG_HEADER_INNER \
	DWORD nCommand;	\
	DWORD nInterfaceID;	\
	char sExtData[MAX_EXT_DATA_LEN]; \
	DWORD nLen;\
	
typedef struct tag_MI_ONLYHEADER
{
	MSG_HEADER_INNER
} MI_ONLYHEADER;

//////////////////////////////////////////////////////////////////////////////
//	class CCommThreadMng 
CCommThreadMng::CCommThreadMng(UINT nTimerCount)
: CThreadAndTimer(nTimerCount)
{
}

CCommThreadMng::~CCommThreadMng()
{
}

void CCommThreadMng::SendMessageToQueue(PTRMSG pMsg) // ��Ϣ���뺯��
{
	SendMessage(pMsg, ((MI_ONLYHEADER*)pMsg)->nLen+sizeof(MI_ONLYHEADER));
}

