#define EXTERN

#include "NewsBin.h"

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

   pszUserAgent = SearchFor("X-Newsreader: Newsbin Pro",pszHeader, ulHeaderLen, pszUserAgent);
   if (pszUserAgent != NULL)
   {
      PSZ pszSubjectBegin;
      PSZ pszSubjectEnd;
      ULONG ulPart = 0;
      ULONG ulTotal = 0;
      PSZ pszFileNameBegin;
      PSZ pszFileNameEnd;
      CHAR szFileName[256];
      PSZ pszDataBegin;
      PSZ pszDataEnd;


      strcpy(szFileName,"");
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
               pszFileMarker = pszSubjectEnd - 1;
               if (*pszFileMarker == ')')
               {
                  pszFileMarker--;

                  while (isdigit(*pszFileMarker))
                     pszFileMarker--;
                  if (*pszFileMarker == '/')
                  {
                     CHAR szTotal[50];
                     PSZ pszTemp;

                     if (pszSubjectEnd - pszFileMarker < 49)
                     {
                        strncpy(szTotal, pszFileMarker + 1, pszSubjectEnd - (pszFileMarker + 2));
                        szTotal[pszSubjectEnd - (pszFileMarker + 2)] = 0x00;
                        ulTotal = atol(szTotal);

                        pszTemp = pszFileMarker;
                        pszFileMarker--;

                        while (isdigit(*pszFileMarker))
                           pszFileMarker--;
                        if (*pszFileMarker == '(')
                        {
                           CHAR szPart[50];

                           if (pszTemp - pszFileMarker < 49)
                           {
                              PSZ pszXPostBegin;
                              PSZ pszXPostEnd;
                              CHAR szXPost[256];
                              BOOL bUpperCase;
                              BOOL bImDone = FALSE;
                              BOOL bTheEndIsNear = FALSE;


                              strncpy(szPart, pszFileMarker + 1, pszTemp - (pszFileMarker + 1));
                              szPart[pszTemp - (pszFileMarker + 1)] = 0x00;

                              ulPart = atol(szPart);

                              pszFileMarker1 = pszFileMarker - 1;
                              pszFileMarker -= 2;

                              bUpperCase = FALSE;
                              while ((pszFileMarker > pszSubjectBegin) && (bImDone == FALSE))
                              {
                                 if (*pszFileMarker == '_')
                                    bTheEndIsNear = TRUE;
                                 if ((*pszFileMarker == '-') && (*(pszFileMarker + 1) != ' '))
                                    bTheEndIsNear = TRUE;
                                 if ((*pszFileMarker == '-') && (*(pszFileMarker + 1) == ' '))
                                 {
                                    pszFileMarker++;
                                    while (*pszFileMarker == ' ')
                                       pszFileMarker++;
                                    bImDone = TRUE;
                                 }
                                 if ((*pszFileMarker == ' ') && (bTheEndIsNear == TRUE))
                                 {
                                    bImDone = TRUE;
                                    pszFileMarker++;
                                 }

                                 pszFileMarker--;
                              }

                              if (*pszFileMarker == ' ')
                              {
                                 pszFileMarker++;

                                 strncpy(szFileName, pszFileMarker, pszFileMarker1 - pszFileMarker);
                                 szFileName[pszFileMarker1 - pszFileMarker] = 0x00;
                              }
                              else
                              {
                                 pszFileMarker = pszSubjectBegin + 9;

                                 strncpy(szFileName, pszFileMarker, pszFileMarker1 - pszFileMarker);
                                 szFileName[pszFileMarker1 - pszFileMarker] = 0x00;
                              }
                           }
                        }
                     }
                  }
               }
               if (strlen(szFileName) > 0)
               {

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
                              sprintf(szNewFileName,"%s\\%s.NBin%d",pszNewsGroup, szFileName, ulPart);

                           }
                           else
                           {
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




                           if (ulTotal > 1)
                           {
                              HDIR          hdirFindHandle = HDIR_SYSTEM;
                              FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
                              ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
                              ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
                              APIRET        rc             = NO_ERROR; /* Return code                  */
                              ULONG          ulNumFiles = 0;
                              CHAR szFindFile[256];
                              ULONG       ulSavedSize = 0;
                              CHAR szMergeFileName[256];

                              sprintf(szFindFile,"%s\\%s.NBin*",pszNewsGroup,szFileName);

                              rc = DosFindFirst( szFindFile,
                                                 &hdirFindHandle,
                                                 FILE_NORMAL,
                                                 &FindBuffer,
                                                 ulResultBufLen,
                                                 &ulFindCount,
                                                 FIL_STANDARD);

                              if (rc == NO_ERROR)
                              {
                                 ulNumFiles = 1;

                                 ulSavedSize = FindBuffer.cbFile;

                                 while (rc == NO_ERROR)
                                 {
                                    ulFindCount = 1;                      /* Reset find count.            */

                                    rc = DosFindNext(hdirFindHandle,      /* Directory handle             */
                                                     &FindBuffer,         /* Result buffer                */
                                                     ulResultBufLen,      /* Result buffer length         */
                                                     &ulFindCount);       /* Number of entries to find    */

                                    if (rc == NO_ERROR)
                                    {
                                       ulNumFiles++;
                                       ulSavedSize += FindBuffer.cbFile;
                                    }
                                 }
                                 DosFindClose(hdirFindHandle);
                              }

                              if (ulNumFiles == ulTotal)
                              {
                                 HFILE hfOutFile;

                                 fflush(NULL);
                                 sprintf(szMergeFileName,"%s\\%s",pszNewsGroup,szFileName);
                                 rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                    OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                 if (rc == 0)
                                 {
                                    USHORT x;

                                    for (x = 1; x <= ulNumFiles; x++)
                                    {
                                       CHAR szNewFileName[256];
                                       HFILE hfOldFile;

                                       sprintf(szNewFileName,"%s\\%s.NBin%d",pszNewsGroup,szFileName, x);
                                       rc = DosOpen(szNewFileName,&hfOldFile,&action,0L,FILE_NORMAL,
                                          OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                          OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                       if (rc == 0)
                                       {
                                          ULONG NewFilePos;
                                          ULONG NewFileBeginPos;
                                          PSZ pszMyFile;
                                          ULONG BytesRead;

                                          DosSetFilePtr(hfOldFile, 0L, FILE_END, &NewFilePos);
                                          rc = DosAllocMem((VOID *)&pszMyFile,NewFilePos ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                          if (rc == 0)
                                          {

                                             DosSetFilePtr(hfOldFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                             DosRead(hfOldFile,pszMyFile,NewFilePos, &BytesRead);
                                             DosClose(hfOldFile);

                                             DosWrite(hfOutFile,pszMyFile,BytesRead,&BytesWritten);
                                             DosFreeMem(pszMyFile);
                                             DosDelete(szNewFileName);
                                          }
                                       }
                                    }
                                    DosClose(hfOutFile);
                                    rc = DecodeUUFile(szMergeFileName);
                                    if (rc != 0)
                                    {
                                       if (bLogs)
                                       {
                                          CHAR szRenameBad[512];

                                          sprintf(szRenameBad,"%s.Bad_%d",szMergeFileName,rc);
                                          DosMove(szMergeFileName,szRenameBad);
                                       }
                                       else
                                          DosDelete(szMergeFileName);
                                    }
                                 }
                              }

                           }
                           else
                           {
                              USHORT rc;

                              rc = DecodeUUFile(szNewFileName);
                              if (rc != 0)
                              {
                                 if (bLogs)
                                 {
                                    CHAR szRenameBad[512];

                                    sprintf(szRenameBad,"%s.Bad_%d",szNewFileName,rc);
                                    DosMove(szNewFileName,szRenameBad);
                                 }
                                 else
                                    DosDelete(szNewFileName);
                              }
                           }



/*                           rc = DosSetCurrentDir("\\");
                           rc = DosSetCurrentDir(szCurrentDir);  */
                           if (strcmp(pszNewsGroup, szGroup) != 0)
                              strcpy(szGroup, pszNewsGroup);

                        }
                     }
                  }
               }
            }
            if (bLogs)
               SaveLog("NewsBin.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup), szFileName);
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

int SaveLog(PSZ pszLogFileName, PSZ pszHeader, ULONG ulHeaderLen, PSZ pszGroup, SHORT sWriteGroup, PSZ pszFileName)
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
            DosWrite(hfLog," -- ",4L,&BytesWritten);
            DosWrite(hfLog,pszFileName,strlen(pszFileName),&BytesWritten);
            DosWrite(hfLog,"\r\n",2L,&BytesWritten);
            DosClose(hfLog);
            break;
      }
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
