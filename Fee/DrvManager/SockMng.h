// CSockMng.h: interface for the CSockMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SockMng__h__)
#define __SockMng__h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __public_drvmanager__

#include "smcmacro.h"

#else

#define MAX_IF_NUM				1000

#define COM_TCPIP		1		
#define COM_UDP			2	
#define COM_ICDCOM		3	

#define CODE_INNER			1	
#define CODE_GATEWAY		2	
#define CODE_SMEI			3	

#define IF_INVALID			1	

#endif

#include "SockConnect.h"
#include "SockServer.h"

#define COM_MAX_ADDRESS_SIZE	 8		
#define MAX_EXTDATA_SIZE		20		

#ifndef _NO_ICDCOMM_
class CICDComDrv;
#endif

class CSockMng  
{
public:
	CSockMng();
	virtual ~CSockMng();
	
	virtual BOOL Create();		
	virtual void Destroy();
	
	int CreateTcpServer(
		WORD wPort,				
		DWORD dwCodeType		
		);
	
	
    int KillTcpServer(
		WORD wPort,            
		DWORD dwCodeType        
		);
	
#ifndef _NO_ICDCOMM_
	
	int CreateICDCOMServer(
		DWORD ProcID			
		);
	
    
    int KillICDCOMServer();
	
	
	HANDLE CreatICDCOMIF( 
		DWORD dwRemoteIP,		
		DWORD dwPID			
		);
	
#endif
	
	HANDLE CreateTcpIF( const char* pRemoteAddr,	
		WORD port,						
		BOOL bAuto,						
		DWORD dwTime			
		);
	
	
	int Send( HANDLE hIF,	
		LPVOID pBuff,	
		int size		
		);
	
	
	
	
	
	int GetAllHandle(
		HANDLE * pHandles ,		
		int& Num			
		);
	
	
	int GetType( 
		HANDLE hIF				
		);
	
	
	int GetIFStatus( 
		HANDLE hIF					
		);
	
	
	int GetLocalAddress( 
		HANDLE hIF ,			
		char* pLocalAddr ,		
		int & size
		);
	
	
	int GetRemoteAddress( 
		HANDLE hIF ,				
		char* pRemoteAddr ,			
		int & size				
		);
	
	
	int GetExtData( 
		HANDLE hIF,				
		char* pExtData ,			
		int & size	
		);
	
	
	int BindExtData( 
		HANDLE hIF,
		char* pExtData ,
		int size	
		);
	
	
	int KillIF(
		HANDLE hIF	
		);
	
#ifndef _NO_ICDCOMM_
	
	int OnICDRead(
		DWORD SourceIP,			
		DWORD SourceprocID,		
		DWORD MyID,				
		LPVOID lpszRcvbuff,		
		int iSize				
		);
#endif
	
protected:
	
	
	virtual int OnRead( HANDLE hIFHandle,
		LPVOID pBuff,		
		int size		
		);
	
	virtual int OnAccept( HANDLE hIFHandle ,	
						  LPCTSTR pRemoteAddress,	
						  int AddrSize,				
						  DWORD dwCodeType		
						  );
	
	
	virtual int OnConnect( 
		HANDLE hIFHandle 
		);
	
	
	virtual int OnError( 
		HANDLE hIFHandle ,	
		int ErrorCode,	
		BOOL bRecover
		);
	
	
	virtual int OnTrace( 
		HANDLE hIFHandle ,	
		BOOL bIn ,	
		int size,
		LPVOID pBuff
		);
	
	
	static int StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
		int iSize, DWORD dwCodeType);
	
	
	static int StOnRead(HANDLE hDrvObject, LPVOID pbuff,int size,HANDLE hUser);
	static int StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser);
	static int StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser);
	
	static int StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser);
	
	
	
	struct DRVOBJECT
	{
		BYTE	Type;		 
		DWORD   Handle;   
		BYTE	Status;	
		BYTE	MyAddress[COM_MAX_ADDRESS_SIZE];	
		BYTE	RemoteAddress[COM_MAX_ADDRESS_SIZE];
		BYTE	Size;							
		BYTE	ExtData[MAX_EXTDATA_SIZE];		
		CSockConnect * pDrv;					
		
	};
	private:
		
		inline static int ConvertHandleToIndex(HANDLE handle)
		{
			return (((int)handle % MAX_IF_NUM - 1) >= 0) ? ((int)handle % MAX_IF_NUM - 1) : (MAX_IF_NUM -1);
		};
		
		
		
		HANDLE GetFreeHandle();
		
		BOOL ReturnFreeHandle(HANDLE hIF, BOOL bTerminate = FALSE);
		
		
		static unsigned int WINAPI ProcessErrorThread(LPVOID lparam);
		
		
		HANDLE m_Thread;
		HANDLE m_Exit;
		
		CRITICAL_SECTION m_CriticalSection;
		
		
		DRVOBJECT m_hConnections[MAX_IF_NUM];
		
		
		CTypedPtrList<CPtrList,DRVOBJECT*> m_FreeList;
		
		CTypedPtrList<CPtrList,DRVOBJECT*> m_TcpBadList;
		
		CTypedPtrList<CPtrList,CSockServer*> m_TcpServerList;
		
		
#ifndef _NO_ICDCOMM_
		CICDComDrv* m_pICDCOMDrv;
		
		
		
		CTypedPtrList<CPtrList,DRVOBJECT*> m_ICDList;
		
		
		
		friend class ICDComDrv;
		
#endif
};

#endif // !defined(__SockMng__h__)
