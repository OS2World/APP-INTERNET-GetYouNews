#include "gyn.h"
#include "uudecodevars.h"
#include "multipart.h"
#include "Generic.h"
#include "preprocess.h"
#include "version.h"

void CrcInit(void);

#define MAX_FILENAME_LEN 512
#define MESSAGE_HEADER_SIZE 30000
#define MAX_DISPLAY_ROW 25

#define DISPLAY_NEWSGROUP_COL 1
#define DISPLAY_HEADER_NUM_COL  36
#define DISPLAY_BODY_SIZE_COL   50
#define DISPLAY_ERROR_MSG_COL    37

typedef struct
{
   BOOL (*vPreprocess)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);
   HMODULE hmPreprocess;
   BOOL bLoaded;
} PREPROCESS_STRUCT;

PREPROCESS_STRUCT Preprocess[100];

typedef struct
{
   BOOL (*vMultipart)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);
   HMODULE hmMultipart;
   BOOL bLoaded;
} MULTIPART_STRUCT;

MULTIPART_STRUCT Multi[100];

typedef struct
{
   BOOL (*vGenMulti)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);
   HMODULE hmGenMulti;
   BOOL bLoaded;
} GENMULTI_STRUCT;

GENMULTI_STRUCT GenMulti[100];

typedef struct
{
   BOOL (*vGeneric)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);
   HMODULE hmGeneric;
   BOOL bLoaded;
} GENERIC_STRUCT;

GENERIC_STRUCT Generic[100];


BOOL (*pMultipart)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);
/*CHAR HeaderInfo[4096];
SHORT HeaderPos;  */
/* CHAR buf[MAX_RX_BUFFER];              /* data buffer for sending and receiving    */

#define MAX_GROUPS 100
ULONG ulMaxData = 10240000;
BOOL TimeToEnd = FALSE;
HMODULE hmLanguage;
CHAR LoadError[256];
HAB hab = (HAB)NULL;
CHAR *szGroups[MAX_GROUPS][2];
SHORT sSkipBad = 0;
SHORT sXPostSkip = 0;
ULONG ulMaxConnections = 0;
ULONG ulGYNConnections = 0;
HMTX hmtx;

PSZ SearchFor(PSZ pszSearch, PSZ pszData, ULONG iDataLen, PSZ pszResult)
{
   PSZ pszPtr1;
   PSZ pszPtr2;
   BOOL FoundData;
   PSZ pszBase;

   FoundData = FALSE;
   pszBase = pszData;
   while((pszData < pszBase + iDataLen) && (!FoundData))
   {
      if (*pszSearch == *pszData)
      {
         pszPtr1 = pszSearch + 1;
         pszPtr2 = pszData + 1;
         while ((*pszPtr1 == *pszPtr2) && (*pszPtr1 != 0x00))
         {
            pszPtr1++;
            pszPtr2++;
         }
         if (*pszPtr1 == 0x00)
         {
            FoundData = TRUE;
            pszResult = pszData;
         }
      }
      pszData++;
   }

   if (FoundData)
      return(pszResult);
   else
      return(NULL);
}

int ListNewsGroups(int Socket)
{
   ULONG ulCounter = 1;
   APIRET rc;
   int iret;
   HFILE hfGroups;
   ULONG action;
   ULONG BytesWritten;
   CHAR buf[MAX_RX_BUFFER + 1];              /* data buffer for sending and receiving    */
   CHAR szListText[256];

   rc = DosOpen("Groups.Dat",&hfGroups,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      WinLoadString( hab, hmLanguage, IDS_GYN_LIST, sizeof(szListText) - 1, szListText );
      printf("%s \r\n", szListText);
      sprintf(buf,"list\r\n");
      rc = send(Socket, buf, strlen(buf), 0);
      DosSleep(1500);
      if (rc > 0)
      {
         while ((iret = recv(Socket, buf, MAX_RX_BUFFER, 0)) == MAX_RX_BUFFER )
         {
               printf("%ldk\r",ulCounter++);
               fflush(NULL);
               DosWrite(hfGroups,buf,iret,&BytesWritten);
               DosSleep(500);
         }

         printf("%ldk\r\n",ulCounter++);
         DosWrite(hfGroups,buf,iret,&BytesWritten);
      }
      DosClose(hfGroups);
   }

   return(rc);
}
/*
int ReceiveFullMessage(int Socket, PSZ pszbuf, PSZ pszData, ULONG *ulPos, ULONG MaxSize, BOOL bPrint, USHORT *usRows)
{

   CHAR szEndLine[50];
   int iret;
   int iError = 0;
   int iMsgNbr;
   CHAR szMsgNbr[5];
   ULONG ulSize;
   double dTime;
   double dTime1;
   double dTotal;
   DATETIME  DateTime;
   DATETIME  DateTime1;

   DosGetDateTime( &DateTime );


   iret = recv(Socket, pszbuf, MAX_RX_BUFFER, 0);
   ulSize = iret;
   *ulPos = 0;
   if (bPrint)
   {
      printf("[%d;%dH%ld",*usRows, DISPLAY_BODY_SIZE_COL + 7, ulSize);
      fflush(NULL);
   }
   if (iret > 0)
   {

      strncpy(szMsgNbr, pszbuf, 3);
      szMsgNbr[3] = 0x00;

      iMsgNbr = atoi(szMsgNbr);
      switch (iMsgNbr)
      {
         case 221:
         case 222:

         strncpy(szEndLine,&pszbuf[iret - 5], 5);
         while (strncmp(szEndLine,"\r\n.\r\n", 5) != 0)
         {
            if (*ulPos + iret < MaxSize)
            {

               memcpy( &pszData[*ulPos], pszbuf, iret );
               *ulPos += iret;
            }
            else
               iError = 1;

            iret = recv(Socket, pszbuf, MAX_RX_BUFFER, 0);
   / *         DosSleep(5);  * /
            ulSize += iret;
            if (bPrint)
            {
               printf("[%d;%dH%ld",*usRows, DISPLAY_BODY_SIZE_COL + 7, ulSize);
               fflush(NULL);
            }

            if (iret < 5)
            {
               if (iret > -1)
               {
                  strncpy(szEndLine,&szEndLine[iret],5 - iret);
                  strncpy(&szEndLine[5 - iret],pszbuf, iret);
               }
               else
               {
                  WinLoadString( hab, hmLanguage, IDS_GYN_ERROR_RECV, sizeof(szEndLine) - 1, szEndLine );
                  iError = 1;
               }
            }
            else
               strncpy(szEndLine,&pszbuf[iret - 5], 5);
         }

         if (*ulPos + iret < MaxSize)
         {
            memcpy( &pszData[*ulPos], pszbuf, iret );
            *ulPos += iret;
         }
         else
            iError = 1;
         break;
         default:
            *ulPos = ulSize;
            if (iret > 0)
               memcpy( pszData, pszbuf, iret );
            break;

      }
   }
   else
      iError = iret;

   DosGetDateTime( &DateTime1 );
   if (bPrint)
   {

      dTime = (((double)DateTime1.minutes * 60) + (double)DateTime1.seconds + ( (double)DateTime1.hundredths) * 0.01);
      dTime1 = (((double)DateTime.minutes * 60)  +(double)DateTime.seconds + ((double)DateTime.hundredths) * 0.01);
      dTime -= dTime1;
      if (dTime != 0.0)
         dTotal = ((double)ulSize / dTime) / 1024.0;
      if (dTime > 0.0)
         printf("[%d;%dH%3.1fk sec[K",*usRows, DISPLAY_BODY_SIZE_COL + 14, dTotal);
/ *      else
         printf("[%d;%dH%ld[K",usRows, DISPLAY_BODY_SIZE_COL + 7, ulSize);  * /
      fflush(NULL);
   }
   return(iError);

} */

int ReceiveFullMessage(int Socket, PSZ pszbuf, PSZ pszData, ULONG *ulPos, ULONG MaxSize, BOOL bPrint, USHORT *usRows, USHORT sTimeout)
{

/*   CHAR szEndLine[50];  */
   int iret;
   int iError = 0;
   int iMsgNbr;
/*   CHAR szMsgNbr[5];  */
/*   ULONG ulSize;  */
   double dTime;
   double dTime1;
   double dTotal;
   DATETIME  DateTime;
   DATETIME  DateTime1;
   SHORT sCount;
   BOOL bNotFinished = TRUE;
   CHAR szTemp[50];
/*   HFILE hfGroups;
   ULONG action;
   ULONG BytesWritten;
   APIRET rc;  */

   WinLoadString( hab, hmLanguage, IDS_GYN_TIMEOUT_IN, sizeof(szTemp) - 1, szTemp );
   DosGetDateTime( &DateTime );


   /*ulSize = 0; */
   *ulPos = 0;
   iret = 0;
   sCount = 0;
   pszData[0] = 0x00;
   do
   {
      iret = recv(Socket, pszbuf, MAX_RX_BUFFER, MSG_DONTWAIT);
      if ((iret > 0) )
      {

         sCount = 0;
/*         if (sCount > 5000)
         {
            printf("[%d;%dHb/dl -[K", *usRows, DISPLAY_BODY_SIZE_COL);
            fflush(NULL);
         }  */
         if (*ulPos + iret < MaxSize)
         {
            memcpy( &pszData[*ulPos], pszbuf, iret );
            *ulPos += iret;
            pszData[*ulPos] = 0x00;
            if (bPrint)
            {
               printf("[%d;%dH%ld[K",*usRows, DISPLAY_BODY_SIZE_COL + 7, *ulPos);
               fflush(NULL);
            }
            if (*ulPos > 5)
            {
               switch (pszData[0])
               {
                  case '2':
                     if (strncmp(&pszData[*ulPos - 5],"\r\n.\r\n", 5) == 0)
                        bNotFinished = FALSE;
                     break;
                  default:
                     if (strncmp(&pszData[*ulPos - 2],"\r\n", 2) == 0)
                        bNotFinished = FALSE;
                     break;
               }
            }
         }
         else
            iError = 2;
      }
      else
      {
         iret = sock_errno();
         switch (iret)
         {
            case SOCEWOULDBLOCK:
               DosSleep(100);
               sCount += 100;
               iret = 1;
               if (bPrint)
               {
                  if (sCount > 5000)
                  {
                     printf(szTemp,*usRows, DISPLAY_BODY_SIZE_COL, ((sTimeout - sCount) / 1000) + 1);
                     fflush(NULL);
                  }
               }
               break;
            default:
               iError = iret;
               iret = 0;
               break;
         }
      }

/*   } while ((iret > 0) || (*ulPos == 0)); */
/*   } while (((strncmp(&pszbuf[iret - 5],"\r\n.\r\n", 5) != 0) && ((*ulPos > 0) && (pszData[0] == '2')) ||
      ((iret == -1) && (*ulPos == 0))) && (sCount < sTimeout) );  */
   } while ( (iret > 0)  && (sCount < sTimeout) && (bNotFinished));


   if ((sCount >= sTimeout))
      iError = 3;

/*   rc = DosOpen("Test.Dat",&hfGroups,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      DosWrite(hfGroups,pszData,*ulPos,&BytesWritten);
      DosClose(hfGroups);
   }  */

/*   if (iret < 0)
   {
      WinLoadString( hab, hmLanguage, IDS_GYN_ERROR_RECV, sizeof(szEndLine) - 1, szEndLine );
      iError = 1;
   }

   if (iret > 0)
   {

      strncpy(szMsgNbr, pszbuf, 3);
      szMsgNbr[3] = 0x00;

      iMsgNbr = atoi(szMsgNbr);
      switch (iMsgNbr)
      {
         case 221:
         case 222:

         strncpy(szEndLine,&pszbuf[iret - 5], 5);
         while (strncmp(szEndLine,"\r\n.\r\n", 5) != 0)
         {
            if (*ulPos + iret < MaxSize)
            {

               memcpy( &pszData[*ulPos], pszbuf, iret );
               *ulPos += iret;
            }
            else
               iError = 1;

            iret = recv(Socket, pszbuf, MAX_RX_BUFFER, 0);
   / *         DosSleep(5);  * /
            ulSize += iret;
            if (bPrint)
            {
               printf("[%d;%dH%ld",*usRows, DISPLAY_BODY_SIZE_COL + 7, ulSize);
               fflush(NULL);
            }

            if (iret < 5)
            {
               if (iret > -1)
               {
                  strncpy(szEndLine,&szEndLine[iret],5 - iret);
                  strncpy(&szEndLine[5 - iret],pszbuf, iret);
               }
               else
               {
                  WinLoadString( hab, hmLanguage, IDS_GYN_ERROR_RECV, sizeof(szEndLine) - 1, szEndLine );
                  iError = 1;
               }
            }
            else
               strncpy(szEndLine,&pszbuf[iret - 5], 5);
         }

         if (*ulPos + iret < MaxSize)
         {
            memcpy( &pszData[*ulPos], pszbuf, iret );
            *ulPos += iret;
         }
         else
            iError = 1;
         break;
         default:
            *ulPos = ulSize;
            if (iret > 0)
               memcpy( pszData, pszbuf, iret );
            break;

      }
   }
   else
      iError = iret;  */

   DosGetDateTime( &DateTime1 );
   if (bPrint)
   {

      dTime = (((double)DateTime1.minutes * 60) + (double)DateTime1.seconds + ( (double)DateTime1.hundredths) * 0.01);
      dTime1 = (((double)DateTime.minutes * 60)  +(double)DateTime.seconds + ((double)DateTime.hundredths) * 0.01);
      dTime -= dTime1;
      if (dTime != 0.0)
         dTotal = ((double)*ulPos / dTime) / 1024.0;
      if (dTime > 0.0)
         printf("[%d;%dH%3.1fk sec[K",*usRows, DISPLAY_BODY_SIZE_COL + 14, dTotal);
/*      else
         printf("[%d;%dH%ld[K",usRows, DISPLAY_BODY_SIZE_COL + 7, ulSize);  */
      fflush(NULL);
   }
   return(iError);

}

int GetData(int Socket, PSZ pszbuf, ULONG ulpszbuflen, PSZ pszData, ULONG *ulPos, ULONG MaxSize, USHORT sTimeout)
{

   int iret;
   int iError = 0;
   int iMsgNbr;
   double dTime;
   double dTime1;
   double dTotal;
   DATETIME  DateTime;
   DATETIME  DateTime1;
   SHORT sCount;
   *ulPos = 0;
   iret = 0;
   sCount = 0;
   pszData[0] = 0x00;
   do
   {
      iret = recv(Socket, pszbuf, ulpszbuflen, MSG_DONTWAIT);
      if ((iret > 0) )
      {

         sCount = 0;
         if (*ulPos + iret < MaxSize)
         {
            memcpy( &pszData[*ulPos], pszbuf, iret );
            *ulPos += iret;
            pszData[*ulPos] = 0x00;
         }
         else
            iError = 2;
      }
      else
      {
         iret = sock_errno();
         switch (iret)
         {
            case SOCEWOULDBLOCK:
               DosSleep(50);
               sCount += 50;
               iret = 1;
               break;
            default:
               iError = iret;
               iret = 0;
               break;
         }
      }

   } while ( (iret > 0)  && (sCount < sTimeout) && (strncmp(&pszData[strlen(pszData) - 2],"\r\n", 2) != 0));

   if ((sCount >= sTimeout) && (*ulPos == 0))
      iError = 3;

   return(iError);

}

int main(int argc, char *argv[])
{
   CHAR szNewsServer[99][256];
   USHORT usNewsServerCount = 0;
   int iret, sret;
   ULONG x;
   char host_sys_buf[MAX_FILENAME_LEN+1];            /* Store Host System Type */
   BOOL ElvisIsDead = TRUE;
   APIRET rc;
   SHORT sHours;
   HFILE hFile;
   ULONG action, BytesWritten;
   ULONG BytesRead;
   BOOL EOH;
   HFILE hfNewsServer;
   PSZ pszINIData;
   PSZ pszUserName = NULL;
   PSZ pszPassword = NULL;
   USHORT usGroupNum = 0;
   USHORT usGroup;
   GROUPINFO_STRUCT *GroupInfo;
   BOOL bGroupsProcessing;
   CHAR szNewDir[256];
   HFILE hfNEWS;
   ULONG ulFileNew;
   ULONG ulTotMem;
   CHAR pos[50];
   CHAR pos1[50];
   PSZ posptr;
   SHORT SrcLines;
   SHORT sListNewsGroups = 0;
   SHORT sNewsGroupsLogs = 0;
   PSZ pszLang;
   CHAR ch;


   hmLanguage =(HMODULE) NULL;
   pszLang = getenv("LANG");

   if (strnicmp(pszLang, "DE_", 3) == 0)
   {
      /* German Supported */
      rc = DosLoadModule(LoadError,sizeof(LoadError),"GYN_DE.DLL",&hmLanguage);
   }

   if (strnicmp(pszLang, "IT_", 3) == 0)
   {
      /* Italian Supported */
      rc = DosLoadModule(LoadError,sizeof(LoadError),"GYN_IT.DLL",&hmLanguage);
   }

         rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));
#ifdef NEVER
if (ElvisIsDead)
{

   HFILE hfOutFile;
   CHAR szLoadError[256];
   HMODULE MMod;
   HAB hab;
   CHAR szFileID[256];
   CHAR szDLLType[256];
   CHAR szFileName[255];
   CHAR szFileNum[5];

/*   rc = DosLoadModule(szLoadError,sizeof(szLoadError),"SharkPos.dll",&MMod);  */
   rc = DosLoadModule(szLoadError,sizeof(szLoadError),"YPOST.dll",&MMod);
   if (rc == 0)
   {
      for (x = 0; x < 1000; x++)
      {

         itoa(x, szFileNum, 10);
         strcpy(szFileName, "h:gyn00000");
         strcpy(&szFileName[strlen(szFileName) - strlen(szFileNum)], szFileNum);
         strcat(szFileName, ".OUT");

         printf("%s\r\n", szFileName);
         fflush(NULL);

         rc = DosOpen(szFileName,&hfOutFile,&action,0L,FILE_NORMAL,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
         if (rc == 0)
         {
            CHAR szUUFileName[256];
            ULONG ulFileSize;
            ULONG NewFileBeginPos;
            ULONG NewFilePos;
            PSZ pszTestData;

            strcpy(szFileID, "");
            WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
            if (strcmp(szFileID,"GetYourNews") == 0)
            {
               WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
               if (strcmp(szDLLType,"Multipart") == 0)
               {
                  PSZ pszHeader;
                  PSZ pszHeaderEnd;
                  PSZ pszBody;
                  PSZ pszBodyEnd;
                  ULONG ulHeaderLen;
                  ULONG ulBodyLen;

                  DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFilePos);
                  rc = DosAllocMem((VOID *)&pszTestData,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                  if (rc == 0)
                  {

                     DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                     DosRead(hfOutFile,pszTestData,NewFilePos, &BytesRead);

                     pszHeader = pszTestData;
                     pszHeaderEnd = SearchFor("\r\n.\r\n",pszTestData, BytesRead, pszHeaderEnd);
                     ulHeaderLen = (pszHeaderEnd + 5) - pszHeader;
                     pszBody = pszHeaderEnd + 5;
                     pszBodyEnd = SearchFor("\r\n.\r\n",pszBody, BytesRead - (pszBody - pszTestData), pszBodyEnd);
                     ulBodyLen = (pszBodyEnd + 5) - pszBody;

                     rc = DosQueryProcAddr(MMod,0,"CheckForMultipart",&pMultipart);
                     if (rc == 0)
                        pMultipart(pszHeader, ulHeaderLen, pszBody, ulBodyLen, "temp", TRUE);

                     DosFreeMem(pszTestData);
                  }
               }
            }
            DosClose(hfOutFile);
         }
         else
            x = 1000;
      }

      DosFreeModule(MMod);
   }

}


return(0);

#endif

   rc = printf("[2J");
   fflush(NULL);
   rc = printf("[6n");
   fflush(NULL);
   gets(pos);
   rc = printf("[1B");
   fflush(NULL);
   rc = printf("[6n");
   fflush(NULL);
   gets(pos1);
   while (strcmp(pos, pos1) != 0)
   {
      strcpy(pos, pos1);
      rc = printf("[1B");
      fflush(NULL);

      rc = printf("[6n");
      fflush(NULL);
      gets(pos1);
   }
   posptr = &pos[2];
   while (*posptr != ';')
      posptr++;
   *posptr = 0x00;
   SrcLines = atoi(&pos[2]);

   rc = printf("[2J");
   fflush(NULL);
/*   printf("\r\n%d\r\n", SrcLines);
   fflush(NULL);
   exit(0);  */

   if (argc > 1)
   {
      int x;

      for (x = 1; x < argc; x++)
      {

         if (strnicmp(argv[x],"/U=", 3) == 0)
         {
            pszUserName = &argv[x][3];
         }
         if (strnicmp(argv[x],"/P=", 3) == 0)
         {
            pszPassword = &argv[x][3];
         }
         if (strnicmp(argv[x],"-U=", 3) == 0)
         {
            pszUserName = &argv[x][3];
         }
         if (strnicmp(argv[x],"-P=", 3) == 0)
         {
            pszPassword = &argv[x][3];
         }
         if (strnicmp(argv[x],"-LIST", 5) == 0)
         {
            sListNewsGroups = 1;
         }
         if (strnicmp(argv[x],"/LIST", 5) == 0)
         {
            sListNewsGroups = 1;
         }

         if (strnicmp(argv[x],"/SKIPBAD", 8) == 0)
         {
            sSkipBad = 1;
         }

         if (strnicmp(argv[x],"-SKIPBAD", 8) == 0)
         {
            sSkipBad = 1;
         }

         if (strnicmp(argv[x],"-CROSSPOSTSKIP", 14) == 0)
         {
            sXPostSkip = 1;
         }

         if (strnicmp(argv[x],"/CROSSPOSTSKIP", 14) == 0)
         {
            sXPostSkip = 1;
         }

         if (strnicmp(argv[x],"-MAXCONNECTIONS", 15) == 0)
         {
            if (strlen(argv[x]) > 16)
               ulMaxConnections = atol(&argv[x][16]);
            else
               ulMaxConnections = 0;
         }
         if (strnicmp(argv[x],"/MAXCONNECTIONS", 15) == 0)
         {
            if (strlen(argv[x]) > 16)
               ulMaxConnections = atol(&argv[x][16]);
            else
               ulMaxConnections = 0;
         }
         if (strnicmp(argv[x],"-LOGS", 5) == 0)
         {
            sNewsGroupsLogs = 1;
         }
         if (strnicmp(argv[x],"/LOGS", 5) == 0)
         {
            sNewsGroupsLogs = 1;
         }

         if ((strnicmp(argv[x],"-?", 2) == 0) ||
             (strnicmp(argv[x],"/?", 2) == 0) ||
             (strnicmp(argv[x],"?", 1) == 0) ||
             (strnicmp(argv[x],"help", 4) == 0) ||
             (strnicmp(argv[x],"/help", 5) == 0) ||
             (strnicmp(argv[x],"-help", 5) == 0))
         {
            SHORT x;
            CHAR szWelcome[128];

            WinLoadString( hab, hmLanguage, IDS_GYN_WELCOME_1, sizeof(szWelcome) - 1, szWelcome );
            printf(szWelcome, VERSION_MAJOR, VERSION_MINOR, VERSION_INCENDENTAL, VERSION_DATE);
            WinLoadString( hab, hmLanguage, IDS_GYN_WELCOME_2, sizeof(szWelcome) - 1, szWelcome );
            for (x = IDS_GYN_WELCOME_2; strlen(szWelcome) > 0; x++)
            {
               printf(szWelcome);
               WinLoadString( hab, hmLanguage, x + 1, sizeof(szWelcome) - 1, szWelcome );

            }

            exit(0);
         }

      }
   }

   rc = DosOpen("NewsServer.INI",&hfNewsServer,&action,0L,FILE_NORMAL,
      OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      HFILE hfNewServer;

      rc = DosOpen("NewsServer2.INI",&hfNewServer,&action,0L,FILE_NORMAL,
         OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
      if (rc == 0)
      {
         CHAR szData[256];
         ULONG NewFilePos;
         ULONG NewFileBeginPos;

         strcpy(szData,"[SERVER]");
         DosWrite(hfNewServer,szData,strlen(szData),&BytesWritten);
         DosSetFilePtr(hfNewsServer, 0L, FILE_END, &NewFilePos);
         rc = DosAllocMem((VOID *)&pszINIData,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
         if (rc == 0)
         {


            DosSetFilePtr(hfNewsServer, 0L, FILE_BEGIN, &NewFileBeginPos);
            DosRead(hfNewsServer,pszINIData,NewFilePos, &BytesRead);
            DosWrite(hfNewServer,pszINIData,NewFilePos,&BytesWritten);
            DosFreeMem(pszINIData);
            DosClose(hfNewsServer);
            DosClose(hfNewServer);
         }
      }
      DosDelete("NewsServer.ini");

   }
   strcpy(szNewsServer[0],"");
   rc = DosOpen("NewsServer2.INI",&hfNewsServer,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      switch (action)
      {
         case FILE_CREATED:
         {
            CHAR szData[256];
            CHAR szCreate[128];

            DosBeep(1200,100);
            DosBeep(1200,100);
            DosBeep(1200,100);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_1, sizeof(szCreate) - 1, szCreate );
            printf(szCreate);
            DosSleep(1500);
            DosBeep(1200,100);
            DosBeep(2400,100);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_2, sizeof(szCreate) - 1, szCreate );
            printf(szCreate);
            DosSleep(4000);
            strcpy(szData,"[SERVER]news.myserver.com\r\n");
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            strcpy(szData,"alt.binaries.sounds.1970s.mp3\r\n");
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            strcpy(szData,"alt.binaries.sounds.1980s.mp3\r\n");
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            strcpy(szData,"alt.binaries.sounds.1990s.mp3\r\n");
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            strcpy(szData,"\r\n");
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_3, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_4, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_5, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_6, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_7, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_8, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_9, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            WinLoadString( hab, hmLanguage, IDS_GYN_CREATE_MSG_10, sizeof(szData) - 1, szData );
            DosWrite(hfNewsServer,szData,strlen(szData),&BytesWritten);
            DosClose(hfNewsServer);
            rc = StartProgramSync("E.exe",1,"NewsServer2.ini",
               "\\QUEUES\\E",SSF_TYPE_DEFAULT, (ULONG)0, (ULONG)4, (HWND)NULL);
            rc = DosOpen("NewsServer2.INI",&hfNewsServer,&action,0L,FILE_NORMAL,
               OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
         }
         case FILE_EXISTED:
         {
            ULONG NewFilePos;
            ULONG NewFileBeginPos;
            PSZ pszNewsServer;
            PSZ pszLineBegin;
            PSZ pszLineEnd;
            PSZ pszCopy;
            BOOL NoBlankLine = TRUE;

            DosSetFilePtr(hfNewsServer, 0L, FILE_END, &NewFilePos);
            rc = DosAllocMem((VOID *)&pszINIData,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
            if (rc == 0)
            {

               DosSetFilePtr(hfNewsServer, 0L, FILE_BEGIN, &NewFileBeginPos);
               DosRead(hfNewsServer,pszINIData,NewFilePos, &BytesRead);
               pszLineBegin = pszINIData;
               pszCopy = pszINIData;
               while (pszLineBegin != NULL)
               {
                  pszLineEnd = SearchFor("\r\n",pszLineBegin, NewFilePos - (pszLineBegin - pszINIData), pszLineEnd);
                  if (pszLineEnd != NULL)
                  {
                     PSZ pszServerLine;

                     pszServerLine = SearchFor("[SERVER]",pszLineBegin, pszLineEnd - pszLineBegin, pszServerLine);
                     if (pszServerLine != NULL)
                     {
                        strncpy(szNewsServer[usNewsServerCount], pszLineBegin + 8, pszLineEnd - (pszLineBegin + 8));
                        szNewsServer[usNewsServerCount][pszLineEnd - (pszLineBegin + 8)] = 0x00;
                        while (szNewsServer[usNewsServerCount][strlen(szNewsServer[usNewsServerCount]) - 1] == ' ')
                           szNewsServer[usNewsServerCount][strlen(szNewsServer[usNewsServerCount]) - 1] = 0x00;
                        usNewsServerCount++;
                     }
                     else
                     {
                        strncpy(pszCopy, pszLineBegin, pszLineEnd - pszLineBegin);
                        pszCopy[pszLineEnd - pszLineBegin] = 0x00;
                        if (strlen(pszCopy) == 0)
                           NoBlankLine = FALSE;
                        if (NoBlankLine)
                        {
                           CHAR szData[128];

                           if (usNewsServerCount > 0)
                              szGroups[usGroupNum][0] = szNewsServer[usNewsServerCount - 1];
                           else
                              szGroups[usGroupNum][0] = NULL;
                           szGroups[usGroupNum++][1] = pszCopy;
                           switch (usGroupNum)
                           {
/*                              case 10:
                                 printf("Getting Greedy?\r\n");
                                 DosBeep(1200,100);
                                 DosSleep(1000);
                                 break;
                              case 20:
                                 printf("This is going to take a while to download\r\n");
                                 DosBeep(1200,100);
                                 DosBeep(1200,100);
                                 DosSleep(1000);
                                 break;
                              case 30:
                                 printf("Damn, I hope you have a big hard drive\r\n");
                                 DosBeep(1200,100);
                                 DosBeep(1200,100);
                                 DosBeep(1200,100);
                                 DosSleep(2000);
                                 break;
                              case 50:
                                 printf("You might as well send out for pizza\r\n");
                                 DosBeep(2400,100);
                                 DosBeep(2400,100);
                                 DosSleep(2000);
                                 break;  */
                              case 99:
                                 usGroupNum = 98;
                                 WinLoadString( hab, hmLanguage, IDS_GYN_SUPPORT, sizeof(szData) - 1, szData );
                                 printf(szData);
                                 DosBeep(2400,100);
                                 DosBeep(2400,100);
                                 DosSleep(4000);
                                 break;
                           }
                        }
                        pszCopy += (pszLineEnd - pszLineBegin) + 1;
                     }
                     pszLineBegin = pszLineEnd + 2;
                  }
                  else
                     pszLineBegin = pszLineEnd;

               }
               *pszCopy = 0x00;
               szGroups[usGroupNum][1] = pszCopy;

            }

            break;
         }
      }
      DosClose(hfNewsServer);

   }
   else
   {
      CHAR szData[128];

      WinLoadString( hab, hmLanguage, IDS_GYN_INI_FILE_ERROR, sizeof(szData) - 1, szData );
      printf(szData);
      exit(0);
   }


#ifndef GYNNR
   printf("If you feel that you used this progam enough to make a donation,\r\n");
   printf("   Donations can be made at www.paypal.com,\r\n");
   printf("   e-mail address: n_woodruff@bellsouth.net\r\n");
   printf("        Thanks martin.baechler,\r\n");
   printf("          The Management.\r\n");
   printf("\r\n\r\n  Press <Enter> to continue\r\n");

   ch = _getch();

#endif

   FindDlls();
/*   FindDllsMulti();
   FindDllsGenMulti();
   FindDllsGeneric();  */

/*   DecodeUUFile("Test1.jpg");  */

   if ((usGroupNum == 0) && (sListNewsGroups))
      usGroupNum++;

   rc = DosAllocMem((VOID *)&GroupInfo, sizeof(GROUPINFO_STRUCT) * usGroupNum ,PAG_READ | PAG_WRITE | PAG_COMMIT);
   usGroup = 0;
   printf("[2J");
   fflush(NULL);

   if (sListNewsGroups)
   {

      strcpy(GroupInfo[0].szServer,szNewsServer[0]);
      strcpy(GroupInfo[0].szGroup,szGroups[usGroup][1]);
      GroupInfo[0].bNotFinished = TRUE;
      GroupInfo[0].usRow = usGroup + 1;
      GroupInfo[0].ulDelaySleep = 16L;
      GroupInfo[0].bSleeping = FALSE;
      GroupInfo[0].SrcLines = SrcLines;
      GroupInfo[0].UpNews.ulLast = 0;
      if (pszUserName != NULL)
         strcpy(GroupInfo[0].szUserID, pszUserName);
      else
         strcpy(GroupInfo[0].szUserID, "");

      if (pszPassword != NULL)
         strcpy(GroupInfo[0].szPassword, pszPassword);
      else
         strcpy(GroupInfo[0].szPassword, "");

      NewsGroupList(&GroupInfo[0]);
      exit(0);
   }


   rc = DosOpen("LastNews2.INI",&hfNEWS,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);

   if (rc == 0)
   {
      PSZ pszNewsDir;
      PSZ pszWorkDir;
      CHAR szMaxMem[50];

      while (usGroup < usGroupNum)
      {
         strcpy(GroupInfo[usGroup].szServer,szGroups[usGroup][0]);
         strcpy(GroupInfo[usGroup].szGroup,szGroups[usGroup][1]);
         GroupInfo[usGroup].bNotFinished = TRUE;
         GroupInfo[usGroup].usRow = usGroup + 1;
         GroupInfo[usGroup].ulDelaySleep = 16L;
         GroupInfo[usGroup].bSleeping = FALSE;
         GroupInfo[usGroup].SrcLines = SrcLines;
         GroupInfo[usGroup].UpNews.ulLast = 0;
         GroupInfo[usGroup].bLogs = sNewsGroupsLogs;
         if (pszUserName != NULL)
            strcpy(GroupInfo[usGroup].szUserID, pszUserName);
         else
            strcpy(GroupInfo[usGroup].szUserID, "");

         if (pszPassword != NULL)
            strcpy(GroupInfo[usGroup].szPassword, pszPassword);
         else
            strcpy(GroupInfo[usGroup].szPassword, "");

         DosSetFilePtr(hfNEWS,0,FILE_BEGIN,&ulFileNew);
         DosRead(hfNEWS,&GroupInfo[usGroup].UpNews,sizeof(UPNEWSGROUP_STRUCT),&BytesRead);
         while ((BytesRead > 0) && ((strcmp(GroupInfo[usGroup].UpNews.szNewsGroup,szGroups[usGroup][1]) != 0)
            || (strcmp(GroupInfo[usGroup].UpNews.szNewsServer,szGroups[usGroup][0]) != 0)))
         {
            DosRead(hfNEWS,&GroupInfo[usGroup].UpNews,sizeof(UPNEWSGROUP_STRUCT),&BytesRead);
         }

         if (BytesRead == 0)
         {
            strcpy(GroupInfo[usGroup].UpNews.szNewsServer,szGroups[usGroup][0]);
            strcpy(GroupInfo[usGroup].UpNews.szNewsGroup,szGroups[usGroup][1]);
         }

         GroupInfo[usGroup].ulDirSize = sizeof(GroupInfo[usGroup].szWorkingDir) - 1;
         GroupInfo[usGroup].szWorkingDir[0] = '\\';
         rc = DosQueryCurrentDir(0,&GroupInfo[usGroup].szWorkingDir[1], &GroupInfo[usGroup].ulDirSize);
         pszNewsDir = GroupInfo[usGroup].szGroup;
         pszWorkDir = GroupInfo[usGroup].szNewsDir;
         while (pszNewsDir < GroupInfo[usGroup].szGroup + strlen(GroupInfo[usGroup].szGroup))
         {
            while ((*pszNewsDir != '.') && (*pszNewsDir != 0x00))
               *(pszWorkDir++) = *(pszNewsDir++);
            *pszWorkDir = 0x00;

            sprintf(szNewDir,"%s\\%s",GroupInfo[usGroup].szWorkingDir, GroupInfo[usGroup].szNewsDir);

            rc = DosSetCurrentDir(szNewDir);
            switch (rc)
            {
               case 0:
                  rc = DosSetCurrentDir(GroupInfo[usGroup].szWorkingDir);
                  break;
               case 3:
               {
                  rc = DosCreateDir(szNewDir, NULL);
                  break;
               }
            }

            if (*pszNewsDir == '.')
            {
               *(pszWorkDir++) = '\\';
               *pszWorkDir = 0x00;
               pszNewsDir++;
            }

         }

         usGroup++;
      }

      if (ulMaxConnections > 0)
         rc = DosCreateMutexSem("\\SEM32\\GYNCON", &hmtx, 0L, FALSE);

      usGroup = 0;
      while (usGroup < usGroupNum)
      {
         rc = DosCreateThread(&GroupInfo[usGroup].ThreadID,(PFNTHREAD)ServiceNewsGroup,
               (ULONG)&GroupInfo[usGroup],STACK_COMMITTED, 1024000L);
         if (rc != 0)
         {
            switch (rc)
            {
               case  8:
                  WinLoadString( hab, hmLanguage, IDS_GYN_DOS_CREATE_ERROR_8, sizeof(GroupInfo[usGroup].szErrorInfo) - 1, GroupInfo[usGroup].szErrorInfo );
                  break;
               case  95:
                  WinLoadString( hab, hmLanguage, IDS_GYN_DOS_CREATE_ERROR_95, sizeof(GroupInfo[usGroup].szErrorInfo) - 1, GroupInfo[usGroup].szErrorInfo );
                  break;
               case  115:
                  WinLoadString( hab, hmLanguage, IDS_GYN_DOS_CREATE_ERROR_115, sizeof(GroupInfo[usGroup].szErrorInfo) - 1, GroupInfo[usGroup].szErrorInfo );
                  break;
               case  164:
                  WinLoadString( hab, hmLanguage, IDS_GYN_DOS_CREATE_ERROR_164, sizeof(GroupInfo[usGroup].szErrorInfo) - 1, GroupInfo[usGroup].szErrorInfo );
                  break;
            }
         }
         usGroup++;
      }

      do
      {
         usGroup = 0;
         while (usGroup < usGroupNum - 1)
         {
            USHORT p1, p2, p3;

            p3 = 0;
            p1 = 0;
            while ((p3 < usGroupNum) && (p1 == 0))
            {
               if (usGroup + 1 == GroupInfo[p3].usRow)
                  p1 = p3;
               p3++;
            }

            p3 = 0;
            p2 = 0;
            while ((p3 < usGroupNum) && (p2 == 0))
            {
               if (usGroup + 2 == GroupInfo[p3].usRow)
                  p2 = p3;
               p3++;
            }

            if ((GroupInfo[p1].bNotFinished == FALSE) && (GroupInfo[p2].bNotFinished == TRUE))
            {
               USHORT usHold;
               CHAR szGroupTrunc[50];

               usHold = GroupInfo[p2].usRow;
               GroupInfo[p2].usRow = GroupInfo[p1].usRow;
               GroupInfo[p1].usRow = usHold;
               usGroup++;

               strncpy(szGroupTrunc,GroupInfo[p2].szGroup, 34);
               szGroupTrunc[34] = 0x00;
               if (strlen(szGroupTrunc) > 31)
                  strcpy(&szGroupTrunc[31],"...");
/*               if (GroupInfo[p2].usRow < MAX_DISPLAY_ROW)  */
               if (GroupInfo[p2].usRow < SrcLines)
               {
                  printf("[%d;%dH%s[K", GroupInfo[p2].usRow, DISPLAY_NEWSGROUP_COL, szGroupTrunc);
                  printf("[%d;%dHMsg #%ld", GroupInfo[p2].usRow, DISPLAY_HEADER_NUM_COL, GroupInfo[p2].ulMsgNum);
                  fflush(NULL);
               }


               strncpy(szGroupTrunc,GroupInfo[p1].szGroup, 34);
               szGroupTrunc[34] = 0x00;
               if (strlen(szGroupTrunc) > 31)
                  strcpy(&szGroupTrunc[31],"...");
/*               if (GroupInfo[p1].usRow < MAX_DISPLAY_ROW)  */
               if (GroupInfo[p1].usRow < SrcLines)
               {
                  CHAR szFinished[128];
                  WinLoadString( hab, hmLanguage, IDS_GYN_DISPLAY_FINISHED, sizeof(szFinished) - 1, szFinished );
                  if (strlen(GroupInfo[p1].szErrorInfo) > 30)
                     GroupInfo[p1].szErrorInfo[30] = 0x00;
                  printf(szFinished,GroupInfo[p1].usRow, DISPLAY_NEWSGROUP_COL, szGroupTrunc, GroupInfo[p1].szErrorInfo);
                  fflush(NULL);
               }
            }
            usGroup++;
         }

         usGroup = 0;
         while (usGroup < usGroupNum - 1)
         {
            USHORT p1, p2, p3;

            p3 = 0;
            p1 = 0;
            while ((p3 < usGroupNum) && (p1 == 0))
            {
               if (usGroup + 1 == GroupInfo[p3].usRow)
                  p1 = p3;
               p3++;
            }

            p3 = 0;
            p2 = 0;
            while ((p3 < usGroupNum) && (p2 == 0))
            {
               if (usGroup + 2 == GroupInfo[p3].usRow)
                  p2 = p3;
               p3++;
            }

            if ((GroupInfo[p1].ulDelaySleep > GroupInfo[p2].ulDelaySleep) && (GroupInfo[p2].bNotFinished == TRUE))
            {
               USHORT usHold;
               CHAR szGroupTrunc[50];

               strncpy(szGroupTrunc,GroupInfo[p2].szGroup, 34);
               szGroupTrunc[34] = 0x00;
               if (strlen(szGroupTrunc) > 31)
                  strcpy(&szGroupTrunc[31],"...");
               usHold = GroupInfo[p2].usRow;
               GroupInfo[p2].usRow = GroupInfo[p1].usRow;
               GroupInfo[p1].usRow = usHold;
               strncpy(szGroupTrunc,GroupInfo[p2].szGroup, 34);
               szGroupTrunc[34] = 0x00;
               if (strlen(szGroupTrunc) > 31)
                  strcpy(&szGroupTrunc[31],"...");
/*               if (GroupInfo[p2].usRow < MAX_DISPLAY_ROW)  */
               if (GroupInfo[p2].usRow < SrcLines)
               {
                  printf("[%d;%dH%s[K",GroupInfo[p2].usRow,  DISPLAY_NEWSGROUP_COL, szGroupTrunc);
                  printf("[%d;%dHMsg #%ld", GroupInfo[p2].usRow, DISPLAY_HEADER_NUM_COL, GroupInfo[p2].ulMsgNum);
                  fflush(NULL);
               }
               strncpy(szGroupTrunc,GroupInfo[p1].szGroup, 34);
               szGroupTrunc[34] = 0x00;
               if (strlen(szGroupTrunc) > 31)
                  strcpy(&szGroupTrunc[31],"...");
/*               if (GroupInfo[p1].usRow < MAX_DISPLAY_ROW)  */
               if (GroupInfo[p1].usRow < SrcLines)
               {
                  printf("[%d;%dH%s[K",GroupInfo[p1].usRow, DISPLAY_NEWSGROUP_COL, szGroupTrunc);
                  printf("[%d;%dHMsg #%ld", GroupInfo[p1].usRow, DISPLAY_HEADER_NUM_COL, GroupInfo[p1].ulMsgNum);
                  fflush(NULL);
               }
               usGroup++;
            }
            usGroup++;
         }


         usGroup = 0;
         DosSetFilePtr(hfNEWS,0,FILE_BEGIN,&ulFileNew);
         while (usGroup < usGroupNum)
            DosWrite(hfNEWS,&GroupInfo[usGroup++].UpNews,sizeof(UPNEWSGROUP_STRUCT),&BytesRead);
         usGroup = 0;
         bGroupsProcessing = FALSE;
         rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));
         WinLoadString( hab, hmLanguage, IDS_GYN_DISPLAY_MAX_MEM, sizeof(szMaxMem) - 1, szMaxMem );
         printf(szMaxMem, SrcLines, 10, ulTotMem);
         DosSleep(2000);
         while (usGroup < usGroupNum)
            bGroupsProcessing |= GroupInfo[usGroup++].bNotFinished;


         if (_kbhit())
         {
            CHAR ch;

            ch = _getch();

            switch (ch)
            {
               case 's':
               case 'S':
                  TimeToEnd = TRUE;
                  break;
            }
         }


      }
      while (bGroupsProcessing);
   }
   DosClose(hfNEWS);
   printf("[%d;1H",usGroupNum + 1);
   return(0);
}



VOID _System ServiceNewsGroup(GROUPINFO_STRUCT *GroupInfo)
{

   unsigned short port;       /* port client will connect to              */
   struct hostent *hostnm;    /* server host name information             */
   struct sockaddr_in server; /* server address                           */
   int s;                     /* client socket                            */
   BOOL ElvisIsDead = TRUE;
   SHORT iret;
   PSZ pszUserName;
   APIRET rc;
   PSZ pszPassword;
/*   HFILE hfGroup;  */
   ULONG ulUpNewsRec;
   ULONG ulFileNew;
   ULONG action;
   ULONG BytesRead;
   ULONG BytesWritten;
   HFILE hFile;
/*   CHAR *szGroups[100];  */
   ULONG x;
   CHAR GroupBuf[MAX_RX_BUFFER];
   PSZ MessageBody;
   ULONG ulMsgBodyPos;
   PSZ MessageHeader;
   ULONG ulMsgHeaderPos;
   ULONG ulSleepSec;
   CHAR szNewDir[256];
   ULONG ulTotMem;
   CHAR szGroupTrunc[50];
   CHAR szGroupLast[256];
   USHORT usMissed = 0;
   CHAR szSleepSeconds[50];
   CHAR szSleepSecondsMany[50];
   CHAR szSleepMinutes[50];
   CHAR szSleepMinutesMany[50];
   CHAR szSleepHours[50];
   CHAR szSleepHoursMany[50];
   APIRET MutexOpen;
   PID pid;
   TID tic;
   ULONG ulSemCount;
   BOOL bGoodToGo;


   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_SECONDS_1, sizeof(szSleepSeconds) - 1, szSleepSeconds );
   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_SECONDS_MANY, sizeof(szSleepSecondsMany) - 1, szSleepSecondsMany );

   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_MINUTES_1, sizeof(szSleepMinutes) - 1, szSleepMinutes );
   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_MINUTES_MANY, sizeof(szSleepMinutesMany) - 1, szSleepMinutesMany );

   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_HOURS_1, sizeof(szSleepHours) - 1, szSleepHours );
   WinLoadString( hab, hmLanguage, IDS_GYN_SLEEP_HOURS_MANY, sizeof(szSleepHoursMany) - 1, szSleepHoursMany );

   strncpy(szGroupTrunc,GroupInfo->szGroup, 34);
   szGroupTrunc[34] = 0x00;
   if (strlen(szGroupTrunc) > 31)
      strcpy(&szGroupTrunc[31],"...");
   if (GroupInfo->usRow < GroupInfo->SrcLines)
   {
      printf("[%d;%dH%s[K",GroupInfo->usRow, DISPLAY_NEWSGROUP_COL, szGroupTrunc);
/*      fflush(NULL);  */
   }

   DosSleep(500 * GroupInfo->usRow);

   sock_init();

   port = 119;  /* News port */


   hostnm = gethostbyname(GroupInfo->szServer);
   if (hostnm != (struct hostent *) 0)
   {

      /*
         * Put a message into the buffer.
      */

      /*
         * Put the server information into the server structure.
         * The port must be put into network byte order.
      */
      server.sin_family      = AF_INET;
      server.sin_port        = htons(port);
/*    server.sin_addr.s_addr = inet_addr(szMyAddress); / * Server's Address   */
      server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);


/*      if (ulMaxConnections > 0)
         MutexOpen = DosOpenMutexSem("\\SEM32\\GYNCON", &hmtx);
      else
         MutexOpen = NO_ERROR; */

      while ((!TimeToEnd) && (GroupInfo->bNotFinished == TRUE))
      {

         /*
            * Get a stream socket.
         */


/*       rc = DosQuerySysInfo(QSV_TOTPHYSMEM,
                              QSV_TOTPHYSMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));  */

/*         rc = DosQuerySysInfo(QSV_MAXPRMEM,
                              QSV_MAXPRMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));  */


         if (ulMaxConnections > 0)
         {

            rc = DosRequestMutexSem(hmtx, (ULONG) SEM_INDEFINITE_WAIT);
            if (rc == NO_ERROR)
            {
               if (ulGYNConnections < ulMaxConnections)
               {
                  ulGYNConnections++;
                  bGoodToGo = TRUE;
               }
               else
                  bGoodToGo = FALSE;

               rc = DosReleaseMutexSem(hmtx);
            }
         }

         if (((rc == NO_ERROR) && (bGoodToGo == TRUE)) || (ulMaxConnections == 0))
         {

/*         if (rc == 0)  */
            rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                                 (PVOID)&ulTotMem,
                                 sizeof(ulTotMem));
            if ((rc == 0) && (ulTotMem > (MESSAGE_HEADER_SIZE + ulMaxData) * 2))
            {

               rc = DosAllocMem((VOID *)&MessageHeader,MESSAGE_HEADER_SIZE ,PAG_READ | PAG_WRITE | PAG_COMMIT);
               if (rc == 0)
               {

                  rc = DosAllocMem((VOID *)&MessageBody,ulMaxData ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                  if (rc == 0)
                  {

                     if ((s = socket(PF_INET, SOCK_STREAM, 0)) >= 0)
                     {

                        if (GroupInfo->usRow < GroupInfo->SrcLines)
                        {
                           printf("[%d;%dHConnecting...[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,iret);
                           fflush(NULL);
                        }
                        if ((iret = connect(s, (struct sockaddr *)&server, sizeof(server))) == 0)
                        {
                           LIST_STRUCT MyNews;
                           SHORT sLines;
                           PSZ pszTemp;
                           PSZ pszTemp1;
                           PSZ pszLineData;
                           CHAR szTemp[128];
                           SHORT sGroupNum = 0;
                           CHAR szCurrentDir[512];
                           ULONG ulDirSize;
                           BOOL bWriteBody = FALSE;
                           BOOL bWriteFullHeader = FALSE;
                           BOOL bWriteSubject = FALSE;
                           ULONG ulCounter = 1;
                           FSALLOCATE fsAlloc;
                           double dFreeSpace;
                           int RFMret;
                           int iSockErr;
                           BOOL bRecError = FALSE;

               /*            bWriteBody = TRUE; */

                           RFMret = GetData(s, GroupBuf, sizeof(GroupBuf), GroupInfo->szWelcomeMsg, &GroupInfo->sWelcomeSize, sizeof(GroupInfo->szWelcomeMsg), 2000);
                           if (RFMret == 0)
                           {
                              sprintf(GroupBuf,"group %s\r\n", GroupInfo->szGroup);

                              rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                              if (rc == strlen(GroupBuf))
                              {
                                 BOOL bReissueGroup = FALSE;

                                 RFMret = GetData(s, GroupBuf, sizeof(GroupBuf), GroupInfo->szSendMsg, &GroupInfo->sSendSize,
                                    sizeof(GroupInfo->szSendMsg), 2000);

                                 while (((GroupInfo->szSendMsg[0] != '2') && (RFMret == 0) && (GroupInfo->bNotFinished == TRUE)) || (bReissueGroup == TRUE))
                                 {
                                    switch (GroupInfo->szSendMsg[0])
                                    {
                                       case '4':
                                       {
                                          switch (GroupInfo->szSendMsg[1])
                                          {
                                             case '8':
                                                /* Appears that we need authentication */
                                                if (strlen(GroupInfo->szUserID) != 0)
                                                {
                                                   sprintf(GroupBuf,"authinfo user %s\r\n", GroupInfo->szUserID);
                                                   rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                                   if (rc != strlen(GroupBuf))
                                                      GroupInfo->bNotFinished = FALSE;
                                                   else
                                                      bReissueGroup = TRUE;
                                                }
                                                else
                                                {
                                                   WinLoadString( hab, hmLanguage, IDS_GYN_USERID_REQUIRED, sizeof(GroupInfo->szErrorInfo) - 1, GroupInfo->szErrorInfo );
                                                   GroupInfo->bNotFinished = FALSE;
                                                }
                                                break;
                                             default:
                                                strcpy(GroupInfo->szErrorInfo, &GroupInfo->szSendMsg[4]);
                                                GroupInfo->bNotFinished = FALSE;
                                                break;
                                          }
                                          break;
                                       }
                                       case '3':
                                       {
                                          switch (GroupBuf[1])
                                          {
                                             case '8':
                                                if (strlen(GroupInfo->szPassword) > 0)
                                                {
                                                   sprintf(GroupBuf,"authinfo pass %s\r\n", GroupInfo->szPassword);
                                                   rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                                   if (rc != strlen(GroupBuf))
                                                      GroupInfo->bNotFinished = FALSE;
                                                   else
                                                      bReissueGroup = TRUE;
                                                }
                                                else
                                                {
                                                   WinLoadString( hab, hmLanguage, IDS_GYN_PASSWORD_REQUIRED, sizeof(GroupInfo->szErrorInfo) - 1, GroupInfo->szErrorInfo );
                                                   GroupInfo->bNotFinished = FALSE;
                                                }
                                                break;
                                             default:
                                                GroupInfo->bNotFinished = FALSE;
                                                break;
                                          }
                                          break;
                                       }
                                       case '2':
                                       {
                                          if (bReissueGroup == TRUE)
                                          {
                                             sprintf(GroupBuf,"group %s\r\n", GroupInfo->szGroup);

                                             rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                             if (rc == strlen(GroupBuf))
                                                bReissueGroup = FALSE;

                                          }

                                          break;
                                       }

                                    }

                                    if (GroupInfo->bNotFinished == TRUE)
                                       RFMret = GetData(s, GroupBuf, sizeof(GroupBuf), GroupInfo->szSendMsg, &GroupInfo->sSendSize,
                                          sizeof(GroupInfo->szSendMsg), 2000);

                                 }
                                 if ((GroupInfo->bNotFinished == TRUE) && (RFMret == 0))
                                 {

                                    pszTemp = GroupInfo->szSendMsg;
                                    pszTemp1 = szTemp;

                                    while (*pszTemp != ' ')
                                       *(pszTemp1++) = *(pszTemp++);

                                    *pszTemp1 = 0x00;

                                    MyNews.SomeNumber = atol(szTemp);

                                    while (*pszTemp == ' ')
                                       pszTemp++;

                                    pszTemp1 = szTemp;

                                    while (*pszTemp != ' ')
                                       *(pszTemp1++) = *(pszTemp++);

                                    *pszTemp1 = 0x00;

                                    MyNews.TotalMsgs = atol(szTemp);

                                    while (*pszTemp == ' ')
                                       pszTemp++;

                                    pszTemp1 = szTemp;

                                    while (*pszTemp != ' ')
                                       *(pszTemp1++) = *(pszTemp++);

                                    *pszTemp1 = 0x00;

                                    MyNews.ulStarting = atol(szTemp);

                                    while (*pszTemp == ' ')
                                       pszTemp++;

                                    pszTemp1 = szTemp;

                                    while (*pszTemp != ' ')
                                       *(pszTemp1++) = *(pszTemp++);

                                    *pszTemp1 = 0x00;

                                    MyNews.ulEnding = atoi(szTemp);

                                    while (*pszTemp == ' ')
                                       pszTemp++;

                                    pszTemp1 = MyNews.szNewsGroup;
                                    while (*pszTemp != 0x00)
                                       *(pszTemp1++) = *(pszTemp++);

                                    *pszTemp1 = 0x00;

                                    if (GroupInfo->UpNews.ulLast > 0)
                                       if ((GroupInfo->UpNews.ulLast >= MyNews.ulStarting) && (GroupInfo->UpNews.ulLast <= MyNews.ulEnding))
                                          MyNews.ulStarting = GroupInfo->UpNews.ulLast + 1;

                                    if (MyNews.ulStarting <= MyNews.ulEnding)
                                    {
                                       if (GroupInfo->ulDelaySleep > 2)
                                          GroupInfo->ulDelaySleep /= 2;
                                    }
                                    else
                                       if (GroupInfo->ulDelaySleep < (3600 * 24))
                                          GroupInfo->ulDelaySleep *= 2;


                                    rc = DosQueryFSInfo(0L,
                                       FSIL_ALLOC,             /* Level 1 allocation info */
                                       (PVOID)&fsAlloc,    /* Buffer                  */
                                       sizeof(fsAlloc));  /* Size of buffer          */

                                    dFreeSpace = (fsAlloc.cSectorUnit * fsAlloc.cbSector);
                                    dFreeSpace *= (fsAlloc.cUnitAvail - 1);
                                    dFreeSpace /= 1024000.0;
                                    RFMret = 0;

                                    GroupInfo->ulMsgNum = MyNews.ulStarting;
                                    while ((GroupInfo->ulMsgNum <= MyNews.ulEnding) && (dFreeSpace > 100) && (!TimeToEnd) && (RFMret == 0))
   /*                                 for (GroupInfo->ulMsgNum = MyNews.ulStarting; (GroupInfo->ulMsgNum <= MyNews.ulEnding) && (dFreeSpace > 100) && (!TimeToEnd) && (RFMret == 0); GroupInfo->ulMsgNum++) */
                                    {
                                       sprintf(GroupBuf,"head %d\r\n",GroupInfo->ulMsgNum);
                                       if (GroupInfo->usRow < GroupInfo->SrcLines)
                                       {
                                          printf("[%d;%dHMsg #%d[K", GroupInfo->usRow, DISPLAY_HEADER_NUM_COL, GroupInfo->ulMsgNum);
                                          fflush(NULL);
                                       }

                                       rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                       if (rc == strlen(GroupBuf))
                                       {

                                          bRecError = FALSE;
                                          RFMret = ReceiveFullMessage(s, GroupBuf, MessageHeader, &ulMsgHeaderPos,
                                             MESSAGE_HEADER_SIZE, FALSE, &GroupInfo->usRow, 10000);
                                          if (RFMret == 0)
                                          {
                                             if (bWriteSubject == TRUE)
                                             {
                                                PSZ pszSubjectBegin;
                                                PSZ pszSubjectEnd;

                                                pszSubjectBegin = SearchFor("Subject:",MessageHeader, ulMsgHeaderPos, pszSubjectBegin);
                                                if (pszSubjectBegin != NULL)
                                                {
                                                   pszSubjectEnd = pszSubjectBegin;
                                                   while (*pszSubjectEnd != 0x0D)
                                                      pszSubjectEnd++;
                                                   pszSubjectEnd += 2;

                                                }

                                             }

                                             strncpy(szTemp, MessageHeader, 3);
                                             szTemp[3] = 0x00;

                                             /* 220 n <a> article retrieved - head and body follow
                                                        (n = article number, <a> = message-id)
                                                221 n <a> article retrieved - head follows
                                                222 n <a> article retrieved - body follows
                                                223 n <a> article retrieved - request text separately
                                                412 no newsgroup has been selected
                                                420 no current article has been selected
                                                423 no such article number in this group
                                                430 no such article found  */

                                             if (strncmp(MessageHeader,"221", 3) == 0)
                                             {
                                                /* valid Message Header */
                                                CHAR szMsgType[10];
                                                BOOL bFileSaved = FALSE;
                                                PSZ pszBadPoster;
                                                PSZ pszNewsGroup;
                                                BOOL bOtherNewsGroup = FALSE;

                                                ulMsgBodyPos = 0;
                                                pszBadPoster = NULL;

                                                if (sXPostSkip == 1)
                                                {

                                                   pszNewsGroup = SearchFor("Newsgroups:",MessageHeader, ulMsgHeaderPos, pszNewsGroup);
                                                   if (pszNewsGroup != NULL)
                                                   {
                                                      PSZ pszNewsGroupEnd;

                                                      pszNewsGroupEnd = SearchFor("\r\n",pszNewsGroup, ulMsgHeaderPos - (pszNewsGroup - MessageHeader), pszNewsGroupEnd);
                                                      if (pszNewsGroupEnd != NULL)
                                                      {
                                                         PSZ pszComma;

                                                         pszComma = SearchFor(",",pszNewsGroup, pszNewsGroupEnd - pszNewsGroup, pszComma);
                                                         if (pszComma != NULL)
                                                         {
                                                            PSZ pszStart;
                                                            PSZ pszEnd;
                                                            SHORT sGroupCount;

                                                            pszStart = pszNewsGroup + 12;
                                                            while (pszStart != NULL)
                                                            {
                                                               pszEnd = SearchFor(",",pszStart, (ULONG)pszNewsGroupEnd - (pszStart - pszNewsGroup), pszEnd);
                                                               if (pszEnd == NULL)
                                                                  pszEnd = pszNewsGroupEnd;

                                                               for (sGroupCount = 0; sGroupCount < MAX_GROUPS; sGroupCount++)
                                                               {
                                                                  if (szGroups[sGroupCount][1] != NULL)
                                                                  {
                                                                     if (strncmp(pszStart, szGroups[sGroupCount][1], pszEnd - pszStart) == 0)
                                                                     {
                                                                        if (strncmp(szGroups[sGroupCount][1],GroupInfo->szGroup, pszEnd - pszStart) == 0)
                                                                           bOtherNewsGroup = FALSE;
                                                                        else
                                                                           bOtherNewsGroup = TRUE;

                                                                        pszStart = NULL;
                                                                        sGroupCount = MAX_GROUPS;
                                                                     }
                                                                  }
                                                                  else
                                                                     sGroupCount = MAX_GROUPS - 1;
                                                               }
                                                               if (sGroupCount == MAX_GROUPS)
                                                               {
                                                                  pszStart = pszEnd;
                                                                  if (strcmp(pszStart, ",") != 0 )
                                                                     pszStart = NULL;
                                                               }
                                                            }
                                                         }
                                                      }
                                                   }
                                                }
                                                else
                                                   bOtherNewsGroup = FALSE;

                                                if ((pszBadPoster == NULL) && (bOtherNewsGroup == FALSE))
                                                {
                                                   sprintf(GroupBuf,"body %d\r\n",GroupInfo->ulMsgNum);
                                                   if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                   {
                                                      printf("[%d;%dHb/dl -[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                                      fflush(NULL);
                                                   }
                                                   rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                                   if (rc == strlen(GroupBuf))
                                                   {

                                                      if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                         RFMret = ReceiveFullMessage(s, GroupBuf, MessageBody, &ulMsgBodyPos, ulMaxData, TRUE, &GroupInfo->usRow, 10000);
                                                      else
                                                         RFMret = ReceiveFullMessage(s, GroupBuf, MessageBody, &ulMsgBodyPos, ulMaxData, FALSE, &GroupInfo->usRow, 10000);

                                                      if (RFMret == 0)
                                                      {
                                                         USHORT usNextMulti;
                                                         USHORT usNextGeneric;
                                                         USHORT usNextPre;

                                                         strncpy(szMsgType, MessageBody, 3);
                                                         szMsgType[3] = 0x00;

                                                         if (strncmp(MessageBody, "222", 3) == 0)
                                                         {

                                                            bFileSaved = FALSE;

                                                            for (usNextPre=0; usNextPre < 100; usNextPre++)
                                                            {
                                                               if (Preprocess[usNextPre].bLoaded == TRUE)
                                                               {
                                                                  bFileSaved = Preprocess[usNextPre].vPreprocess(MessageHeader,
                                                                     ulMsgHeaderPos, MessageBody, ulMsgBodyPos, GroupInfo->szNewsDir, GroupInfo->bLogs);
                                                                  if (bFileSaved)
                                                                     usNextPre = 100;
                                                               }
                                                               else
                                                                  usNextPre = 100;
                                                            }

                                                            if (!bFileSaved)
                                                            {
                                                               for (usNextMulti=0; usNextMulti < 100; usNextMulti++)
                                                               {
                                                                  if (Multi[usNextMulti].bLoaded == TRUE)
                                                                  {
                                                                     bFileSaved = Multi[usNextMulti].vMultipart(MessageHeader,
                                                                        ulMsgHeaderPos, MessageBody, ulMsgBodyPos, GroupInfo->szNewsDir, GroupInfo->bLogs);
                                                                     if (bFileSaved)
                                                                        usNextMulti = 100;
                                                                  }
                                                                  else
                                                                     usNextMulti = 100;
                                                               }
                                                            }

                                                            if (!bFileSaved)
                                                            {
                                                               for (usNextMulti=0; usNextMulti < 100; usNextMulti++)
                                                               {
                                                                  if (GenMulti[usNextMulti].bLoaded == TRUE)
                                                                  {
                                                                     bFileSaved = GenMulti[usNextMulti].vGenMulti(MessageHeader,
                                                                        ulMsgHeaderPos, MessageBody, ulMsgBodyPos, GroupInfo->szNewsDir, GroupInfo->bLogs);
                                                                     if (bFileSaved)
                                                                        usNextMulti = 100;
                                                                  }
                                                                  else
                                                                     usNextMulti = 100;
                                                               }
                                                            }
                                                            if (!bFileSaved)
                                                            {
                                                               for (usNextGeneric=0; usNextGeneric < 100; usNextGeneric++)
                                                               {
                                                                  if (Generic[usNextGeneric].bLoaded == TRUE)
                                                                  {
                                                                     bFileSaved = Generic[usNextGeneric].vGeneric(MessageHeader,
                                                                        ulMsgHeaderPos, MessageBody, ulMsgBodyPos, GroupInfo->szNewsDir, GroupInfo->bLogs);
                                                                     if (bFileSaved)
                                                                        usNextGeneric = 100;
                                                                  }
                                                                  else
                                                                     usNextGeneric = 100;
                                                               }
                                                            }

                                                            if (!bFileSaved)
                                                            {

                                                               bFileSaved = CheckForSmallMessage(MessageHeader,
                                                                  ulMsgHeaderPos, MessageBody, ulMsgBodyPos, GroupInfo->szNewsDir);
#ifndef RELEASE
                                                               if (bFileSaved == TRUE)
                                                               {
                                                                  CHAR szFileName[256];

                                                                  sprintf(szFileName,"%s\\SMALL.OUT", GroupInfo->szNewsDir);

                                                                  strcpy(GroupInfo->szErrorInfo,"small message, writting to disk");
                                                                  rc = DosOpen(szFileName,&hFile,&action,0L,FILE_NORMAL,
                                                                     OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                                  if (rc == 0)
                                                                  {

                                                                     DosWrite(hFile,MessageHeader,ulMsgHeaderPos,&BytesWritten);
                                                                     DosWrite(hFile,MessageBody,ulMsgBodyPos,&BytesWritten);
                                                                     DosWrite(hFile,"\r\n--------------------\r\n",24L, &BytesWritten);
                                                                     DosClose(hFile);
                                                                  }
                                                               }
#endif
                                                            }
#ifndef RELEASE
                                                            if (bFileSaved != TRUE)
                                                            {
                                                               CHAR szFileName[256];
                                                               CHAR szNumber[10];
                                                               CHAR szTemp[10];
                                                               USHORT x;

                                                               strcpy(szNumber, "00000");
                                                               itoa(usMissed++,szTemp,10);
                                                               x = strlen(szTemp);
                                                               strncpy(&szNumber[5 - x], szTemp, x);


                                                               sprintf(szFileName,"%s\\GYN%s.OUT", GroupInfo->szNewsDir, szNumber);

                                                               strcpy(GroupInfo->szErrorInfo,"Unsaved message, Aborting Newsgroup");
                                                               rc = DosOpen(szFileName,&hFile,&action,0L,FILE_NORMAL,
                                                                  OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                               if (rc == 0)
                                                               {

                                                                  DosWrite(hFile,MessageHeader,ulMsgHeaderPos,&BytesWritten);
                                                                  DosWrite(hFile,MessageBody,ulMsgBodyPos,&BytesWritten);
                                                                  DosClose(hFile);
                                                               }

                                                            }
#endif
                                                         }
                                                         else
                                                         {
                                                            /* invalid body message */
                                                            if (rc == 1)
                                                            {
                                                               WinLoadString( hab, hmLanguage, IDS_GYN_BODY_ERROR_1, sizeof(szTemp) - 1, szTemp );
                                                               sprintf(GroupInfo->szErrorInfo,szTemp, GroupInfo->ulMsgNum, ulMaxData);
                                                               WinLoadString( hab, hmLanguage, IDS_GYN_BODY_ERROR_2, sizeof(szTemp) - 1, szTemp );
                                                               if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                               {
                                                                  printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                                                  fflush(NULL);
                                                               }
                                                               ulMsgBodyPos = 0;
                                                            }
                                                         }
                                                      }
                                                      else
                                                      {
                                                         /* recieve error */
                                                         GroupInfo->ulMsgNum -= 2;
                                                         switch (RFMret)
                                                         {
                                                            case 1:
                                                               WinLoadString( hab, hmLanguage, IDS_GYN_BODY_ERROR_1, sizeof(szTemp) - 1, szTemp );
                                                               sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                               bRecError = TRUE;
                                                               break;
                                                            case 2:
                                                               WinLoadString( hab, hmLanguage, IDS_GYN_BODY_ERROR_2, sizeof(szTemp) - 1, szTemp );
                                                               sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                               bRecError = TRUE;
                                                               break;
                                                            case 3:
                                                               sprintf(GroupInfo->szErrorInfo,"Recv timeout",GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                               bRecError = TRUE;
                                                               break;
                                                            default:
                                                               if (RFMret != 0)
                                                               {
                                                                  WinLoadString( hab, hmLanguage, IDS_GYN_HEADER_ERROR_UNKNOWN, sizeof(szTemp) - 1, szTemp );
                                                                  sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, RFMret);
                                                                  MyNews.ulEnding = GroupInfo->ulMsgNum - 2;
                                                                  GroupInfo->ulMsgNum--;
                                                                  ulMsgHeaderPos = 0;
                                                                  DosSleep(2000);
                                                                  bRecError = TRUE;
                                                                  if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                                  {
                                                                     printf("[%d;%dHMsg Header %d bytes[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, ulMsgHeaderPos);
                                                                     fflush(NULL);
                                                                  }
                                                                  DosSleep(1000);
                                                               }
                                                               break;
                                                         }
                                                      }
                                                   }
                                                   else
                                                   {
                                                      /* Send Error */
                                                      WinLoadString( hab, hmLanguage, IDS_GYN_SEND_ERROR, sizeof(szTemp) - 1, szTemp );
                                                      if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                      {
                                                         printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                                         fflush(NULL);
                                                      }
                                                      DosSleep(5000);
                                                   }
                                                }
                                                else
                                                {

                                                   if (bOtherNewsGroup == FALSE)
                                                   {
                                                      WinLoadString( hab, hmLanguage, IDS_GYN_BAD_POSTER_MESSAGE, sizeof(szTemp) - 1, szTemp );
                                                      if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                      {
                                                         printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                                         fflush(NULL);
                                                      }
                                                   }
                                                   else
                                                   {
                                                      if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                      {
                                                         printf("[%d;%dHSKIP - Other Group[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                                         fflush(NULL);
                                                      }
                                                    }
                                                 }
                                             }
                                             else
                                             {
                                                /* invalid header */

                                                USHORT iLen;

                                                if (MessageHeader[0] == '4')
                                                {

                                                   iLen = ulMsgHeaderPos;
                                                   while ((MessageHeader[iLen - 1] == 0x0D) || (MessageHeader[iLen - 1] == 0x0A) && (iLen > 1))
                                                   {
                                                      iLen--;
                                                      MessageHeader[iLen] = 0x00;
                                                   }
                                                   if (iLen > 255)
                                                      MessageHeader[255] = 0x00;
                                                   strcpy(GroupInfo->szErrorInfo, &MessageHeader[4]);
                                                   if (strlen(GroupInfo->szErrorInfo) > 29)
                                                      GroupInfo->szErrorInfo[29] = 0x00;
                                                   if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                   {
                                                      printf("[%d;%dH%s[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, GroupInfo->szErrorInfo);
                                                      fflush(NULL);
                                                   }
                                                }
                                                else
                                                {
                                                   rc = DosOpen("Header.Err",&hFile,&action,0L,FILE_NORMAL,
                                                      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                   if (rc == 0)
                                                   {

                                                      DosWrite(hFile,MessageHeader,ulMsgHeaderPos,&BytesWritten);
                                                      DosWrite(hFile,"\r\n--------------------\r\n",24L, &BytesWritten);
                                                      DosClose(hFile);
                                                   }
                                                   DosBeep(1200,100);
                                                   DosSleep(100);
                                                   DosBeep(1400,100);
                                                }
                                             }
                                          }
                                          else
                                          {
                                             /* Recieve error on header */
                                             GroupInfo->ulMsgNum -= 2;
                                             switch (RFMret)
                                             {
                                                case 1:
                                                   WinLoadString( hab, hmLanguage, IDS_GYN_HEADER_ERROR_1, sizeof(szTemp) - 1, szTemp );
                                                   sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                   bRecError = TRUE;
                                                   break;
                                                case 2:
                                                   WinLoadString( hab, hmLanguage, IDS_GYN_HEADER_ERROR_2, sizeof(szTemp) - 1, szTemp );
                                                   sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                   bRecError = TRUE;
                                                   break;
                                                case 3:
                                                   sprintf(GroupInfo->szErrorInfo,"Recv timeout",GroupInfo->ulMsgNum, MESSAGE_HEADER_SIZE);
                                                   bRecError = TRUE;
                                                   break;
                                                default:
                                                   if (RFMret != 0)
                                                   {
                                                      WinLoadString( hab, hmLanguage, IDS_GYN_HEADER_ERROR_UNKNOWN, sizeof(szTemp) - 1, szTemp );
                                                      sprintf(GroupInfo->szErrorInfo,szTemp,GroupInfo->ulMsgNum, RFMret);
                                                      MyNews.ulEnding = GroupInfo->ulMsgNum - 2;
                                                      GroupInfo->ulMsgNum--;
                                                      ulMsgHeaderPos = 0;
                                                      DosSleep(2000);
                                                      bRecError = TRUE;
                                                      if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                      {
                                                         printf("[%d;%dHMsg Header %d bytes[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, ulMsgHeaderPos);
                                                         fflush(NULL);
                                                      }
                                                      DosSleep(1000);
                                                   }
                                                   break;
                                             }
                                          }
                                       }
                                       else
                                       {
                                          /* send error */
                                          WinLoadString( hab, hmLanguage, IDS_GYN_SEND_ERROR, sizeof(szTemp) - 1, szTemp );
                                          if (GroupInfo->usRow < GroupInfo->SrcLines)
                                          {
                                             printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                             fflush(NULL);
                                          }
                                          DosSleep(5000);
                                       }

                                       rc = DosQueryFSInfo(0L,
                                                     FSIL_ALLOC,             /* Level 1 allocation info */
                                                     (PVOID)&fsAlloc,    /* Buffer                  */
                                                     sizeof(fsAlloc));  /* Size of buffer          */
                                       dFreeSpace = (fsAlloc.cSectorUnit * fsAlloc.cbSector);
                                       dFreeSpace *= (fsAlloc.cUnitAvail - 1);
                                       dFreeSpace /= 1024000.0;
                                       if (dFreeSpace <= 100)
                                          if (GroupInfo->usRow < GroupInfo->SrcLines)
                                          {
                                             WinLoadString( hab, hmLanguage, IDS_GYN_NO_FREE_SPACE, sizeof(szTemp) - 1, szTemp );
                                             printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, dFreeSpace);
                                             fflush(NULL);
                                             DosSleep(5000);
                                          }



                                       sprintf(GroupBuf,"next\r\n");
                                       rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                       if (rc == strlen(GroupBuf))
                                       {

                                          RFMret = GetData(s, GroupBuf, sizeof(GroupBuf), GroupInfo->szSendMsg, &GroupInfo->sSendSize,
                                             sizeof(GroupInfo->szSendMsg), 2000);

                                          if (strncmp(GroupInfo->szSendMsg,"223",3) == 0)
                                          {
                                             CHAR szTemp[10];
                                             PSZ pszTemp, pszTemp1;
                                             ULONG ulNext;

                                             pszTemp = szTemp;
                                             pszTemp1 = &GroupInfo->szSendMsg[4];

                                             while (*pszTemp1 != ' ')
                                                *(pszTemp++) = *(pszTemp1++);

                                             *pszTemp = 0x00;
                                             ulNext = atol(szTemp);

                                             if ((ulNext > GroupInfo->ulMsgNum + 5) && (ulMaxConnections > 0) &&
                                                (ulMaxConnections == ulGYNConnections))
                                             {

                                                if (GroupInfo->usRow < GroupInfo->SrcLines)
                                                {
                                                   printf("[%d;%dH%s[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,"Missing Articles - Closing");
                                                   fflush(NULL);
                                                }
                                                DosSleep(2500);
                                                GroupInfo->ulDelaySleep = 180;
                                                RFMret = 3;
                                             }
                                             else
                                                GroupInfo->ulMsgNum = ulNext;
                                          }
                                          else
                                             RFMret = 3;

                                       }
                                       else
                                          RFMret = 3;

                                       GroupInfo->UpNews.ulLast = GroupInfo->ulMsgNum;
                                    } /* end for loop */
                                 } /* if notfinished */
                              }
                              else
                              {
                                 /* group Send Error */
                                 /* DosBeep(1200,100);  */
                                 WinLoadString( hab, hmLanguage, IDS_GYN_SEND_ERROR, sizeof(szTemp) - 1, szTemp );
                                 if (GroupInfo->usRow < GroupInfo->SrcLines)
                                 {
                                    printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                                    fflush(NULL);
                                 }
                                 DosSleep(5000);
                              }

                           }
                           else
                           {
                              /* recieve error on welcome */
/*                              DosBeep(1200, 100);  */
                              DosSleep(100);
                           }

                        } /* end connected */
                        else
                        {
                           /* Error on Connect */

                           CHAR szTemp[128];

                           WinLoadString( hab, hmLanguage, IDS_GYN_ERROR_CONNECT, sizeof(szTemp) - 1, szTemp );
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                           {
                              printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,iret);
                              fflush(NULL);
                           }
                           strcpy(GroupInfo->szErrorInfo,"Could not connect to Server");
                           DosSleep(3000);
                           GroupInfo->ulDelaySleep = 600;
                        }

                        soclose(s);
                     }
                     else
                     {
                        psock_errno("Socket()");
                        if (GroupInfo->usRow < GroupInfo->SrcLines)
                        {
                           printf("[%d;%dHSocket Error", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                           fflush(NULL);
                        }
                        strcpy(GroupInfo->szErrorInfo,"Socket Error");
                        DosSleep(5000);
                     }

                     DosFreeMem(MessageBody);
                  }
                  else
                  {
                     /* printf("Memory allocation error %d. Terminating\r\n",rc);  */

                     if (GroupInfo->usRow < GroupInfo->SrcLines)
                     {
                        printf("[%d;%dHInsufficent Memory Body %d[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, rc);
                        fflush(NULL);
                     }
                     DosSleep(2000);
                     strcpy(GroupInfo->szErrorInfo,"Insufficent Memory, Message Body");
                     GroupInfo->ulDelaySleep += 2;
                  }

                  DosFreeMem(MessageHeader);

               }
               else
               {
                  if (GroupInfo->usRow < GroupInfo->SrcLines)
                  {
                     printf("[%d;%dHInsufficent Memory Header %d[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,8);
                     fflush(NULL);
                  }
                  DosSleep(2000);
                  strcpy(GroupInfo->szErrorInfo,"Insufficent Memory, Message Header");
                  GroupInfo->ulDelaySleep += 2;
               }

            }
            else
            {
               if (rc == 0)
               {
                  CHAR szTemp[128];

                  WinLoadString( hab, hmLanguage, IDS_GYN_AVAILABLE, sizeof(szTemp) - 1, szTemp );
                  if (GroupInfo->usRow < GroupInfo->SrcLines)
                  {
                     printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, ulTotMem,
                        (MESSAGE_HEADER_SIZE + ulMaxData) * 2);
                     fflush(NULL);
                  }
                  DosSleep(5000);
               }
               else
               {
                  CHAR szTemp[128];

                  WinLoadString( hab, hmLanguage, IDS_GYN_SYSTEM_READ, sizeof(szTemp) - 1, szTemp );
                  if (GroupInfo->usRow < GroupInfo->SrcLines)
                  {
                     printf(szTemp, GroupInfo->usRow, DISPLAY_ERROR_MSG_COL);
                     fflush(NULL);
                  }
                  DosSleep(5000);
               }
               GroupInfo->ulDelaySleep += 2;
            }


            if (ulMaxConnections > 0)
            {

               rc = DosRequestMutexSem(hmtx, (ULONG) SEM_INDEFINITE_WAIT);
               if (rc == NO_ERROR)
               {
                  ulGYNConnections--;

                  rc = DosReleaseMutexSem(hmtx);
               }
               else
                  ulGYNConnections--;
            }
         } /* end Semaphore request */
         else
         {
            if (rc == NO_ERROR)
            {
               CHAR szTemp[256];

               GroupInfo->ulDelaySleep = 8;
               WinLoadString( hab, hmLanguage, IDS_GYN_WAITING_CONNECTION, sizeof(szTemp) - 1, szTemp );
               if (GroupInfo->usRow < GroupInfo->SrcLines)
                  printf(szTemp, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
               DosSleep(1000);

            }
            else
            {
               if (GroupInfo->usRow < GroupInfo->SrcLines)
                  printf("[%d;%dHError Requestion Semaphore[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
               DosSleep(5000);
            }

            rc = 0;
         }

         if (GroupInfo->bNotFinished == TRUE)
         {

            ulSleepSec = 0;
            while ((ulSleepSec < GroupInfo->ulDelaySleep) && (!TimeToEnd))
            {
               GroupInfo->bSleeping = TRUE;
               if (GroupInfo->usRow < GroupInfo->SrcLines)
               {
                  if (GroupInfo->ulDelaySleep - (ulSleepSec) < 60)
                  {
                     SHORT sSeconds = GroupInfo->ulDelaySleep - (ulSleepSec++);
                     switch (sSeconds)
                     {
                        case 1:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                              printf(szSleepSeconds, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sSeconds);
                           break;
                        default:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                              printf(szSleepSecondsMany, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sSeconds);
                           break;
                     }
                  }
                  if ((GroupInfo->ulDelaySleep - (ulSleepSec) > 59) && (GroupInfo->ulDelaySleep - (ulSleepSec) < 3600))
                  {
                     SHORT sMinutes = (GroupInfo->ulDelaySleep - (ulSleepSec++)) / 60;
                     switch (sMinutes)
                     {
                        case 1:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                             printf(szSleepMinutes, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sMinutes);
                           break;
                        default:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                              printf(szSleepMinutesMany, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sMinutes);
                           break;
                     }
                  }
                  if ((GroupInfo->ulDelaySleep - (ulSleepSec) > 3599))
                  {
                     SHORT sHours = (GroupInfo->ulDelaySleep - (ulSleepSec++)) / 3600;
                     switch (sHours)
                     {
                        case 1:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                              printf(szSleepHours, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sHours);
                           break;
                        default:
                           if (GroupInfo->usRow < GroupInfo->SrcLines)
                              printf(szSleepHoursMany, GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, sHours);
                           break;
                     }
                  }

               }
               if (GroupInfo->usRow < GroupInfo->SrcLines)
                  fflush(NULL);
               DosSleep(1000);
            }
         }
/*         ElvisIsDead = FALSE;  */
         GroupInfo->bSleeping = FALSE;

      } /* while Elvis is Dead */

   }
   else
   {
      CHAR szTemp[128];

      WinLoadString( hab, hmLanguage, IDS_GYN_GETHOSTBYNAME_FAILED, sizeof(szTemp) - 1, szTemp );
      sprintf(GroupInfo->szErrorInfo, szTemp, GroupInfo->szServer);

   }

   GroupInfo->bNotFinished = FALSE;
/*   if (GroupInfo->usRow < MAX_DISPLAY_ROW)
      printf("[%d;45HFinished[K",GroupInfo->usRow);  */
   WinLoadString( hab, hmLanguage, IDS_GYN_FINISHED, sizeof(szNewDir) - 1, szNewDir );
   strcpy(GroupInfo->szErrorInfo, szNewDir);
   return;
}

VOID NewsGroupList(GROUPINFO_STRUCT *GroupInfo)
{

   unsigned short port;       /* port client will connect to              */
   struct hostent *hostnm;    /* server host name information             */
   struct sockaddr_in server; /* server address                           */
   int s;                     /* client socket                            */
   BOOL ElvisIsDead = TRUE;
   SHORT iret;
   PSZ pszUserName;
   APIRET rc;
   PSZ pszPassword;
/*   HFILE hfGroup;  */
   ULONG ulUpNewsRec;
   ULONG ulFileNew;
   ULONG action;
   ULONG BytesRead;
   ULONG BytesWritten;
   HFILE hFile;
/*   CHAR *szGroups[100];  */
   ULONG x;
   CHAR GroupBuf[MAX_RX_BUFFER];
   PSZ MessageBody;
   ULONG ulMsgBodyPos;
   PSZ MessageHeader;
   ULONG ulMsgHeaderPos;
   ULONG ulSleepSec;
   CHAR szNewDir[256];
   ULONG ulTotMem;
   CHAR szGroupTrunc[50];
   CHAR szGroupLast[256];


   strncpy(szGroupTrunc,GroupInfo->szGroup, 34);
   szGroupTrunc[34] = 0x00;
   if (strlen(szGroupTrunc) > 31)
      strcpy(&szGroupTrunc[31],"...");

   DosSleep(500 * GroupInfo->usRow);

   sock_init();

   port = 119;  /* News port */


   hostnm = gethostbyname(GroupInfo->szServer);
   if (hostnm != (struct hostent *) 0)
   {

      /*
         * Put a message into the buffer.
      */

      /*
         * Put the server information into the server structure.
         * The port must be put into network byte order.
      */
      server.sin_family      = AF_INET;
      server.sin_port        = htons(port);
/*    server.sin_addr.s_addr = inet_addr(szMyAddress); / * Server's Address   */
      server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);


         /*
            * Get a stream socket.
         */


/*       rc = DosQuerySysInfo(QSV_TOTPHYSMEM,
                              QSV_TOTPHYSMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));  */

/*         rc = DosQuerySysInfo(QSV_MAXPRMEM,
                              QSV_MAXPRMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));  */

         rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                              (PVOID)&ulTotMem,
                              sizeof(ulTotMem));

/*         if (rc == 0)  */
         if ((rc == 0) && (ulTotMem > (MESSAGE_HEADER_SIZE + ulMaxData) * 2))
         {

            rc = DosAllocMem((VOID *)&MessageHeader,MESSAGE_HEADER_SIZE ,PAG_READ | PAG_WRITE | PAG_COMMIT);
            if (rc == 0)
            {

               rc = DosAllocMem((VOID *)&MessageBody,ulMaxData ,PAG_READ | PAG_WRITE | PAG_COMMIT);
               if (rc == 0)
               {

                  if ((s = socket(PF_INET, SOCK_STREAM, 0)) >= 0)
                  {
                     if ((iret = connect(s, (struct sockaddr *)&server, sizeof(server))) == 0)
                     {
                        LIST_STRUCT MyNews;
                        SHORT sLines;
                        PSZ pszTemp;
                        PSZ pszTemp1;
                        PSZ pszLineData;
                        CHAR szTemp[128];
                        SHORT sGroupNum = 0;
                        CHAR szCurrentDir[512];
                        ULONG ulDirSize;
                        BOOL bWriteBody = FALSE;
                        BOOL bWriteFullHeader = FALSE;
                        BOOL bWriteSubject = FALSE;
                        ULONG ulCounter = 1;

            /*            bWriteBody = TRUE; */

                        iret = recv((int)s, GroupBuf, MAX_RX_BUFFER, 0);

                        /* Authentication */

                        if (strlen(GroupInfo->szUserID) != 0)
                        {

                           sprintf(GroupBuf,"authinfo user %s\r\n", GroupInfo->szUserID);
                           rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                           if (rc > 0)
                           {
                              iret = recv((int)s, GroupBuf, MAX_RX_BUFFER, 0);
                              if (iret > 0)
                              {
                                 PSZ pszPassReq;

                                 GroupBuf[iret] = 0x00;
                                 pszPassReq = SearchFor("381",GroupBuf, iret, pszPassReq);
                                 if (pszPassReq != NULL)
                                 {
                                    if (pszPassword != NULL)
                                    {

                                       sprintf(GroupBuf,"authinfo pass %s\r\n", GroupInfo->szPassword);
                                       rc = send(s, GroupBuf, strlen(GroupBuf), 0);
                                       iret = recv((int)s, GroupBuf, MAX_RX_BUFFER, 0);
                                    }
                                    else
                                    {
                                       sprintf(GroupInfo->szErrorInfo, "Password required. Use the /P=xxx on command line\r\n");
                                       GroupInfo->bNotFinished = FALSE;
                                       return;
                                    }
                                 }

                                 DosSleep(1000);
                                 pszPassReq = SearchFor("281",GroupBuf, iret, pszPassReq);
                                 if (pszPassReq == NULL)
                                 {
                                    sprintf(GroupInfo->szErrorInfo,"Invalid UserID/Password supplied\r\n");
                                    GroupInfo->bNotFinished = FALSE;
                                    return;
                                 }
                              }
                           }
                        }

                        ListNewsGroups(s);

                        soclose(s);
                     }
                     else
                     {

                        if (GroupInfo->usRow < GroupInfo->SrcLines)
                        {
                           printf("[%d;%dHError %d Connect to Server", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,iret);
                           fflush(NULL);
                        }
                        strcpy(GroupInfo->szErrorInfo,"Could not connect to Server");
                        DosSleep(5000);
                     }
                  }
                  else
                  {
                     psock_errno("Socket()");
                     if (GroupInfo->usRow < GroupInfo->SrcLines)
                     {
                        printf("[%d;%dHSocket Error", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                        fflush(NULL);
                     }
                     strcpy(GroupInfo->szErrorInfo,"Socket Error");
                     DosSleep(5000);
         /*          exit(3);  */
                  }

                  DosFreeMem(MessageBody);
               }
               else
               {
                  /* printf("Memory allocation error %d. Terminating\r\n",rc);  */

                  if (GroupInfo->usRow < GroupInfo->SrcLines)
                     printf("[%d;%dHInsufficent Memory Message Body-[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
                  strcpy(GroupInfo->szErrorInfo,"Insufficent Memory, Message Body");
                  GroupInfo->ulDelaySleep += 2;
               }

               DosFreeMem(MessageHeader);

            }
            else
            {
               if (GroupInfo->usRow < GroupInfo->SrcLines)
                  printf("[%d;%dHInsufficent Memory Message Header-[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL);
               strcpy(GroupInfo->szErrorInfo,"Insufficent Memory, Message Header");
               GroupInfo->ulDelaySleep += 2;
            }

         }
         else
         {
            if (rc == 0)
            {
               if (GroupInfo->usRow < GroupInfo->SrcLines)
               {
                  printf("[%d;%dH%ld avail, Need %ld[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, ulTotMem,
                     (MESSAGE_HEADER_SIZE + ulMaxData) * 2);
                  fflush(NULL);
               }
               DosSleep(5000);
            }
            else
            {
               if (GroupInfo->usRow < GroupInfo->SrcLines)
               {
                  printf("[%d;%dHError on system read memory[K", GroupInfo->usRow, DISPLAY_ERROR_MSG_COL);
                  fflush(NULL);
               }
               DosSleep(5000);
            }
            GroupInfo->ulDelaySleep += 2;
         }
/*         ElvisIsDead = FALSE;  */
         GroupInfo->bSleeping = FALSE;
   }
   else
       sprintf(GroupInfo->szErrorInfo, "Gethostbyname failed for %s\n", GroupInfo->szServer);

   GroupInfo->bNotFinished = FALSE;
/*   if (GroupInfo->usRow < MAX_DISPLAY_ROW)
      printf("[%d;45HFinished[K",GroupInfo->usRow);  */
   strcpy(GroupInfo->szErrorInfo, "Finished");
   return;
}



BOOL CheckForSmallMessage(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszGroup)
{
   PSZ pszLines;
   static CHAR szGroup[256];

   pszLines = SearchFor("Lines:",pszHeader, ulHeaderLen, pszLines);
   if (pszLines != NULL)
   {
      CHAR szNumLines[20];
      PSZ pszNumLines;
      PSZ pszStart;
      ULONG ulNumLines;
      ULONG ulCount;

      pszNumLines = szNumLines;
      pszLines += 6;
      pszStart = pszLines;
      while ((*pszLines != 0x0D) && (pszLines < pszStart + sizeof(szNumLines)))
      {
         *(pszNumLines++) = *(pszLines++);
      }

      if (*pszLines == 0x0D)
      {
         *pszNumLines = 0x00;

         ulNumLines = atol(szNumLines);

         if (ulNumLines < 100)
         {

            return(1); /* not interested in message body */
         }
         else
         {


            pszLines = SearchFor("Content-Type: text/plain",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Plain Text message\r\n");  */
               return(1);
            }
            pszLines = SearchFor("From: \"Kristy\"",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("From: sc@sc.com",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("From: \"RDT64\"",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("From: \"Tommy\"",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("Spam Generator",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from Spam Generator\r\n");  */
               return(1);
            }
            pszLines = SearchFor("Organization: The South African Internet Exchange",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("@fnklsdjefd.net",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("From: \"dberry\"",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("From: zamiel",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from Zamiel\r\n");  */
               return(1);
            }
            pszLines = SearchFor("MSJ Marketing",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from MSJ Marketing\r\n");  */
               return(1);
            }
            pszLines = SearchFor("Subject: Free P.0.R.N.0!",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from virtualwebmedia.com\r\n");  */
               return(1);
            }
            pszLines = SearchFor("P0RN",pszHeader, ulHeaderLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("X-Newsreader:\r\n",pszHeader, ulHeaderLen, pszLines);
            if (pszLines == NULL)
            {
                pszLines = SearchFor("X-Newsposter:\r\n",pszHeader, ulHeaderLen, pszLines);
                if (pszLines == NULL)
                  return(1);
            }
            pszLines = SearchFor("14sep2001",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("BANNED IN THE USA",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("Cass",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("S p y",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("smutty",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("kinky",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("Wildest",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("Test of",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("Serious Trouble",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }
            pszLines = SearchFor("NOT A SCAM",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from virtualwebmedia.com\r\n");  */
               return(1);
            }
            pszLines = SearchFor("This is not spam.",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
   /*               printf("Ignoring posts from virtualwebmedia.com\r\n");  */
               return(1);
            }
            pszLines = SearchFor("://",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
/*               printf("Ignoring posts from virtualwebmedia.com\r\n");  */
               return(1);
            }
            pszLines = SearchFor("www.",pszBody, ulBodyLen, pszLines);
            if (pszLines != NULL)
            {
               return(1);
            }

            pszLines = SearchFor("\r\n",pszBody, ulBodyLen, pszLines);
            ulCount = 0;
            while (pszLines != NULL)
            {
               ulCount++;
               pszLines += 2;
               pszLines = SearchFor("\r\n",pszLines, ulBodyLen - (pszLines - pszBody), pszLines);

            }
            if (ulCount < ulNumLines)
               return(1);
         }
      }
   }
   else
      return(1);
   return(0);
}


BOOL CheckForPlainMessage(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen)
{
   PSZ pszContentType;

   pszContentType = SearchFor("Content-Type: multipart;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType != NULL)
   {
      CHAR szBoundryInfo[256];
      PSZ pszBoundryInfoPtr;
      PSZ pszContentTypeEnd;
      PSZ pszBoundry;
      PSZ pszBoundryEnd;

      pszContentTypeEnd = pszContentType;

      while ((*pszContentTypeEnd != 0x0D) && (pszContentTypeEnd < pszHeader + ulHeaderLen))
         pszContentTypeEnd++;
      if (*pszContentTypeEnd == 0x0D)
      {

         pszBoundry = SearchFor("boundary=",pszContentType, pszContentTypeEnd - pszContentType, pszBoundry);
         if (pszBoundry != NULL)
         {
            pszBoundryInfoPtr = szBoundryInfo;
            pszBoundry += 10;

            while ((*pszBoundry != '\"') && (pszBoundry < pszContentTypeEnd))
               *(pszBoundryInfoPtr++) = *(pszBoundry++);

            if (*pszBoundry == '\"')
            {
               PSZ pszMsgBoundryBegin;
               PSZ pszMsgBoundryEnd;

               *pszBoundryInfoPtr = 0x00;

               pszMsgBoundryBegin = SearchFor(szBoundryInfo,pszBody, ulBodyLen, pszMsgBoundryBegin);
               if (pszMsgBoundryBegin != NULL)
               {
                  pszMsgBoundryEnd = SearchFor(szBoundryInfo,pszMsgBoundryBegin + 1,
                     ulBodyLen - ((pszMsgBoundryBegin +1) - pszBody), pszMsgBoundryEnd);
                  if (pszMsgBoundryEnd != NULL)
                  {
                     PSZ pszContentType;

                     pszContentType = SearchFor("Content-Type: text/plain",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszContentType);
                     if (pszContentType != NULL)
                     {
                        /* Text only message */

                        return(1);
                     }
                  }
               }
            }
         }
      }
   }
   return(0);
}

/*
BOOL CheckForWinVN(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup)
{
   PSZ pszLines;
   static CHAR szGroup[256];

   pszLines = SearchFor("X-Newsreader: WinVN",pszHeader, ulHeaderLen, pszLines);
   if (pszLines != NULL)
   {
      printf("WinVM messages filtered out\r\n");
      SaveLog("WinVM.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
      if (strcmp(pszNewsGroup, szGroup) != 0)
         strcpy(szGroup, pszNewsGroup);
      return(1); / * not interested in message body * /
   }
   return(0);
}  */

int SaveLog(PSZ pszLogFileName, PSZ pszHeader, ULONG ulHeaderLen, PSZ pszGroup, SHORT sWriteGroup)
{
   HFILE hfLog;
   ULONG action;
   APIRET rc;
   ULONG ulFileNew;
   ULONG BytesWritten;
   CHAR szPostNumber[50];
   PSZ pszPostNumBegin;
   PSZ pszPostNumEnd;
   PSZ pszSubjectBegin;
   PSZ pszSubjectEnd;
   CHAR szSubject[256];


   strcpy(szPostNumber,"No Number");
   pszPostNumBegin = pszHeader;

   while ((*pszPostNumBegin != ' ') && (pszPostNumBegin < pszHeader + ulHeaderLen))
      pszPostNumBegin++;

   if (*pszPostNumBegin == ' ')
   {
      pszPostNumBegin++;

      pszPostNumEnd = pszPostNumBegin;

      while ((*pszPostNumEnd != ' ') && (pszPostNumEnd < pszHeader + ulHeaderLen))
         pszPostNumEnd++;

      if (*pszPostNumEnd == ' ')
      {

         strncpy(szPostNumber, pszPostNumBegin, pszPostNumEnd - pszPostNumBegin);
         szPostNumber[pszPostNumEnd - pszPostNumBegin] = 0x00;

      }
   }

   strcpy(szSubject, "Unknown Subject");
   pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
   if (pszSubjectBegin != NULL)
   {
      pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
      if (pszSubjectEnd != NULL)
      {
         if ((pszSubjectEnd - pszSubjectBegin) < 256)
         {
            strncpy(szSubject, pszSubjectBegin, pszSubjectEnd - pszSubjectBegin);
            szSubject[pszSubjectEnd - pszSubjectBegin] = 0x00;
         }
         else
         {
            strncpy(szSubject, pszSubjectBegin, 255);
            szSubject[255] = 0x00;

         }
      }
   }


   rc = DosOpen(pszLogFileName,&hfLog,&action,0L,FILE_NORMAL,
         OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      switch (action)
      {
         case FILE_EXISTED:
            DosSetFilePtr(hfLog,0,FILE_END,&ulFileNew);

         case FILE_CREATED:
            if (sWriteGroup != 0)
            {
               DosWrite(hfLog,pszGroup,strlen(pszGroup),&BytesWritten);
               DosWrite(hfLog,"\r\n",2L,&BytesWritten);
            }

            DosWrite(hfLog,szPostNumber,strlen(szPostNumber),&BytesWritten);
            DosWrite(hfLog," ",1L,&BytesWritten);
            DosWrite(hfLog,szSubject,strlen(szSubject),&BytesWritten);
            DosWrite(hfLog,"\r\n",2L,&BytesWritten);
            break;
      }
      DosClose(hfLog);
   }
   return(0);
}


PID StartProgramSync(PSZ progname, SHORT relation, PSZ Args,PSZ queue,
                        SHORT SessType, ULONG ReportType, ULONG Output, HWND hWndEnable)
{
   USHORT  rc;
   PID  ProcID;
   STARTDATA sd;
   StartParams *StartP;

   memset(&sd, '\0', sizeof(STARTDATA));
   sd.Length   = sizeof(STARTDATA);
   sd.Related  = SSF_RELATED_CHILD;
   sd.FgBg     = SSF_FGBG_FORE;
   sd.TraceOpt = 0;
   sd.PgmName = progname;
   sd.TermQ   = queue;   /* use queue created */
   sd.InheritOpt = 1;
   sd.Environment = 0;
   sd.SessionType = SessType;
   sd.PgmInputs = Args;
   sd.PgmControl = SSF_CONTROL_VISIBLE;

   DosAllocMem((VOID *)&StartP,sizeof(StartParams),PAG_READ | PAG_WRITE | PAG_COMMIT);
   StartP->Report = ReportType;
   StartP->Output = 0;
   StartP->Enable = hWndEnable;

   rc = DosCreateQueue(&StartP->hqueue, 1, queue);
   if ( rc == 0)
   {
      rc = DosStartSession(&sd, &StartP->WaitSessID, &ProcID);
      if ( rc == 0 )
      {
         rc = DosCreateThread(&StartP->WaitThreadID,(PFNTHREAD)waitsess,
               (ULONG)StartP,STACK_COMMITTED, 10240L);
         while (StartP->Output == 0)
            DosSleep(100);
      }
      else
      {
         rc = DosCloseQueue(StartP->hqueue);

         DosBeep(1200,500);
      }
   }
   else
      DosBeep(2400,500);

   DosFreeMem((PVOID)StartP);
   return(rc);
}


VOID _System waitsess(StartParams *StartP)
{
   ULONG  rc;
   STATUSDATA StData;


   StData.Length = sizeof(StData);
   StData.SelectInd = SET_SESSION_UNCHANGED;
   StData.BondInd = SET_SESSION_UNCHANGED;

   do
   {
      /* Keep setting the Session ID for UNCHANGED. When Session goes away
            it will return an error  */
      rc = DosSetSession(StartP->WaitSessID,&StData);
      DosSleep(100);
   } while (rc == 0) ;

   rc = DosCloseQueue(StartP->hqueue);

/*   if (StartP->Enable != NULL)
   {
      WinEnableWindow(StartP->Enable,TRUE);
      DosSleep(1000);
   } */
   StartP->Output = 1;

   _endthread();
}

INT FindDlls(void)
{
   FILEFINDBUF3 findbuf ;
   HDIR        hDir = 1 ;
   ULONG      usSearchCount = 1 ;
   CHAR        OldPath[256];
   CHAR        Name[25];
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   ULONG new;
   HMODULE MMod;
   CHAR FilePath[25];
   SHORT ScaleNum;
   SHORT x;
   CHAR szFileName[512];
   CHAR szLoadError[512];
   CHAR bUnload;
   SHORT sNextMulti = 0;
   SHORT sNextGenMulti = 0;
   SHORT sNextGeneric = 0;
   SHORT sNextPreprocess = 0;
   HAB hab = (HAB)NULL;

   pathLen = sizeof(OldPath);
   rc = DosQueryCurrentDir(0,OldPath,&pathLen);
   ScaleNum = 0;
   pathLen = sizeof(OldPath);
   rc = DosFindFirst("*.DLL", &hDir, 0L, &findbuf, (ULONG)sizeof(findbuf),
                              &usSearchCount, FIL_STANDARD) ;
   while(rc == 0)
   {
      sprintf(szFileName,"\\%s\\%s",OldPath, findbuf.achName);
      rc = DosLoadModule(szLoadError,sizeof(szLoadError),szFileName,&MMod);
      if (rc == 0)
      {
         bUnload = TRUE;
         strcpy(szFileID, "");
         WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"Multipart") == 0)
            {
               printf("Loading Multipart message support for program\r\n");
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }

               rc = DosQueryProcAddr(MMod,0,"CheckForMultipart",(int(** _System)())&Multi[sNextMulti].vMultipart);
               Multi[sNextMulti].hmMultipart = MMod;
               Multi[sNextMulti].bLoaded = TRUE;
               bUnload = FALSE;
               sNextMulti++;
            }
            if (strcmp(szDLLType,"GenMulti") == 0)
            {
               printf("Loading support for\r\n");
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }
               rc = DosQueryProcAddr(MMod,0,"CheckForMultipart",(int(** _System)())&GenMulti[sNextGenMulti].vGenMulti);
               GenMulti[sNextGenMulti].hmGenMulti = MMod;
               GenMulti[sNextGenMulti].bLoaded = TRUE;
               bUnload = FALSE;
               sNextGenMulti++;
            }
            if (strcmp(szDLLType,"Generic") == 0)
            {
               printf("Loading Decode support for\r\n");
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }
               rc = DosQueryProcAddr(MMod,0,"CheckForGeneric",(int(** _System)())&Generic[sNextGeneric].vGeneric);
               Generic[sNextGeneric].hmGeneric = MMod;
               Generic[sNextGeneric].bLoaded = TRUE;
               bUnload = FALSE;
               sNextGeneric++;
            }
            if (strcmp(szDLLType,"Preprocess") == 0)
            {
               printf("Loading Decode support for\r\n");
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }
               rc = DosQueryProcAddr(MMod,0,"CheckForPreProcess",(int(** _System)())&Preprocess[sNextPreprocess].vPreprocess);
               Preprocess[sNextPreprocess].hmPreprocess = MMod;
               Preprocess[sNextPreprocess].bLoaded = TRUE;
               bUnload = FALSE;
               sNextPreprocess++;
            }
/*            if (strcmp(szDLLType,"Decoder") == 0)
            {
               printf("Loading Decode support for\r\n");
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }
               rc = DosQueryProcAddr(MMod,0,"CheckForGeneric",(int(** _System)())&Generic[sNextGeneric].vGeneric);
               Generic[sNextGeneric].hmGeneric = MMod;
               Generic[sNextGeneric].bLoaded = TRUE;
               bUnload = FALSE;
               sNextGeneric++;
            }  */
         }
         if (bUnload)
            DosFreeModule(MMod);
      }
      rc = DosFindNext(hDir, &findbuf, sizeof(findbuf), &usSearchCount) ;
   }
   DosFindClose(hDir);
   return(0);
}

INT FindDllsMulti(void)
{
   FILEFINDBUF3 findbuf ;
   HDIR        hDir = 1 ;
   ULONG      usSearchCount = 1 ;
   CHAR        OldPath[256];
   CHAR        Name[25];
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   ULONG new;
   HMODULE MMod;
   CHAR FilePath[25];
   SHORT ScaleNum;
   SHORT x;
   CHAR szFileName[512];
   CHAR szLoadError[512];
   CHAR bUnload;
   SHORT sNextMulti = 0;
   HAB hab = (HAB)NULL;

   pathLen = sizeof(OldPath);
   rc = DosQueryCurrentDir(0,OldPath,&pathLen);
   ScaleNum = 0;
   pathLen = sizeof(OldPath);
   printf("Loading Multipart message support for program\r\n");
   rc = DosFindFirst("*.DLL", &hDir, 0L, &findbuf, (ULONG)sizeof(findbuf),
                              &usSearchCount, FIL_STANDARD) ;
   while(rc == 0)
   {
      sprintf(szFileName,"\\%s\\%s",OldPath, findbuf.achName);
      rc = DosLoadModule(szLoadError,sizeof(szLoadError),szFileName,&MMod);
      if (rc == 0)
      {
         bUnload = TRUE;
         strcpy(szFileID, "");
         WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"Multipart") == 0)
            {
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }

/*               DosSleep(500);  */
               rc = DosQueryProcAddr(MMod,0,"CheckForMultipart",(int(** _System)())&Multi[sNextMulti].vMultipart);
               Multi[sNextMulti].hmMultipart = MMod;
               Multi[sNextMulti].bLoaded = TRUE;
               bUnload = FALSE;
               sNextMulti++;
            }
         }
         if (bUnload)
            DosFreeModule(MMod);
      }
      rc = DosFindNext(hDir, &findbuf, sizeof(findbuf), &usSearchCount) ;
   }
   DosFindClose(hDir);
   return(0);
}

INT FindDllsGenMulti(void)
{
   FILEFINDBUF3 findbuf ;
   HDIR        hDir = 1 ;
   ULONG      usSearchCount = 1 ;
   CHAR        OldPath[256];
   CHAR        Name[25];
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   ULONG new;
   HMODULE MMod;
   CHAR FilePath[25];
   SHORT ScaleNum;
   SHORT x;
   CHAR szFileName[512];
   CHAR szLoadError[512];
   CHAR bUnload;
   SHORT sNextMulti = 0;
   HAB hab = (HAB)NULL;

   pathLen = sizeof(OldPath);
   rc = DosQueryCurrentDir(0,OldPath,&pathLen);
   ScaleNum = 0;
   pathLen = sizeof(OldPath);
   printf("Loading support for\r\n");
   rc = DosFindFirst("*.DLL", &hDir, 0L, &findbuf, (ULONG)sizeof(findbuf),
                              &usSearchCount, FIL_STANDARD) ;
   while(rc == 0)
   {
      sprintf(szFileName,"\\%s\\%s",OldPath, findbuf.achName);
      rc = DosLoadModule(szLoadError,sizeof(szLoadError),szFileName,&MMod);
      if (rc == 0)
      {
         bUnload = TRUE;
         strcpy(szFileID, "");
         WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"GenMulti") == 0)
            {
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }

/*               DosSleep(500);  */
               rc = DosQueryProcAddr(MMod,0,"CheckForMultipart",(int(** _System)())&GenMulti[sNextMulti].vGenMulti);
               GenMulti[sNextMulti].hmGenMulti = MMod;
               GenMulti[sNextMulti].bLoaded = TRUE;
               bUnload = FALSE;
               sNextMulti++;
            }
         }
         if (bUnload)
            DosFreeModule(MMod);
      }
      rc = DosFindNext(hDir, &findbuf, sizeof(findbuf), &usSearchCount) ;
   }
   DosFindClose(hDir);
   return(0);
}

INT FindDllsGeneric(void)
{
   FILEFINDBUF3 findbuf ;
   HDIR        hDir = 1 ;
   ULONG      usSearchCount = 1 ;
   CHAR        OldPath[256];
   CHAR        Name[25];
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   ULONG new;
   HMODULE MMod;
   CHAR FilePath[25];
   SHORT ScaleNum;
   SHORT x;
   CHAR szFileName[512];
   CHAR szLoadError[512];
   CHAR bUnload;
   SHORT sNextGeneric = 0;
   HAB hab = (HAB)NULL;

   pathLen = sizeof(OldPath);
   rc = DosQueryCurrentDir(0,OldPath,&pathLen);
   ScaleNum = 0;
   pathLen = sizeof(OldPath);
   printf("Loading Decode support for\r\n");
   rc = DosFindFirst("*.DLL", &hDir, 0L, &findbuf, (ULONG)sizeof(findbuf),
                              &usSearchCount, FIL_STANDARD) ;
   while(rc == 0)
   {
      sprintf(szFileName,"\\%s\\%s",OldPath, findbuf.achName);
/*      rc = DosLoadModule(szLoadError,sizeof(szLoadError),findbuf.achName,&MMod); */
      rc = DosLoadModule(szLoadError,sizeof(szLoadError),szFileName,&MMod);
      if (rc == 0)
      {
         bUnload = TRUE;
         strcpy(szFileID, "");
         WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"Generic") == 0)
            {
               for (x = 201; x< 211; x++)
               {
                  WinLoadString(hab, MMod, x, 255, szSupportFor);
                  if (strlen(szSupportFor) > 0)
                  {
                     printf("   %s\r\n", szSupportFor);
                  }
                  else
                     x = 211;
               }

/*               DosSleep(500);  */
               rc = DosQueryProcAddr(MMod,0,"CheckForGeneric",(int(** _System)())&Generic[sNextGeneric].vGeneric);
               Generic[sNextGeneric].hmGeneric = MMod;
               Generic[sNextGeneric].bLoaded = TRUE;
               bUnload = FALSE;
               sNextGeneric++;
            }
         }
         if (bUnload)
            DosFreeModule(MMod);
      }
      rc = DosFindNext(hDir, &findbuf, sizeof(findbuf), &usSearchCount) ;
   }
   DosFindClose(hDir);
   Generic[sNextGeneric].bLoaded = FALSE;
   return(0);
}

int DecodeUUFile(PSZ pszFileName)
{
   HFILE hfOutFile;
   ULONG action;
   APIRET rc;
   int UURet = 0;

   rc = DosOpen(pszFileName,&hfOutFile,&action,0L,FILE_NORMAL,
      OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
      OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      CHAR szLoadError[256];
      HMODULE MMod;
      HAB hab;
      CHAR szFileID[256];
      CHAR szDLLType[256];
      CHAR szUUFileName[256];
      ULONG ulFileSize;
      ULONG NewFileBeginPos;
      ULONG NewFilePos;
      PSZ pszTestData;
      ULONG BytesRead;

      DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFilePos);
      rc = DosAllocMem((VOID *)&pszTestData,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
      if (rc == 0)
      {

         DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
         DosRead(hfOutFile,pszTestData,NewFilePos, &BytesRead);

         if (bUUCodeDecoderNotLoaded == TRUE)
         {
            rc = DosLoadModule(szLoadError,sizeof(szLoadError),"UUdecode.dll",&MMod);
            if (rc == 0)
            {
               strcpy(szFileID, "");
               WinLoadString(hab, MMod, IDS_GYN, 256, szFileID);
               if (strcmp(szFileID,"GetYourNews") == 0)
               {
                  WinLoadString(hab, MMod, IDS_GYN_TYPE, 256, szDLLType);
                  if (strcmp(szDLLType,"Decoder") == 0)
                  {
                     rc = DosQueryProcAddr(MMod,0,"UUDecode",(int(** _System)())&UUDecode);
                     if (rc == 0)
                        bUUCodeDecoderNotLoaded = FALSE;
                  }
               }
            }
         }
         if (bUUCodeDecoderNotLoaded == FALSE)
         {
            UURet = UUDecode(pszTestData, NewFilePos, szUUFileName, &ulFileSize);
            if (UURet == 0)
            {
               DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
               DosWrite(hfOutFile,pszTestData,ulFileSize, &BytesRead);
               DosSetFileSize(hfOutFile, ulFileSize);
            }
         }
         DosFreeMem(pszTestData);
      }

      DosClose(hfOutFile);
/*      if (UURet != 0)
         DosDelete(pszFileName);  */
   }
   return(UURet);
}


#ifdef NEVER
                              case '4':
                              {
                                 USHORT usErrorSeconds;
                                 USHORT myrand;

                                 if (strlen(GroupBuf) > 24)
                                    GroupBuf[24] = 0x00;
                                 if (GroupInfo->usRow < GroupInfo->SrcLines)
                                 {
                                    printf("[%d;%dH%s[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL,GroupBuf);
                                    fflush(NULL);
                                 }
                                 DosSleep(5000);
                                 myrand = rand();
   /*                              usErrorSeconds = (USHORT)(18.0 * ((double) myrand / (double)RAND_MAX));  */
                                 GroupInfo->ulDelaySleep = (USHORT)(20.0 * ((double) myrand / (double)RAND_MAX));
   /*                              while (usErrorSeconds > 0)
                                 {
                                    switch (usErrorSeconds)
                                    {
                                       case 1:
                                          printf("[%d;%dHSleeping for %d second[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, usErrorSeconds);
                                          break;
                                       default:
                                          printf("[%d;%dHSleeping for %d seconds[K", GroupInfo->usRow, DISPLAY_BODY_SIZE_COL, usErrorSeconds);
                                          break;
                                    }
                                    fflush(NULL);
                                    DosSleep(1000);
                                    usErrorSeconds--;
                                 } */
                                 break;
                              }
#endif

