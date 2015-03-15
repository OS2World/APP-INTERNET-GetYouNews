#define EXTERN

#include "Ozum.h"

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
   PSZ pszOzum;
   ULONG ulNumFiles = 0;
   static CHAR szGroup[256];

   pszOzum = SearchFor("Posted by Ozum",pszBody, ulBodyLen, pszOzum);
   if (pszOzum != NULL)
   {
      PSZ pszLineBegin;
      PSZ pszLineEnd;
      PSZ pszDataBegin;
      PSZ pszDataEnd;

      pszOzum = SearchFor("\r\n",pszOzum, ulBodyLen - (pszOzum - pszBody), pszOzum);
      if (pszOzum != NULL)
      {
         pszOzum += 2;

         pszLineBegin = pszOzum;
         pszLineBegin = SearchFor("<",pszLineBegin, ulBodyLen - (pszLineBegin - pszBody), pszLineBegin);
         if (pszLineBegin != NULL)
         {

            pszDataBegin = SearchFor("\r\n\r\n",pszLineBegin, ulBodyLen - (pszLineBegin - pszBody), pszDataBegin);

            if (bLogs)
               SaveLog("Ozum.Log",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup));
            if (strcmp(pszNewsGroup, szGroup) != 0)
               strcpy(szGroup, pszNewsGroup);
            while (*pszLineBegin == '<')
            {

               pszLineEnd = SearchFor("\r\n",pszLineBegin, ulBodyLen - (pszLineBegin - pszBody), pszLineEnd);
               if (pszLineEnd != NULL)
               {
                  CHAR szFileName[256];
                  PSZ pszFileNameBegin;
                  PSZ pszFileNameEnd;

                  pszFileNameEnd = pszLineEnd-1;

                  pszFileNameBegin = pszFileNameEnd - 1;
                  if (*pszFileNameBegin == '\"')
                     pszFileNameBegin--;
                  while ((*pszFileNameBegin != '\\') && (*pszFileNameBegin != '\"') &&
                     (*pszFileNameBegin != ':') && (pszFileNameBegin > pszLineBegin))
                     pszFileNameBegin--;

                  pszFileNameBegin++;

                  strncpy(szFileName, pszFileNameBegin, pszFileNameEnd - pszFileNameBegin);
                  szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;

                  if (pszDataBegin != NULL)
                  {
                     ULONG ulDirSize;
                     CHAR szNewFileName[256];
                     APIRET rc;

                     pszDataBegin += 4;
                     pszDataEnd = SearchFor("\r\n\r\n",pszDataBegin, ulBodyLen - (pszDataBegin - pszBody), pszDataEnd);

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
                           CHAR szMergeFileName[256];

                           sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
/*                           printf("Saving Ozum filename %s\r\n",szFileName);  */
                           rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                              OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                           if (rc == 0)
                           {
                              DosWrite(hfOutFile,pszDataBegin, pszDataEnd - pszDataBegin, &BytesWritten);

                              DosClose(hfOutFile);

                           }
/*                           rc = DosSetCurrentDir("\\");
                           rc = DosSetCurrentDir(szCurrentDir);  */

                        }

                        pszDataBegin = pszDataEnd;
                     }
                  }
                  pszLineBegin = pszLineEnd + 2;
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