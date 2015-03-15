#define EXTERN

#include "yEnc.h"
#include "gynplugin.h"

int crc_tab[256] =

{
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
   0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
   0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
   0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
   0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
   0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
   0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
   0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
   0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
   0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
   0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
   0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
   0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
   0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
   0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
   0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
   0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
   0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
   0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
   0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
   0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
   0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
}
;

int crc_val;
long crc_anz;

void CrcInit(void);

HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */
HWND hWndFrame;   /* Handle to the Window Frame                     */
HWND hWndClient;  /* Handle to the Client Window                    */
PQMSG  pQmsg;
BOOL bDecoderNotLoaded = TRUE;


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



void CrcAdd(int c)
   {
   unsigned long ch1,ch2,cc;


     /* X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 */
     /* for (i = 0; i < size; i++) */
     /*      crccode = crc32Tab[(int) ((crccode) ^ (buf[i])) & 0xff] ^  */
     /*                (((crccode) >> 8) & 0x00FFFFFFL); */
     /*   return(crccode); */


    cc= (c) & 0x000000ffL;
    ch1=(crc_val ^ cc) & 0xffL;
    ch1=crc_tab[ch1];
    ch2=(crc_val>>8L) & 0xffffffL;  // Correct version
    crc_val=ch1 ^ ch2;
    crc_anz++;
   }


void CrcInit()
{
   crc_val = -1L ;
   crc_anz=0L;
}


int yDecode(PSZ pszData, ULONG ulDataLen, PSZ pszFileName, ULONG *ulSize, ULONG *ulStartPos, ULONG *ulEndPos, ULONG *ulPart, ULONG *ulTotal, PSZ *pszNewData, ULONG *ulDecoLen, BOOL byEncFix)
{

   unsigned char srcbuf[4100];
   unsigned char * srcp;
   unsigned char desbuf[4100];
   unsigned char * desp;
   int deslen;
   long decolen;
   long flen;
   USHORT srclen;
   ULONG yLine, ySize, yPart, yBegin, yEnd;
   PSZ pszPartBegin;
   PSZ pszNameBegin;
   PSZ pszSizeBegin;
   PSZ pszLineBegin;
   PSZ pszTotalBegin;
   PSZ yPartBuf;
   PSZ yDecLine;
   PSZ pszBeginData;
   PSZ pszCRLF;
   ULONG yPartSize;
   ULONG yPartFree;
   PSZ pszDataPtr;
   unsigned char c;
   CHAR szTemp[100];
   PSZ pszTemp;
   APIRET rc;
   PSZ yPartPtr;
   BOOL EndOfFile = FALSE;
   BOOL bEndOfLine = FALSE;
   USHORT usLineCount;
   HFILE hfOutFile;
   ULONG action;
   ULONG BytesWritten;

/*   rc = DosOpen("MyTemp.Yenc",&hfOutFile,&action,0L,FILE_NORMAL,
      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
      OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
   if (rc == 0)
   {
      DosWrite(hfOutFile,pszData, ulDataLen,&BytesWritten);

      DosClose(hfOutFile);
   }  */



   pszCRLF = pszData;
   pszDataPtr = pszData;
   while (((*pszCRLF != 0x0D) && (*pszCRLF != 0x0A)) && (pszCRLF < pszData + *ulSize))
      pszCRLF++;

   pszPartBegin = SearchFor("part=",pszDataPtr, pszCRLF - pszDataPtr, pszPartBegin);
   if (pszPartBegin != NULL)
   {

      pszTemp = szTemp;
      pszPartBegin += 5;

      while (*pszPartBegin != ' ')
         *(pszTemp++) = *(pszPartBegin++);
      *pszTemp = 0x00;

      yPart = atol(szTemp);
      *ulPart = yPart;
   }
   else
   {
      yPart = 0;
      *ulPart = 0;
   }

      pszNameBegin = SearchFor("name=",pszDataPtr, pszCRLF - pszDataPtr, pszNameBegin);
      if (pszNameBegin != NULL)
      {
         PSZ pszFilePtr;

         pszNameBegin += 5;
         pszFilePtr = pszFileName;
         while (((*pszNameBegin != 0x0D) && (*pszNameBegin != 0x0A)) && (pszFilePtr < pszFileName + 255))
            *(pszFilePtr++) = *(pszNameBegin++);
         *pszFilePtr = 0x00;

         pszSizeBegin = SearchFor("size=",pszDataPtr, pszCRLF - pszDataPtr, pszSizeBegin);
         if (pszSizeBegin != NULL)
         {
            CHAR szTemp[100];
            PSZ pszTemp;

            pszTemp = szTemp;
            pszSizeBegin += 5;

            while ((*pszSizeBegin != ' ') && (isdigit(*pszSizeBegin)))
               *(pszTemp++) = *(pszSizeBegin++);
            *pszTemp = 0x00;

            ySize = atol(szTemp);
            *ulSize = ySize;


            pszTotalBegin = SearchFor("total=",pszDataPtr, pszCRLF - pszDataPtr, pszTotalBegin);
            if (pszTotalBegin != NULL)
            {

               pszTemp = szTemp;
               pszTotalBegin += 6;

               while (*pszTotalBegin != ' ')
                  *(pszTemp++) = *(pszTotalBegin++);
               *pszTemp = 0x00;

               *ulTotal = atol(szTemp);
            }
            else
               *ulTotal = 0;

            pszLineBegin = SearchFor("line=",pszDataPtr, pszCRLF - pszDataPtr, pszLineBegin);
            if (pszLineBegin != NULL)
            {
               pszTemp = szTemp;
               pszLineBegin += 5;

               while (*pszLineBegin != ' ')
                  *(pszTemp++) = *(pszLineBegin++);
               *pszTemp = 0x00;

               yLine = atol(szTemp);

               if (yPart > 0)
               {
                  PSZ pszYPartBegin;
                  PSZ pszYBeginBegin;
                  PSZ pszYEndBegin;

                  pszDataPtr = pszCRLF + 2;
                  pszCRLF += 2;
                  while ((*pszCRLF != 0x0D) && (*pszCRLF != 0x0A))
                     pszCRLF++;

                  pszYPartBegin = SearchFor("=ypart",pszDataPtr, pszCRLF - pszDataPtr, pszYPartBegin);
                  if (pszYPartBegin != NULL)
                  {
                     pszYBeginBegin = SearchFor("begin=",pszDataPtr, pszCRLF - pszDataPtr, pszYBeginBegin);
                     if (pszYBeginBegin != NULL)
                     {

                        pszTemp = szTemp;
                        pszYBeginBegin += 6;

                        while (*pszYBeginBegin != ' ')
                           *(pszTemp++) = *(pszYBeginBegin++);
                        *pszTemp = 0x00;

                        yBegin = atol(szTemp);

                        *ulStartPos = yBegin;

                        pszYEndBegin = SearchFor("end=",pszDataPtr, pszCRLF - pszDataPtr, pszYEndBegin);
                        if (pszYEndBegin != NULL)
                        {
                           pszTemp = szTemp;
                           pszYEndBegin += 4;

                           while ((*pszYEndBegin != 0x0D) && (*pszYEndBegin != 0x0A))
                              *(pszTemp++) = *(pszYEndBegin++);
                           *pszTemp = 0x00;

                           yEnd = atol(szTemp);
                           *ulEndPos = yEnd;

                           rc = DosAllocMem((VOID *)pszNewData,yEnd - yBegin + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                           if (rc == 0)
                           {
                              yPartSize = yEnd - yBegin + 100;
                              yPartFree = yPartSize;
                           }
                           else
                              return(1);
                        }
                        else
                           return(7);
                     }
                     else
                        return(6);
                  }
                  else
                     return(5); /* or some number error */
               }
               else
               {
                  ULONG ulTotMem;
                  ULONG retrys = 0;

                  do
                  {

                     rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                                          (PVOID)&ulTotMem,
                                          sizeof(ulTotMem));
                     retrys++;
                     if (ulTotMem < ySize + 100)
                        DosSleep(10000);
                     else
                     {
                        rc = DosAllocMem((VOID *)pszNewData,ySize + 100 ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                        if (rc == 0)
                           retrys = 8;
                        else
                           retrys = 9;
                     }
                  } while((ulTotMem < ySize + 100) && (retrys < 7));


                  if (retrys != 8)
                     return(1);
                  yPartFree = ySize + 100;

                  *ulStartPos = 1;
                  *ulEndPos = ySize;
               }

               yPartPtr = *pszNewData;

               pszDataPtr = pszCRLF + 2;

               CrcInit();  // Analyse only CRC per part

               decolen=0;
               deslen=0;
               desp=desbuf;

               if (yLine > sizeof(srcbuf))
                     EndOfFile = TRUE;

               while ((pszDataPtr < pszData + ulDataLen) && (!EndOfFile))
               {
                  pszBeginData = srcbuf;

                  usLineCount = 0;
                  while (((*pszDataPtr != 0x0D) && (*(pszDataPtr + 1) != 0x0A) && (pszDataPtr < pszData + ulDataLen)) && (usLineCount <= yLine))
                  {

                     if (strncmp(pszDataPtr, "..", 2) == 0)
                     {
                        if (byEncFix == TRUE)
                           usLineCount--;
                     }
                     *(pszBeginData++) = *(pszDataPtr++);
                     usLineCount++;
                  }

                  if (*pszDataPtr == 0x0D)
                  {

                     *pszBeginData = 0x00;
                     pszDataPtr += 1;
                     if (*pszDataPtr == 0x0A)
                        pszDataPtr += 1;


                     if (strncmp(srcbuf, "=ybegin ", 8) == 0)
                        return(8);

                     if (strncmp(srcbuf, "=yend ", 6) == 0)
                     {

                        EndOfFile = TRUE;
/*                        pszDataPtr = pszData + ulDataLen; */
                        if (deslen > 0)
                        {
                           if (deslen > yPartFree)
                              return(10);

                           memcpy(yPartPtr, desbuf, deslen);

                        }
                     }
                     else
                     {

/*                        srclen = strlen(srcbuf);  */

                        srcp = srcbuf;
                        if (strncmp(srcbuf, "..", 2) == 0)
                        {
                           if (byEncFix == TRUE)
                              srcp = &srcbuf[1];
                        }

                        bEndOfLine = FALSE;
                        do
                        {
                           c = *(srcp++);
                           if (c != 0)
                           {
                              if (c == '=')
                              {
                                 c = *(srcp++);
                                 if (c == 0)
                                    return(9);
                                 c = (unsigned char)(c-64);
                              }

                              c = (unsigned char)(c-42);
                              *(desp++) = c;
                              deslen++;
                              decolen++;

                              CrcAdd(c);

                              if (deslen >= 4096)
                              {
                                    if (deslen > yPartFree)
                                       return(10);

                                    memcpy(yPartPtr, desbuf, deslen);
                                    yPartPtr=yPartPtr+deslen;
                                    yPartFree = yPartFree - deslen;

                                    deslen=0;
                                    desp=desbuf;
                              }

                           } /* end c != 0 */
                           else
                              bEndOfLine = TRUE;

                        } while (!bEndOfLine);
                     }
                  } /* end if of data in one line */
                  else
                     return(11);
               } /* end While for next line of data */

               if (yPart > 0)
               {
                  PSZ pszEndSize;

                  pszEndSize = SearchFor("size=",srcbuf, strlen(srcbuf), pszEndSize);
                  if (pszEndSize != NULL)
                  {
                     CHAR szSize[10];
                     PSZ pszSize;

                     pszSize = szSize;
                     pszEndSize +=5;

                     while (*pszEndSize != ' ')
                        *(pszSize++) = *(pszEndSize++);
                     *pszSize = 0x00;

                     if (decolen < atol(szSize))
                     {
/*                        printf("Corrupt yEnc Binary - endsize mismatch\r\n");  */
                        return(12);
                     }
                     if (decolen > atol(szSize))
                     {
/*                           printf("Extra data yEnc Binary mismatch, %d bytes\r\n", decolen - atol(szSize));  */
                        return(13);
                     }

                  }

                  pszEndSize = SearchFor("crc32=",srcbuf, strlen(srcbuf), pszEndSize);
                  if (pszEndSize != NULL)
                  {
                     CHAR szCRC[10];
                     PSZ pszCRC;
                     ULONG ulCRC;

                     pszCRC = szCRC;
                     pszEndSize +=6;

                     while (*pszEndSize != 0x00)
                        *(pszCRC++) = *(pszEndSize++);
                     *pszCRC = 0x00;

                     pszCRC = szCRC;

                     ulCRC = 0x00;
                     while (*pszCRC != 0x00)
                     {
                        ulCRC = ulCRC << 4;
                        switch (*pszCRC)
                        {
                           case '0':
                           case '1':
                           case '2':
                           case '3':
                           case '4':
                           case '5':
                           case '6':
                           case '7':
                           case '8':
                           case '9':
                              ulCRC += *pszCRC - 0x30;
                              break;
                           case 'A':
                           case 'B':
                           case 'C':
                           case 'D':
                           case 'E':
                           case 'F':
                              ulCRC += (*pszCRC - 0x41) + 10;
                              break;
                           case 'a':
                           case 'b':
                           case 'c':
                           case 'd':
                           case 'e':
                           case 'f':
                              ulCRC += (*pszCRC - 0x61) + 10;
                              break;
                        }
                        pszCRC++;
                     }
/*                     if (ulCRC != crc_val)
                     {
   / *                        printf("Corrupt yEnc Binary - CRC does not match\r\n");  * /
                        return(14);
                     }              */
                  }
               }

            }
            else
               return(4);
         }
         else
            return(3);
      }
      else
         return(2);

   *ulDecoLen = decolen;
   return(0);
}

extern int _dllentry;

BOOL CheckForPreProcess(PSZ pszHeader, ULONG ulHeaderLen, PSZ pszBody, ULONG ulBodyLen, PSZ pszNewsGroup, BOOL bLogs)
{
   int x = _dllentry;
   PSZ pszOTHERBegin;
   PSZ pszOTHEREnd;
   APIRET rc;
   ULONG action;
   ULONG BytesWritten;
   PSZ pszyEncFix;
   BOOL byEncFix = FALSE;


   pszyEncFix = SearchFor("X-Newsreader: Forte Agent 1.92/32.572",pszHeader, ulHeaderLen, pszyEncFix);
   if (pszyEncFix != NULL)
      byEncFix = TRUE;

   pszyEncFix = SearchFor("YENC-POWER-POST",pszHeader, ulHeaderLen, pszyEncFix);
   if (pszyEncFix != NULL)
      byEncFix = TRUE;

   pszOTHERBegin = SearchFor("=ybegin ",pszBody, ulBodyLen, pszOTHERBegin);
   if (pszOTHERBegin != NULL)
   {
      PSZ pszFileName;
      CHAR szFileSize[20];
      PSZ pszPartBegin;
      ULONG ulMsgPart;

      pszOTHEREnd = SearchFor("\r\n.\r\n",pszOTHERBegin, ulBodyLen - (pszOTHERBegin - pszBody), pszOTHEREnd);
      if (pszOTHEREnd == NULL)
         pszOTHEREnd = SearchFor("\r\n.",pszOTHERBegin, ulBodyLen - (pszOTHERBegin - pszBody), pszOTHEREnd);
      if (pszOTHEREnd != NULL)
      {

         CHAR szFileName[256];
         ULONG ulSize, ulStart, ulEnd, ulPart, ulDecoLen, ulTotal;
         PSZ pszOutData;


/*         printf("Decoding yEnc file\r\n");  */
         rc = yDecode(pszOTHERBegin, pszOTHEREnd - pszOTHERBegin, szFileName,
            &ulSize, &ulStart, &ulEnd, &ulPart, &ulTotal, &pszOutData, &ulDecoLen, byEncFix);
         if (rc == 0)
         {

            rc = 0;
            if (rc == 0)
            {
               HFILE hfOutFile;
               CHAR szNewFileName[256];

               if (ulPart > 0)
               {
                  sprintf(szNewFileName,"%s\\%s.yEnc%d",pszNewsGroup,szFileName, ulPart);
/*                  printf("Saving yEnc file %s part %d\r\n", szFileName, ulPart);  */
               }
               else
               {
                  sprintf(szNewFileName,"%s\\%s",pszNewsGroup,szFileName);
/*                  printf("Saving yEnc file %s\r\n", szNewFileName);  */
               }
               rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                  OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
               if (rc == 0)
               {
                  ULONG NewFilePos;

/*                                                DosSetFilePtr(hfOutFile, ulStart - 1, FILE_BEGIN, &NewFilePos); */
                  DosWrite(hfOutFile,pszOutData, ulDecoLen,&BytesWritten);

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

                     sprintf(szFindFile,"%s\\%s.yEnc*",pszNewsGroup,szFileName);

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

                     if (ulSavedSize >= ulSize)
                     {
                        HFILE hfOutFile;
                        CHAR szMergeFileName[256];

/*                        printf("Merging yEnc %s\r\n", szFileName);  */
                        sprintf(szMergeFileName,"%s\\%s", pszNewsGroup, szFileName);
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

                              sprintf(szNewFileName,"%s\\%s.yEnc%d",pszNewsGroup,szFileName, x);
                              rc = DosOpen(szNewFileName,&hfOldFile,&action,ulDecoLen,FILE_NORMAL,
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
                     }

                  }

               }
/*               rc = DosSetCurrentDir("\\");
               rc = DosSetCurrentDir(szCurrentDir);  */

            }

            DosFreeMem(pszOutData);

            return(1);
         }
         else
         {

            if (bLogs)
            {

               if (rc > 7)
               {
                  HFILE hfOutFile;
                  CHAR szNewFileName[256];
                  APIRET ret;

                  if (ulPart > 0)
                  {
                     sprintf(szNewFileName,"%s\\%s.yEnc%d.Error",pszNewsGroup,szFileName, ulPart);
                  }
                  else
                  {
                     sprintf(szNewFileName,"%s\\%s.yEnc.Error",pszNewsGroup,szFileName);
                  }
                  ret = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                     OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                     OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                  if (ret == 0)
                  {
                     ULONG NewFilePos;
                     CHAR szErrorMsg[512];

                     switch (rc)
                     {
                        case 8:
                           strcpy(szErrorMsg,"No =ybegin found");
                           break;
                        case 9:
                           strcpy(szErrorMsg,"Control character found at end of line");
                           break;
                        case 10:
                           strcpy(szErrorMsg,"Amount of data is greater than allocated size");
                           break;
                        case 11:
                           strcpy(szErrorMsg,"No end of line found");
                           break;
                        case 12:
                           strcpy(szErrorMsg,"Decrypted size is less than stated size");
                           break;
                        case 13:
                           strcpy(szErrorMsg,"Decrypted size is greater than stated size\r\n");
                           break;
                        case 14:
                           strcpy(szErrorMsg,"Decrypted data does not match stated CRC\r\n");
                           break;

                     }

                     DosWrite(hfOutFile,szErrorMsg, strlen(szErrorMsg),&BytesWritten);
                     switch (rc)
                     {
                        case 12:
                        case 13:
                        case 14:
                           DosWrite(hfOutFile,pszHeader,ulHeaderLen,&BytesWritten);
                           break;

                     }

                     DosClose(hfOutFile);
                  }


                  if (ulPart > 0)
                     sprintf(szNewFileName,"%s\\%s.yEnc%d.body",pszNewsGroup,szFileName, ulPart);
                  else
                     sprintf(szNewFileName,"%s\\%s.body",pszNewsGroup,szFileName);

                  rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                     OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                     OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                  if (rc == 0)
                  {

                     DosWrite(hfOutFile,pszBody, ulBodyLen,&BytesWritten);

                     DosClose(hfOutFile);

                  }
               }
            }

            DosFreeMem(pszOutData);

            return(1);
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
            break;
      }
      DosClose(hfLog);
   }
   return(0);
}


