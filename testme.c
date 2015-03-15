
#define INCL_PM
#define INCL_DOSFILEMGR   /* File Manager values */
#define INCL_DOSERRORS    /* DOS error values */
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSMODULEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gynplugin.h"
#include "uudecodevars.h"


int main(int argc, char *argv[])
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

#ifdef NOTTHISONE

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
               {


                  rc = UUInitialize();

                  if (rc == 0)
                  {
                     HDIR          hdirFindHandle = HDIR_SYSTEM;
                     FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
                     ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
                     ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
                     APIRET        rc             = NO_ERROR; /* Return code                  */
                     ULONG          ulNumFiles = 0;
                     CHAR szFindFile[256];
                     ULONG       ulSavedSize = 0;

                     strcpy(szFindFile,"available-ZMO_GSCT_G03.mpg");

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


                           printf(FindBuffer.achName);
                           fflush(NULL);
                           UULoadFile(FindBuffer.achName,NULL,1);

                           if (UUDecodeFile(UUGetFileListItem(0), NULL) != 0)
                              printf("- Error decoding file\r\n");
                           else
                              printf("- decoded\r\n");
                           fflush(NULL);


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

                  }






                  return(0);
               }
               else
               {

                  UULoadFile = NULL;

                  return(4);
               }
            }
            else
               return(3);
         }
         else
            return(2);
      }
      else
         return(1);


#endif

   CHAR szFileName[256];
   CHAR szFileName1[256];
   HFILE hfOutFile;
   HFILE hfOldFile;
   ULONG BytesWritten;

   strcpy(szFileName,argv[1]);

   printf("\r\n%s\r\n",szFileName);
   fflush(NULL);
      rc = DosOpen(szFileName,&hfOutFile,&action,0L,FILE_NORMAL,
         OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
         OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
      if (rc == 0)
      {
         USHORT x;
         USHORT usMaxFiles;

         usMaxFiles = atoi(argv[2]);

         for (x = 1; x <= usMaxFiles; x++)
         {
            CHAR szNewFileName[256];
            CHAR szNumText[10];
            HFILE hfOldFile;

            sprintf(szNewFileName,"%s.000",szFileName);
            itoa(x,szNumText,10);
            strcpy(&szNewFileName[strlen(szNewFileName) - strlen(szNumText)],szNumText);
            printf("%s\r\n",szNewFileName);
            fflush(NULL);
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

      }

   printf("Done\r\n");
   return(0);
}



