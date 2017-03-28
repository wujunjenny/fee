#ifndef __CSmppCodeAndDecode__h__
#define  __CSmppCodeAndDecode__h__



struct ReportState
{
	char    sSrcState[10]; 
	DWORD   dwStateVal;
};



#define ESM_CLASS_STATUS_REPORT 0X04  
//#define ESM_CLASS_UDHI			0x40

class	CSmppCodeAndDecode
{       
public:
	
	CSmppCodeAndDecode();
	int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	
	int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private: 
	int fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,long int  nOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,unsigned char      cOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,char      *cpOrgValid, int size);
	int fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize);
	int fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	
	
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,long int  &nDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,unsigned char      &cDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,char      *cpDestValid, int size);
	int fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	
	int fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
    int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt, UL &nDestValid,  int size);
	
	//SMGW42-55, 2005-7-12, jdz, modi begin//
	int fnPutCheckResultSM(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//SMGW42-55, 2005-7-12, jdz, modi end//
	
	
    int fnPutShReportSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutShReportSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetShReportSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnGetShReportSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	
    int fnPutPpsUserCheck(\
		char **cppDestBuff, int& nDestSize,\
		char **cppOrgValid, int& nOrgSize);
    int fnPutPpsUserCheckResp(\
		char **cppDestBuff, int& nDestSize,\
		char **cppOrgValid, int& nOrgSize);
    int fnPutPpsResultNotify(\
		char **cppDestBuff, int& nDestSize,\
		char **cppOrgValid, int& nOrgSize);
    int fnPutPpsResultNotifyResp(\
		char **cppDestBuff, int& nDestSize,\
		char **cppOrgValid, int& nOrgSize);
	
	
    int fnGetPpsUserCheck(char **cppOrgBuff, int& nDeCnt,\
        char **cppDestValid, int& nDestSize);
    int fnGetPpsUserCheckResp(char **cppOrgBuff, int& nDeCnt,\
        char **cppDestValid, int& nDestSize);
    int fnGetPpsResultNotify(char **cppOrgBuff, int& nDeCnt,\
        char **cppDestValid, int& nDestSize);
    int fnGetPpsResultNotifyResp(char **cppOrgBuff, int& nDeCnt,\
        char **cppDestValid, int& nDestSize);
    int fnPutDelvrdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	
	
private:
	
	//  lzh
	
    int  m_iStatusReportSmidStartPos;
	int  m_iStatusReportSmidLenth;
    int  m_iStatusReportSmidFormat;
	int  m_iStatusReportStatusStartPos;
	int  m_iStatusReportStatusLenth;
	int  m_iStatusReportErrStartPos;
	int  m_iStatusReportErrLenth;
	
	int	m_iStatusReportSubmitTimeStartPos;
    int m_iStatusReportSubmitTimeLenth; 
	int m_iStatusReportDeliverTimeStartPos;
    int m_iStatusReportDeliverTimeLenth; 
    char  m_sStatusReportStatusCorrectStr[256];
	
	ReportState m_StateArr[100];
	
	int m_iStateCount;
    
};
#endif