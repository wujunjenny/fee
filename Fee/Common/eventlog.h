// eventlog.h : header file
//


#ifndef __EVENT_LOG__
#define __EVENT_LOG__
struct EventInfo
{
    char sDbName[48];
	char sSrcName[32];
};


BOOL WriteEventLog(int EventID,
				   short fwEventType,
                   LPCSTR sErrorTitle,
				   LPCSTR sDbName,
				   LPCSTR sSrcName);
#endif