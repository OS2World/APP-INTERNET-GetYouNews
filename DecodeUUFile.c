#include "DecodeUUFile.h"
#include "gynplugin.h"

int UUDecode(PSZ pszBody, ULONG ulBodyLen, PSZ pszFileName, ULONG *ulFileSize, PSZ *pszNewFilePos);
PSZ SearchFor(PSZ pszSearch, PSZ pszData, ULONG iDataLen, PSZ pszResult);

CHAR szNewFileName[1024];
CHAR szReadLine[1024];
CHAR szDecodedLine[1024];
CHAR szFileName[512];

int main(int argc, char *argv[])
{
   HFILE hfOutFile;
   ULONG action;
   APIRET rc;
   int UURet = 0;

   if (argc > 1)
   {

      strcpy(szFileName, argv[1]);
      action = 0;
      rc = DosOpen(szFileName,&hfOutFile,&action,0L,FILE_NORMAL,
         OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
         OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,0L);
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
         ULONG BytesWritten;
         PSZ pszNewFilePos;

         DosSetFilePtr(hfOutFile, 0L, FILE_END, &NewFilePos);
         rc = DosAllocMem((VOID *)&pszTestData,NewFilePos + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
         if (rc == 0)
         {
            HFILE hfNewOutFile;

            DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);
            DosRead(hfOutFile,pszTestData,NewFilePos, &BytesRead);

            UURet = UUDecode(pszTestData, NewFilePos, szUUFileName, &ulFileSize, &pszNewFilePos);
            while (UURet == 0)
            {
               rc = DosOpen(szUUFileName,&hfNewOutFile,&action,0L,FILE_NORMAL,
                  OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
               if (rc == 0)
               {

/*                DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFileBeginPos);  */
                  printf("Saving file %s\r\n",szUUFileName);
                  fflush(NULL);
                  DosWrite(hfNewOutFile,pszTestData,ulFileSize, &BytesRead);
/*                DosSetFileSize(hfOutFile, ulFileSize);  */
                  DosClose(hfNewOutFile);
               }
               NewFilePos -= pszNewFilePos - pszTestData;
               pszTestData = pszNewFilePos;
               UURet = UUDecode(pszTestData, NewFilePos, szUUFileName, &ulFileSize, &pszNewFilePos);
            }
            DosFreeMem(pszTestData);
         }
         else
         {
            switch (rc)
            {
               case 8:
               {
                  ULONG NewFilePos1;
                  ULONG ulTotalRead = 0;
                  PSZ pszCopyPtr;
                  PSZ pszCopyPtr1;
                  HFILE hfBinaryFile;
                  BOOL bFoundBegin;

                  strcpy(szNewFileName,"");
                  DosSetFilePtr(hfOutFile, 0L, FILE_BEGIN, &NewFilePos1);
                  DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                  ulTotalRead += BytesRead;
                  pszCopyPtr = szReadLine;
                  while (ulTotalRead < NewFilePos)
                  {
                     while ((*pszCopyPtr == 0x0D) || (*pszCopyPtr == 0x0A))
                        pszCopyPtr++;
                     while ((strlen(szNewFileName) == 0) && (ulTotalRead < NewFilePos))
                     {
                        if (strnicmp(pszCopyPtr,"BEGIN", 5) == 0)
                        {
                           PSZ pszFileName;

                           pszFileName = szNewFileName;
                           pszCopyPtr += 10;
                           while ((*pszCopyPtr != 0x0D) && (*pszCopyPtr != 0x0A))
                           {
                              if (pszCopyPtr < szReadLine + BytesRead)
                                *(pszFileName++) = *(pszCopyPtr++);
                              else
                              {
                                 DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                                 pszCopyPtr = szReadLine;
                                 ulTotalRead += BytesRead;
                              }
                           }
                           *pszFileName = 0x00;

                           while ((*pszCopyPtr == 0x0D) || (*pszCopyPtr == 0x0A))
                              pszCopyPtr++;
                        }
                        else
                        {
                           while ((*pszCopyPtr != 0x0D) && (*pszCopyPtr != 0x0A) && (ulTotalRead < NewFilePos))
                           {
                              if ((pszCopyPtr < szReadLine + sizeof(szReadLine)))
                                 pszCopyPtr++;
                              else
                              {
                                 DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                                 pszCopyPtr = szReadLine;
                                 ulTotalRead += BytesRead;
                              }
                           }
                           while ((*pszCopyPtr == 0x0D) || (*pszCopyPtr == 0x0A))
                              pszCopyPtr++;
                        }
                     }
                     if (strlen(szNewFileName) > 0)
                     {

                        printf("%s\r\n",szNewFileName);
                        rc = DosOpen(szNewFileName,&hfBinaryFile,&action,0L,FILE_NORMAL,
                           OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                           OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                        pszCopyPtr1 = szDecodedLine;
                        while ((strnicmp(pszCopyPtr,"END\r\n", 5) != 0) && (ulTotalRead  <= NewFilePos))
                        {
                           CHAR szBinaryData[1024];
                           ULONG ulBinaryDataLen;

                           while ((*pszCopyPtr != 0x0D) && (*pszCopyPtr != 0x0A) &&
                                 (ulTotalRead  <= NewFilePos))
                           {
                              if (pszCopyPtr < szReadLine + BytesRead)
                                 *(pszCopyPtr1++) = *(pszCopyPtr++);
                              else
                              {
                                 DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                                 if (BytesRead == 0)
                                    ulTotalRead++;
                                 else
                                    ulTotalRead += BytesRead;
                                 pszCopyPtr = szReadLine;
                              }
                           }
                           *pszCopyPtr1 = 0x00;

                           if (strnicmp(szDecodedLine,"END\r\n", 5) != 0)
                           {
                              rc = UUDecodeLine(szDecodedLine, (ULONG)pszCopyPtr1 - (ULONG)szDecodedLine,
                                    szBinaryData, &ulBinaryDataLen);
                              if (rc == 0)
                                 DosWrite(hfBinaryFile,szBinaryData,ulBinaryDataLen, &BytesWritten);
                           }
                           pszCopyPtr1 = szDecodedLine;
                           while ((*pszCopyPtr == 0x0D) || (*pszCopyPtr == 0x0A))
                           {
                              if (pszCopyPtr < szReadLine + BytesRead)
                              {
                                 pszCopyPtr++;
                              }
                              else
                              {
                                 DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                                 pszCopyPtr = szReadLine;
                                 ulTotalRead += BytesRead;
                              }
                           }
/*                           printf("%d\r",ulTotalRead);
                           fflush(NULL);  */
                        }
                        DosClose(hfBinaryFile);
                        while ((*pszCopyPtr != 0x0D) && (*pszCopyPtr != 0x0A))
                        {
                           if (pszCopyPtr < szReadLine + BytesRead)
                           {
                              pszCopyPtr++;
                           }
                           else
                           {
                              DosRead(hfOutFile,szReadLine,1024L, &BytesRead);
                              pszCopyPtr = szReadLine;
                              ulTotalRead += BytesRead;
                           }
                        }
                        strcpy(szNewFileName,"");
                     }
                  }
               }
               break;

               default:
                  printf("Cannot alloc memory. Error %d, size %ld\r\n", rc, NewFilePos);
                  break;
            }

         }
         DosClose(hfOutFile);
   /*      if (UURet != 0)
            DosDelete(pszFileName); */
      }
      return(UURet);
   }
   return(UURet);
}

int UUDecode(PSZ pszBody, ULONG ulBodyLen, PSZ pszFileName, ULONG *ulFileSize, PSZ *pszNewFilePos)
{
   PSZ pszUUDecodeBegin;
   USHORT usFileLen;
   PSZ pszBegin;
   USHORT NoError = 0;
   PSZ pszTemp;
   CHAR szTemp[256];
   USHORT usMode;
   BOOL bNotEndOfDecode = TRUE;

   pszBegin = pszBody;
   *ulFileSize = 0;

   pszUUDecodeBegin = SearchFor("begin ",pszBody, ulBodyLen, pszUUDecodeBegin);
   if (pszUUDecodeBegin == NULL)
      pszUUDecodeBegin = SearchFor("BEGIN ",pszBody, ulBodyLen, pszUUDecodeBegin);
   if (pszUUDecodeBegin != NULL)
   {
      pszUUDecodeBegin += 6;

      pszTemp = szTemp;
      while ((*pszUUDecodeBegin != ' ') && (pszTemp < (szTemp + 4)))
         *(pszTemp++) = *(pszUUDecodeBegin++);
      if (*pszUUDecodeBegin == ' ')
      {

         *pszTemp = 0x00;

         usMode = atol(szTemp);

         pszUUDecodeBegin++;

         usFileLen = 0;
         while ((*pszUUDecodeBegin != '\r') && (*pszUUDecodeBegin != '\n') && (usFileLen++ < 256))
            *(pszFileName++) = *(pszUUDecodeBegin++);

         *pszFileName = 0x00;
         if (usFileLen < 256)
         {
            int n;
            char ch, *p;
            int mode, n1;
            int iLPlace;
            PSZ pszEndLine;
            CHAR szCRLF[3];


            szCRLF[0] = 0x0D;
            szCRLF[1] = 0x0A;
            szCRLF[2] = 0x00;

            if (*pszUUDecodeBegin == '\r')
               pszUUDecodeBegin += 2;
            else
               pszUUDecodeBegin += 1;


            do
            {
               if (pszUUDecodeBegin < pszBody + ulBodyLen)
               {

                  n = DEC(*pszUUDecodeBegin);
                  if (n > 0)
                  {

                     for (++pszUUDecodeBegin; n > 0/* , pszUUDecodeBegin <pszEndLine */; pszUUDecodeBegin += 4, n -= 3)
                     {
                        if (n >= 3)
                        {
                           *(pszBegin++) = DEC(pszUUDecodeBegin[0]) << 2 | DEC(pszUUDecodeBegin[1]) >> 4;
                           *(pszBegin++) = DEC(pszUUDecodeBegin[1]) << 4 | DEC(pszUUDecodeBegin[2]) >> 2;
                           *(pszBegin++) = DEC(pszUUDecodeBegin[2]) << 6 | DEC(pszUUDecodeBegin[3]);
                           *ulFileSize += 3;
                        }
                        else
                        {

                           if (n >= 1)
                           {
                              *(pszBegin++) = DEC(pszUUDecodeBegin[0]) << 2 | DEC(pszUUDecodeBegin[1]) >> 4;
                              *ulFileSize += 1;
                           }
                           if (n >= 2)
                           {
                              *(pszBegin++) = DEC(pszUUDecodeBegin[1]) << 4 | DEC(pszUUDecodeBegin[2]) >> 2;
                              *ulFileSize += 1;
                           }
                           if (n >= 3)
                           {
                              *(pszBegin++) = DEC(pszUUDecodeBegin[2]) << 6 | DEC(pszUUDecodeBegin[3]);
                              *ulFileSize += 1;
                           }
                        }
                     }
                     if ((*pszUUDecodeBegin == 0x0D) || (*pszUUDecodeBegin == 0x0A))
                     {
                        while (((*pszUUDecodeBegin == '\r') || (*pszUUDecodeBegin == '\n')) && (pszUUDecodeBegin < pszBody + ulBodyLen))
                           pszUUDecodeBegin ++;
                     }
                     else
                     {
                        NoError = 1;

                        printf("Missing Carrige return\r\n");
                     }
                  }
                  else
                     bNotEndOfDecode = FALSE;
               }
               else
                  NoError = 2;

            }  while ((pszUUDecodeBegin < pszBody + ulBodyLen) && (NoError == 0) && (bNotEndOfDecode));

            *pszNewFilePos = pszUUDecodeBegin;
         }
         else
            NoError = 5;
      }
      else
         NoError = 4;
   }
   else
      NoError = 3;
   return(NoError);
}

int UUDecodeLine(PSZ pszFileLine, ULONG ulLineLength, PSZ pszDecodedLine, ULONG *ulLineSize)
{
   PSZ pszUUDecodeBegin;
   USHORT usFileLen;
   PSZ pszBegin;
   USHORT NoError = 0;
   PSZ pszTemp;
   CHAR szTemp[256];
   USHORT usMode;
   BOOL bNotEndOfDecode = TRUE;

         if (ulLineLength < 256)
         {
            int n;
            char ch, *p;
            int mode, n1;
            int iLPlace;
            PSZ pszEndLine;

            pszEndLine = pszFileLine;
            pszBegin = pszDecodedLine;
            *ulLineSize = 0;

            do
            {
               if (pszEndLine < pszFileLine + ulLineLength)
               {

                  n = DEC(*pszEndLine);
                  if (n > 0)
                  {

                     for (++pszEndLine; n > 0; pszEndLine += 4, n -= 3)
                     {
                        if (n >= 3)
                        {
                           *(pszBegin++) = DEC(pszEndLine[0]) << 2 | DEC(pszEndLine[1]) >> 4;
                           *(pszBegin++) = DEC(pszEndLine[1]) << 4 | DEC(pszEndLine[2]) >> 2;
                           *(pszBegin++) = DEC(pszEndLine[2]) << 6 | DEC(pszEndLine[3]);
                           *ulLineSize += 3;
                        }
                        else
                        {

                           if (n >= 1)
                           {
                              *(pszBegin++) = DEC(pszEndLine[0]) << 2 | DEC(pszEndLine[1]) >> 4;
                              *ulLineSize += 1;
                           }
                           if (n >= 2)
                           {
                              *(pszBegin++) = DEC(pszEndLine[1]) << 4 | DEC(pszEndLine[2]) >> 2;
                              *ulLineSize += 1;
                           }
                           if (n >= 3)
                           {
                              *(pszBegin++) = DEC(pszEndLine[2]) << 6 | DEC(pszEndLine[3]);
                              *ulLineSize += 1;
                           }
                        }
                     }
                  }
                  else
                     bNotEndOfDecode = FALSE;
               }
               else
                  NoError = 2;

            }  while ((pszEndLine < pszFileLine + ulLineLength) && (NoError == 0) && (bNotEndOfDecode));
         }
         else
            NoError = 5;

   return(NoError);
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


