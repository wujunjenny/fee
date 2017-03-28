#pragma once
#include "otlobj.h"
class COciObj :
	public COTLObj
{
public:
	COciObj(void);
	virtual ~COciObj(void);
	static void RegisterClass();
	static COciObj* Create();
public:
	virtual int init(const char* pszParam) override;
	virtual int commit()override;
	virtual int ExcuteSQL(const char* pszstmt)override;
	virtual int ExcuteSQL(const char* pszstmt,input_params in_params,output_params &out_params)override;
	virtual int ExcuteSQL(const char* pszstmt,std::list<in_param_ref> in_params,std::list<out_param> &out_params,std::list<record_set> &out_curs) override;
	virtual int QueryProc(const char* procname,std::string& s_stmt,std::string& curname) override;
	virtual int QueryParam(const char* pszstmt,paramdec_list& inlist,paramdec_list& out) override;

protected:
	std::shared_ptr<otl_connect> m_pdb;
	bool m_bconnect;
};

