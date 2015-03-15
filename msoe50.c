#define EXTERN

#include "MSOE50.h"

#include "uudecodevars.h"
#include "gynplugin.h"

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bDecoderNotLoaded = TRUE;
HMODULE hmContent;
BOOL (*CheckForContentType)(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup);


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
   PSZ pszOLE;
   BOOL bTextMsgFound = FALSE;
   BOOL bMsgFound = FALSE;
   static CHAR szGroup[256];
   int x;

   x = _dllentry;




   pszOLE = SearchFor("X-MIMEOLE: Produced By Microsoft MimeOLE V5.00",pszHeader, ulHeaderLen, pszOLE);
   if (pszOLE == NULL)
      pszOLE = SearchFor("X-MimeOLE: Produced By Microsoft MimeOLE V5.00",pszHeader, ulHeaderLen, pszOLE);
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
                           CHAR szFileName[256];
                           PSZ pszFileName;
                           CHAR szEncoding[50];
                           PSZ pszEncoding;
                           PSZ pszContentEncoding;
                           PSZ pszFileBegin;
                           PSZ pszFileEnd;
                           PSZ pszDataBegin;

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
/*                                    printf("Saving Microsoft OE 5.0 %s, Encoding %s\r\n",szFileName, szEncoding);  */
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
                                       SaveLog("OutlookExpress50.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
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

         HFILE hfMSOE;
         ULONG action;
         APIRET rc;
         ULONG ulFileNew;
         ULONG BytesWritten;

/*         printf("Possible Text message, Microsoft OE 5.0 - saving for later\r\n");  */
         rc = DosOpen("MSOE50.OUT",&hfMSOE,&action,0L,FILE_NORMAL,
            OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
         if (rc == 0)
         {
            switch (action)
            {
               case FILE_EXISTED:
                  DosSetFilePtr(hfMSOE,0,FILE_END,&ulFileNew);

               case FILE_CREATED:
                  DosWrite(hfMSOE,pszHeader,ulHeaderLen,&BytesWritten);
                  DosWrite(hfMSOE,pszBody,ulBodyLen,&BytesWritten);
                  DosClose(hfMSOE);
                  break;
            }
         }
         return(0);
         pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
         if (pszSubjectBegin != NULL)
         {
            pszSubjectEnd = SearchFor("\r\n",pszSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszSubjectEnd);
            if (pszSubjectEnd != NULL)
            {
               PSZ pszPart;

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
                        ulTotal = atoi(szTemp);


                        pszPart = SearchFor("[",pszSubjectBegin, (pszSubjectEnd - pszSubjectBegin), pszPart);
                        if (pszPart != NULL)
                        {
                           PSZ pszFileEnd;

                           pszPart--;

                           pszFileEnd = pszPart;
                           pszPart--;
                           while ((*pszPart != ' ') && (*pszPart != '\\') && (*pszPart != ':')
                              && (pszPart > pszSubjectBegin))
                              pszPart--;

                           if (pszPart > pszSubjectBegin)
                           {

                              PSZ pszDataBegin;
                              PSZ pszDataEnd;

                              pszPart++;
                              strncpy(szFileName, pszPart, pszFileEnd - pszPart);
                              szFileName[pszFileEnd - pszPart] = 0x00;
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
/*                                    printf("Saving Microsoft OE 5.0 %s, part %d of %d\r\n",szFileName, usPart, ulTotal);  */
                                    sprintf(szNewFileName,"%s\\%s.M$50-%d", pszNewsGroup,szFileName, usPart);
                                    rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    if (rc == 0)
                                    {
                                       DosWrite(hfOutFile,pszDataBegin,
                                          pszDataEnd - pszDataBegin ,&BytesWritten);

                                       DosClose(hfOutFile);
                                    }
/*                                    rc = DosSetCurrentDir("\\");
                                    rc = DosSetCurrentDir(szCurrentDir);  */
                                    if (bLogs)
                                       SaveLog("OutlookExpress50.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
                                    if (strcmp(pszNewsGroup, szGroup) != 0)
                                       strcpy(szGroup, pszNewsGroup);

                                 }

                                 return(1);
                              }
                           }
                        }
                     }
                  }
               }
            }
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
            DosClose(hfLog);
            break;
      }
   }
   return(0);
}






