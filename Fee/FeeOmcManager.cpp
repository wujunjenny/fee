#include "stdafx.h"
#include "FeeOmcManager.h"
#include <iostream>
#include <fstream>
#include <http/HttpContainer.h>
#include <os\os.h>
#include <stream\stream.h>
#include <IDrv\IDrv.h>
#include <rapidjson\document.h>
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson\writer.h>
#include <rapidjson\reader.h>
#include "Globaldef.h"


//#pragma comment(lib,"ITcpDrv.lib")
#ifdef _DEBUG 
	#ifndef _WIN64 
		#pragma comment(lib,"ITcpDrv32D.lib") 
	#else 
		#pragma comment(lib,"ITcpDrv64D.lib") 
	#endif 
#else  
	#ifndef _WIN64 
		#pragma comment(lib,"ITcpDrv32.lib") 
	#else 
		#pragma comment(lib,"ITcpDrv64.lib") 
	#endif  
#endif 								

#pragma comment(lib,"Ws2_32.lib")

#define JSON_FUN_INIT(read,writer)			  IMemStream __Myin;  \
											  IMemStream __Myout; \
											  MemStreamWrap _sout( __Myout); \
											  MemStreamWrap _sin( __Myin);   \
										  	  rapidjson::Writer<MemStreamWrap>  writer(_sout); \
											  req.GetEntity(&__Myin); \
											  rapidjson::Document read; \
											  read.ParseStream(_sin); \
											  writer.StartObject(); 

#define JSON_FUN_END(writer)	  			  writer.EndObject();\
											  rsp.SetEntity(&__Myout);\
											  rsp.SetHeader("Content-Type","application/json;charset=gb2312");

#define JSON_FUN_RETURN(writer,error,msg)     writer.Key("result");\
											  writer.Int(error);\
											  writer.Key("msg");\
											  writer.String(msg);\
											  JSON_FUN_END(writer);\
											  return 0;


FeeOmcManager::FeeOmcManager(void)
{
	Init();
	REGISTER_CALLBACK_DEFAULT(m_pHttp,FeeOmcManager,QueryFeeStatus,this);

}


FeeOmcManager::~FeeOmcManager(void)
{
	if(m_pbaseDrv)
		m_pbaseDrv->ShutDown();
	if(m_pHttp)
		delete m_pHttp;
}



int FeeOmcManager::Init()
{
	int nPort = 7123;

	FILE* fp = fopen("OmcConfig.json", "rb"); // 非Windows平台使用"r"
	if(fp==NULL)
	{
		fp =  fopen("OmcConfig.json", "wb");
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		writer.StartObject();
		writer.Key("http-port");
		writer.Int(6789);
		writer.EndObject();

	}
	else
	{
		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		rapidjson::Document reader;
		reader.ParseStream(is);
		if(reader.HasParseError())
		{
		}
		else
		{
			nPort;
			auto itr = reader.FindMember("http-port");
			if(itr != reader.MemberEnd())
			{
				nPort= itr->value.GetInt();
			}


		}

	}
	fclose(fp);
	m_pbaseDrv = CreateDrvInstance("IOCP");
	m_pbaseDrv->Init(1);
	m_pHttp = new CHttpContainer(m_pbaseDrv);
	m_pHttp->StartServer(nPort);
	return 0;
}


int FeeOmcManager::QueryFeeStatus(HttpRequest& req,HttpResponse& rsp,HANDLE hSession,HANDLE hDrv)
{
	JSON_FUN_INIT(read,writer)
	writer.Key("nRcvCount");
	writer.Int(g_nRecvDeliverCount);
	writer.Key("nSendCount");
	writer.Int(SendCount);
	writer.Key("nSendSuccess");
	writer.Int(SendSucCount);
	writer.Key("nSendFail");
	writer.Int(SendFailCount);
	writer.Key("nMemMapCount");
	writer.Int(g_MsgMapCount);
	writer.Key("nQueueSize");
	writer.Int(g_messagequesize);
	writer.Key("nRcvBuffSize");
	writer.Int(g_recvbuff);
	writer.Key("nDBTimeOut");
	writer.Int(g_nExecTimeOutCount);

	JSON_FUN_RETURN(writer,0,"");		
	return 0;
}
