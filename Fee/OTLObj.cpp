#include "stdafx.h"
#include "OTLObj.h"

#pragma init_seg(lib)
std::list<COTLObj::class_create> COTLObj::class_info;
int COTLObj::isinitialize = 0;


COTLObj::COTLObj(void)
{
}


COTLObj::~COTLObj(void)
{
}


std::shared_ptr<COTLObj> COTLObj::CreateObj(const char* pstr)
{
	auto itr = class_info.begin();
	while(itr!=class_info.end())
	{
		if(itr->name == pstr && itr->creator)
			return std::shared_ptr<COTLObj>((itr->creator)()) ;
		itr++;
	}
	return nullptr;
}

