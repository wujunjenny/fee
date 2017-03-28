// Pool.cpp: implementation of the CSockMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SockMsgQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockMsgQueue::CSockMsgQueue(int size)
{
	m_pBuff=new char[size];
	m_DataSize=0;
	m_BuffSize=size;
}

CSockMsgQueue::~CSockMsgQueue()
{
	delete []m_pBuff;
}

LPVOID CSockMsgQueue::GetData(int &size)
{
	size=m_DataSize;
	return m_pBuff;
}


BOOL CSockMsgQueue::OutPool(int size)
{
	if(size>m_DataSize||size<0)
    {   
        ASSERT(0);
		return FALSE;
    }
	memmove(m_pBuff,((char*)m_pBuff)+size,m_DataSize-size);
	m_DataSize-=size;
	return TRUE;
}

BOOL CSockMsgQueue::PutPool(LPVOID pBuff,int size)
{
	if(m_DataSize+size > m_BuffSize||size<0)
    {
        ASSERT(0);
        TRACE("Too many message to pool \n");
		return FALSE;
    }
	memcpy(((char*)m_pBuff)+m_DataSize,pBuff,size);
	m_DataSize+=size;
	return TRUE;
}

int CSockMsgQueue::GetDataSize()
{
	return m_DataSize;
}

int CSockMsgQueue::GetFreeSize()
{
	return m_BuffSize-m_DataSize;
}


void CSockMsgQueue::ClearBuffer()
{
	m_DataSize=0;
}