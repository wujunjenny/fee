// RegKey.h: interface for the CSMRegKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__RegKey__h__)
#define __RegKey__h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSMRegKey  
{
public:
	void DeleteKey();
	LONG Read(LPCTSTR pszKey , BYTE* pData , DWORD& dwLength);
	LONG Read(LPCTSTR pszKey , CString& sVal);
	LONG Read(LPCTSTR pszKey , DWORD& dwVal);
	LONG Write(LPCTSTR pszKey , const BYTE* pData , DWORD dwLength);
	LONG Write(LPCTSTR pszKey , LPCTSTR pszVal);
	LONG Write(LPCTSTR pszKey , DWORD dwVal);
	VOID Close();
	LONG Open(HKEY hKeyRoot , LPCTSTR pszPath);
	CSMRegKey();
	virtual ~CSMRegKey();
	operator HKEY() const;
protected:
	CString m_sPath;
	HKEY m_hKey;
};

#endif // !defined(__RegKey__h__)
