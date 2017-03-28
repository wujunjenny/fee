// DataSource.h: interface for the CDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DataSource__h__)
#define __DataSource__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ݿ�����
enum ENUM_DBMS{
	MS_SQL_DB_LIB = 0,
		SYBASE_CT_LIB = 1,
		ODBC_LIB = 2,
		ADO_LIB = 3,
		//		ORACLE_PROC_LIB = 4,
        DBSRC_NULL = 5,
} ;

/*����Դ�࣬����Ϊ����Դ���������*/
class CDataSource  
{
public:
	
	CDataSource();
	virtual ~CDataSource();
private:
	DWORD m_DBType;   //����Դ����
	CString m_DataSrc;  //����Դ����
	//--------------------------------
	CString m_Password;  //��������
	CString m_DBName;   //���ݿ���
	CString m_ServerName; //��������
	WORD	m_ConnectNum;  //������
	CString m_UserID;      //�û���
    DWORD   m_DBProvider;    //ADO����Դ�ṩ������
public:
	DWORD GetProvider();
	void SetProvider(DWORD provider);
	
	void SetDataSrcType(DWORD srctyp);
	
	
	DWORD GetDataSrcType();
	
    
	void SetUserID(const char* id);
	
	
    
	CString GetUserID();
	
	void SetConnectNumber(WORD num);
	
    
	WORD GetConnectNumber();
	
	
	
	void SetServerName(const char* srv);
	
    
	CString GetServerName();
	
    
	void SetPassword(const char* pass);
	
    
	CString GetPassword();
	
	void SetDBName(const char* db);
	
	CString GetDBName();
	
	void SetDataSrc(const char* src);
	
	CString GetDataSrc();
	
};

#endif // !defined(__DataSource__h__)
