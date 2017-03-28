#include "stdafx.h"

#define OTL_STL
#define OTL_STREAM_POOLING_ON
#define OTL_ORA9I
#include <..\otl\otlv4_h\otlv4.h>
#include <occi.h>
#include "OciObj.h"
#include <sstream>

REG_CLASS(COciObj);

COciObj::COciObj(void)
{
	m_bconnect = false;
}


COciObj::~COciObj(void)
{
	if(m_pdb)
		m_pdb->logoff();
}


int COciObj::init(const char* param)
{
	if(!COTLObj::isinitialize)
	{
		auto  rt = otl_connect::otl_initialize(1); // initialize OCI environment

		if(rt==1)
		{
			COTLObj::isinitialize = 1;
			printf("otl_initialize ok \n");
		}
		else
		{
			printf("otl_initialize fail \n");
			return -1;
		}
	}

	if(!m_pdb)
		m_pdb = std::make_shared<otl_connect>();

	if(!m_pdb->connected)
	{
			try{
				m_pdb->rlogon(param);
			}
			catch(otl_exception& p)
			{

				 //std::cerr<<p.msg<<std::endl; // print out error message
				 //std::cerr<<p.stm_text<<std::endl; // print out SQL that caused the error
				 //std::cerr<<p.var_info<<std::endl; // print out the variable that caused the error
				 last_msg = (const char*)p.msg;
				 error_sql = (const char*)p.stm_text;
				 error_var_info = (const char*)p.var_info;
				return -2;
			}
	}

	return 0;
}


int COciObj::commit()
{
	return 0;
}


int COciObj::ExcuteSQL(const char* pszstmt)
{
	if(!m_pdb)
		return -1;
	try
	{
		//otl_stream is(1,pszstmt,*m_pdb);
		m_pdb->direct_exec(pszstmt);
	}
	catch(otl_exception& p)
	{
				last_msg = (const char*)p.msg;
				error_sql = (const char*)p.stm_text;
				error_var_info = (const char*)p.var_info;
				return -1;

	}

	return 0;
}

int COciObj::ExcuteSQL(const char* pszstmt,input_params in_params,output_params &out_params)
{
	if(!m_pdb)
		return -1;
	try
	{
		otl_stream is(1,pszstmt,*m_pdb);
		//m_pdb->direct_exec(pszstmt);
		for(auto it = in_params.begin();it!=in_params.end();it++)
		{
			if((*it)->ref)
			{
				if(strlen((char*)(*it)->valueRef))
					is << (char*)(*it)->valueRef;
				else
					is << otl_null();
			}
			else
			{
				if((*it)->psvalue->size()==0)
					is << otl_null();
				else
					is << (*(*it)->psvalue);
			}
		}
		is << endr;
		//output
		int desc_len = 0;
		otl_var_desc* pout =  is.describe_out_vars(desc_len);  
		int stream_type = is.get_stream_type();
		
		int index = 0;
		while(!is.eof())
		{
			if(index == desc_len)
			{
				index = 0;
				is >> endr;
			}
			std::shared_ptr<out_param> pparam = std::make_shared<out_param>();
			pparam->name = pout[index].name;
			switch(pout[index].ftype)
			{
			case otl_var_refcur:
				{
					otl_refcur_stream cur;
					is >> cur;
					pparam->type = param_type_cursor;
					int col_num=0;
					auto pdesc = cur.describe_out_vars(col_num);
					int cur_index = 0;
					//while(!cur.eof())
					//{
					//	pparam->cursor_row_names.push_back(pdesc[cur_index].name);
					//	switch(pdesc[cur_index].ftype)
					//	{
					//	case otl_var_char:
					//		{
					//			std::string val;
					//			cur>>val;
					//			pparam->cursor_row_values.push_back(std::move(val));
					//			break;
					//		}
					//	case otl_var_double:
					//	case otl_var_float:
					//		{
					//			double x;
					//			cur >>x;
					//			std::stringstream vs;
					//			vs << x;
					//			pparam->cursor_row_values.push_back(std::move(vs.str()));
					//			break;
					//		}
					//	case otl_var_ltz_timestamp:
					//	case otl_var_timestamp:
					//	case otl_var_tz_timestamp:
					//		{
					//			otl_datetime x;
					//			cur >>x;
					//			std::stringstream vs;
					//
					//			vs << x.year <<"/"<<x.month<<"/"<<x.day<<" "<<x.hour<<":"<< x.minute<<":"<<x.second;
					//			pparam->cursor_row_values.push_back(std::move(vs.str()));
					//			break;
					//		}
					//	default:
					//		{
					//			std::string va;
					//			cur >> va;
					//			pparam->cursor_row_values.push_back(std::move(va));
					//			break;
					//		}
					//	}
					//	cur_index++;
					//}
					//cur.close();
					break;
				}
			case otl_var_char:
				{
					std::string va;
					is >> va;
					pparam->value = va;
					pparam->type = param_type_var_char;
					break;
				}
			case otl_var_double:
			case otl_var_float:
				{
					double x;
					is >>x;
					std::stringstream vs;
					vs << x;
					pparam->value = vs.str();
					pparam->type = param_type_var_char;
					break;
				}
			case otl_var_ltz_timestamp:
			case otl_var_timestamp:
			case otl_var_tz_timestamp:
				{
					otl_datetime x;
					is >>x;
					std::stringstream vs;
					
					vs << x.year <<"/"<<x.month<<"/"<<x.day<<" "<<x.hour<<":"<< x.minute<<":"<<x.second;
					pparam->value = vs.str();
					pparam->type = param_type_var_char;

					break;
				}
			default:
				{
					std::string va;
					is >> va;
					pparam->value = va;
					pparam->type = param_type_var_char;
					break;
				}
			//int rt_type = is.get_stream_type();
			}
			out_params.push_back(pparam);
			index++;
		}
		
	}
	catch(otl_exception& p)
	{
				last_msg = (const char*)p.msg;
				error_sql = (const char*)p.stm_text;
				error_var_info = (const char*)p.var_info;
				return -1;

	}
	return 0;
}

int COciObj::ExcuteSQL(const char* pszstmt,std::list<in_param_ref> in_params,std::list<out_param> &out_params,std::list<record_set> &out_curs)
{
	return 0;
}


int COciObj::QueryProc(const char* procname_,std::string& s_stmt,std::string& curname)
{
	std::string procname=procname_;
	std::string pk_name;
	std::string proc_name;
	int pos = procname.find('.');

	if(pos>0)
	{
		pk_name = procname.substr(0,pos);
		proc_name = procname.substr(pos+1);
	}
	else
		proc_name = procname;

	otl_stream is2;

	s_stmt.resize(50000);
	curname.resize(256);
	int stm_type;

	try
	{
		otl_stream::create_stored_proc_call(
					*m_pdb,
					is2,
					&s_stmt[0],
					stm_type,
					&curname[0],
					proc_name.c_str(),
					pk_name.size()?pk_name.c_str():nullptr,
					nullptr,
					false,
					2000,
					otl_var_char					
					);

	}
	catch(otl_exception& p)
	{
				last_msg = (const char*)p.msg;
				error_sql = (const char*)p.stm_text;
				error_var_info = (const char*)p.var_info;
				return -1;

	}

	return 0;
}


int COciObj::QueryParam(const char* pszstmt,paramdec_list& inlist,paramdec_list& out)
{
	if(!m_pdb)
		return -1;
	try
	{
		otl_stream is(1,pszstmt,*m_pdb);

		int desc_len = 0;

		auto p_invar =  is.describe_in_vars(desc_len);

		for(int i=0;i<desc_len;i++)
		{
			std::shared_ptr<COTLObj::param_dec> p=std::make_shared<COTLObj::param_dec>();
			p->name = p_invar[i].name;
			p->type = p_invar[i].param_type;
			inlist.push_back(p);
		}

		desc_len = 0;
		otl_var_desc* pout =  is.describe_out_vars(desc_len);  


		for(int i=0;i<desc_len;i++)
		{
			std::shared_ptr<COTLObj::param_dec> p=std::make_shared<COTLObj::param_dec>();
			p->name = pout[i].name;
			p->type = pout[i].param_type;
			out.push_back(p);
		}


	}
	catch(otl_exception& p)
	{
				last_msg = (const char*)p.msg;
				error_sql = (const char*)p.stm_text;
				error_var_info = (const char*)p.var_info;
				return -1;

	}
	return 0;

};
