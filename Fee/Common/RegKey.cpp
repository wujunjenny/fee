// RegKey.cpp: implementation of the CSMRegKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "DBLink.h"
#include "RegKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMRegKey::CSMRegKey()
{
	m_hKey = NULL ;
}

CSMRegKey::~CSMRegKey()
{
	Close() ;
}

LONG CSMRegKey::Open(HKEY hKeyRoot, LPCTSTR pszPath)
{
	DWORD dw ;
	m_sPath = pszPath ;
	return RegCreateKeyEx(hKeyRoot,pszPath , 0L, NULL, 
		REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,
		&m_hKey , &dw);
}

VOID CSMRegKey::Close()
{
	if(m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL ;
	}
}

LONG CSMRegKey::Write(LPCTSTR pszKey, DWORD dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	return RegSetValueEx(m_hKey ,pszKey , 0L,REG_DWORD,
		(const BYTE*)&dwVal , sizeof(DWORD));
}

LONG CSMRegKey::Write(LPCTSTR pszKey, LPCTSTR pszVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pszVal);
	ASSERT(AfxIsValidAddress(pszVal,strlen(pszVal),FALSE));
	return RegSetValueEx(m_hKey,pszKey,0L,REG_SZ,
		(const BYTE*)pszVal,strlen(pszVal)+1);
}

LONG CSMRegKey::Write(LPCTSTR pszKey, const BYTE * pData, DWORD dwLength)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pData && dwLength >0);
	ASSERT(AfxIsValidAddress(pData,dwLength,FALSE));
	return RegSetValueEx(m_hKey,pszKey,0L,REG_BINARY,pData,dwLength);	
}

LONG CSMRegKey::Read(LPCTSTR pszKey, DWORD & dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	DWORD dwType ;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest ;
	LONG lRet = RegQueryValueEx(m_hKey,(LPSTR)pszKey,NULL,&dwType,(BYTE*)&dwDest,&dwSize) ;
	if(lRet == ERROR_SUCCESS)
		dwVal = dwDest ;
	return lRet ;
}

LONG CSMRegKey::Read(LPCTSTR pszKey, CString & sVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	DWORD dwType ;
	DWORD dwSize = 200 ;
	char string[200];
	LONG lReturn = RegQueryValueEx(m_hKey , (LPSTR)pszKey ,NULL,
		&dwType , (BYTE*)string , &dwSize );
	if(lReturn == ERROR_SUCCESS)
		sVal = string  ;
	return lReturn ;
}

LONG CSMRegKey::Read(LPCTSTR pszKey, BYTE * pData, DWORD & dwLength)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	DWORD dwType ;
	return RegQueryValueEx(m_hKey , (LPSTR)pszKey ,NULL ,
		&dwType , pData ,&dwLength);
}	

CSMRegKey::operator HKEY() const
{
	return m_hKey;
}

void CSMRegKey::DeleteKey()
{
	if(m_hKey==NULL)
		return ;
	int i;
	char keyName[1000];
	for(i=0;;i++)
	{
		LONG er;
		er=::RegEnumKey(m_hKey,i,keyName,1000);
		if(er!=ERROR_SUCCESS)
			break;
		CSMRegKey subkey;
		if(subkey.Open(m_hKey,keyName)==ERROR_SUCCESS)
		{
			subkey.DeleteKey();
			subkey.Close();
			RegDeleteKey(m_hKey,keyName);
		}
	}
}
