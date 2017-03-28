// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(__StdAfx__h__)
#define __StdAfx__h__
#include <SDKDDKVer.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _INTSAFE_H_INCLUDED_
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxdb.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <afxsock.h>		// MFC socket extensions

#include <afxtempl.h>
#include <process.h>
#include <afxcontrolbars.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//屏蔽无隐患的编译警告信息
#pragma warning (disable:4146)
#pragma warning (disable:4200)
#pragma warning (disable:4996)




#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif



#endif // !defined(__StdAfx__h__)
