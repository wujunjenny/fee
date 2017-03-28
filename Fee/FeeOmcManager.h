#pragma once

#include <stream\stream.h>
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <http/HttpContainer.h>



class IDrvInvoker;
class CHttpContainer;

class FeeOmcManager
{
public:
	FeeOmcManager(void);
	~FeeOmcManager(void);
	int Init();

public:
	//CServiceMng* m_pMng;

	int QueryFeeStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv);
	IDrvInvoker* m_pbaseDrv;
	CHttpContainer* m_pHttp;

};

