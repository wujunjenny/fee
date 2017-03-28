#ifndef __CRouteTable__h__
#define __CRouteTable__h__

//#define MAX_ADDRESS_STRING		21
#define MAX_ADDRESS_STRING		32
#define CHARKINDSUM				(256)

#define ANS_NULLHANDLE				0

typedef struct tag_node
{
	HANDLE Handle;
	tag_node* SonNode[CHARKINDSUM];
	tag_node* Parent;
}ADDRESSNODE;

class CRouteTable
{
protected:
	ADDRESSNODE* Head;
	int m_iCount;
	
	ADDRESSNODE* NewNode();
	BOOL DeleteTree(ADDRESSNODE* pNode);
	ADDRESSNODE* GetNextPos(ADDRESSNODE *node);
	BOOL GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize);
	UCHAR  GetIndex(char ch);
	unsigned char GetChar(int index); 
	
public:
	CRouteTable();
	~CRouteTable();
	
	virtual HANDLE GetItem(LPCTSTR pAddress);
	HANDLE ReplaceItem(LPCTSTR pAddress, HANDLE NewItem);
	BOOL DeleteItem(LPCTSTR pAddress);
	BOOL InsertItem(LPCTSTR pAddress, HANDLE NewItem);
	POSITION GetFirstItem();
	HANDLE GetNextItem(POSITION& pos,LPTSTR pRevBuff=NULL,int iBuffSize=0);
	int GetCount(){return m_iCount;};
	BOOL DeleteAllItem();
	
};

#endif