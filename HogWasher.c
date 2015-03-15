#define EXTERN

#include "HogWasher.h"

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
   CHAR szFileName[256];
   PSZ pszFileName;
   PSZ pszSubjectBegin;
   PSZ pszSubjectEnd;
   static CHAR szGroup[256];

   pszUserAgent = SearchFor("User-Agent: Hogwasher/2.6.1",pszHeader, ulHeaderLen, pszUserAgent);
   if (pszUserAgent == NULL)
      pszUserAgent = SearchFor("User-Agent: Hogwasher/3.0b7 (Carbon/OSX)",pszHeader, ulHeaderLen, pszUserAgent);
   if (pszUserAgent == NULL)
      pszUserAgent = SearchFor("User-Agent: Hogwasher/3.0 (Carbon/OSX)",pszHeader, ulHeaderLen, pszUserAgent);
   if (pszUserAgent != NULL)
   {
      PSZ pszContent;

      pszContent = SearchFor("Content-Type: message/partial;",pszHeader, ulHeaderLen, pszContent);
      if (pszContent != NULL)
      {
         ULONG ulNumber, ulTotal;
         PSZ pszContentEnd;

         pszContentEnd = pszContent;

         while ((*pszContentEnd != 0x0D) && (pszContentEnd < pszHeader + ulHeaderLen))
            pszContentEnd++;
         if (*pszContentEnd == 0x0D)
         {
            CHAR szTemp[20];
            PSZ pszTemp;
            PSZ pszNumber;
            PSZ pszTotal;

            pszNumber = SearchFor("number=",pszContent, pszContentEnd - pszContent, pszNumber);
            if (pszNumber != NULL)
            {
               pszNumber += 7;
               pszTemp = szTemp;

               while ((*pszNumber != ';') &&( pszNumber < pszContentEnd))
                  *(pszTemp++) = *(pszNumber++);

               if (*pszNumber == ';')
               {

                  *pszTemp = 0x00;

                  ulNumber = atol(szTemp);
                  pszTotal = SearchFor("total=",pszNumber, pszContentEnd - pszNumber, pszTotal);
                  if (pszTotal != NULL)
                  {

                     pszTotal += 6;
                     pszTemp = szTemp;

                     while ((*pszTotal != ';') &&( pszTotal < pszContentEnd))
                        *(pszTemp++) = *(pszTotal++);

                     if (*pszTotal == ';')
                     {

                        *pszTemp = 0x00;

                        ulTotal = atol(szTemp);

                        pszSubjectBegin = SearchFor("Subject:",pszHeader, ulHeaderLen, pszSubjectBegin);
                        if (pszSubjectBegin != NULL)
                        {

                           pszSubjectEnd = pszSubjectBegin;

                           while ((*pszSubjectEnd != 0x0D) && (pszSubjectEnd < pszHeader + ulHeaderLen))
                              pszSubjectEnd++;

                           if (*pszSubjectEnd == 0x0D)
                           {
                              PSZ pszFileBegin;

                              pszFileBegin = pszSubjectEnd;

                              while ((*pszFileBegin != ' ') && (pszFileBegin > pszSubjectBegin))
                                 pszFileBegin--;

                              if (*pszFileBegin == ' ')
                              {
                                 pszFileBegin--;

                                 while ((*pszFileBegin != ' ') && (pszFileBegin > pszSubjectBegin))
                                    pszFileBegin--;

                                 if (*pszFileBegin == ' ')
                                 {
                                    PSZ pszFileEnd;

                                    pszFileEnd = pszFileBegin;
                                    pszFileBegin--;

                                    while ((*pszFileBegin != ' ') && (pszFileBegin > pszSubjectBegin) &&
                                       (*pszFileBegin != '\\') & (*pszFileBegin != ':'))
                                       pszFileBegin--;

                                    if (pszFileBegin != pszSubjectBegin)
                                    {
                                       CHAR szNewFileName[256];
                                       APIRET rc;


                                       pszFileBegin++;
                                       strncpy(szFileName, pszFileBegin, pszFileEnd - pszFileBegin);

                                       szFileName[pszFileEnd - pszFileBegin] = 0x00;


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

/*                                          printf("Saving Hogwasher filename %s, part %d of %d\r\n",szFileName, ulNumber,ulTotal);  */
                                          sprintf(szNewFileName,"%s\\%s.HW%d",pszNewsGroup, szFileName, ulNumber);
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
                                                pszDataEnd = SearchFor("\r\n\r\n",pszDataBegin, ulBodyLen - (pszDataBegin - pszBody), pszDataEnd);

                                                if (pszDataEnd != NULL)
                                                   DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);
                                             }

                                             DosClose(hfOutFile);

                                          }
/*                                          rc = DosSetCurrentDir("\\");
                                          rc = DosSetCurrentDir(szCurrentDir);  */
                                          if (bLogs)
                                             SaveLog("Hogwasher.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
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
         }
      }
      else
      {
         pszContent = SearchFor("Content-Type: text/plain;",pszHeader, ulHeaderLen, pszContent);
         if (pszContent != NULL)
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
            break;
      }
      DosClose(hfLog);
   }
   return(0);
}

