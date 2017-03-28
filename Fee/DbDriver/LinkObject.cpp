// LnkObj.cpp: implementation of the CLinkObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LinkObject.h"
#include "datasource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkObject::CLinkObject()
{
	m_pProc = NULL ;
	m_pFather = NULL ;
}

CLinkObject::~CLinkObject()
{
	if(m_pProc)
	{
		DestroyLink();
		m_pProc = NULL ;
	}
}

BOOL CLinkObject::CreateLink()
{
	return FALSE;
}

void CLinkObject::DestroyLink()
{
	return ;
}

BOOL CLinkObject::IsLinkOk()
{
	return FALSE ;
}

CString CLinkObject::GetDataSrc()
{
	ASSERT(m_DataSrc);
	return m_DataSrc;
}

DWORD CLinkObject::GetSrcType()
{
	ASSERT(m_pFather);
	if(m_pFather)
		return m_pFather->GetDataSrcType();
	return 0xFFFFFFFF;
}

void* CLinkObject::GetProc()
{
	return m_pProc ;
}
