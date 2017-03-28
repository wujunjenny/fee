#pragma once
#include <memory>
#include <string>
#include <list>

class COTLObj
{
public:
	COTLObj(void);
	virtual ~COTLObj(void);
	static std::shared_ptr<COTLObj> CreateObj(const char* pstr);
	static int isinitialize;
public:
	std::string last_msg;
	std::string error_sql;
	std::string error_var_info;

	typedef COTLObj* (*create_fn) () ;

	struct class_create
	{
		std::string name;
		create_fn creator;
		class_create(const char* nm,create_fn fn) { name = nm; creator=fn;};
	};

	enum param_type
	{
		param_type_var_char = 1,
		param_type_cursor
	};

	struct in_param_ref
	{
		bool ref;
		int type;
		int len;
		void* valueRef;
		std::shared_ptr<std::string> psvalue;
	};

	struct out_param
	{
		int type;
		std::string value;
		std::string name;
		std::list<std::string> cursor_row_values;
		std::list<std::string> cursor_row_names; 
	};

	struct param_dec
	{
		int type;
		std::string name;
	};

	typedef std::list<std::shared_ptr<param_dec>> paramdec_list;



	typedef std::list<std::shared_ptr<in_param_ref>> input_params;
	typedef std::list<std::shared_ptr<out_param>> output_params;
	struct record_desc
	{
		std::string name;
		int type;
	};

	struct record_set
	{
		std::list<record_desc> descriptions;
		std::list<std::string> values;
	};


	static std::list<class_create> class_info;
	virtual int init(const char* pszParam)=NULL;
	virtual int commit() = NULL;
	virtual int ExcuteSQL(const char* pszstmt) = NULL;
	virtual int ExcuteSQL(const char* pszstmt,input_params in_params,output_params &out_params) = NULL;
	virtual int ExcuteSQL(const char* pszstmt,std::list<in_param_ref> in_params,std::list<out_param> &out_params,std::list<record_set> &out_curs) = NULL;

	virtual int QueryProc(const char* procname,std::string& s_stmt,std::string& curname) = NULL;
	virtual int QueryParam(const char* pszstmt,paramdec_list& inlist,paramdec_list& out) =NULL;
	//virtual int AddToBuffer() = NULL;
	//virtual int FecthBuffer() = NULL;
	//virtual int SaveFixMsg() = NULL;
	//virtual int DeleteFixMsg() = NULL;
	//virtual int UpdateFeeMonthTable() = NULL;

protected:


};

//template<class T>
//void registObj(const char* classname)
//{
//	COTLObj::class_info.push_back(COTLObj::class_info(classname,T::Create()));
//}


class REG_CLASS_IMPL 
{
public:
	REG_CLASS_IMPL(const char* classname , COTLObj::create_fn fun)
	{
		COTLObj::class_info.push_back( COTLObj::class_create(classname,fun));
	}
};

#define REG_CLASS(T)  \
	COTLObj* T##NewFun(){ return new T;};\
	REG_CLASS_IMPL T##NewObj(#T,T##NewFun);
