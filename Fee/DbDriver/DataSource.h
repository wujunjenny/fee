// DataSource.h: interface for the CDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DataSource__h__)
#define __DataSource__h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//数据库类型
enum ENUM_DBMS{
	MS_SQL_DB_LIB = 0,
		SYBASE_CT_LIB = 1,
		ODBC_LIB = 2,
		ADO_LIB = 3,
		//		ORACLE_PROC_LIB = 4,
        DBSRC_NULL = 5,
} ;

/*数据源类，定义为数据源的相关特性*/
class CDataSource  
{
public:
	
	CDataSource();
	virtual ~CDataSource();
private:
	DWORD m_DBType;   //数据源类型
	CString m_DataSrc;  //数据源名称
	//--------------------------------
	CString m_Password;  //连接密码
	CString m_DBName;   //数据库名
	CString m_ServerName; //服务器名
	WORD	m_ConnectNum;  //连接数
	CString m_UserID;      //用户名
    DWORD   m_DBProvider;    //ADO数据源提供者类型
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
