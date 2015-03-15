#define EXTERN

#include "UUCode.h"
#include "gynplugin.h"


HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bDecoderNotLoaded = TRUE;

#include "uudecodevars.h"

INT main(int argc, char *argv[])
{
   QMSG qmsg;
   PID  pid;
   TID  tid;

   argc = argc;
   argv = argv;

   return(0);
}



extern int _dllentry;

BOOL CheckForGeneric(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs)
{
   int x = _dllentry;
   PSZ pszUUDecodeBegin;
   PSZ pszUUDecodeEnd;
   BOOL bFileSaved = FALSE;



   pszUUDecodeBegin = SearchFor("\r\nbegin ",pszBody, ulBodyLen, pszUUDecodeBegin);
   if (pszUUDecodeBegin == NULL)
      pszUUDecodeBegin = SearchFor("\nbegin ",pszBody, ulBodyLen, pszUUDecodeBegin);
   while (pszUUDecodeBegin != NULL)
   {

      pszUUDecodeBegin += 2;
      pszUUDecodeEnd = SearchFor("end\r\n",pszUUDecodeBegin,
         ulBodyLen - (pszUUDecodeBegin - pszBody), pszUUDecodeEnd);
      if (pszUUDecodeEnd == NULL)
         pszUUDecodeEnd = SearchFor("\r\n.",pszUUDecodeBegin,
            ulBodyLen - (pszUUDecodeBegin - pszBody), pszUUDecodeEnd);
      if (strncmp((pszUUDecodeBegin - 1),"\n", 1) != 0)
      {
         pszUUDecodeBegin = pszUUDecodeEnd;
         pszUUDecodeEnd = NULL;
      }

      if (pszUUDecodeEnd != NULL)
      {
         PSZ pszFileNameBegin;
         PSZ pszFileNameEnd;
         CHAR szFileName[128];
         ULONG ulDirSize;
         APIRET rc;

         /* bFileSaved = TRUE; */
         pszUUDecodeEnd += 5;

         pszFileNameEnd = pszUUDecodeBegin;
         while ((*pszFileNameEnd != 0x0D) && (*pszFileNameEnd != 0x0A))
            pszFileNameEnd++;

         pszFileNameBegin = pszFileNameEnd;
         pszFileNameBegin = SearchFor("\\",pszUUDecodeBegin + 9, pszFileNameEnd - (pszUUDecodeBegin + 9), pszFileNameBegin);
         if (pszFileNameBegin != NULL)
         {
            PSZ pszTemp;

            while (pszFileNameBegin != NULL)
            {
               pszTemp = pszFileNameBegin;
               pszFileNameBegin = SearchFor("\\",pszFileNameBegin + 1, pszFileNameEnd - pszFileNameBegin + 1 , pszFileNameBegin);

            }
            pszFileNameBegin = pszTemp + 1;
         }
         else
         {
            pszFileNameBegin = SearchFor(":",pszUUDecodeBegin + 9, pszFileNameEnd - (pszUUDecodeBegin + 9) , pszFileNameBegin);
            if (pszFileNameBegin != NULL)
               pszFileNameBegin + 1;
            else
               pszFileNameBegin = pszUUDecodeBegin + 10;
         }


         if (pszFileNameEnd > pszFileNameBegin)
         {
            HFILE hfOutFile;
            ULONG action;
            ULONG BytesWritten;
            CHAR szMergeFileName[256];
            CHAR szFileID[256];
            CHAR szDLLType[256];
   
            strncpy(szFileName, pszFileNameBegin,(USHORT)( pszFileNameEnd - pszFileNameBegin));
            szFileName[pszFileNameEnd - pszFileNameBegin] = 0x00;

/*         Constructor();  */

/*         ulDirSize = sizeof(szCurrentDir);
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




            sprintf(szMergeFileName,"%s\\%s",pszNewsGroup, szFileName);
/*            printf("Saving UUCode %s ", szFileName);  */
            rc = DosOpen(szMergeFileName,&hfOutFile,&action,0L,FILE_NORMAL,
               OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
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
               pszTestData = SearchFor("\r\nbegin ",pszBody, ulBodyLen, pszUUDecodeBegin);
               if (pszTestData != NULL)
               {

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
                     UURet = UUDecode(pszTestData, ulBodyLen - (pszTestData - pszBody), szUUFileName, &ulFileSize);
                     if (UURet == 0)
                     {
                        DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
                        DosWrite(hfOutFile,pszTestData,ulFileSize, &BytesRead);
                        DosSetFileSize(hfOutFile, ulFileSize);
                        bFileSaved = TRUE;
                     }
                  }

                  DosFreeMem(pszTestData);
               }


               DosClose(hfOutFile);
               if (UURet != 0)
                  DosDelete(szMergeFileName);
            }
         }


         pszUUDecodeBegin = pszUUDecodeEnd;
         pszUUDecodeBegin = SearchFor("\r\nbegin",
                              pszUUDecodeBegin, ulBodyLen - (pszUUDecodeBegin - pszBody),
                              pszUUDecodeBegin);
/*         if (pszUUDecodeBegin == NULL)
         {

            pszUUDecodeBegin = pszUUDecodeEnd;
            pszUUDecodeBegin = SearchFor("begin 666",
                                 pszUUDecodeBegin, ulBodyLen - (pszUUDecodeBegin - pszBody),
                                 pszUUDecodeBegin);
            if (pszUUDecodeBegin == NULL)
            {

               pszUUDecodeBegin = pszUUDecodeEnd;
               pszUUDecodeBegin = SearchFor("begin 755",
                                    pszUUDecodeBegin, ulBodyLen - (pszUUDecodeBegin - pszBody),
                                    pszUUDecodeBegin);
               if (pszUUDecodeBegin == NULL)
               {

                  pszUUDecodeBegin = pszUUDecodeEnd;
                  pszUUDecodeBegin = SearchFor("begin 600",
                                       pszUUDecodeBegin, ulBodyLen - (pszUUDecodeBegin - pszBody),
                                       pszUUDecodeBegin);
               }
            }
         } */
         if (pszUUDecodeBegin != NULL)
         {
/*            pszUUDecodeBegin += 2;  */
/*            printf("Multiple pictures in body\r\n");  */
         }
      }
      else
      {
/*         if (bFileSaved != TRUE)
            printf("Bad UUCode Post\r\n");  */

         pszUUDecodeBegin = pszUUDecodeEnd;

      }


   }

   return(bFileSaved);
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