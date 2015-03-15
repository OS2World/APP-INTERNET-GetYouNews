#define EXTERN

#include "WinVN.h"

#include "uudecodevars.h"
#include "gynplugin.h"

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bDecoderNotLoaded = TRUE;
HMODULE hmContent;


INT main(int argc, char *argv[])
{
   QMSG qmsg;
   PID  pid;
   TID  tid;
   static CHAR szTitle[24];

   argc = argc;
   argv = argv;

   return(0);
}

extern int _dllentry;

BOOL CheckForMultipart(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs)
{
   int x = _dllentry;

   PSZ pszUserAgent;
   static CHAR szGroup[256];

   pszUserAgent = SearchFor("X-Newsreader: WinVN 0.99.9",pszHeader, ulHeaderLen, pszUserAgent);
   if (pszUserAgent != NULL)
   {
      PSZ pszSubjectBegin;
      PSZ pszSubjectEnd;
      ULONG ulPart;
      ULONG ulTotal;
      PSZ pszFileNameBegin;
      PSZ pszFileNameEnd;
      CHAR szFileName[256];
      PSZ pszDataBegin;
      PSZ pszDataEnd;


      pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszUserAgent);
      if (pszSubjectBegin != NULL)
      {
         pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
         if (pszSubjectEnd != NULL)
         {
            CHAR szTemp[100];
            PSZ pszTemp;
            PSZ pszLineTemp;
            PSZ pszFileMarker;
            PSZ pszFileMarker1;
            ULONG ulDirSize;
            CHAR szNewFileName[256];
            APIRET rc;

            pszFileMarker = SearchFor("Re:",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszFileMarker);
            if (pszFileMarker == NULL)
            {


               pszFileMarker = SearchFor(" - ",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszFileMarker);
               if (pszFileMarker != NULL)
               {
                  pszFileMarker1 = SearchFor(" - ",pszFileMarker + 1, pszSubjectEnd - (pszFileMarker +1), pszFileMarker1);
                  while (pszFileMarker1 != NULL)
                  {
                     pszFileMarker = pszFileMarker1;
                     pszFileMarker1 = SearchFor(" - ",pszFileMarker + 1, pszSubjectEnd - (pszFileMarker +1), pszFileMarker1);

                  }

                  pszFileMarker += 3;
                  pszFileMarker1 = SearchFor("[",pszFileMarker + 1, pszSubjectEnd - (pszFileMarker +1), pszFileMarker1);
                  if (pszFileMarker1 == NULL)
                     pszFileMarker1 = SearchFor("(",pszFileMarker + 1, pszSubjectEnd - (pszFileMarker +1), pszFileMarker1);
                  if (pszFileMarker1 != NULL)
                     pszFileMarker1--;
                  else
                     pszFileMarker1 = pszSubjectEnd;


                  if (pszFileMarker1 != pszSubjectEnd)
                  {

                     pszLineTemp = SearchFor("[",pszFileMarker1, pszSubjectEnd - pszFileMarker1, pszLineTemp);
                     if (pszLineTemp == NULL)
                        pszLineTemp = SearchFor("(",pszFileMarker1, pszSubjectEnd - pszFileMarker1, pszLineTemp);
                     if (pszLineTemp != NULL)
                     {
                        pszLineTemp++;

                        pszTemp = szTemp;
                        while ((*pszLineTemp != '/') && (pszLineTemp < pszSubjectEnd))
                           *(pszTemp++) = *(pszLineTemp++);

                        if (*pszLineTemp == '/')
                        {
                           *pszTemp = 0x00;


                           ulPart = atol(szTemp);
                           pszLineTemp++;

                           pszTemp = szTemp;
                           while (((*pszLineTemp != ']') && (*pszLineTemp != ')')) && (pszLineTemp < pszSubjectEnd))
                              *(pszTemp++) = *(pszLineTemp++);

                           if ((*pszLineTemp == ']') || (*pszLineTemp == ')'))
                           {
                              *pszTemp = 0x00;
                              ulTotal = atol(szTemp);

                           }
                        }
                     }
                     else
                     {

                        ulPart = 0;
                        ulTotal = 0;
                     }
                  }


                  strncpy(szFileName, pszFileMarker, pszFileMarker1 - pszFileMarker);
                  szFileName[pszFileMarker1 - pszFileMarker] = 0x00;


                  pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszDataBegin);
                  if (pszDataBegin != NULL)
                  {

                     pszDataEnd = SearchFor("\r\n.",pszBody, ulBodyLen, pszDataEnd);
                     if (pszDataEnd != NULL)
                     {

/*                        ulDirSize = sizeof(szCurrentDir);
                        rc = DosQueryCurrentDir(0,szCurrentDir, &ulDirSize);
                        sprintf(szNewDir,"\\%s\\%s",szCurrentDir, pszNewsGroup);
                        rc = DosSetCurrentDir(szNewDir);
                        if (rc != 0)
                        {
                           switch (rc)
                           {
                              case 3:
                              {
                                 rc = DosCreateDir(szNewDir, NULL);
                                 if (rc == 0)
                                    rc = DosSetCurrentDir(szNewDir);
                                 break;
                              }
                           }
                        }  */


                        rc = 0;
                        if (rc == 0)
                        {
                           HFILE hfOutFile;
                           ULONG action;
                           ULONG BytesWritten;
                           CHAR szNewFileName[256];

                           if (ulTotal > 1)
                           {
/*                              printf("Saving WinVN filename %s part %d of %d\r\n",szFileName, ulPart, ulTotal);  */
                              sprintf(szNewFileName,"%s\\%s.WVN%d",pszNewsGroup, szFileName, ulPart);

                           }
                           else
                           {
/*                              printf("Saving WinVN filename %s\r\n",szFileName);  */
                              sprintf(szNewFileName,"%s\\%s",pszNewsGroup, szFileName);
                           }

                           rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                              OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                           if (rc == 0)
                           {
                              DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);

                              DosClose(hfOutFile);

                           }
/*                           rc = DosSetCurrentDir("\\");
                           rc = DosSetCurrentDir(szCurrentDir);  */
                           if (bLogs)
                              SaveLog("WinVN.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                           if (strcmp(pszNewsGroup, szGroup) != 0)
                              strcpy(szGroup, pszNewsGroup);

                        }
                     }
                  }
               }
            }
/*            else
               printf("WinVM Response - Ignoring\r\n");  */
         }
      }

      return(1);
   }

   return(0);
}


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

/*
int Constructor(void)
{
   HMODULE MMod;
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   CHAR FilePath[25];
   CHAR szLoadError[512];


   if (bDecoderNotLoaded == TRUE)
   {

      rc = DosLoadModule(szLoadError,sizeof(szLoadError),"UUdecode.dll",&MMod);
      if (rc == 0)
      {
         strcpy(szFileID, "");
         WinLoadString((HMODULE)NULL, MMod, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString((HMODULE)NULL, MMod, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"Decoder") == 0)
            {

               rc = DosQueryProcAddr(MMod,0,"UULoadFile",(int(** _System)())&UULoadFile);
               rc = DosQueryProcAddr(MMod,0,"UUGetFileListItem",(int(** _System)())&UUGetFileListItem);
               rc = DosQueryProcAddr(MMod,0,"UUDecodeFile",(int(** _System)())&UUDecodeFile);
               rc = DosQueryProcAddr(MMod,0,"UUInitialize",(int(** _System)())&UUInitialize);

               if (UUInitialize != NULL)
                  UUInitialize();
               else
                  UULoadFile = NULL;

               bDecoderNotLoaded = FALSE;
            }
         }
      }
   }
   return(0);
}  */

/*
int LoadContent(void)
{
   CHAR        szFileID[256];
   CHAR        szDLLType[256];
   CHAR        szSupportFor[256];
   ULONG       pathLen;
   APIRET      rc;
   ULONG numbytes;
   HFILE handle;
   ULONG action;
   CHAR FilePath[25];
   CHAR szLoadError[512];


   rc = DosLoadModule(szLoadError,sizeof(szLoadError),"Content.dll",&hmContent);
   if (rc == 0)
   {
      strcpy(szFileID, "");
      WinLoadString((HMODULE)NULL, hmContent, IDS_GYN, 256, szFileID);
      if (strcmp(szFileID,"GetYourNews") == 0)
      {
         WinLoadString((HMODULE)NULL, hmContent, IDS_GYN_TYPE, 256, szDLLType);
         if (strcmp(szDLLType,"Generic") == 0)
         {

            rc = DosQueryProcAddr(hmContent,0,"CheckForMultipart",(int(** _System)())&CheckForContentType);

         }
      }
   }
   return(rc);
}

int UnLoadContent(void)
{

   DosFreeModule(hmContent);
   return(0);
}  */

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
