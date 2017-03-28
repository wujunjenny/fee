#ifndef __TLV_H__
#define __TLV_H__

//#include "define.h"

class CTLV  
{
private:
	struct STLV{
		DWORD	dwTag;
		DWORD	dwLen;
		BYTE*	bstrValue;
		STLV*	pNext;
	};
	STLV*	m_pSTLV;
	
public:
	int FromStream(const BYTE*	pStream, const DWORD& dwStreamLength, DWORD& dwStreamPosition);
	const BYTE* GetItem(DWORD dwTag, DWORD& dwLen);
	int AddItem(DWORD dwTag, DWORD dwLen, const BYTE* bstrValue);
	int ToStream(BYTE *pStream, DWORD& dwStreamLength);
		
	//***SMGW40-01, 2004-12-23, jdz, modi begin***//
	long int GetLongIntItemValue(DWORD dwTag);
	bool AddLongIntItem(DWORD dwTag, const long int bstrValue);
	bool SetLongIntItem(DWORD dwTag, const long int bstrValue);
	//***SMGW40-01, 2004-12-23, jdz, modi end***//
		
	CTLV();
	~CTLV();
};

#endif //__TLV_H__











