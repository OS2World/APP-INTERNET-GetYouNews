#define INCL_PM
#define INCL_DOSFILEMGR   /* File Manager values */
#define INCL_DOSERRORS    /* DOS error values */
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "uudecodevars.h"
#include "gynplugin.h"

int main(int argc, char *argv[])
{
   HDIR          hdirFindHandle = HDIR_SYSTEM;
   FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
   ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
   ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
   APIRET        rc             = NO_ERROR; /* Return code                  */
   ULONG          ulNumFiles = 0;
   CHAR szFindFile[256];
   ULONG       ulSavedSize = 0;

   HFILE hfOutFile;

   CHAR szMergeFileName[256];
   ULONG action;
   ULONG BytesWritten;

   printf("\r\n%s\r\n",argv[1]);

   rc = DosOpen(argv[1],&hfOutFile,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
      OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      USHORT x;

      ulNumFiles = atoi(argv[2]);
      for (x = 1; x <= ulNumFiles; x++)
      {
         CHAR szNewFileName[256];
         HFILE hfOldFile;

         sprintf(szNewFileName,"%s.Thoth%d",argv[1], x);
         printf("%s\r\n",szNewFileName);
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
      printf("Decoding...\r\n");
      DecodeUUFile(argv[1]);
   }
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
                     rc = DosQueryProcAddr(MMod,0,"UUDecode",&UUDecode);
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
}


