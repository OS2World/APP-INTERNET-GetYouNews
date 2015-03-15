#define EXTERN

#include "NewsPost.h"

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

   pszUserAgent = SearchFor("User-Agent: Newspost/2.0",pszHeader, ulHeaderLen, pszUserAgent);
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
      PSZ pszContent;


      pszContent = SearchFor("Content-Type: text/plain;",pszHeader, ulHeaderLen, pszContent);
      if (pszContent != NULL)
      {
/*         printf("Newspost text message\r\n");  */
         return(1);

      }
      else
      {

         pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszUserAgent);
         if (pszSubjectBegin != NULL)
         {
            pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
            if (pszSubjectEnd != NULL)
            {
               CHAR szTemp[100];
               PSZ pszTemp;
               PSZ pszLineTemp;
               PSZ pszLineTemp1;

               pszLineTemp = SearchFor("yEnc",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszLineTemp);
               if (pszLineTemp != NULL)
               {
                  return(0);
               }

               pszLineTemp = SearchFor("(",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszLineTemp);
               if (pszLineTemp != NULL)
               {

                  pszLineTemp1 = SearchFor("(",pszLineTemp + 1, pszSubjectEnd - (pszLineTemp +1) , pszLineTemp1);
                  while (pszLineTemp1 != NULL)
                  {
                     pszLineTemp = pszLineTemp1;
                     pszLineTemp1 = SearchFor("(",pszLineTemp + 1, pszSubjectEnd - (pszLineTemp +1) , pszLineTemp1);
                  }
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
                        while ((*pszLineTemp != ')') && (pszLineTemp < pszSubjectEnd))
                           *(pszTemp++) = *(pszLineTemp++);

                        if (*pszLineTemp == ')')
                        {
                           *pszTemp = 0x00;
                           ulTotal = atol(szTemp);

                           pszFileNameBegin = SearchFor(" - ",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszFileNameBegin);
                           if (pszFileNameBegin == NULL)
                              pszFileNameBegin = pszSubjectBegin + 8;
                           if (pszFileNameBegin != NULL)
                           {
                              pszLineTemp = SearchFor(" - ",pszFileNameBegin + 1, pszSubjectEnd - (pszFileNameBegin + 1), pszLineTemp);
                              while (pszLineTemp != NULL)
                              {
                                 pszFileNameBegin = pszLineTemp;
                                 pszLineTemp = SearchFor(" - ",pszFileNameBegin + 1, pszSubjectEnd - (pszFileNameBegin + 1), pszLineTemp);
                              }
                              pszFileNameEnd = SearchFor("(",pszFileNameBegin, pszSubjectEnd - pszFileNameBegin, pszFileNameEnd);
                              if ((pszFileNameEnd != NULL) && (pszFileNameEnd > pszFileNameBegin))
                              {
                                 ULONG ulDirSize;
                                 CHAR szNewFileName[256];
                                 APIRET rc;

                                 pszFileNameBegin += 3;
                                 pszFileNameEnd--;

                                 strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                                 szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;

                                 pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszDataBegin);
                                 if (pszDataBegin != NULL)
                                 {

                                    pszDataEnd = SearchFor("\r\n.\r\n",pszBody, ulBodyLen, pszDataEnd);
                                    if (pszDataEnd != NULL)
                                    {

/*                                       ulDirSize = sizeof(szCurrentDir);
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

/*                                             printf("Saving Newspost filename %s part %d of %d\r\n",szFileName, ulPart, ulTotal);  */
                                             sprintf(szNewFileName,"%s\\%s.NP%d",pszNewsGroup, szFileName, ulPart);
                                          }
                                          else
                                          {
/*                                             printf("Saving Newspost filename %s ",szFileName);  */
                                             sprintf(szNewFileName,"%s\\%s",pszNewsGroup,szFileName);

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

                                             sprintf(szFindFile,"%s\\%s.NP*",pszNewsGroup, szFileName);

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
                                                CHAR szMergeFileName[256];

                                                sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
/*                                                printf("Merging Newspost file %s", szFileName);  */
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

                                                      sprintf(szNewFileName,"%s\\%s.NP%d",pszNewsGroup,szFileName, x);
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
                                             CHAR szMergeFileName[256];

                                             sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
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


/*                                          rc = DosSetCurrentDir("\\");
                                          rc = DosSetCurrentDir(szCurrentDir);  */
                                          if (bLogs)
                                             SaveLog("NewsPost.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                                          if (strcmp(pszNewsGroup, szGroup) != 0)
                                             strcpy(szGroup, pszNewsGroup);
                                       }
                                    }
                                 }
                              }
                           }
                           else
                              return(0);
                        }
                     }
                  }
               }
            }
         }

         return(1);
      }
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
            DosClose(hfLog);
            break;
      }
   }
   return(0);
}

int DecodeUUFile(PSZ pszFileName)
{
   HFILE hfOutFile;
   ULONG action;
   APIRET rc;

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
      int UURet = 0;

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
      if (UURet != 0)
         DosDelete(pszFileName);
   }
   return(0);
}
