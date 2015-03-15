#define INCL_PM
#define INCL_DOSFILEMGR   /* File Manager values */
#define INCL_DOSERRORS    /* DOS error values */
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <ftpapi.h>

#include <types.h>
#include <netinet\in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#ifndef TCPV40HDRS
#include <unistd.h>
#endif

#include "gynplugin.h"
#include "gyn.rch"


#define MAX_TX_BUFFER 1024
#define MAX_RX_BUFFER 1024
#define MAX_TX_SIZE 10
#define MAX_RX_SIZE 10


typedef struct
{
   ULONG SomeNumber;
   ULONG TotalMsgs;
   ULONG ulStarting;
   ULONG ulEnding;
   CHAR szNewsGroup[512];

}  LIST_STRUCT;

typedef struct
{
   CHAR szNewsServer[512];
   CHAR szNewsGroup[512];
   ULONG ulLast;
} UPNEWSGROUP_STRUCT;

int ReceiveFullMessage(int Socket, PSZ pszbuf, PSZ pszData, ULONG *ulPos, ULONG MaxSize, BOOL bPrint, USHORT *usRows, USHORT sTimeout);
int GetData(int Socket, PSZ pszbuf, ULONG ulpszbuflen, PSZ pszData, ULONG *ulPos, ULONG MaxSize, USHORT sTimeout);
PSZ SearchFor(PSZ pszSearch, PSZ pszData, ULONG iDataLen, PSZ pszResult);
int SaveLog(PSZ pszLogFileName, PSZ pszHeader, ULONG ulHeaderLen, PSZ pszGroup, SHORT sWriteGroup);
BOOL CheckForSmallMessage(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszGroup);
BOOL CheckForPlainMessage(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen);

typedef struct
{
   HQUEUE   hqueue;
   ULONG WaitSessID;
   TID WaitThreadID;
   ULONG Report;
   ULONG Output;
   HWND Enable;
}  StartParams;

typedef struct
{
   CHAR szServer[256];
   CHAR szGroup[256];
   CHAR szUserID[256];
   CHAR szPassword[256];
   CHAR szWelcomeMsg[256];
   ULONG sWelcomeSize;
   CHAR szSendMsg[128];
   ULONG sSendSize;
   TID ThreadID;
   CHAR szWorkingDir[256];
   CHAR szNewsDir[256];
   ULONG ulDirSize;
   CHAR szErrorInfo[256];
   BOOL bNotFinished;
   USHORT usRow;
   ULONG ulDelaySleep;
   USHORT SrcLines;
   BOOL bSleeping;
   ULONG ulMsgNum;
   UPNEWSGROUP_STRUCT UpNews;
   BOOL bLogs;

} GROUPINFO_STRUCT;


PID StartProgramSync(PSZ progname, SHORT relation, PSZ Args,PSZ queue,
                        SHORT SessType, ULONG ReportType, ULONG Output, HWND hWndEnable);
VOID _System waitsess(StartParams *StartP);
VOID _System ServiceNewsGroup(GROUPINFO_STRUCT *GroupInfo);
VOID NewsGroupList(GROUPINFO_STRUCT *GroupInfo);
INT FindDlls(void);
INT FindDllsMulti(void);
INT FindDllsGenMulti(void);
INT FindDllsGeneric(void);



