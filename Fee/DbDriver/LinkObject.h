// LnkObj.h: interface for the CLinkObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__LinkObject_h__)
#define __LinkObject_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//addby wj
#define DBPROCESS void
typedef DBPROCESS * PDBPROCESS;
typedef long            DBINT;
typedef short           DBSMALLINT;
typedef unsigned short  DBUSMALLINT;
//end add


class CDataSource ;

class CLinkObject : public CObject  
{
public:
	void* GetProc();
	DWORD GetSrcType();
	CString GetDataSrc();
	virtual BOOL IsLinkOk();
	virtual void DestroyLink();
	virtual BOOL CreateLink();
	CLinkObject();
	virtual ~CLinkObject();
	
protected:
	CDataSource* m_pFather;
	void* m_pProc;
	CString m_DataSrc;
};

#endif // !defined(__LinkObject_h__)
