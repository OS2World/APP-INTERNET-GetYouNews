#define EXTERN

#include "ForteAgent.h"

#include "uudecodevars.h"
#include "base64vars.h"
#include "gynplugin.h"

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bDecoderNotLoaded = TRUE;
BOOL bBase64DecoderNotLoaded = TRUE;
HMODULE hmBase64;
HMODULE hmContent;
CHAR szGroup[256];
CHAR szGroup192[256];


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
   int x;
   PSZ pszNEWSPoster;
   HFILE hfNNTP;
   ULONG action;
   APIRET rc;
   ULONG BytesWritten;
   ULONG ulFileNew;
   BOOL bMsgFound = FALSE;
   CHAR szVersion[256];

   x = _dllentry;
   pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.8/32.548",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.91/32.564",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.9/32.560",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.7/32.534",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.7/32.532",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.93/32.576",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.92/32.572",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.5/32.452",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 2.0/32.640",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 2.0/32.652",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster != NULL)
   {
      PSZ pszContentType;
      PSZ pszSubjectBegin;
      PSZ pszSubjectEnd;
      CHAR szFileName[256];
      PSZ pszTemp;

      pszNEWSPoster += 14;

      pszTemp = szVersion;
      while (*pszNEWSPoster != 0x0D)
         *(pszTemp++) = *(pszNEWSPoster++);

      *pszTemp = 0x00;
      strcpy(szVersion,"ForteAgent2.LOG");
      if (strcmp(szVersion,"Forte Agent 1.8/32.548" ) == 0)
         strcpy(szVersion,"ForteAgent18.log");
      if (strncmp(szVersion,"Forte Agent 1.93/32.576", 23 ) == 0)
         strcpy(szVersion,"ForteAgent193.LOG");
      if (strncmp(szVersion,"Forte Agent 1.92/32.572", 23 ) == 0)
         strcpy(szVersion,"ForteAgent192.LOG");
      if (strcmp(szVersion,"Forte Agent 1.91/32.564" ) == 0)
         strcpy(szVersion,"ForteAgent191.LOG");
      if (strcmp(szVersion,"Forte Agent 1.9/32.560" ) == 0)
         strcpy(szVersion,"ForteAgent19.LOG");
      if (strcmp(szVersion,"Forte Agent 1.7/32.534" ) == 0)
         strcpy(szVersion,"ForteAgent17.LOG");
      if (strcmp(szVersion,"Forte Agent 1.7/32.532") == 0)
         strcpy(szVersion,"ForteAgent17.LOG");
      if (strcmp(szVersion,"Forte Agent 1.5/32.452") == 0)
         strcpy(szVersion,"ForteAgent15.LOG");

      pszContentType = SearchFor("Content-Type: text/plain;",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType != NULL)
      {
         return(1);

      }
      pszContentType = SearchFor("Re:",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType != NULL)
      {
         return(1);
      }
/*      Constructor();  */
      pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
      if (pszSubjectBegin != NULL)
      {
         pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
         if (pszSubjectEnd != NULL)
         {

            if (pszContentType == NULL)
            {

               PSZ pszFileMarker;
               PSZ pszFileMarker1;
               CHAR szTemp[100];
               PSZ pszLineTemp;
               ULONG ulPart;
               ULONG ulTotal;
               ULONG ulDirSize;


               pszFileMarker = SearchFor(" - ",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszFileMarker);
               if (pszFileMarker != NULL)
               {
                  pszFileMarker1 = SearchFor(" - ",pszFileMarker + 1, (pszSubjectEnd - (pszFileMarker + 1)), pszFileMarker1);
                  while (pszFileMarker1 != NULL)
                  {
                     pszFileMarker = pszFileMarker1;
                     pszFileMarker1 = SearchFor(" - ",pszFileMarker + 1, (pszSubjectEnd - (pszFileMarker + 1)), pszFileMarker1);
                  }

                  pszFileMarker += 3;
                  pszFileMarker1 = SearchFor("(",pszFileMarker + 1, (pszSubjectEnd - (pszFileMarker + 1)), pszFileMarker1);
                  if (pszFileMarker1 != NULL)
                  {
                     PSZ pszTemp;
                     PSZ pszTemp1;

                     pszTemp = pszFileMarker1;
                     while (pszTemp != NULL)
                     {
                        pszTemp = SearchFor("(",pszFileMarker1 + 1, (pszSubjectEnd - (pszFileMarker1 + 1)), pszTemp);
                        if (pszTemp != NULL)
                           pszFileMarker1 = pszTemp;
                     }
                     pszFileMarker1--;
                     if (*pszFileMarker1 != ' ')
                        pszFileMarker1 = pszSubjectEnd;
                  }
                  else
                     pszFileMarker1 = pszSubjectEnd;

                  strncpy(szFileName, pszFileMarker, (pszFileMarker1 - pszFileMarker));
                  szFileName[pszFileMarker1 - pszFileMarker] = 0x00;

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
                        while ((*pszLineTemp != ')') && (pszLineTemp < pszSubjectEnd))
                           *(pszTemp++) = *(pszLineTemp++);

                        if (*pszLineTemp == ')')
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



/*                  ulDirSize = sizeof(szCurrentDir);
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
                        sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup, szFileName, ulPart);
/*                        printf("Saving %s\r\n filename %s, part %d of %d\r\n",szVersion, szFileName, ulPart,ulTotal);  */
                     }
                     else
                     {

                        sprintf(szNewFileName,"%s\\%s",pszNewsGroup, szFileName);
/*                        printf("Saving %s\r\n filename %s ",szVersion, szFileName);  */
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
                           pszDataEnd = SearchFor("\r\n.\r\n",pszBody, ulBodyLen, pszDataEnd);

                           if ((pszDataEnd != NULL) && (pszDataEnd > pszDataBegin))
                           {
                              bMsgFound = TRUE;

                              DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);
                           }
                        }

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

                           sprintf(szFindFile,"%s\\%s.FA*",pszNewsGroup, szFileName);

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

                              while (rc != ERROR_NO_MORE_FILES)
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
                           }

                           DosFindClose(hdirFindHandle);
                           if (ulNumFiles == ulTotal)
                           {
                              HFILE hfOutFile;
                              CHAR szMergeFileName[256];

/*                              printf("Merging file %s", szFileName);  */
                              sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
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

                                    sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup, szFileName, x);
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
/*                                 UULoadFile(szMergeFileName,NULL,1);
                                 UUDecodeFile(UUGetFileListItem(0), NULL);
                                 if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                                    printf("- Error decoding file\r\n");
                                 else
                                    printf("- decoded\r\n");  */

                              }
                           }

                        }
                        else
                        {
                           CHAR szMergeFileName[256];

                           sprintf(szMergeFileName,"$s\\%s",pszNewsGroup, szFileName);

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
/*                           UULoadFile(szMergeFileName,NULL,1);
                           UUDecodeFile(UUGetFileListItem(0), NULL);
                           if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                              printf("- Error decoding file\r\n");
                           else
                              printf("- decoded\r\n");  */

                        }

                     }
                     else
                     {

/*                        switch(rc)
                        {
                           case ERROR_FILE_NOT_FOUND:
                              printf("File Not Found.\r\n");
                              break;
                           case ERROR_PATH_NOT_FOUND:
                              printf("Path Not Found \r\n");
                              break;
                           case ERROR_TOO_MANY_OPEN_FILES:
                              printf("Too Many Open Files. \r\n");
                              break;
                           case ERROR_ACCESS_DENIED:
                              printf("Access Denied.\r\n");
                              break;
                           case ERROR_INVALID_ACCESS:
                              printf("Invalid Access.\r\n");
                              break;
                           case ERROR_NOT_DOS_DISK:
                              printf("Not a Dos Disk");
                              break;
                           case ERROR_SHARING_VIOLATION:
                              printf("Sharing Violation.\r\n");
                              break;
                           case ERROR_SHARING_BUFFER_EXCEEDED:
                              printf("Sharing Buffer Exceeded.\r\n");
                              break;
                           case ERROR_CANNOT_MAKE:
                              printf("Cannot Make File. \r\n");
                              break;
                           case ERROR_INVALID_PARAMETER:
                              printf("Invalid Parameter.\r\n");
                              break;
                           case ERROR_DEVICE_IN_USE:
                              printf("Device in Use.\r\n");
                              break;
                           case ERROR_DRIVE_LOCKED:
                              printf("Drive Locked.\r\n");
                              break;
                           case ERROR_OPEN_FAILED:
                              printf("Open Failed.\r\n");
                              break;
                           case ERROR_DISK_FULL:
                              printf("Disk Full.\r\n");
                              break;
                           case ERROR_FILENAME_EXCED_RANGE:
                              printf("File Name Exceded Range.\r\n");
                              break;
                           case ERROR_PIPE_BUSY:
                              printf("Pipe Busy.\r\n");
                              break;
                           default:
                              printf("Unknown Error\r\n");
                              break;
                        }  */

                        bMsgFound = FALSE;
                     }

/*                     rc = DosSetCurrentDir("\\");
                     rc = DosSetCurrentDir(szCurrentDir);  */
                     if (bLogs)
                        SaveLog(szVersion,pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                     if (strcmp(pszNewsGroup, szGroup) != 0)
                        strcpy(szGroup, pszNewsGroup);
                  }
               }
               else
               {
                  CHAR szDecodedFile[256];

/*                  printf("No valid filename found - cannot save\r\n");  */

                  pszContentType = SearchFor("Content-Type: message/partial;",pszHeader, ulHeaderLen, pszContentType);
                  if (pszContentType != NULL)
                  {
                     PSZ pszID;
                     PSZ pszContentTypeEnd;
                     USHORT usNumber;
                     USHORT usTotal;
                     CHAR szTemp[10];


                     pszContentTypeEnd = SearchFor("\r\n",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszContentTypeEnd);
                     if (pszContentTypeEnd != NULL)
                     {

                        pszID = SearchFor("id=\"<",pszContentType, pszContentTypeEnd - pszContentType, pszID);
                        if (pszID != NULL)
                        {
                           PSZ pszIDEnd;
                           PSZ pszFileName;
                           PSZ pszFileNameEnd;
                           PSZ pszEncoding;
                           PSZ pszEncodingEnd;
                           CHAR szEncoding[50];

                           pszIDEnd = SearchFor(">\"",pszID, pszContentTypeEnd - pszID, pszIDEnd);
                           if (pszIDEnd != NULL)
                           {
                              PSZ pszNumber;
                              PSZ pszTotal;

                              pszID += 5;

                              strncpy(szFileName, pszID, pszIDEnd - pszID);
                              szFileName[pszIDEnd - pszID] = 0x00;

                              pszNumber = SearchFor("number=",pszContentType, pszContentTypeEnd - pszContentType, pszNumber);
                              if (pszNumber != NULL)
                              {
                                 PSZ pszTemp;

                                 pszNumber += 7;
                                 pszTemp = szTemp;

                                 while (isdigit(*pszNumber))
                                    *(pszTemp++) = *(pszNumber++);
                                 pszTemp = 0x00;

                                 usNumber = atoi(szTemp);
                              }
                              pszTotal = SearchFor("total=",pszContentType, pszContentTypeEnd - pszContentType, pszTotal);
                              if (pszTotal != NULL)
                              {
                                 PSZ pszTemp;
                                 HFILE hfOutFile;
                                 CHAR szNewFileName[256];

                                 pszTotal += 6;
                                 strcpy(szTemp,"");
                                 pszTemp = szTemp;

                                 while (isdigit(*pszTotal))
                                    *(pszTemp++) = *(pszTotal++);
                                 *pszTemp = 0x00;

                                 usTotal = atoi(szTemp);

                                 if (usTotal > 1)
                                    sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup, szFileName, usNumber);
                                 else
                                    sprintf(szNewFileName,"%s\\%s",pszNewsGroup, szFileName);

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
                                       pszDataEnd = SearchFor("\r\n.\r\n",pszBody, ulBodyLen, pszDataEnd);

                                       if ((pszDataEnd != NULL) && (pszDataEnd > pszDataBegin))
                                       {
                                          bMsgFound = TRUE;
                                          pszDataEnd += 2;

                                          DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);
                                       }
                                    }

                                    DosClose(hfOutFile);

                                 }
                                 if (usTotal > 1)
                                 {
                                    HDIR          hdirFindHandle = HDIR_SYSTEM;
                                    FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
                                    ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
                                    ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
                                    APIRET        rc             = NO_ERROR; /* Return code                  */
                                    ULONG          ulNumFiles = 0;
                                    CHAR szFindFile[256];
                                    ULONG       ulSavedSize = 0;

                                    sprintf(szFindFile,"%s\\%s.FA*",pszNewsGroup,szFileName);

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

                                       while (rc != ERROR_NO_MORE_FILES)
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
                                    }

                                    DosFindClose(hdirFindHandle);
                                    if (ulNumFiles == usTotal)
                                    {
                                       HFILE hfOutFile;
                                       ULONG NewFilePos;
                                       ULONG NewFileBeginPos;
                                       PSZ pszMyFile;
                                       PSZ pszDecode;
                                       CHAR szMergeFileName[256];
                                       CHAR szEncoding[50];
                                       PSZ pszMsgBegin;
                                       PSZ pszMsgEnd;
                                       ULONG ulFileLen;
                                       ULONG BytesRead;

                                       sprintf(szMergeFileName,"%s\\%s.FA1",pszNewsGroup,szFileName);
                                       rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                          OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                          OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                       if (rc == 0)
                                       {
                                          DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFilePos);
                                          rc = DosAllocMem((VOID *)&pszMyFile,NewFilePos ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                          if (rc == 0)
                                          {

                                             DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                             DosRead(hfOutFile,pszMyFile,NewFilePos, &BytesRead);

                                             pszFileName = SearchFor("Content-Description:",pszMyFile, BytesRead, pszFileName);
                                             if (pszFileName != NULL)
                                             {
                                                pszFileNameEnd = SearchFor("\r\n",pszFileName, BytesRead - (pszFileName - pszMyFile), pszFileNameEnd);
                                                if (pszFileNameEnd != NULL)
                                                {
                                                   pszFileName += 21;
                                                   strncpy(szDecodedFile, pszFileName, pszFileNameEnd - pszFileName);
                                                   szDecodedFile[pszFileNameEnd - pszFileName] = 0x00;

                                                   pszEncoding = SearchFor("Content-Transfer-Encoding:",pszMyFile, BytesRead, pszEncoding);
                                                   if (pszEncoding != NULL)
                                                   {
                                                      pszEncodingEnd = SearchFor("\r\n",pszEncoding, BytesRead - (pszEncoding - pszMyFile), pszEncodingEnd);
                                                      if (pszEncodingEnd != NULL)
                                                      {
                                                         pszEncoding += 27;
                                                         strncpy(szEncoding, pszEncoding, pszEncodingEnd - pszEncoding);
                                                         szEncoding[pszEncodingEnd - pszEncoding] = 0x00;

/*                                                         if (stricmp(szEncoding,"base64") == 0)
                                                         {
                                                            if (LoadBase64() == 0)
                                                            {

                                                               pszMsgBegin = SearchFor("\r\n\r\n",pszMyFile, BytesRead, pszMsgBegin);
                                                               if (pszMsgBegin != NULL)
                                                               {
                                                                  pszMsgBegin += 4;
                                                                  pszDecode = pszMsgBegin;
                                                                  pszMsgEnd = SearchFor("\r\n.",pszMyFile, BytesRead, pszMsgEnd);
                                                                  if (pszMsgEnd != NULL)
                                                                  {
                                                                     HFILE hfOutFile1;

                                                                     Base64Decode(pszMsgBegin, pszMsgEnd - pszMsgBegin, &ulFileLen);
                                                                     sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szDecodedFile);
                                                                     rc = DosOpen(szMergeFileName,&hfOutFile1,&action,0L,FILE_NORMAL,
                                                                        OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                                                        OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                                     if (rc == 0)
                                                                     {
                                                                        USHORT x;

                                                                        DosWrite(hfOutFile1,pszDecode,ulFileLen,&BytesWritten);

                                                                        DosClose(hfOutFile1);
                                                                     }
                                                                  }
                                                               }
                                                               UnLoadBase64();
                                                            }
                                                         }  */
                                                      }
                                                   }
                                                }
                                             }
                                             DosFreeMem(pszMyFile);
                                          }
                                          DosClose(hfOutFile);
                                       }

                                       sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szDecodedFile);
                                       rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                          OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                          OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                       if (rc == 0)
                                       {
                                          USHORT x;

                                          DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFileBeginPos);
                                          for (x = 1; x <= ulNumFiles; x++)
                                          {
                                             CHAR szNewFileName[256];
                                             HFILE hfOldFile;

                                             sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup,szFileName, x);
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

                                                   DosWrite(hfOutFile,pszMyFile,NewFilePos,&BytesWritten);
                                                   DosFreeMem(pszMyFile);
                                                }
                                                DosClose(hfOldFile);
                                             }
                                          }
                                          DosClose(hfOutFile);

                                          if (stricmp(szEncoding,"base64") == 0)
                                          {
                                             if (LoadBase64() == 0)
                                             {

                                                sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szDecodedFile);
                                                rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                                   OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                                   OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                if (rc == 0)
                                                {
                                                   ULONG NewFilePos;
                                                   ULONG NewFileBeginPos;
                                                   PSZ pszMyFile;
                                                   PSZ pszDecode;
                                                   ULONG BytesRead;
                                                   ULONG BytesWritten;

                                                   DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFilePos);
                                                   rc = DosAllocMem((VOID *)&pszMyFile,NewFilePos ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                                                   if (rc == 0)
                                                   {

                                                      DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                                      DosRead(hfOutFile,pszMyFile,NewFilePos, &BytesRead);

                                                      pszDecode = SearchFor("\r\n\r\n",pszMyFile, BytesRead, pszDecode);
                                                      if (pszDecode != NULL)
                                                      {
                                                         pszDecode += 4;
                                                         Base64Decode(pszDecode, NewFilePos - (pszDecode - pszMyFile), &NewFilePos);

                                                         DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                                                         DosWrite(hfOutFile,pszDecode,NewFilePos, &BytesRead);
                                                      }

                                                      DosFreeMem(pszMyFile);
                                                   }

                                                   DosClose(hfOutFile);
                                                }

                                                UnLoadBase64();
                                                for (x = 1; x <= ulNumFiles; x++)
                                                {
                                                   CHAR szNewFileName[256];
      
                                                   sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup,szFileName, x);
                                                   DosDelete(szNewFileName);
                                                }
                                             }
                                          }
/*                                          else
                                          {
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
                                          } */
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
/*                                 pszFileName = SearchFor("Content-Description:",pszBody, ulBodyLen, pszFileName);
                                 if (pszFileName != NULL)
                                 {
                                    pszFileNameEnd = SearchFor("\r\n",pszFileName, ulBodyLen - (pszFileName - pszBody), pszFileNameEnd);
                                    if (pszFileNameEnd != NULL)
                                    {
                                       pszFileName += 21;
                                       strncpy(szFileName, pszFileName, pszFileNameEnd - pszFileName);
                                       szFileName[pszFileNameEnd - pszFileName] = 0x00;

                                       pszEncoding = SearchFor("Content-Transfer-Encoding:",pszBody, ulBodyLen, pszEncoding);
                                       if (pszEncoding != NULL)
                                       {
                                          pszEncodingEnd = SearchFor("\r\n",pszBody, ulBodyLen - (pszEncoding - pszBody), pszEncodingEnd);
                                          if (pszEncodingEnd != NULL)
                                          {
                                             strncpy(szEncoding, pszEncoding, pszEncodingEnd - pszEncoding);
                                             szEncoding[pszEncodingEnd - pszEncoding] = 0x00;

                                             if (usTotal == 1)
                                             {
                                                if (stricmp(szEncoding,"base64") == 0)
                                                {
                                                   if (LoadBase64() == 0)
                                                   {
                                                      PSZ pszMsgBegin;
                                                      PSZ pszMsgEnd;

                                                      Base64Decode(pszMsgBoundryBegin, pszMsgBoundryEnd - pszMsgBoundryBegin, &ulFileLen);
                                                      UnLoadBase64();
                                                   }
                                                }
                                             }

                                          }

                                       }
                                    }
                                 } */




                              }
                           }
                        }
                     }


                  }
                  if (bLogs)
                     SaveLog(szVersion,pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                  if (strcmp(pszNewsGroup, szGroup) != 0)
                     strcpy(szGroup, pszNewsGroup);
               }
            }
         }
      }

      return(bMsgFound);
   }
   else
   {
      pszNEWSPoster = SearchFor("X-Newsreader: Forte Agent 1.92/32.57",pszHeader, ulHeaderLen, pszNEWSPoster);
      if (pszNEWSPoster != NULL)
      {
         HFILE hfOutFile;
         PSZ pszContentType;
         PSZ pszSubjectBegin;
         PSZ pszSubjectEnd;
         CHAR szFileName[256];
         PSZ pszTemp;

         pszContentType = SearchFor("Re:",pszHeader, ulHeaderLen, pszContentType);
         if (pszContentType != NULL)
         {
            return(1);
         }


         if ((SearchFor("=ybegin",pszBody, ulBodyLen, pszContentType) != NULL) &&
            (SearchFor("total=",pszBody, ulBodyLen, pszContentType) == NULL))
         {
            return(0);
         }




         pszNEWSPoster += 14;

         pszTemp = szVersion;
         while (*pszNEWSPoster != 0x0D)
            *(pszTemp++) = *(pszNEWSPoster++);

         *pszTemp = 0x00;
         if (strcmp(szVersion,"Forte Agent 1.92/32.572" ) == 0)
            strcpy(szVersion,"ForteAgent192.log");

         pszContentType = SearchFor("Content-Type: text/plain;",pszHeader, ulHeaderLen, pszContentType);
         if (pszContentType != NULL)
         {
            return(1);

         }
         pszContentType = SearchFor("Re:",pszHeader, ulHeaderLen, pszContentType);
         if (pszContentType != NULL)
         {
            return(1);
         }
         pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
         if (pszSubjectBegin != NULL)
         {
            pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
            if (pszSubjectEnd != NULL)
            {

               if (pszContentType == NULL)
               {

                  PSZ pszFileMarker;
                  PSZ pszFileMarker1;
                  CHAR szTemp[100];
                  PSZ pszLineTemp;
                  ULONG ulPart;
                  ULONG ulTotal;
                  ULONG ulDirSize;


                  pszFileMarker = SearchFor("\"",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszFileMarker);
                  if (pszFileMarker != NULL)
                  {

                     pszFileMarker++;
                     pszFileMarker1 = SearchFor("\"",pszFileMarker, (pszSubjectEnd - pszFileMarker), pszFileMarker1);
                     if (pszFileMarker1 == NULL)
                        pszFileMarker1 = pszSubjectEnd;

                     strncpy(szFileName, pszFileMarker, (pszFileMarker1 - pszFileMarker));
                     szFileName[pszFileMarker1 - pszFileMarker] = 0x00;

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
                           while ((*pszLineTemp != ')') && (pszLineTemp < pszSubjectEnd))
                              *(pszTemp++) = *(pszLineTemp++);

                           if (*pszLineTemp == ')')
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

                     rc = 0;

                     if (rc == 0)
                     {
                        HFILE hfOutFile;
                        ULONG action;
                        ULONG BytesWritten;
                        CHAR szNewFileName[256];

                        if (ulTotal > 1)
                        {
                           sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup, szFileName, ulPart);
   /*                        printf("Saving %s\r\n filename %s, part %d of %d\r\n",szVersion, szFileName, ulPart,ulTotal);  */
                        }
                        else
                        {

                           sprintf(szNewFileName,"%s\\%s",pszNewsGroup, szFileName);
   /*                        printf("Saving %s\r\n filename %s ",szVersion, szFileName);  */
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
                              pszDataEnd = SearchFor("\r\n.\r\n",pszBody, ulBodyLen, pszDataEnd);

                              if ((pszDataEnd != NULL) && (pszDataEnd > pszDataBegin))
                              {
                                 bMsgFound = TRUE;

                                 DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);
                              }
                           }

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

                              sprintf(szFindFile,"%s\\%s.FA*",pszNewsGroup, szFileName);

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

                                 while (rc != ERROR_NO_MORE_FILES)
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
                              }
                              DosFindClose(hdirFindHandle);

                              if (ulNumFiles == ulTotal)
                              {
                                 HFILE hfOutFile;
                                 CHAR szMergeFileName[256];

   /*                              printf("Merging file %s", szFileName);  */
                                 sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
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

                                       sprintf(szNewFileName,"%s\\%s.FA%d",pszNewsGroup, szFileName, x);
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

   /*                                 UULoadFile(szMergeFileName,NULL,1);
                                    UUDecodeFile(UUGetFileListItem(0), NULL);
                                    if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                                       printf("- Error decoding file\r\n");
                                    else
                                       printf("- decoded\r\n");  */

                                 }
                              }

                           }
                           else
                           {
                              CHAR szMergeFileName[256];

                              sprintf(szMergeFileName,"$s\\%s",pszNewsGroup, szFileName);

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
   /*                           UULoadFile(szMergeFileName,NULL,1);
                              UUDecodeFile(UUGetFileListItem(0), NULL);
                              if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                                 printf("- Error decoding file\r\n");
                              else
                                 printf("- decoded\r\n");  */

                           }

                        }
                        else
                        {

   /*                        switch(rc)
                           {
                              case ERROR_FILE_NOT_FOUND:
                                 printf("File Not Found.\r\n");
                                 break;
                              case ERROR_PATH_NOT_FOUND:
                                 printf("Path Not Found \r\n");
                                 break;
                              case ERROR_TOO_MANY_OPEN_FILES:
                                 printf("Too Many Open Files. \r\n");
                                 break;
                              case ERROR_ACCESS_DENIED:
                                 printf("Access Denied.\r\n");
                                 break;
                              case ERROR_INVALID_ACCESS:
                                 printf("Invalid Access.\r\n");
                                 break;
                              case ERROR_NOT_DOS_DISK:
                                 printf("Not a Dos Disk");
                                 break;
                              case ERROR_SHARING_VIOLATION:
                                 printf("Sharing Violation.\r\n");
                                 break;
                              case ERROR_SHARING_BUFFER_EXCEEDED:
                                 printf("Sharing Buffer Exceeded.\r\n");
                                 break;
                              case ERROR_CANNOT_MAKE:
                                 printf("Cannot Make File. \r\n");
                                 break;
                              case ERROR_INVALID_PARAMETER:
                                 printf("Invalid Parameter.\r\n");
                                 break;
                              case ERROR_DEVICE_IN_USE:
                                 printf("Device in Use.\r\n");
                                 break;
                              case ERROR_DRIVE_LOCKED:
                                 printf("Drive Locked.\r\n");
                                 break;
                              case ERROR_OPEN_FAILED:
                                 printf("Open Failed.\r\n");
                                 break;
                              case ERROR_DISK_FULL:
                                 printf("Disk Full.\r\n");
                                 break;
                              case ERROR_FILENAME_EXCED_RANGE:
                                 printf("File Name Exceded Range.\r\n");
                                 break;
                              case ERROR_PIPE_BUSY:
                                 printf("Pipe Busy.\r\n");
                                 break;
                              default:
                                 printf("Unknown Error\r\n");
                                 break;
                           }  */

                           bMsgFound = FALSE;
                        }

   /*                     rc = DosSetCurrentDir("\\");
                        rc = DosSetCurrentDir(szCurrentDir);  */
                        if (strcmp(pszNewsGroup, szGroup) != 0)
                           strcpy(szGroup, pszNewsGroup);
                     }
                  }
                  else
                  {
/*                     bMsgFound = TRUE;  */
   /*                  printf("No valid filename found - cannot save\r\n");  */
                     if (strcmp(pszNewsGroup, szGroup) != 0)
                        strcpy(szGroup, pszNewsGroup);
                  }
               }
            }
         }

         if (bLogs)
            SaveLog("ForteAgent192.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup192));
         strcpy(szGroup192, pszNewsGroup);

         return(bMsgFound);
      }

      return(0);
   }


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
} */

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

int LoadBase64(void)
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

      rc = DosLoadModule(szLoadError,sizeof(szLoadError),"Base64.dll",&hmBase64);
      if (rc == 0)
      {
         strcpy(szFileID, "");
         WinLoadString((HMODULE)NULL, hmBase64, IDS_GYN, 256, szFileID);
         if (strcmp(szFileID,"GetYourNews") == 0)
         {
            WinLoadString((HMODULE)NULL, hmBase64, IDS_GYN_TYPE, 256, szDLLType);
            if (strcmp(szDLLType,"Decoder") == 0)
            {

               rc = DosQueryProcAddr(hmBase64,0,"Base64Decode",(int(** _System)())&Base64Decode);

            }
         }
         bBase64DecoderNotLoaded = FALSE;
      }
   }
   else
      rc = 0;
   return(rc);
}

int UnLoadBase64(void)
{

   DosFreeModule(hmBase64);
   bBase64DecoderNotLoaded = TRUE;
   return(0);
}

