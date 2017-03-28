// DrvManager.cpp: implementation of the CSockMng class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SockConnect.h"
//#include "ICDComDrv.h"
#include "SockServer.h"
#include "SockMng.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockMng::CSockMng()
{
	::InitializeCriticalSection(&m_CriticalSection);
	
	for (int i= 0; i < MAX_IF_NUM; i++)
	{
		memset(&(m_hConnections[i]), 0, sizeof(DRVOBJECT));
		m_hConnections[i].Handle = i+1;
		m_FreeList.AddTail(&m_hConnections[i]);
	};
	
	m_Exit = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	
	// SMGW431-06, 2005-10-27, wzy modify begin //
	//DWORD id;
	// SMGW431-06, 2005-10-27, wzy modify end //
	
	
	
    m_Thread = (HANDLE)(::_beginthreadex(NULL, 0, ProcessErrorThread, this, 0, NULL));
	
	
#ifndef _NO_ICDCOMM_
	m_pICDCOMDrv = NULL;
#endif
	
	
}

CSockMng::~CSockMng()
{
	
	::CloseHandle(m_Exit);
	::CloseHandle(m_Thread);
	::DeleteCriticalSection(&m_CriticalSection);
	
	TRACE("DrvManager Delete!\n");
}


BOOL CSockMng::Create()
{
	return TRUE;
}



void CSockMng::Destroy()
{
	POSITION pos, tmppos;
	
	//删除过程：
	//首先删除服务器，在删除所有的DRVOBJECT。
	
	
	//删除所有服务器
	for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
	{
		CSockServer* pTcpServer = m_TcpServerList.GetNext(pos);
		delete pTcpServer;
	};
	
	
	
	//退出删除线程
	::SetEvent(m_Exit);
	Sleep(10);
	::WaitForSingleObject(m_Thread, INFINITE);
	
	//因为删除线程唤醒后，就立即退出了，有可能还有对象没有删除
	//所以要再检查。
	for (pos = m_TcpBadList.GetHeadPosition(); pos != NULL; )
	{
		tmppos = pos;
		DRVOBJECT* pDrvObject = m_TcpBadList.GetNext(pos);
		
#ifdef _debug_killif_
		TRACE("Destroy Return DrvObject %d.\n", pDrvObject->Handle);
#endif
        //强制退出
		ReturnFreeHandle((HANDLE)pDrvObject->Handle, TRUE);
		m_TcpBadList.RemoveAt(tmppos);
	};
	
	//确保清除所有的DRVOBJECT对象
	for (int i = 0; i < MAX_IF_NUM; i++)
		if (m_hConnections[i].Type != 0)
		{
			//说明还有DRVOBJECT对象没有清除，外部没有调用KILLIF。
			ASSERT(0);
            //强制退出
			ReturnFreeHandle((HANDLE)(m_hConnections[i].Handle), TRUE);
		};
		
#ifndef _NO_ICDCOMM_
		
		if (m_pICDCOMDrv)
			delete m_pICDCOMDrv;
#endif
		
		m_FreeList.RemoveAll();
		m_TcpBadList.RemoveAll();
		m_TcpServerList.RemoveAll();
		
#ifndef _NO_ICDCOMM_
		m_ICDList.RemoveAll();
#endif
		
}


int CSockMng::CreateTcpServer(WORD wPort/*本端端口号*/, DWORD dwCodeType)
{
	CSockServer* pServer = new CSockServer(StOnAccept, this, wPort, dwCodeType);
	if (pServer)
	{
		m_TcpServerList.AddTail(pServer);
		return 0;
	}
	else
		return 1;
}


int CSockMng::KillTcpServer(WORD wPort, DWORD dwCodeType)
{
    POSITION pos, tmppos;
    for (pos = m_TcpServerList.GetHeadPosition(); pos != NULL; )
    {
        tmppos = pos;
        CSockServer* pServer = m_TcpServerList.GetNext(pos);
        if ((pServer->GetTcpServerPort() == wPort) 
            && (pServer->GetExtData() == dwCodeType))
        {
            delete pServer;
            m_TcpServerList.RemoveAt(tmppos);
			
            // 删除成功
            return 0;
        };
    }
	
    // 删除失败
    return 1;
}

#ifndef _NO_ICDCOMM_

int CSockMng::CreateICDCOMServer(DWORD ProcID/*本端进程号*/)
{
	if (m_pICDCOMDrv)
		delete m_pICDCOMDrv;
	
	m_pICDCOMDrv = new CICDComDrv(this, ProcID);
	
	return 0;
}


int CSockMng::KillICDCOMServer()
{
	if (m_pICDCOMDrv)
    {
		delete m_pICDCOMDrv;
        m_pICDCOMDrv = NULL;
        return 0;
    }
    else
        return 1;
}

HANDLE CSockMng::CreatICDCOMIF( DWORD dwRemoteIP, DWORD dwPID)
{
	HANDLE hDrvObject = GetFreeHandle();
	
    if (NULL == hDrvObject)
        return NULL;
	
	
#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF to ENTER critical.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	DrvObject.Type = COM_ICDCOM;
	DrvObject.Status = STATUS_ACTIVE;
	*((DWORD*)(DrvObject.RemoteAddress)) = dwRemoteIP;
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = dwPID;
	
	
	char name[100];
	
	if (0 != gethostname(name,100))
    {
		
        ::LeaveCriticalSection(&m_CriticalSection);
		
        return NULL;
    }
	
	HOSTENT* phost = gethostbyname(name);
    if (NULL == phost)
    {
        ::LeaveCriticalSection(&m_CriticalSection);
		
        return NULL;
    }
	
	DWORD dwMyAddr = *((DWORD*)(phost->h_addr_list[0]));
	
	*((DWORD*)(DrvObject.MyAddress)) = dwMyAddr;
	if (m_pICDCOMDrv)
		*((DWORD*)(DrvObject.MyAddress + 4)) = m_pICDCOMDrv->GetProcID();
	
	m_ICDList.AddTail(&DrvObject);
	
	::LeaveCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("CreatICDCOMIF LEAVE critical.\n");
#endif
	
	return hDrvObject;
	
}

#endif


HANDLE CSockMng::CreateTcpIF( const char* pRemoteAddr, WORD port, BOOL bAuto, DWORD dwTime)
{
	HANDLE hDrvObject = GetFreeHandle();
	
    if (NULL == hDrvObject)
        return NULL;
	
	
	CSockConnect * pNew = new CSockConnect(hDrvObject, pRemoteAddr, port, bAuto, dwTime, StOnRead, 
		StOnError, StOnConnect, this);
	
	if (!pNew)
	{
		
        ReturnFreeHandle(hDrvObject);
		ASSERT(0);
		return NULL;
	}
	
#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to ENTER critical.\n");
#endif
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	DrvObject.Type = COM_TCPIP;
    
    DrvObject.Status = STATUS_BROKEN;
	*((DWORD*)(DrvObject.RemoteAddress)) = inet_addr(pRemoteAddr);
	*((DWORD*)(DrvObject.RemoteAddress + 4)) = port;
	
	DrvObject.pDrv = pNew;
	
	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("CreateTcpIF to LEAVE critical.\n");
#endif
	
	return hDrvObject;
	
}


int CSockMng::Send( HANDLE hIF, LPVOID pBuff, int size)
{
	int iRet;
	
	if (hIF == NULL)
		return FALSE;
	
#ifdef _debug_drv_lock_
	TRACE("Send to Enter Drv_Lock\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle != (DWORD)hIF)
	{
		
		::LeaveCriticalSection(&m_CriticalSection);
		
#ifdef _debug_drv_lock_
		TRACE("Send Leaved DrvLock\n");
#endif
		return FALSE;
	};
	
	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		iRet = m_pICDCOMDrv->Send(*((DWORD*)(DrvObject.RemoteAddress)), 
			*((DWORD*)(DrvObject.RemoteAddress + 4)), pBuff, size);
#endif
	}
	else
		if (DrvObject.Type == COM_TCPIP)
		{
			CSockConnect* pDrv = DrvObject.pDrv;
			iRet = pDrv->Send(pBuff, size);
		}
		else
		{
			ASSERT(0);
			::LeaveCriticalSection(&m_CriticalSection);
			return FALSE;
		}
		
		::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
		TRACE("Send Leaved Drv_Lock\n");
#endif
		
		//TRACE 发送消息
		OnTrace(hIF, FALSE, size, pBuff);
		
		return iRet;
}

int CSockMng::GetType( HANDLE hIF )
{
	int iRet;
	
	if (hIF == NULL)
		return 0;
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Type;
	else
		iRet = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);
	
	return iRet;
	
}


int CSockMng::GetAllHandle(HANDLE * pHandles ,int& Num )
{
	if ((pHandles == NULL)||(Num <= 0))
	{
		Num = 0;
		return 0;
	};
	int i,j;
	for (i = 0, j = 0; ((i < MAX_IF_NUM) && (j < Num)); i++)
		if (m_hConnections[i].Type != 0)
			pHandles[j++] = (HANDLE)(m_hConnections[i].Handle);
		
		Num = j;
		
		return Num;
}


int CSockMng::GetIFStatus( HANDLE hIF )
{
	int iRet;
	
	if (hIF == NULL)
		return STATUS_ABORT;
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
		iRet = DrvObject.Status;
	else
		iRet = STATUS_ABORT ;
	
	::LeaveCriticalSection(&m_CriticalSection);
	
	return iRet;
}


int CSockMng::GetLocalAddress( HANDLE hIF , char* pLocalAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0 )
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;
	
	in_addr tmpaddr;
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.MyAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.MyAddress+4)));
			
			memcpy(pLocalAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pLocalAddr[size] = 0;
			break;
			
		case COM_UDP:
			ASSERT(0);
			size = 0;
			break;
			
		default:
			ASSERT(0);
			size = 0;
		};
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);
	
	return size;
}


int CSockMng::GetRemoteAddress( HANDLE hIF , char* pRemoteAddr , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;
	char addr[100];
	memset(addr, 0, 100);
	int i = 0;
	
	in_addr tmpaddr;
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
	{
		switch(DrvObject.Type)
		{
		case COM_TCPIP:
		case COM_ICDCOM:
			tmpaddr.s_addr = *((DWORD*)(DrvObject.RemoteAddress));
			strcpy(addr, inet_ntoa(tmpaddr));
			i = strlen(addr);
			addr[i] = ' ';
			sprintf(addr+(i+1), "%d", *((DWORD*)(DrvObject.RemoteAddress+4)));
			
			memcpy(pRemoteAddr, addr, min(strlen(addr), (unsigned int)size-1));
			size = min(strlen(addr), (unsigned int)size-1);
			pRemoteAddr[size] = 0;
			break;
			
		case COM_UDP:
			size = 0;
			ASSERT(0);
			break;
			
		default:
			size = 0;
			ASSERT(0);
		};
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);
	
	return size;
	
}


int CSockMng::GetExtData( HANDLE hIF, char* pExtData , int & size )
{
	if (hIF == NULL)
		return 0;
    if (size <= 0)
        return 0;
	
#ifdef _debug_drv_lock_
	TRACE("GetExtData To ENTER Drv_lock.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(pExtData, DrvObject.ExtData, min(size, DrvObject.Size));
		size = DrvObject.Size;
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetExtData LEAVE Drv_lock.\n");
#endif
	
	return size;
	
}


int CSockMng::BindExtData( HANDLE hIF, char* pExtData , int size )
{
	if (hIF == NULL)
		return 0;
	
	::EnterCriticalSection(&m_CriticalSection);
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
	{
		memcpy(DrvObject.ExtData, pExtData, min(size, MAX_EXTDATA_SIZE));
		size = min(size, MAX_EXTDATA_SIZE);
		DrvObject.Size = size;
	}
	else
		size = 0;
	
	::LeaveCriticalSection(&m_CriticalSection);
	
	return size;
	
}


int CSockMng::KillIF(HANDLE hIF)
{
	if (hIF == NULL)
		return 0;
	
#ifdef _debug_drv_lock_
	TRACE("KillIF to ENTER Drv_lock.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("KillIF ENTERED Drv_lock.\n");
#endif
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle != (DWORD)hIF)
	{
		::LeaveCriticalSection(&m_CriticalSection);
		
#ifdef _debug_drv_lock_
		TRACE("KillIF LEAVE Drv_lock.\n");
#endif
		return 0;
	};
	
	if (DrvObject.Type == COM_ICDCOM)
	{
#ifndef _NO_ICDCOMM_
		m_ICDList.RemoveAt(m_ICDList.Find(&DrvObject));
		ReturnFreeHandle(hIF);
#endif
	}
	else
		if (DrvObject.Type == COM_TCPIP)
		{
			//将连接状态或者断链状态DRVOBJECT移到BADLIST中
			if (DrvObject.Status != STATUS_ABORT)
			{
				//设置为废弃状态
				DrvObject.Status = STATUS_ABORT;
				
				//::EnterCriticalSection(&m_BadLock);
				
				m_TcpBadList.AddTail(&DrvObject);
				
				//::LeaveCriticalSection(&m_BadLock);
			}
		}
		else
			ASSERT(0);
		
		::LeaveCriticalSection(&m_CriticalSection);
		
#ifdef _debug_drv_lock_
		TRACE("KillIF LEAVE Drv_lock.\n");
#endif
		
		return 0;
		
}


int CSockMng::StOnAccept(HANDLE hUser, HANDLE hFile, LPCTSTR pRemoteAddress, 
						 int iSize, DWORD dwCodeType)
{
	CSockMng* pThis = (CSockMng*)hUser;
	
	HANDLE hDrvObject = pThis->GetFreeHandle();
	if (hDrvObject == NULL)
	{
		closesocket((SOCKET)hFile);
		return FALSE;
	};
	
	CSockConnect* pNew = new CSockConnect(hDrvObject, hFile, StOnRead, StOnError, StOnConnect, pThis);
	
	::EnterCriticalSection(&pThis->m_CriticalSection);
	
	DRVOBJECT& DrvObject = pThis->m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	DrvObject.Type = COM_TCPIP;
	DrvObject.Status = STATUS_ACTIVE;
	DrvObject.pDrv = pNew;
	
	
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hFile, (sockaddr*)&addr, &addrsize);
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);
	
	if (pThis->OnAccept(hDrvObject, pRemoteAddress, iSize, dwCodeType) != 0)
	{
		
		pThis->ReturnFreeHandle(hDrvObject);
		return FALSE;
	};
	
	return TRUE;
	
}

int CSockMng::StOnRead(HANDLE hDrvObject, LPVOID pbuff, int size,HANDLE hUser)
{
	BOOL bIFOK = TRUE;
	
	CSockMng* pThis = (CSockMng*)hUser;
	
	::EnterCriticalSection(&pThis->m_CriticalSection);
	
	DRVOBJECT& DrvObject = pThis->m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	//IF已经被删除
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);
	
	if (bIFOK)
	{
        
        int iRealSize = pThis->OnRead(hDrvObject, pbuff, size);
		
        if ((iRealSize > 0) && (iRealSize <= size))
        {
            pThis->OnTrace(hDrvObject, TRUE, iRealSize, pbuff);
            return iRealSize;
        }
        else if (CODE_NO_LEN == iRealSize)
            return CODE_NO_LEN;
        else
        {
			
            pThis->OnTrace(hDrvObject, TRUE, size, pbuff);
            return CODE_ERROR;
			
        }
		
	}
	else
		return size;
	
}

int CSockMng::StOnConnect(HANDLE hDrvObject, HANDLE hSocket, HANDLE hUser)
{
	BOOL bIFOK = TRUE;
	
	CSockMng* pThis = (CSockMng*)hUser;
	
	::EnterCriticalSection(&pThis->m_CriticalSection);
	
	DRVOBJECT &DrvObject = pThis->m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	
	sockaddr_in addr;
	int addrsize = sizeof(sockaddr);
	getsockname((SOCKET)hSocket, (sockaddr*)&addr, &addrsize);
	*((DWORD*)(DrvObject.MyAddress)) = addr.sin_addr.s_addr;
	*((DWORD*)(DrvObject.MyAddress+4)) = (DWORD)ntohs(addr.sin_port);
	
	
	if (DrvObject.Status != STATUS_ABORT)
		DrvObject.Status = STATUS_ACTIVE;
	
	//IF已经被删除
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);
	
	if (bIFOK)
		return pThis->OnConnect(hDrvObject);
	else
		return 0;
	
}

int CSockMng::StOnError(HANDLE hDrvObject, int Error, BOOL bAuto, HANDLE hUser)
{
	BOOL bIFOK = TRUE;
	
	CSockMng* pThis = (CSockMng*)hUser;
	
	::EnterCriticalSection(&pThis->m_CriticalSection);
	
	DRVOBJECT& DrvObject = pThis->m_hConnections[ConvertHandleToIndex(hDrvObject)];
	
	
	if ((DrvObject.Type == COM_TCPIP) && (DrvObject.Status != STATUS_ACTIVE))
		bIFOK = FALSE;
	
	DrvObject.Status = STATUS_BROKEN;
	
	::LeaveCriticalSection(&pThis->m_CriticalSection);
	
	if (bIFOK)
		return pThis->OnError(hDrvObject, Error, bAuto);
	else
		return 0;
	
	
}

int CSockMng::StOnTrace(HANDLE hDrvObject, BOOL bIn, int iSize, LPVOID pBuff, HANDLE hUser)
{
	CSockMng* pThis = (CSockMng*)hUser;
	return pThis->OnTrace(hDrvObject, bIn, iSize, pBuff);
}

//////////////////////////////////////////////////////////////////////////////////////
HANDLE CSockMng::GetFreeHandle()
{
	DRVOBJECT* pDrvObject;
	DWORD handle;
	
#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle to ENTER Drv_lock.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle ENTERED Drv_lock.\n");
#endif
	
	if (m_FreeList.IsEmpty())
	{
		::LeaveCriticalSection(&m_CriticalSection);
		
#ifdef _debug_drv_lock_
		TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif
		return NULL;
	};
	
	pDrvObject = (DRVOBJECT*)m_FreeList.RemoveHead();
	
	handle = pDrvObject->Handle + MAX_IF_NUM;
	pDrvObject->Handle = handle;
	
	::LeaveCriticalSection(&m_CriticalSection);
#ifdef _debug_drv_lock_
	TRACE("GetFreeHandle LEAVE Drv_lock.\n");
#endif
	
	return (HANDLE)handle;
}

BOOL CSockMng::ReturnFreeHandle(HANDLE hIF, BOOL bTerminate)
{
	
#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle to ENTER Drv_lock.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle ENTERED Drv_lock.\n");
#endif
	
	DRVOBJECT& DrvObject = m_hConnections[ConvertHandleToIndex(hIF)];
	
	if (DrvObject.Handle == (DWORD)hIF)
	{
		if (DrvObject.Type == COM_TCPIP)
		{
			if (DrvObject.pDrv) 
			{
                if (!(DrvObject.pDrv->Stop(20, bTerminate)))
                {
                    ::LeaveCriticalSection(&m_CriticalSection);
                    return FALSE;
                }
				
				delete DrvObject.pDrv;
				DrvObject.pDrv = NULL;
			}
			else
			{
				ASSERT(0);
			}
		};
		
		m_FreeList.AddTail(&DrvObject);
		
		
		DWORD handle = DrvObject.Handle;
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
		
	}
	else
	{
		ASSERT(0);
		
		
		DWORD handle = DrvObject.Handle;
		memset(&DrvObject, 0, sizeof(DRVOBJECT));
		DrvObject.Handle = handle;
	};
	
	::LeaveCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("ReturnFreeHandle LEAVE Drv_lock.\n");
#endif
	
    return TRUE;
}


int CSockMng::OnAccept( HANDLE hFile, LPCTSTR pRemoteAddress, int AddrSize, DWORD dwCodeType)
{
	
	return 1;
}


int CSockMng::OnRead( HANDLE hIFHandle, LPVOID pBuff, int size)
{
	return size;
}

int CSockMng::OnConnect( HANDLE hIFHandle )
{
	return 0;
}

int CSockMng::OnError( HANDLE hIFHandle , int ErrorCode, BOOL bRecover)
{
	return 0;
}

int CSockMng::OnTrace(HANDLE hIFHandle, BOOL bIn, int size, LPVOID pBuff )
{
	return 0;
}


unsigned int WINAPI CSockMng::ProcessErrorThread(LPVOID lparam)
{
	CSockMng* pParent = (CSockMng*)lparam;
	POSITION pos;
	POSITION postemp;
	DRVOBJECT* pDrvObject;
	
	while (::WaitForSingleObject(pParent->m_Exit, 0) != WAIT_OBJECT_0)
	{
		::EnterCriticalSection(&(pParent->m_CriticalSection));
		
		if (!pParent->m_TcpBadList.IsEmpty())
			for (pos = pParent->m_TcpBadList.GetHeadPosition(); pos != NULL; )
			{
				postemp = pos;
				pDrvObject = pParent->m_TcpBadList.GetNext(pos);
				
				
                if (pParent->ReturnFreeHandle( (HANDLE)(pDrvObject->Handle) ))
                {
                    pParent->m_TcpBadList.RemoveAt(postemp);
					
#ifdef _debug_killif_
					TRACE("ProcessErrorThread Return DrvObject %d.\n", pDrvObject->Handle);
#endif
                }
			};
			
			::LeaveCriticalSection(&(pParent->m_CriticalSection));
			
			Sleep(3000);
	}
	
	return 0;
	
}

#ifndef _NO_ICDCOMM_

int CSockMng::OnICDRead(DWORD SourceIP, DWORD SourceprocID, DWORD MyID, LPVOID lpszRcvbuff, int iSize)
{
	POSITION pos;
	DRVOBJECT* pDrvObject;
	HANDLE hDrv = NULL;
	
#ifdef _debug_drv_lock_
	TRACE("OnICDRead to ENTER Drv_lock.\n");
#endif
	
	::EnterCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("OnICDRead ENTERED Drv_lock.\n");
#endif
	
	for (pos = m_ICDList.GetHeadPosition(); pos != NULL; )
	{
		pDrvObject = (DRVOBJECT*)m_ICDList.GetNext(pos);
		DWORD* pIP = (DWORD*)pDrvObject->RemoteAddress;
		DWORD* pPID = (DWORD*)(pDrvObject->RemoteAddress+4);
		if ((*pIP == SourceIP) && (*pPID == SourceprocID))
		{
			hDrv = (HANDLE)pDrvObject->Handle;
			break;
		};
		
	}
	::LeaveCriticalSection(&m_CriticalSection);
	
#ifdef _debug_drv_lock_
	TRACE("OnICDRead LEAVE Drv_lock.\n");
#endif
	
	if (hDrv)
	{
		OnTrace(hDrv, TRUE, iSize, lpszRcvbuff);
		
		return OnRead(hDrv, lpszRcvbuff, iSize);
	}
	else
		return 0;
	
}

#endif
