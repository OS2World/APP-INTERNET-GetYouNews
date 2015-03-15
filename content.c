#define EXTERN

#include "Content.h"

#include "uudecodevars.h"
#include "base64vars.h"
#include "gynplugin.h"

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bBase64DecoderNotLoaded = TRUE;
HMODULE hmBase64;


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
   PSZ pszOLE;
   BOOL bTextMsgFound = FALSE;
   BOOL bMsgFound = FALSE;
   PSZ pszContentType;
   PSZ pszSubjectBegin;
   PSZ pszSubjectEnd;
   CHAR szTemp[10];
   PSZ pszTemp;
   USHORT usPart;
   USHORT ulTotal;
   CHAR szFileName[256];
   PSZ pszFileName;
   ULONG ulFileLen = 0;


   x = _dllentry;
   pszContentType = SearchFor("Content-Type: multipart/mixed;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-type: multipart/mixed;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-Type: Multipart/Mixed;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-Type: multipart/alternative;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-type: multipart/alternative;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-Type: multipart/related;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType == NULL)
      pszContentType = SearchFor("Content-Type: multipart;",pszHeader, ulHeaderLen, pszContentType);
   if (pszContentType != NULL)
   {
      CHAR szBoundryInfo[256];
      PSZ pszBoundryInfo;
      PSZ pszBoundryBegin;
      BOOL bSaveFile = FALSE;


      pszBoundryBegin = SearchFor("boundary=\"",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszBoundryBegin);
      if (pszBoundryBegin == NULL)
      {
         pszBoundryBegin = SearchFor("Boundary=",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszBoundryBegin);
         if (pszBoundryBegin != NULL)
            pszBoundryBegin += 9;
         else
         {
            pszBoundryBegin = SearchFor("boundary=",pszContentType, ulHeaderLen - (pszContentType - pszHeader), pszBoundryBegin);
            if (pszBoundryBegin != NULL)
               pszBoundryBegin += 9;

         }
      }
      else
         pszBoundryBegin += 10;
      if (pszBoundryBegin)
      {
         pszBoundryInfo = szBoundryInfo;


         while ((*pszBoundryBegin != '\"') && (*pszBoundryBegin != 0x0D) &&
               (pszBoundryInfo < szBoundryInfo + sizeof(szBoundryInfo)) &&
               (pszBoundryBegin < pszHeader + ulHeaderLen))
               *(pszBoundryInfo++) = *(pszBoundryBegin++);

         if (pszBoundryBegin != NULL)
         {
            PSZ pszMsgContent;
            PSZ pszMsgBoundryBegin;
            PSZ pszMsgBoundryEnd;

            *pszBoundryInfo = 0x00;

            pszMsgBoundryBegin = SearchFor(szBoundryInfo,pszBody, ulBodyLen, pszMsgBoundryBegin);
            if (pszMsgBoundryBegin == NULL)
            {
/*               printf("Invalid formated content message. Just more SPAM!\r\n");  */
               bMsgFound = TRUE;
            }
            while (pszMsgBoundryBegin != NULL)
            {
               pszMsgBoundryEnd = SearchFor(szBoundryInfo,pszMsgBoundryBegin + 1,
                  ulBodyLen - (pszMsgBoundryBegin - pszBody), pszMsgBoundryEnd);

               if (pszMsgBoundryEnd != NULL)
               {
                  PSZ pszThisMsgContentType;
                  /* more than one message remaining */

                  strcpy(szFileName, "");
                  pszThisMsgContentType = SearchFor("Content-Type: text/plain",pszMsgBoundryBegin,
                     pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                  if (pszThisMsgContentType == NULL)
                     pszThisMsgContentType = SearchFor("Content-type: text/plain",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                  if (pszThisMsgContentType == NULL)
                     pszThisMsgContentType = SearchFor("Content-Type: Text/Plain",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                  if (pszThisMsgContentType == NULL)
                     pszThisMsgContentType = SearchFor("Content-Type: text/html",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                  if (pszThisMsgContentType != NULL)
                  {
                     PSZ pszEndContent;
                     bMsgFound = TRUE;
                     ulFileLen = 0;
                     pszEndContent = SearchFor("\r\n",pszThisMsgContentType,
                        (ULONG)pszMsgBoundryEnd - (pszThisMsgContentType - pszMsgBoundryBegin), pszEndContent);
                     if (pszEndContent != NULL)
                     {
                        CHAR szContent[256];

                        if ((pszEndContent - pszThisMsgContentType) < sizeof(szContent) - 1)
                        {
                           strncpy(szContent,pszThisMsgContentType, pszEndContent - pszThisMsgContentType);
                           szContent[pszEndContent - pszThisMsgContentType] = 0x00;

/*                           printf("%s\r\n", szContent);  */
                        }
                     }
                  }
                  else
                  {

                     pszThisMsgContentType = SearchFor("Content-Type: image/jpeg;",pszMsgBoundryBegin,
                        pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType == NULL)
                        pszThisMsgContentType = SearchFor("Content-type: image/jpeg;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType == NULL)
                        pszThisMsgContentType = SearchFor("Content-Type: Image/JPEG;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType == NULL)
                        pszThisMsgContentType = SearchFor("Content-Type: application/octet-stream;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType == NULL)
                        pszThisMsgContentType = SearchFor("Content-Type: image/pjpeg;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType == NULL)
                        pszThisMsgContentType = SearchFor("Content-Type: video/x-msvideo;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                     if (pszThisMsgContentType != NULL)
                     {
                        PSZ pszDisposition;
                        PSZ pszContentEncoding;
                        CHAR szEncoding[50];
                        PSZ pszEncoding;

                        CHAR szContentType[256];
                        PSZ pszContentBegin;
                        PSZ pszContentEnd;

                        strcpy(szContentType, "Unknown");
                        pszContentBegin = SearchFor("Content-Type: ",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszContentBegin);
                        if (pszContentBegin != NULL)
                        {
                           pszContentBegin += 14;

                           pszContentEnd = SearchFor(";",pszContentBegin,
                              pszMsgBoundryEnd - pszContentBegin, pszContentEnd);
                           if ((pszContentEnd != NULL) && ((pszContentEnd - pszContentBegin) < 255))
                           {
                              strncpy(szContentType, pszContentBegin, pszContentEnd - pszContentBegin);
                              szContentType[pszContentEnd - pszContentBegin] = 0x00;
                           }

                        }

                        pszDisposition = SearchFor("Content-Disposition: attachment;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                        if (pszDisposition == NULL)
                           pszDisposition = SearchFor("Content-disposition: attachment;",pszMsgBoundryBegin,
                              pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                        if (pszDisposition != NULL)
                        {
                           PSZ pszFileNameBegin;
                           PSZ pszFileNameEnd;
                           pszFileNameBegin = SearchFor("filename=",pszDisposition,
                              pszMsgBoundryEnd - pszDisposition, pszFileNameBegin);
                           if (pszFileNameBegin != NULL)
                           {
                              pszFileNameBegin += 9;
                              pszFileNameEnd = SearchFor("\r\n",pszFileNameBegin,
                                 pszMsgBoundryEnd - pszFileNameBegin, pszFileNameEnd);

                              if (pszFileNameEnd != NULL)
                              {
                                 PSZ pszOriginal;

                                 pszFileNameEnd--;
                                 if (*pszFileNameEnd != '\"')
                                    pszFileNameEnd++;

                                 if (*pszFileNameBegin == '\"')
                                    pszFileNameBegin++;
                                 pszOriginal = pszFileNameBegin;

                                 while ((pszFileNameBegin < pszFileNameEnd))
                                 {
                                    if ((*pszFileNameBegin == ':') || (*pszFileNameBegin == '\\'))
                                       pszOriginal = pszFileNameBegin + 1;
                                    pszFileNameBegin++;
                                 }
                                 pszFileNameBegin = pszOriginal;

                                 strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                                 szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;

                              }
                           }
                        }
                        else
                        {
                           PSZ pszFileNameBegin;
                           PSZ pszFileNameEnd;

                           pszFileNameBegin = SearchFor("name=",pszMsgBoundryBegin,
                              pszMsgBoundryEnd - pszMsgBoundryBegin, pszFileNameBegin);
                           if (pszFileNameBegin != NULL)
                           {
                              pszFileNameEnd = SearchFor("\r",pszFileNameBegin,
                                 pszMsgBoundryEnd - pszFileNameBegin, pszFileNameEnd);
                              if (pszFileNameEnd != NULL)
                              {
                                 pszFileNameBegin += 5;
                                 if (*pszFileNameBegin == '\"')
                                    pszFileNameBegin++;
                                 if (*(pszFileNameEnd - 1) == '\"')
                                    pszFileNameEnd--;

                                 strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                                 szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;
                              }

                           }

                        }


                        pszContentEncoding = SearchFor("Content-Transfer-Encoding:",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszContentEncoding);
                        if (pszContentEncoding != NULL)
                        {
                           pszContentEncoding += 27;

                           pszEncoding = szEncoding;
                           while ((*pszContentEncoding != 0x0D) && (pszEncoding < szEncoding + (sizeof(szEncoding) - 1)))
                              *(pszEncoding++) = *(pszContentEncoding++);

                           *pszEncoding = 0x00;

                           if (stricmp(szEncoding,"base64") == 0)
                           {
                              if (strlen(szFileName) > 0)
                              {

                                 if (LoadBase64() == 0)
                                 {

/*                                    printf("Decoding BASE64\r\n");  */

                                    Base64Decode(pszMsgBoundryBegin, pszMsgBoundryEnd - pszMsgBoundryBegin, &ulFileLen);
                                    UnLoadBase64();
                                 }
                              }

                           }
                           else
                              ulFileLen = pszMsgBoundryEnd - pszMsgBoundryBegin;
                        }
                        else
                           ulFileLen = pszMsgBoundryEnd - pszMsgBoundryBegin;

/*                        if (strlen(szFileName) == 0)
                        {



                           HDIR          hdirFindHandle = HDIR_SYSTEM;
                           FILEFINDBUF3  FindBuffer     = {0};
                           ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
                           ULONG         ulFindCount    = 1;
                           APIRET        rc             = NO_ERROR;
                           ULONG          ulNumFiles = 0;
                           CHAR szFindFile[256];
                           ULONG       ulSavedSize = 0;
                           CHAR szCurrentDir[512];
                           CHAR szNewDir[512];
                           ULONG ulDirSize;

                           ulDirSize = sizeof(szCurrentDir);
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
                           }

                           if (rc != 0)
                           {

                              strcpy(szFindFile,"Unknown*.jpg");

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
                                    ulFindCount = 1;

                                    rc = DosFindNext(hdirFindHandle,
                                                     &FindBuffer,
                                                     ulResultBufLen,
                                                     &ulFindCount);

                                    if (rc == NO_ERROR)
                                    {
                                       ulNumFiles++;
                                       ulSavedSize += FindBuffer.cbFile;
                                    }
                                 }

                                 sprintf(szFileName,"Unknown%d.jpg",ulNumFiles);
                              }
                              else
                                 strcpy(szFileName,"Unknown.jpg");


                              rc = DosSetCurrentDir("\\");
                              rc = DosSetCurrentDir(szCurrentDir);
                           }

                        } */

                        if (strlen(szFileName) > 0)
                        {

                           APIRET rc;

/*                           ulDirSize = sizeof(szCurrentDir);
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

                              bMsgFound = TRUE;
/*                              printf("Saving %s filename %s, Encoding %s",szContentType, szFileName, szEncoding);  */
                              sprintf(szNewFileName, "%s\\%s", pszNewsGroup, szFileName);
                              rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                              if (rc == 0)
                              {
                                 DosWrite(hfOutFile,pszMsgBoundryBegin, ulFileLen,&BytesWritten);

                                 DosClose(hfOutFile);
                                 if (strcmp(szEncoding,"x-uuencode") == 0)
                                 {

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

/*                                    UULoadFile(szNewFileName,NULL,1);
                                    UUDecodeFile(UUGetFileListItem(0), NULL);
                                    if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                                       printf("- Error decoding file\r\n");
                                    else
                                       printf("- decoded\r\n");  */

                                 }
/*                                 else
                                    printf("\r\n");  */
                              }
/*                              rc = DosSetCurrentDir("\\");
                              rc = DosSetCurrentDir(szCurrentDir);  */
                           }
/*                           else
                              printf("Error changing directories %s error %d\r\n", szNewDir, rc);  */

                        }
                        else
                        {

                           bMsgFound = FALSE;
/*                           printf("No filename to save\r\n");  */
                        }
                     }
                     else
                     {


                        pszThisMsgContentType = SearchFor("Content-type: multipart/appledouble;",pszMsgBoundryBegin,
                           pszMsgBoundryEnd - pszMsgBoundryBegin, pszThisMsgContentType);
                        if (pszThisMsgContentType != NULL)
                        {
                           PSZ pszNewMsgBoundryBegin;
                           PSZ pszNewMsgBoundryEnd;
                           PSZ pszNewBoundryBegin;
                           PSZ pszNewBoundryEnd;
                           CHAR szNewBoundryInfo[256];
                           PSZ pszNewBoundryInfo;


                           pszNewBoundryBegin = SearchFor("boundary=\"",pszMsgBoundryBegin, ulBodyLen - (pszMsgBoundryBegin - pszBody), pszNewBoundryBegin);
                           if (pszNewBoundryBegin)
                           {
                              pszNewBoundryInfo = szNewBoundryInfo;

                              pszNewBoundryBegin += 10;

                              while ((*pszNewBoundryBegin != '\"') && (*pszNewBoundryBegin != 0x0D) &&
                                    (pszNewBoundryInfo < szNewBoundryInfo + sizeof(szNewBoundryInfo)) &&
                                    (pszNewBoundryBegin < pszBody + ulBodyLen))
                                    *(pszNewBoundryInfo++) = *(pszNewBoundryBegin++);

                              if (*pszNewBoundryBegin == '\"')
                              {
                                 PSZ pszMsgContent;
                                 PSZ pszNewMsgBoundryBegin;
                                 PSZ pszNewMsgBoundryEnd;

                                 *pszBoundryInfo = 0x00;

                                 pszNewMsgBoundryBegin = SearchFor(szNewBoundryInfo,pszMsgBoundryBegin, ulBodyLen - (pszMsgBoundryBegin - pszBody), pszNewMsgBoundryBegin);
                                 while (pszNewMsgBoundryBegin != NULL)
                                 {
                                    pszNewMsgBoundryEnd = SearchFor(szNewBoundryInfo,pszNewMsgBoundryBegin + 1,
                                       ulBodyLen - (pszNewMsgBoundryBegin - pszBody), pszNewMsgBoundryEnd);

                                    if (pszNewMsgBoundryEnd != NULL)
                                    {
                                       PSZ pszThisMsgContentType;


                                       pszThisMsgContentType = SearchFor("Content-type: application/applefile;",pszNewMsgBoundryBegin,
                                          pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszThisMsgContentType);
                                       if (pszThisMsgContentType == NULL)
                                          pszThisMsgContentType = SearchFor("Content-type: image/jpeg;",pszNewMsgBoundryBegin,
                                             pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszThisMsgContentType);
                                       if (pszThisMsgContentType != NULL)
                                       {
                                          PSZ pszDisposition;
                                          PSZ pszContentEncoding;
                                          CHAR szEncoding[50];
                                          PSZ pszEncoding;

                                          CHAR szContentType[256];
                                          PSZ pszContentBegin;
                                          PSZ pszContentEnd;

                                          strcpy(szContentType, "Unknown");
                                          pszContentBegin = SearchFor("Content-type: ",pszNewMsgBoundryBegin,
                                             pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszContentBegin);
                                          if (pszContentBegin != NULL)
                                          {
                                             pszContentBegin += 14;

                                             pszContentEnd = SearchFor(";",pszContentBegin,
                                                pszNewMsgBoundryEnd - pszContentBegin, pszContentEnd);
                                             if ((pszContentEnd != NULL) && ((pszContentEnd - pszContentBegin) < 255))
                                             {
                                                strncpy(szContentType, pszContentBegin, pszContentEnd - pszContentBegin);
                                                szContentType[pszContentEnd - pszContentBegin] = 0x00;
                                             }

                                          }

                                          pszDisposition = SearchFor("Content-disposition: attachment",pszNewMsgBoundryBegin,
                                             pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszThisMsgContentType);
                                          if (pszDisposition != NULL)
                                          {
                                             PSZ pszFileNameBegin;
                                             PSZ pszFileNameEnd;
                                             pszFileNameBegin = SearchFor("name=",pszNewMsgBoundryBegin,
                                                pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszFileNameBegin);
                                             if (pszFileNameBegin != NULL)
                                             {
                                                pszFileNameBegin += 5;
                                                pszFileNameEnd = SearchFor("\r\n",pszFileNameBegin,
                                                   pszNewMsgBoundryEnd - pszFileNameBegin, pszFileNameEnd);

                                                if (pszFileNameEnd != NULL)
                                                {
                                                   PSZ pszOriginal;

                                                   pszFileNameEnd--;
                                                   if (*pszFileNameEnd == ';')
                                                      pszFileNameEnd--;
                                                   if (*pszFileNameEnd != '\"')
                                                      pszFileNameEnd++;

                                                   pszOriginal = pszFileNameBegin;
                                                   pszFileNameBegin = pszFileNameEnd;

                                                   while ((*pszFileNameBegin != ' ') && (*pszFileNameBegin != '\\') &&
                                                      (*pszFileNameBegin != ':') && (pszFileNameBegin > pszOriginal))
                                                         pszFileNameBegin--;

                                                   pszFileNameBegin++;

                                                   strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                                                   szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;


                                                }
                                             }
                                          }


                                          pszContentEncoding = SearchFor("Content-transfer-encoding:",pszNewMsgBoundryBegin,
                                             pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, pszContentEncoding);
                                          if (pszContentEncoding != NULL)
                                          {
                                             pszContentEncoding += 27;

                                             pszEncoding = szEncoding;
                                             while ((*pszContentEncoding != 0x0D) && (pszEncoding < szEncoding + (sizeof(szEncoding) - 1)))
                                                *(pszEncoding++) = *(pszContentEncoding++);

                                             *pszEncoding = 0x00;

                                             if (strcmp(szEncoding,"base64") == 0)
                                             {
                                                if (LoadBase64() == 0)
                                                {

/*                                                   printf("Decoding BASE64\r\n");  */

                                                   Base64Decode(pszNewMsgBoundryBegin, pszNewMsgBoundryEnd - pszNewMsgBoundryBegin, &ulFileLen);
                                                   UnLoadBase64();
                                                }

                                             }
                                             else
                                                ulFileLen = pszNewMsgBoundryEnd - pszNewMsgBoundryBegin;
                                          }
                                          else
                                             ulFileLen = pszNewMsgBoundryEnd - pszNewMsgBoundryBegin;

                                          if (strlen(szFileName) > 0)
                                          {

                                             APIRET rc;

/*                                             ulDirSize = sizeof(szCurrentDir);
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

                                                bMsgFound = TRUE;
                                                if (strcmp(szContentType, "application/applefile") == 0)
                                                   sprintf(szNewFileName,"%s\\%s.app", pszNewsGroup,szFileName);
                                                else
                                                   sprintf(szNewFileName,"%s\\%s", pszNewsGroup, szFileName);
/*                                                printf("Saving %s filename %s, Encoding %s\r\n",szContentType, szNewFileName, szEncoding);  */
                                                rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                                   OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                                   OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                                if (rc == 0)
                                                {
                                                   DosWrite(hfOutFile,pszNewMsgBoundryBegin, ulFileLen,&BytesWritten);

                                                   DosClose(hfOutFile);
                                                }
/*                                                rc = DosSetCurrentDir("\\");
                                                rc = DosSetCurrentDir(szCurrentDir);  */
                                             }
/*                                             else
                                                printf("Error changing directories %s error %d\r\n", szNewDir, rc);  */

                                          }
                                          else
                                          {

                                             bMsgFound = FALSE;
/*                                             printf("No filename to save\r\n");  */
                                          }
                                       }


                                    }
                                    if (pszNewMsgBoundryEnd != NULL)
                                       pszNewMsgBoundryBegin = pszNewMsgBoundryEnd + strlen(szBoundryInfo);
                                    else
                                       pszNewMsgBoundryBegin = NULL;


                                 }

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

      return(bMsgFound);
   }
   else
   {

      pszContentType = SearchFor("Content-Type: image/jpeg;",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType != NULL)
      {
         PSZ pszDisposition;
         PSZ pszContentEncoding;
         CHAR szEncoding[50];
         PSZ pszEncoding;

         CHAR szContentType[256];
         PSZ pszContentBegin;
         PSZ pszContentEnd;

         strcpy(szContentType, "Unknown");
         pszContentBegin = SearchFor("Content-Type: ",pszHeader, ulHeaderLen, pszContentBegin);
         if (pszContentBegin != NULL)
         {
            pszContentBegin += 14;

            pszContentEnd = SearchFor(";",pszContentBegin, ulHeaderLen - (pszContentBegin - pszHeader), pszContentEnd);
            if ((pszContentEnd != NULL) && ((pszContentEnd - pszContentBegin) < 255))
            {
               strncpy(szContentType, pszContentBegin, pszContentEnd - pszContentBegin);
               szContentType[pszContentEnd - pszContentBegin] = 0x00;
            }
         }

         pszDisposition = SearchFor("Content-Disposition: inline;",pszHeader, ulHeaderLen, pszDisposition);
         if (pszDisposition != NULL)
         {
            PSZ pszFileNameBegin;
            PSZ pszFileNameEnd;
            pszFileNameBegin = SearchFor("filename=",pszDisposition, ulHeaderLen - (pszDisposition - pszHeader), pszFileNameBegin);
            if (pszFileNameBegin != NULL)
            {
               pszFileNameBegin += 9;
               pszFileNameEnd = SearchFor("\r\n",pszFileNameBegin, ulHeaderLen - (pszFileNameBegin - pszHeader), pszFileNameEnd);

               if (pszFileNameEnd != NULL)
               {
                  PSZ pszOriginal;

                  pszFileNameEnd--;
                  if (*pszFileNameEnd != '\"')
                     pszFileNameEnd++;

                  pszOriginal = pszFileNameBegin;
                  pszFileNameBegin = pszFileNameEnd;

                  while ((*pszFileNameBegin != ' ') && (*pszFileNameBegin != '\\') &&
                     (*pszFileNameBegin != ':') && (pszFileNameBegin > pszOriginal))
                        pszFileNameBegin--;

                  pszFileNameBegin++;

                  strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                  szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;


               }
            }
         }
         else
         {
            PSZ pszFileNameBegin;
            PSZ pszFileNameEnd;

            pszFileNameBegin = SearchFor("name=",pszHeader, ulHeaderLen, pszFileNameBegin);
            if (pszFileNameBegin != NULL)
            {
               pszFileNameEnd = SearchFor("\r",pszFileNameBegin, ulHeaderLen - (pszFileNameBegin - pszHeader), pszFileNameEnd);
               if (pszFileNameEnd != NULL)
               {
                  pszFileNameBegin += 5;
                  if (*pszFileNameBegin == '\"')
                     pszFileNameBegin++;
                  if (*(pszFileNameEnd - 1) == '\"')
                     pszFileNameEnd--;

                  strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                  szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;
               }

            }

         }


         pszContentEncoding = SearchFor("Content-Transfer-Encoding:",pszHeader, ulHeaderLen, pszContentEncoding);
         if (pszContentEncoding != NULL)
         {
            pszContentEncoding += 27;

            pszEncoding = szEncoding;
            while ((*pszContentEncoding != 0x0D) && (pszEncoding < szEncoding + (sizeof(szEncoding) - 1)))
               *(pszEncoding++) = *(pszContentEncoding++);

            *pszEncoding = 0x00;

            if (strcmp(szEncoding,"base64") == 0)
            {
               if (strlen(szFileName) > 0)
               {

                  if (LoadBase64() == 0)
                  {

/*                     printf("Decoding BASE64\r\n");  */

                     Base64Decode(pszBody, ulBodyLen, &ulFileLen);
                     UnLoadBase64();
                  }
               }

            }
            else
               ulFileLen = ulBodyLen;
         }
         else
            ulFileLen = ulBodyLen;

/*                        if (strlen(szFileName) == 0)
            {



               HDIR          hdirFindHandle = HDIR_SYSTEM;
               FILEFINDBUF3  FindBuffer     = {0};
               ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
               ULONG         ulFindCount    = 1;
               APIRET        rc             = NO_ERROR;
               ULONG          ulNumFiles = 0;
               CHAR szFindFile[256];
               ULONG       ulSavedSize = 0;
               CHAR szCurrentDir[512];
               CHAR szNewDir[512];
               ULONG ulDirSize;

               ulDirSize = sizeof(szCurrentDir);
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
               }

               if (rc != 0)
               {

                  strcpy(szFindFile,"Unknown*.jpg");

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
                        ulFindCount = 1;

                        rc = DosFindNext(hdirFindHandle,
                                         &FindBuffer,
                                         ulResultBufLen,
                                         &ulFindCount);

                        if (rc == NO_ERROR)
                        {
                           ulNumFiles++;
                           ulSavedSize += FindBuffer.cbFile;
                        }
                     }

                     sprintf(szFileName,"Unknown%d.jpg",ulNumFiles);
                  }
                  else
                     strcpy(szFileName,"Unknown.jpg");


                  rc = DosSetCurrentDir("\\");
                  rc = DosSetCurrentDir(szCurrentDir);
               }

            } */

         if (strlen(szFileName) > 0)
         {

            APIRET rc;

/*            ulDirSize = sizeof(szCurrentDir);
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

               bMsgFound = TRUE;
/*               printf("Saving %s filename %s, Encoding %s\r\n",szContentType, szFileName, szEncoding);  */
               sprintf(szNewFileName,"%s\\%s", pszNewsGroup, szFileName);
               rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                  OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
               if (rc == 0)
               {
                  DosWrite(hfOutFile,pszBody, ulFileLen,&BytesWritten);

                  DosClose(hfOutFile);
                  if (strcmp(szEncoding,"x-uuencode") == 0)
                  {

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
/*                     UULoadFile(szNewFileName,NULL,1);
                     UUDecodeFile(UUGetFileListItem(0), NULL);
/*                     if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                        printf("- Error decoding file\r\n");
                     else
                        printf("- decoded\r\n");  */

                  }
               }
/*               rc = DosSetCurrentDir("\\");
               rc = DosSetCurrentDir(szCurrentDir);  */
               return(1);
            }
/*            else
               printf("Error changing directories %s error %d\r\n", szNewDir, rc);  */

         }
         else
         {

            bMsgFound = FALSE;
/*            printf("No filename to save\r\n");  */
         }
      }
      pszContentType = SearchFor("Content-Type: application/octet-stream;",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType == NULL)
         pszContentType = SearchFor("Content-Type: application/x-shockwave-flash;",pszHeader, ulHeaderLen, pszContentType);
      if (pszContentType != NULL)
      {
         PSZ pszDisposition;

         pszDisposition = SearchFor("Content-Disposition: inline",pszHeader, ulHeaderLen, pszDisposition);
         if (pszDisposition != NULL)
         {
            PSZ pszFileName;
            CHAR szFileName[256];


            pszFileName = SearchFor("name=\"",pszHeader, ulHeaderLen, pszFileName);
            if (pszFileName != NULL)
            {
               PSZ pszTemp;
               PSZ pszMsgBoundryBegin;
               PSZ pszMsgBoundryEnd;
               ULONG ulFileLen;


               pszTemp = szFileName;

               pszFileName += 6;

               while ((*pszFileName != '"') && (*pszFileName != 0x0D))
                  *(pszTemp++) = *(pszFileName++);
               *pszTemp = 0x00;



               pszMsgBoundryBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszMsgBoundryBegin);
               if (pszMsgBoundryBegin != NULL)
               {
                  pszMsgBoundryEnd = SearchFor("\r\n\r\n",pszBody, ulBodyLen, pszMsgBoundryEnd);
                  if (pszMsgBoundryEnd != NULL)
                  {
                     if (LoadBase64() == 0)
                     {

/*                        printf("Decoding BASE64\r\n");  */

                        Base64Decode(pszMsgBoundryBegin, pszMsgBoundryEnd - pszMsgBoundryBegin, &ulFileLen);
                        UnLoadBase64();

                        if (strlen(szFileName) > 0)
                        {

                           APIRET rc;

/*                           ulDirSize = sizeof(szCurrentDir);
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

/*                              printf("Saving filename %s\r\n",szFileName);  */
                              sprintf(szNewFileName, "%s\\%s", pszNewsGroup, szFileName);
                              rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                              if (rc == 0)
                              {
                                 DosWrite(hfOutFile,pszMsgBoundryBegin, ulFileLen,&BytesWritten);

                                 DosClose(hfOutFile);
                              }
/*                              rc = DosSetCurrentDir("\\");
                              rc = DosSetCurrentDir(szCurrentDir);  */
                              return(1);
                           }
/*                           else
                              printf("Error changing directories %s error %d\r\n", szNewDir, rc);  */

                        }
                        else
                        {

                           bMsgFound = FALSE;
/*                           printf("No filename to save\r\n");  */
                        }

                     }
                  }
               }
            }
         }
      }

      pszContentType = SearchFor("This is a multi-part message in MIME format.",pszBody, ulBodyLen, pszContentType);
      if (pszContentType != NULL)
      {
         BOOL bPartFound = FALSE;
         CHAR szNextPart[256];
         PSZ pszNextPart;
         PSZ pszEndNextPart;

         pszNextPart = SearchFor("_NextPart_",pszBody, ulBodyLen, pszNextPart);
         if (pszNextPart != NULL)
         {
            PSZ pszTemp;

            pszTemp = szNextPart;
            while (*pszNextPart != 0x0D)
               *(pszTemp++) = *(pszNextPart++);

            *pszTemp = 0x00;
            while ((pszEndNextPart = SearchFor(szNextPart,pszNextPart + 1, ulBodyLen - ((pszNextPart + 1) - pszBody), pszNextPart)) != NULL)
            {
               pszContentType = SearchFor("Content-Type: text/plain;",pszNextPart, pszEndNextPart - pszNextPart, pszContentType);
               if (pszContentType != NULL)
               {
/*                  printf("Content-Type: text/plain;\r\n");  */
                  bPartFound = TRUE;
               }
               else
               {

                  pszContentType = SearchFor("Content-Type: image/jpeg;",pszNextPart, pszEndNextPart - pszNextPart, pszContentType);
                  if (pszContentType != NULL)
                  {
                     CHAR szFileName[256];
                     PSZ pszFileName;
                     PSZ pszEncoding;

                     pszFileName = SearchFor("filename=",pszNextPart, pszEndNextPart - pszNextPart, pszFileName);
                     if (pszFileName != NULL)
                     {
                        pszFileName += 9;

                        if (*pszFileName == '\"')
                        {
                           pszFileName++;
                           pszTemp = szFileName;
                           while (*pszFileName != '\"')
                              *(pszTemp++) = *(pszFileName++);
                           *pszTemp = 0x00;
                        }
                        else
                        {
                           pszTemp = szFileName;
                           while (*pszFileName != 0x0D)
                              *(pszTemp++) = *(pszFileName++);
                           *pszTemp = 0x00;
                        }
                        pszEncoding = SearchFor("Content-Transfer-Encoding: base64",pszNextPart, pszEndNextPart - pszNextPart, pszEncoding);
                        if (pszEncoding != NULL)
                        {
                           if (LoadBase64() == 0)
                           {

/*                              printf("Decoding BASE64 -");  */
                              fflush(NULL);
                              Base64Decode(pszNextPart, pszEndNextPart - pszNextPart, &ulFileLen);
                              UnLoadBase64();
/*                              printf(" Decoded\r\n");  */
                              fflush(NULL);

                              if (strlen(szFileName) > 0)
                              {

                                 APIRET rc;

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
                                    CHAR szNewFileName[256];

/*                                    printf("Saving filename %s\r\n",szFileName);  */
                                    sprintf(szNewFileName, "%s\\%s", pszNewsGroup, szFileName);
                                    rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    if (rc == 0)
                                    {
                                       DosWrite(hfOutFile,pszNextPart, ulFileLen,&BytesWritten);

                                       DosClose(hfOutFile);
                                    }
/*                                    rc = DosSetCurrentDir("\\");
                                    rc = DosSetCurrentDir(szCurrentDir);  */
                                    bPartFound = TRUE;
                                 }
/*                                 else
                                    printf("Error changing directories %s error %d\r\n", szNewDir, rc);  */
                              }
                              else
                              {

                                 bPartFound = FALSE;
/*                                 printf("No filename to save\r\n");  */
                              }
                           }
                        }
                     }
                  }
               }
               pszNextPart = pszEndNextPart;
            }
         }
         return(bPartFound);
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

/*   DosFreeModule(hmBase64);  */
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
         DosDelete(pszFileName); */
   }
   return(UURet);
}


