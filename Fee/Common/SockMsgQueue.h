// CSockMsgQueue.h: interface for the CPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CSockMsgQueue__h__)
#define __CSockMsgQueue__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFULT_POOL_SIZE 1024*1024*10

class CSockMsgQueue  
{
public:
	CSockMsgQueue(int size=DEFULT_POOL_SIZE);
	~CSockMsgQueue();
	int GetFreeSize();
	int GetDataSize();
	LPVOID GetData(int &size);
	BOOL OutPool(int size);
	BOOL PutPool(LPVOID pBuff,int size);
	void ClearBuffer();
protected:
	char* m_pBuff;
	int m_BuffSize;
	int m_DataSize;
};

#endif // !defined(__CSockMsgQueue__h__)
