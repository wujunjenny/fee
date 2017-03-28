#include "stdafx.h"
#include "RouteTable.h"

CRouteTable::CRouteTable()
{
	Head=NewNode();
	m_iCount = 0;
}

CRouteTable::~CRouteTable()
{
	DeleteTree(Head);
	delete Head;
}

BOOL CRouteTable::DeleteAllItem()
{
	m_iCount = 0;
	return DeleteTree(Head);
}

BOOL CRouteTable::DeleteTree(ADDRESSNODE* pNode)
{
	for (int i=0; i<CHARKINDSUM; i++)
		if (pNode->SonNode[i]!=NULL)
		{
			DeleteTree(pNode->SonNode[i]);
			delete pNode->SonNode[i];
			pNode->SonNode[i]=NULL;
		};
		return TRUE;
}

ADDRESSNODE* CRouteTable::NewNode()
{
	ADDRESSNODE* pnode=new ADDRESSNODE;
	if (!pnode)
	{
		ASSERT(0);
		return NULL;
	};
	pnode->Handle=ANS_NULLHANDLE;
	for(int i=0; i<CHARKINDSUM; i++) pnode->SonNode[i]=NULL;
	pnode->Parent=NULL;
	return pnode;
}

UCHAR  CRouteTable::GetIndex(char ch)
{
	return (UCHAR)ch;
}

unsigned char CRouteTable::GetChar(int index)
{
    return (unsigned char)index;
}   


BOOL CRouteTable::InsertItem(LPCTSTR pAddress, HANDLE NewItem)
{
	
	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING);
    if (len >= MAX_ADDRESS_STRING)
    {
        return FALSE;
    }
	
	ADDRESSNODE *s,*p=Head;
	UCHAR index=0;
	int i=0;
	
	while (i<len)
	{		
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL) 
		{ p=p->SonNode[index]; i++; continue;}
		else
			for ( ; i<len; i++)
			{
				s=NewNode();
				index=GetIndex(pAddress[i]);
				p->SonNode[index]=s;
				s->Parent=p;
				p=s;
			};
	};
	if (p->Handle!=ANS_NULLHANDLE) return FALSE;
	p->Handle=NewItem;
	m_iCount++;
	return TRUE;
}

HANDLE CRouteTable::GetItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE *p=Head;
	UCHAR index,i=0;
	HANDLE rethdl=p->Handle;//ANS_NULLHANDLE;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			if (p->Handle!= ANS_NULLHANDLE) rethdl=p->Handle; //KEEP the last handle.
			continue;
		}
		else
		{
			if (p->Handle!=ANS_NULLHANDLE) rethdl=p->Handle;
			break;
		};
	}
	return rethdl;
}

HANDLE CRouteTable::ReplaceItem(LPCTSTR pAddress, HANDLE NewItem)
{
	int len=strlen(pAddress);
    ASSERT(len <  MAX_ADDRESS_STRING);
	ADDRESSNODE *s,*p=Head;
	UCHAR index=0;
	int i=0;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ p=p->SonNode[index]; i++; continue;}
		else
			for (; i<len; i++)
			{
				s=NewNode();
				index=GetIndex(pAddress[i]);
				p->SonNode[index]=s;
				s->Parent=p;
				p=s;
			};
	};
	HANDLE rethdl=p->Handle;
	p->Handle=NewItem;
	if (rethdl == ANS_NULLHANDLE)
		m_iCount++;
	return rethdl;
}

BOOL CRouteTable::DeleteItem(LPCTSTR pAddress)
{
	int len=strlen(pAddress);
	ADDRESSNODE *p=Head;
	UCHAR index,i=0;
	while (i<len)
	{
		index=GetIndex(pAddress[i]);
		if (p->SonNode[index]!=NULL)
		{ 
			p=p->SonNode[index];
			i++; 
			continue;
		}
		else
			return FALSE; //NOT FOUND THE ITEM.
	};
	if (p->Handle==ANS_NULLHANDLE) return FALSE;//NOT FOUND THE HANDLE.
	p->Handle=ANS_NULLHANDLE;
	m_iCount--;
	return TRUE;
}

POSITION CRouteTable::GetFirstItem()
{
	ADDRESSNODE *p=Head;
	BOOL bFounded=FALSE;
	while (!bFounded)
	{
		int i;
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
			if (i==CHARKINDSUM) bFounded=TRUE;
	};
	
	//如果表为非空。否则返回NULL
	if (m_iCount > 0)
		return (POSITION)p;
	else
		return NULL;
}

BOOL CRouteTable::GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE* node=(ADDRESSNODE*)pos;
	ADDRESSNODE* p;
	if (node==NULL) return FALSE;
	TCHAR key[MAX_ADDRESS_STRING];
	TCHAR keychar='0';
	int keynum=0;
	while (node->Parent!=NULL)
	{
		p=node->Parent;
		int i;
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]==node)
			{
				keychar=GetChar(i);
				key[keynum++]=keychar;
				node=p;
				break;
			};
			if (i==CHARKINDSUM) return FALSE;
	};
	if (iBuffSize < keynum+1) return FALSE;
	int i;
	for (i=0; i<keynum; i++)
		pRevBuff[i]=key[keynum-i-1];
	pRevBuff[i]=0;
	return TRUE;
}

HANDLE CRouteTable::GetNextItem(POSITION& pos, LPTSTR pRevBuff,int iBuffSize)
{
	ADDRESSNODE* node=(ADDRESSNODE*)pos;
	if (node==NULL) return ANS_NULLHANDLE;
	while(node!=NULL)
		if (node->Handle==ANS_NULLHANDLE) 
			node=GetNextPos(node);
		else 
		{ 
			HANDLE hRet=node->Handle; 
			if ((pRevBuff!=NULL) && (iBuffSize>0))
				if (!GetItemKey((POSITION&)node,pRevBuff,iBuffSize)) *pRevBuff=0;
				
				
				node=GetNextPos(node);
				while (node != NULL)
					if (node->Handle==ANS_NULLHANDLE) 
						node=GetNextPos(node);
					else
						break;
					
					
					pos=(POSITION)node;
					return hRet;
		};
		pos=NULL;
		return ANS_NULLHANDLE;
}

ADDRESSNODE* CRouteTable::GetNextPos(ADDRESSNODE *node)
{
	ADDRESSNODE *p;
	p=node->Parent;
	if (p==NULL) return NULL;
	int i=0;
	while (p->SonNode[i]!=node) i++;
	i++;
	for (; i<CHARKINDSUM; i++) if (p->SonNode[i]!=NULL) break;
	if (i==CHARKINDSUM) return p;
	
	BOOL bFounded=FALSE;
	p=p->SonNode[i];
	while (!bFounded)
	{
		for (i=0; i<CHARKINDSUM; i++)
			if (p->SonNode[i]!=NULL) { p=p->SonNode[i]; break;};
			if (i==CHARKINDSUM) bFounded=TRUE;
	};
	return p;
}


