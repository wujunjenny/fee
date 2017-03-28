// TcpServer.cpp: implementation of the CSockServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SockServer.h"


#ifdef _WIN64
#pragma pack(push,8)
#endif

typedef struct my_fd_set {
        u_int fd_count;               /* how many are SET? */
        SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
} my_fd_set;

#undef FD_SET
#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((my_fd_set*)set)-> fd_count; __i++) { \
        if (((my_fd_set*)set)->fd_array[__i] == (SOCKET)(fd)) { \
            break; \
        } \
    } \
    if (__i == ((my_fd_set*)set)->fd_count) { \
        if (((my_fd_set*)set)->fd_count < FD_SETSIZE) { \
            ((my_fd_set*)set)->fd_array[__i] = (SOCKET)(fd); \
            ((my_fd_set*)set)->fd_count++; \
        } \
    } \
} while(0)

#ifdef _WIN64
#pragma pack(pop)
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockServer::CSockServer(LPONACCEPT pFun, HANDLE User, WORD wPort, DWORD extData)
{
	m_ExtData = extData;
	BindOnAccept(pFun, User);
	m_wPort = wPort;
	Start();
}

CSockServer::~CSockServer()
{
	Stop(0xffffffff,1);
	//	Stop();
}

void CSockServer::BindOnAccept(LPONACCEPT pFun,HANDLE User)
{
	m_pOnAccept=pFun;
	m_hUser=User;
}

void CSockServer::OnAccept(HANDLE hFile, struct sockaddr FAR* pDestAddr, int iSize)
{
	char szIP[40];
	char szRemoteAddr[100];
	memset(szIP, 0, 40);
	memset(szRemoteAddr, 0, 100);
	strcpy(szIP, inet_ntoa(((sockaddr_in*)pDestAddr)->sin_addr));
	sprintf(szRemoteAddr, "%s %d", szIP, ntohs(((sockaddr_in*)pDestAddr)->sin_port));
	
	if (m_pOnAccept)
		(*m_pOnAccept)(m_hUser, hFile, szRemoteAddr, strlen(szRemoteAddr), m_ExtData);
}

int CSockServer::Run(DWORD)
{
	BOOL bBd=TRUE;
	
    while (!IsExit())
    {
		if(bBd==TRUE)
		{
			if(BuildSocket())
				bBd=FALSE;
			else
				Sleep(100);
		}
		if(bBd==FALSE)
		{
			if(!ListenSocket())
			{
				bBd=TRUE;
				CloseSocket();
			}
		}
	}
	//	TRACE("Exit TcpListener\n");
	CloseSocket();
    return 0;
}

/*******************************************************************************
Function	: BuildSocket
Description	: 建立SOCKET为侦听
Input		:
Return		: 
返回值为布尔型，
TRUE表示发送成功
FALSE表示发送失败
*******************************************************************************/
BOOL CSockServer::BuildSocket()
{
    sockaddr_in local_addr;
	memset(&local_addr,0,sizeof(local_addr));
	local_addr.sin_port=htons(m_wPort);
    local_addr.sin_family=AF_INET;
	m_socket=socket(AF_INET,SOCK_STREAM,0);
	
	if(m_socket==INVALID_SOCKET)
	{
		int err;
		err=WSAGetLastError();
		return FALSE;
	}
	
    BOOL bReuseAddr = TRUE;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseAddr, 
		sizeof(BOOL));
	
	int rt;
	rt=bind(m_socket,(struct sockaddr*)&local_addr,sizeof(local_addr));
	if(rt==SOCKET_ERROR)
	{
		int iErr;
		iErr = WSAGetLastError();
		
        // char ErrStr[100];
        // sprintf(ErrStr, "Server bind socket error, errorcode %d.", iErr);
		// DebugOutput(ErrStr, NULL, 0);
		
        CloseSocket();      // add by zhanggang 2000.07.09
		return FALSE;
	}
	u_long mode=1;
	ioctlsocket(m_socket,FIONBIO,&mode);//设定SOCKET模式为异步
	listen(m_socket,SOMAXCONN);
	return TRUE;
}

/*******************************************************************************
Function	: CloseSocket
Description	: 关闭SOCKET
Return		: 
返回值为布尔型，
TRUE表示发送成功
FALSE表示发送失败
*******************************************************************************/
BOOL CSockServer::CloseSocket()
{
    LINGER  TcpLinger;
    int iRet = 0;
	if(m_socket==INVALID_SOCKET)
		return TRUE;

    TcpLinger.l_onoff  = 1;
    TcpLinger.l_linger = 0;    
    
    iRet = setsockopt((int)m_socket, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
        sizeof(LINGER));
    if(iRet == SOCKET_ERROR)
	{	
        ASSERT(0);
		int er;
		er=WSAGetLastError();        
    }
	
    //end add
	closesocket(m_socket);
	m_socket=INVALID_SOCKET;
	return TRUE;	
}

/*******************************************************************************
Function	: ListenSocket
Description	: 侦听SOCKET
Return		: 
返回值为布尔型，
TRUE表示发送成功
FALSE表示发送失败
*******************************************************************************/
BOOL CSockServer::ListenSocket()
{
	int rt;
	struct timeval timeout;
	my_fd_set exceptfd;
	my_fd_set readfd;
	timeout.tv_sec=0;
	timeout.tv_usec=100000;
	FD_ZERO(&readfd);
	FD_ZERO(&exceptfd);
	FD_SET(m_socket,&readfd);
	FD_SET(m_socket,&exceptfd);
	rt=select(0,(fd_set*)&readfd,0,(fd_set*)&exceptfd,&timeout);
	
	if(rt>0)
	{
		if(FD_ISSET(m_socket,&readfd))
		{
			//可读就绪，
			SOCKET newsocket;
			struct sockaddr dest_addr;
			int len;
			len=sizeof(dest_addr);
			newsocket=accept(m_socket,&dest_addr,&len);
			if(newsocket!=INVALID_SOCKET)
            {
                //
                LINGER  TcpLinger;
                int iRet = 0;    
                TcpLinger.l_onoff  = 1;
                TcpLinger.l_linger = 0;    
				
                iRet = setsockopt(newsocket, SOL_SOCKET, SO_LINGER, (const char*)&TcpLinger, 
                    sizeof(LINGER));
                if(iRet == SOCKET_ERROR)
				{	
                    ASSERT(0);
					int er;
					er=WSAGetLastError();        
                }
                //end add
				
				OnAccept((HANDLE)newsocket, &dest_addr, len);
            }
		}
		else
			if(FD_ISSET(m_socket,&exceptfd))
			{
				return FALSE;
			}
	}
	else
		if(rt==SOCKET_ERROR)
		{
			return FALSE;
		}
		return TRUE;
}
