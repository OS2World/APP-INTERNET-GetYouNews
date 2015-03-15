#define EXTERN

#include "MSOE55.h"

#include "uudecodevars.h"
#include "gynplugin.h"

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bBase64DecoderNotLoaded = TRUE;
HMODULE hmContent;
BOOL (*CheckForContentType)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs);


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
   PSZ pszOLE;
   BOOL bTextMsgFound = FALSE;
   BOOL bMsgFound = FALSE;
   static CHAR szGroup[256];

   pszOLE = SearchFor("X-MimeOLE: Produced By Microsoft MimeOLE V5.50",pszHeader, ulHeaderLen, pszOLE);
   if (pszOLE != NULL)
   {
      PSZ pszContentType;
      PSZ pszSubjectBegin;
      PSZ pszSubjectEnd;
      CHAR szTemp[10];
      PSZ pszTemp;
      USHORT usPart;
      USHORT ulTotal;
      CHAR szFileName[256];
      PSZ pszFileName;


      pszContentType = SearchFor("Content-Type: multipart/mixed;",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType != NULL)
      {
         CHAR szBoundryInfo[256];
         PSZ pszBoundryInfo;
         PSZ pszBoundryBegin;


         pszBoundryBegin = SearchFor("boundary=",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszBoundryBegin);
         if (pszBoundryBegin)
         {
            pszBoundryInfo = szBoundryInfo;

            pszBoundryBegin += 10;

            while ((*pszBoundryBegin != '\"') && (*pszBoundryBegin != 0x0D) &&
               (pszBoundryInfo < szBoundryInfo + sizeof(szBoundryInfo)) &&
               (pszBoundryBegin < pszHeader + ulHeaderLen))
               *(pszBoundryInfo++) = *(pszBoundryBegin++);

            if (*pszBoundryBegin == '\"')
            {
               PSZ pszMsgContent;
               PSZ pszMsgBoundryBegin;
               PSZ pszMsgBoundryEnd;

               *pszBoundryInfo = 0x00;

               pszMsgBoundryBegin = SearchFor(szBoundryInfo,pszBody, ulBodyLen, pszMsgBoundryBegin);
               while (pszMsgBoundryBegin != NULL)
               {
                  pszMsgBoundryEnd = SearchFor(szBoundryInfo,pszMsgBoundryBegin + 1,
                     ulBodyLen - (pszMsgBoundryBegin - pszBody), pszMsgBoundryEnd);

                  if (pszMsgBoundryEnd != NULL)
                  {
                     PSZ pszThisMsgContentType;
                     /* more than one message remaining */
                     pszThisMsgContentType = SearchFor("Content-Type: text/plain",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType != NULL)
                     {
                        bTextMsgFound = TRUE;
                     }
                     else
                     {
                        pszThisMsgContentType = SearchFor("Content-Type: application/octet-stream;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                        if (pszThisMsgContentType != NULL)
                        {
                           CHAR szEncoding[50];
                           PSZ pszEncoding;
                           PSZ pszContentEncoding;
                           PSZ pszFileBegin;
                           PSZ pszFileEnd;
                           PSZ pszDataBegin;
                           ULONG ulDirSize;

                           pszContentEncoding = SearchFor("Content-Transfer-Encoding:",pszMsgBoundryBegin,
                              pszMsgBoundryEnd - pszMsgBoundryBegin, pszContentEncoding);
                           if (pszContentEncoding != NULL)
                           {
                              pszContentEncoding += 27;

                              pszEncoding = szEncoding;
                              while ((*pszContentEncoding != 0x0D) && (pszEncoding < szEncoding + (sizeof(szEncoding) - 1)))
                                 *(pszEncoding++) = *(pszContentEncoding++);

                              *pszEncoding = 0x00;



                           }
                           else
                              strcpy(szEncoding, "");

                           pszFileName = szFileName;
                           pszFileBegin = SearchFor("filename=",pszMsgBoundryBegin,
                              pszMsgBoundryEnd - pszMsgBoundryBegin, pszFileBegin);
                           if (pszFileBegin != NULL)
                           {
                              pszFileEnd = pszFileBegin;

                              while ((*pszFileEnd != 0x0D) && (pszFileEnd < pszBody + ulBodyLen))
                                 pszFileEnd++;

                              if (*pszFileEnd == 0x0D)
                              {
                                 APIRET rc;
                                 PSZ pszFileNameStart;
                                 pszFileEnd--;

                                 pszDataBegin = pszFileEnd + 2;
                                 if (*pszFileEnd == '\"')
                                    pszFileEnd--;

                                 pszFileNameStart = pszFileEnd;
                                 pszFileEnd++;

                                 while ((*pszFileNameStart != '\"') && (*pszFileNameStart != '\\') &&
                                    (*pszFileNameStart != ':') && (*pszFileNameStart != '=') &&
                                    (pszFileNameStart > pszFileBegin + 8))
                                    pszFileNameStart--;

                                 pszFileNameStart++;

                                 strncpy(szFileName, pszFileNameStart, pszFileEnd - pszFileNameStart);
                                 szFileName[pszFileEnd - pszFileNameStart] = 0x00;


/*                                 ulDirSize = sizeof(szCurrentDir);
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
                                    CHAR szMergeFileName[256];

                                    sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
                                    bMsgFound = TRUE;
/*                                    printf("Saving Microsoft OE 5.5 filename %s, Encoding %s\r\n",szFileName, szEncoding);  */
                                    rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    if (rc == 0)
                                    {
                                       DosWrite(hfOutFile,pszDataBegin,
                                          pszMsgBoundryEnd - pszDataBegin ,&BytesWritten);

                                       DosClose(hfOutFile);
                                    }
/*                                    rc = DosSetCurrentDir("\\");
                                    rc = DosSetCurrentDir(szCurrentDir);  */
                                    if (bLogs)
                                       SaveLog("OutlookExpress55.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                                    if (strcmp(pszNewsGroup, szGroup) != 0)
                                       strcpy(szGroup, pszNewsGroup);

                                 }
                              }
                           }
                        }
                     }

                  }

                  if (pszMsgBoundryEnd != NULL)
                     pszMsgBoundryBegin = pszMsgBoundryEnd + strlen(szBoundryInfo);
                  else
                     pszMsgBoundryBegin = NULL;
               }
            }
         }
      }
      else
      {
         pszContentType = SearchFor("Content-Type: message/partial;",pszHeader, ulHeaderLen, pszContentType);
         if (pszContentType != NULL)
         {
            PSZ pszTotalBegin;
            PSZ pszTotalEnd;
            PSZ pszPartBegin;
            CHAR szTotal[10];
            PSZ pszTotal;
            ULONG ulTotal;
            CHAR szPart[10];
            PSZ pszPart;
            ULONG ulPart;
            PSZ pszDataBegin;
            PSZ pszDataEnd;
            CHAR szFileName[256];
            PSZ pszFileNameBegin;
            PSZ pszFileNameEnd;
            PSZ pszSubjectBegin;
            PSZ pszSubjectEnd;

            pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
            if (pszSubjectBegin != NULL)
            {
               pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - ( pszSubjectBegin - pszHeader), pszSubjectEnd);
               if (pszSubjectEnd != NULL)
               {

                  pszFileNameBegin  = SearchFor("[",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszFileNameBegin);
                  if (pszFileNameBegin != NULL)
                  {
                     pszFileNameBegin -= 2;
                     while (((*pszFileNameBegin != ' ') || (*(pszFileNameBegin - 1) != ' ')) &&
                        (pszFileNameBegin > pszSubjectBegin))
                        pszFileNameBegin--;
                     if ((*pszFileNameBegin == ' ') && (*(pszFileNameBegin - 1) == ' '))
                     {

                        pszFileNameBegin++;
                        pszFileNameEnd  = SearchFor("[",pszFileNameBegin, pszFileNameBegin - pszSubjectBegin, pszFileNameEnd);
                        if (pszFileNameEnd != NULL)
                        {
                           pszFileNameEnd--;

                           if ((pszFileNameEnd - pszFileNameBegin) < sizeof(szFileName))
                           {

                              strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                              szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;
                           }
                           else
                              strcpy(szFileName,"Unknown.MSOE");
                        }
                     }
                     else
                        strcpy(szFileName,"Unknown.MSOE");
                  }
               }
            }

            pszTotalBegin = SearchFor("total=",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszTotalBegin);
            if (pszTotalBegin != NULL)
            {
               pszTotalBegin += 6;

               pszTotal = szTotal;
               while ((*pszTotalBegin != ';') && (*pszTotalBegin != '\r') && (pszTotal < szTotal + sizeof(szTotal)))
                  *(pszTotal++) = *(pszTotalBegin++);

               *pszTotal = 0x00;

               ulTotal = atol(szTotal);

               pszPartBegin = SearchFor("number=",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszPartBegin);
               if (pszPartBegin != NULL)
               {
                  pszPartBegin += 7;

                  pszPart = szPart;
                  while ((*pszPartBegin != '\r') && (pszPart < szPart + sizeof(szPart)))
                     *(pszPart++) = *(pszPartBegin++);

                  *pszPart = 0x00;

                  ulPart = atol(szPart);


                  pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen , pszDataBegin);
                  if (pszDataBegin != NULL)
                  {

                     pszDataEnd = SearchFor("\r\n.",pszBody, ulBodyLen , pszDataEnd);

                     if (pszDataEnd != NULL)
                     {

                        APIRET rc;

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

                           if (ulPart > 0)
                           {
                              sprintf(szNewFileName,"%s\\%s.MSOE55%d",pszNewsGroup,szFileName, ulPart);
/*                              printf("Saving Microsoft 5.5 filename %s, part %d of %d\r\n",szFileName, ulPart,ulTotal);  */
                           }
                           else
                           {

                              sprintf(szNewFileName,"%s\\%s",pszNewsGroup,szFileName);
/*                              printf("Saving Microsoft 5.5 filename %s\r\n",szFileName);  */
                           }

                           rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                              OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                           if (rc == 0)
                           {
                              PSZ pszDataBegin;
                              PSZ pszDataEnd;

                              pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszDataBegin);
                              if (pszDataBegin != NULL)
                              {

                                 pszDataBegin += 2;
                                 pszDataEnd = SearchFor("\r\n\r\n.",pszDataBegin, ulBodyLen - (pszDataBegin - pszBody), pszDataEnd);

                                 if (pszDataEnd != NULL)
                                    DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);
                              }

                              DosClose(hfOutFile);
                              if (ulPart > 0)
                              {
                                 HDIR          hdirFindHandle = HDIR_SYSTEM;
                                 FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
                                 ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
                                 ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
                                 APIRET        rc             = NO_ERROR; /* Return code                  */
                                 ULONG          ulNumFiles = 0;
                                 CHAR szFindFile[256];
                                 ULONG       ulSavedSize = 0;

                                 sprintf(szFindFile,"%s\\%s.MSOE55*",pszNewsGroup,szFileName);

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
/*                                    printf("Merging MSOE55 file %s\r\n", szFileName);  */
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

                                          sprintf(szNewFileName,"%s\\%s.MSOE55%d",pszNewsGroup, szFileName, x);
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
                                             rc = DosAllocMem((VOID *)&pszMyFile,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                             if (rc == 0)
                                             {

                                                DosSetFilePtr(hfOldFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                                DosRead(hfOldFile,pszMyFile,NewFilePos, &BytesRead);
                                                DosClose(hfOldFile);

                                                DosWrite(hfOutFile,pszMyFile,BytesRead,&BytesWritten);
                                                DosWrite(hfOutFile,"\r\n",2,&BytesWritten);
                                                DosFreeMem(pszMyFile);
                                                DosDelete(szNewFileName);
                                             }


                                          }
                                       }
                                       DosClose(hfOutFile);

                                       rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                          OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                          OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                       if (rc == 0)
                                       {
                                          ULONG ulFileNew;
                                          PSZ pszTheWholeThing;

                                          DosSetFilePtr(hfOutFile,0,FILE_END,&ulFileNew);
                                          rc = DosAllocMem((VOID *)&pszTheWholeThing,ulFileNew + 100,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                          if (rc == 0)
                                          {
                                             PSZ pszHeaderBegin;
                                             PSZ pszHeaderEnd;
                                             PSZ pszBodyBegin;
                                             PSZ pszBodyEnd;
                                             ULONG BytesRead;
                                             ULONG ulFileBegin;

                                             DosSetFilePtr(hfOutFile,0,FILE_BEGIN,&ulFileBegin);
                                             DosRead(hfOutFile,pszTheWholeThing,ulFileNew,&BytesRead);

                                             DosClose(hfOutFile);
                                             pszHeaderBegin = pszTheWholeThing;
                                             pszHeaderEnd = SearchFor("This is a multi-part message in MIME format.\r\n",pszTheWholeThing, ulFileNew , pszHeaderEnd);
                                             if (pszHeaderEnd != NULL)
                                             {
                                                BOOL bFileSaved = FALSE;

                                                pszHeaderEnd += 44;

                                                pszBodyBegin = pszHeaderEnd + 1;
                                                pszBodyEnd = pszBodyBegin + (ulFileNew - (pszBodyBegin - pszTheWholeThing));

                                                if (LoadContent() == 0)
                                                {

                                                   bFileSaved = CheckForContentType(pszHeaderBegin, pszHeaderEnd - pszHeaderBegin,
                                                      pszBodyBegin, pszBodyEnd - pszBodyBegin, ".", FALSE);

                                                   UnLoadContent();

                                                }

                                             }

                                             DosFreeMem(pszTheWholeThing);

                                          }
                                          else
                                             DosClose(hfOutFile);

                                       }
                                    }
                                 }

                              }

                           }
/*                           rc = DosSetCurrentDir("\\");
                           rc = DosSetCurrentDir(szCurrentDir);  */
                           if (bLogs)
                              SaveLog("MSOE55.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                           if (strcmp(pszNewsGroup, szGroup) != 0)
                              strcpy(szGroup, pszNewsGroup);


                           return(1);
                        }
                     }
                  }
               }
            }
         }
         else
         {

            pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
            if (pszSubjectBegin != NULL)
            {
               pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
               if (pszSubjectEnd != NULL)
               {
                  PSZ pszPart;
                  PSZ pszFileNameBegin;
                  PSZ pszFileNameEnd;

                  pszPart = SearchFor("Re:",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszPart);
                  if (pszPart != NULL)
                  {
                     return(1);
                  }

                  pszFileNameBegin = SearchFor("[",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszFileNameBegin);
                  if (pszFileNameBegin != NULL)
                  {
                     BOOL bQuotes;
                     PSZ pszPart;
                     PSZ pszDataBegin;
                     PSZ pszDataEnd;

                     pszFileNameBegin--;
                     pszFileNameEnd = pszFileNameBegin;
                     pszFileNameBegin--;

                     while ((*pszFileNameBegin != ' ') && (*pszFileNameBegin != '\\') && (*pszFileNameBegin != ':')
                        && (pszFileNameBegin > pszSubjectBegin))
                        pszFileNameBegin--;

                     pszFileNameBegin++;
                     if ((pszFileNameEnd - pszFileNameBegin) < sizeof(szFileName))
                     {

                        strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                        szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;
                     }
                     else
                        strcpy(szFileName,"Unknown.MSOE");


                     pszPart = SearchFor("[",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszPart);
                     if (pszPart != NULL)
                     {
                        pszPart++;
                        pszTemp = szTemp;

                        while ((*pszPart != '/') && (pszTemp < szTemp + sizeof(szTemp)))
                           *(pszTemp++) = *(pszPart++);

                        if (*pszPart == '/')
                        {
                           *pszTemp = 0x00;
                           usPart = atoi(szTemp);

                           pszPart++;
                           pszTemp = szTemp;

                           while ((*pszPart != ']') && (pszTemp < szTemp + sizeof(szTemp)))
                              *(pszTemp++) = *(pszPart++);

                           if (*pszPart == ']')
                           {

                              *pszTemp = 0x00;
                              ulTotal = atol(szTemp);


                              pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszDataBegin);
                              if (pszDataBegin != NULL)
                              {
                                 CHAR szNewFileName[256];
                                 APIRET rc;


                                 pszDataBegin += 2;
                                 pszDataEnd = SearchFor("\r\n\r\n",pszDataBegin,
                                    ulBodyLen - (pszDataBegin - pszBody), pszDataEnd);



/*                                 ulDirSize = sizeof(szCurrentDir);
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

                                    bMsgFound = TRUE;
/*                                    printf("Saving Microsoft OE 5.5 filename %s, part %d of %d\r\n",szFileName, usPart, ulTotal);  */
                                    sprintf(szNewFileName,"%s\\%s.M$55-%d", pszNewsGroup,szFileName, usPart);
                                    rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    if (rc == 0)
                                    {
                                       DosWrite(hfOutFile,pszDataBegin,
                                          pszDataEnd - pszDataBegin ,&BytesWritten);

                                       DosClose(hfOutFile);


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

                                          sprintf(szFindFile,"%s\\%s.M$55-*",pszNewsGroup, szFileName);

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
/*                                             printf("Merging MSOE55 file %s\r\n", szFileName);  */
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

                                                   sprintf(szNewFileName,"%s\\%s.M$55-%d",pszNewsGroup,szFileName, x);
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
                                                      rc = DosAllocMem((VOID *)&pszMyFile,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                                      if (rc == 0)
                                                      {

                                                         DosSetFilePtr(hfOldFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                                         DosRead(hfOldFile,pszMyFile,NewFilePos, &BytesRead);
                                                         DosClose(hfOldFile);

                                                         DosWrite(hfOutFile,pszMyFile,BytesRead,&BytesWritten);
                                                         DosWrite(hfOutFile,"\r\n",2,&BytesWritten);
                                                         DosFreeMem(pszMyFile);
                                                         DosDelete(szNewFileName);
                                                      }


                                                   }
                                                }
                                                DosClose(hfOutFile);

                                                rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                                   OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                                   OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                if (rc == 0)
                                                {
                                                   ULONG ulFileNew;
                                                   PSZ pszTheWholeThing;

                                                   DosSetFilePtr(hfOutFile,0,FILE_END,&ulFileNew);
                                                   rc = DosAllocMem((VOID *)&pszTheWholeThing,ulFileNew + 100,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                                   if (rc == 0)
                                                   {
                                                      PSZ pszHeaderBegin;
                                                      PSZ pszHeaderEnd;
                                                      PSZ pszBodyBegin;
                                                      PSZ pszBodyEnd;
                                                      ULONG BytesRead;
                                                      ULONG ulFileBegin;

                                                      DosSetFilePtr(hfOutFile,0,FILE_BEGIN,&ulFileBegin);
                                                      DosRead(hfOutFile,pszTheWholeThing,ulFileNew,&BytesRead);

                                                      DosClose(hfOutFile);
                                                      pszHeaderBegin = pszTheWholeThing;
                                                      pszHeaderEnd = SearchFor("This is a multi-part message in MIME format.\r\n",pszTheWholeThing, ulFileNew , pszHeaderEnd);
                                                      if (pszHeaderEnd != NULL)
                                                      {
                                                         BOOL bFileSaved = FALSE;

                                                         pszHeaderEnd += 44;

                                                         pszBodyBegin = pszHeaderEnd + 1;
                                                         pszBodyEnd = pszBodyBegin + (ulFileNew - (pszBodyBegin - pszTheWholeThing));

                                                         if (LoadContent() == 0)
                                                         {

                                                            bFileSaved = CheckForContentType(pszHeaderBegin, pszHeaderEnd - pszHeaderBegin,
                                                               pszBodyBegin, pszBodyEnd - pszBodyBegin, ".", FALSE);

                                                            UnLoadContent();
                                                         }


                                                      }
                                                      else
                                                      {
                                                         UULoadFile(szMergeFileName,NULL,1);
                                                         UUDecodeFile(UUGetFileListItem(0), NULL);
/*                                                         if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                                                            printf("- Error decoding file\r\n");
                                                         else
                                                            printf("- decoded\r\n");  */
                                                      }

                                                      DosFreeMem(pszTheWholeThing);

                                                   }
                                                   else
                                                      DosClose(hfOutFile);

                                                }
                                             }
                                          }

                                       }

                                    }
/*                                    rc = DosSetCurrentDir("\\");
                                    rc = DosSetCurrentDir(szCurrentDir);  */
                                    if (bLogs)
                                       SaveLog("OutlookExpress55.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                                    if (strcmp(pszNewsGroup, szGroup) != 0)
                                       strcpy(szGroup, pszNewsGroup);

                                 }

                                 return(1);
                              }
                           }
                        }
                     }
                  }
                  else
                     return(1);
               }
            }
         }
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


   if (bBase64DecoderNotLoaded == TRUE)
   {

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
         bBase64DecoderNotLoaded = FALSE;
      }
   }
   return(rc);
}

int UnLoadContent(void)
{

/*   DosFreeModule(hmContent);  */
   return(0);
}


