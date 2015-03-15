#define EXTERN
#define NNTPPP2K24 1
#define NNTPPP2K24C 2
#define NNTPPP2K25B 3
#define NNTPPP2K25 4


#include "NNTPPower.h"

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
   int x;
   PSZ pszNEWSPoster;
   HFILE hfNNTP;
   ULONG action;
   APIRET rc;
   ULONG BytesWritten;
   ULONG ulFileNew;
   static CHAR szGroup[256];
   static CHAR szGroup1[256];
   ULONG ulPart;
   ULONG ulTotal;
   CHAR szProgramVer[256];
   CHAR szLogFileName[256];
   USHORT usPosterType;

   x = _dllentry;


   pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000 (Build 25)",pszHeader, ulHeaderLen, pszNEWSPoster);
   if (pszNEWSPoster == NULL)
   {
      pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000 (Build 25b; yEnc)",pszHeader, ulHeaderLen, pszNEWSPoster);
      if (pszNEWSPoster == NULL)
         pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000 (Build 25b)",pszHeader, ulHeaderLen, pszNEWSPoster);
      if (pszNEWSPoster == NULL)
      {
         pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000 (Build 24c)",pszHeader, ulHeaderLen, pszNEWSPoster);
         if (pszNEWSPoster == NULL)
         {
            pszNEWSPoster = SearchFor("X-Binary-Poster: NNTP POWER-POST 2000 (Build 24)",pszHeader, ulHeaderLen, pszNEWSPoster);
            if (pszNEWSPoster != NULL)
            {
               strcpy(szLogFileName,"NNTPPP2K24.LOG");
               usPosterType = NNTPPP2K24;
            }
         }
         else
         {
            strcpy(szLogFileName,"NNTPPP2K24C.LOG");
            usPosterType = NNTPPP2K24C;
         }
      }
      else
      {
         strcpy(szLogFileName,"NNTPPP2K25B.LOG");
         usPosterType = NNTPPP2K25B;
      }
   }
   else
   {
      usPosterType = NNTPPP2K25;
      strcpy(szLogFileName,"NNTPPP2K25.LOG");
   }

/*   if (pszNEWSPoster == NULL)
      pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000",pszHeader, ulHeaderLen, pszNEWSPoster); */
   if (pszNEWSPoster != NULL)
   {

      PSZ pszContentType;
      PSZ pszSubjectBegin;
      PSZ pszSubjectEnd;
      CHAR szFileName[256];
      PSZ pszPosterEnd;
      PSZ pszReply;
      PSZ pszDataBegin;


      strcpy(szFileName, "");
      ulPart = 0;
      ulTotal = 0;

      pszNEWSPoster = SearchFor("oster: ",pszHeader, ulHeaderLen, pszNEWSPoster);
      pszNEWSPoster += 7;
      pszPosterEnd = SearchFor(")",pszNEWSPoster, 255, pszPosterEnd);
      if (pszPosterEnd == NULL)
         pszPosterEnd = SearchFor(" - ",pszNEWSPoster, 255, pszPosterEnd);
      if (pszPosterEnd == NULL)
         pszPosterEnd = SearchFor("\r\n",pszNEWSPoster, 255, pszPosterEnd);
      else
         pszPosterEnd += 1;
      if (pszPosterEnd != NULL)
      {
         strncpy(szProgramVer, pszNEWSPoster, pszPosterEnd - pszNEWSPoster);
         szProgramVer[pszPosterEnd - pszNEWSPoster] = 0x00;
      }


      pszContentType = SearchFor("Content-Type: text/plain;",pszHeader, ulHeaderLen, pszContentType);
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

            pszReply = SearchFor("Re:",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszReply);
            if (pszReply == NULL)
            {

               PSZ pszFileMarker;
               PSZ pszFileMarker1;
               PSZ pszPartMarker;
               PSZ pszPartMarker1;
               PSZ pszSeparator;
               CHAR szTemp[100];
               PSZ pszTemp;
               PSZ pszLineTemp;
               ULONG ulPart;
               ULONG ulTotal;
               ULONG ulDirSize;
               PSZ pszFileTag;
               PSZ pszSubTemp;


               switch (usPosterType)
               {
                  case  NNTPPP2K25:
                  {
                     CHAR szTotalFileParts[50] = "";
                     CHAR szTotalFiles[50] = "";
                     CHAR szFileParts[50] = "";
                     CHAR szFilePartsTotal[50] = "";
                     PSZ pszFileTotalBegin = NULL;
                     PSZ pszFileTotalEnd = NULL;
                     PSZ pszFilePartBegin = NULL;
                     PSZ pszFilePartEnd = NULL;
                     BOOL bNotFound = TRUE;
                     BOOL bNotEnd = TRUE;
                     PSZ pszTempSubjectBegin;


                     pszTempSubjectBegin = pszSubjectBegin;
                     while (bNotFound && bNotEnd)
                     {
                        pszFileTotalBegin = SearchFor("[",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFileTotalBegin);
                        if (pszFileTotalBegin == NULL)
                        {
                           pszFileTotalEnd = SearchFor("]",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFileTotalEnd);
                           if (pszFileTotalEnd != NULL)
                           {
                              BOOL bFileSep = FALSE;

                              pszFileTotalBegin = pszFileTotalEnd - 1;

                              while ((isdigit(*pszFileTotalBegin)) || (*pszFileTotalBegin == '/'))
                              {
                                 if (*pszFileTotalBegin == '/')
                                    bFileSep = TRUE;
                                 pszFileTotalBegin--;
                              }
                              if (isdigit(*(pszFileTotalBegin + 1)))
                              {
                                 if (bFileSep == FALSE)
                                    pszFileTotalBegin = NULL;
                              }
                              else
                                 pszFileTotalBegin = FALSE;
                           }
                        }
                        if (pszFileTotalBegin)
                        {
                           PSZ pszTemp;

                           pszTemp = pszFileTotalBegin;

                           pszTemp++;

                           if (pszTemp < pszSubjectEnd)
                           {
                              if (isdigit(*pszTemp))
                              {
                                 PSZ pszTemp1;

                                 pszTemp1 = szTotalFileParts;

                                 while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                    *(pszTemp1++) = *(pszTemp++);
                                 if (pszTemp < pszSubjectEnd)
                                 {
                                    if (*pszTemp == '/')
                                    {
                                       *pszTemp1 = 0x00;

                                       pszTemp++;

                                       if (pszTemp < pszSubjectEnd)
                                       {
                                          pszTemp1 = szTotalFiles;

                                          while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                             *(pszTemp1++) = *(pszTemp++);
                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if ((*pszTemp == ']') || (*pszTemp == '/'))
                                             {
                                                *pszTemp1 = 0x00;
                                                bNotEnd = FALSE;
                                                pszFileTotalEnd = pszTemp;
                                                bNotFound = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                    else
                                    {
                                       bNotFound = TRUE;
                                       pszTempSubjectBegin = pszTemp;
                                    }
                                 }
                                 else
                                    bNotEnd = FALSE;
                              }
                              else
                              {
                                 bNotFound = TRUE;
                                 pszTempSubjectBegin = pszTemp;
                              }
                           }
                           else
                              bNotEnd = FALSE;
                        }
                        else
                           bNotEnd = FALSE;
                     }



                     bNotFound = TRUE;
                     bNotEnd = TRUE;
                     pszTempSubjectBegin = pszSubjectBegin;
                     while (bNotFound && bNotEnd)
                     {
                        pszFilePartBegin = SearchFor("(",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFilePartBegin);
                        if (pszFilePartBegin)
                        {
                           PSZ pszTemp;
                           while ((pszTemp = SearchFor("(",pszFilePartBegin + 1, pszSubjectEnd - (pszFilePartBegin + 1), pszTemp)) != NULL)
                              pszFilePartBegin = pszTemp;

                           pszTemp = pszFilePartBegin;

                           pszTemp++;

                           if (pszTemp < pszSubjectEnd)
                           {
                              if (isdigit(*pszTemp))
                              {
                                 PSZ pszTemp1;

                                 pszTemp1 = szFileParts;

                                 while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                    *(pszTemp1++) = *(pszTemp++);
                                 if (pszTemp < pszSubjectEnd)
                                 {
                                    if (*pszTemp == '/')
                                    {
                                       *pszTemp1 = 0x00;

                                       pszTemp++;

                                       if (pszTemp < pszSubjectEnd)
                                       {
                                          pszTemp1 = szFilePartsTotal;

                                          while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                             *(pszTemp1++) = *(pszTemp++);
                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if (*pszTemp == ')')
                                             {
                                                *pszTemp1 = 0x00;
                                                bNotEnd = FALSE;
                                                pszFilePartEnd = pszTemp;
                                                bNotFound = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                    else
                                    {
                                       bNotFound = TRUE;
                                       pszTempSubjectBegin = pszTemp;
                                    }
                                 }
                                 else
                                    bNotEnd = FALSE;
                              }
                              else
                              {
                                 bNotFound = TRUE;
                                 pszTempSubjectBegin = pszTemp;
                              }
                           }
                           else
                              bNotEnd = FALSE;
                        }
                        else
                           bNotEnd = FALSE;
                     }


                     ulPart = 0;
                     ulTotal = 0;
                     if ((pszFileTotalBegin != NULL) && (pszFilePartBegin != NULL))
                     {
                        ulPart = atol(szFileParts);
                        ulTotal = atol(szFilePartsTotal);

                        if (pszFileTotalBegin < pszFilePartBegin)
                        {
                           PSZ pszTempCopy;
                           PSZ pszTempCheck;

                           pszTempCopy = pszFileTotalEnd + 1;

                           while ((*pszTempCopy == ' ') || (*pszTempCopy == '-'))
                              pszTempCopy++;

                           if ((pszFilePartBegin - 1) - pszTempCopy > 255)
                              pszTempCopy = pszFilePartBegin - 255;

                           pszTempCheck = pszFilePartBegin - 1;

                           while ((*pszTempCheck != '\\') && (*pszTempCheck != '<') && (*pszTempCheck != '>') &&
                              (pszTempCheck > pszTempCopy))
                              pszTempCheck--;
                           if (pszTempCheck != pszTempCopy)
                              pszTempCopy = pszTempCheck + 1;

                           strncpy(szFileName, pszTempCopy, (pszFilePartBegin - 1) - pszTempCopy);
                           szFileName[(pszFilePartBegin - 1) - pszTempCopy] = 0x00;
                        }
                        else
                        {
                           PSZ pszTempCopy;
                           PSZ pszTempCheck;

                           pszTempCopy = pszSubjectBegin + 9;
                           if ((pszFilePartBegin - 2) - pszTempCopy > 255)
                              pszTempCopy = pszFilePartBegin - 255;

                           pszTempCheck = pszFilePartBegin - 2;

                           while ((*pszTempCheck != '\\') && (*pszTempCheck != '<') && (*pszTempCheck != '>') &&
                              (pszTempCheck > pszTempCopy))
                              pszTempCheck--;
                           if (pszTempCheck != pszTempCopy)
                              pszTempCopy = pszTempCheck + 1;

                           strncpy(szFileName, pszTempCopy, (pszFilePartBegin - 2) - pszTempCopy);
                           szFileName[(pszFilePartBegin - 2) - pszTempCopy] = 0x00;
                        }
                     }
                     else
                     {
                        if ((pszFileTotalBegin == NULL) && (pszFilePartBegin != NULL))
                        {
                           PSZ pszUserCommentBegin;
                           PSZ pszUserCommentEnd;
                           PSZ pszTemp;

                           pszUserCommentBegin = SearchFor("-",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszUserCommentBegin);
                           if (pszUserCommentBegin != NULL)
                           {
                              pszUserCommentEnd = SearchFor("-",pszUserCommentBegin + 1, pszSubjectEnd - (pszUserCommentBegin + 1), pszUserCommentEnd);
                              if (pszUserCommentEnd != NULL)
                              {
                                 if (pszUserCommentEnd < pszFilePartBegin)
                                 {
                                    PSZ pszTempCheck;

                                    pszUserCommentEnd++;

                                    while (*pszUserCommentEnd == ' ')
                                       pszUserCommentEnd++;

                                    ulPart = atol(szFileParts);
                                    ulTotal = atol(szFilePartsTotal);

                                    pszTempCheck = pszFilePartBegin - 1;

                                    while ((*pszTempCheck != '\\') && (*pszTempCheck != '<') && (*pszTempCheck != '>') &&
                                       (pszTempCheck > pszUserCommentEnd))
                                       pszTempCheck--;
                                    if (pszTempCheck != pszUserCommentEnd)
                                       pszUserCommentEnd = pszTempCheck + 1;

                                    strncpy(szFileName, pszUserCommentEnd, (pszFilePartBegin - 1) - pszUserCommentEnd);
                                    szFileName[(pszFilePartBegin - 1) - pszUserCommentEnd] = 0x00;
                                 }
                                 else
                                 {
                                    PSZ pszTempCheck;

                                    pszFilePartEnd = pszFilePartBegin - 1;
                                    pszFilePartBegin = pszSubjectBegin + 9;

                                    ulPart = atol(szFileParts);
                                    ulTotal = atol(szFilePartsTotal);

                                    pszTempCheck = pszFilePartEnd;

                                    while ((*pszTempCheck != '\\') && (*pszTempCheck != '<') && (*pszTempCheck != '>') &&
                                       (pszTempCheck > pszFilePartBegin))
                                       pszTempCheck--;
                                    if (pszTempCheck != pszFilePartBegin)
                                       pszFilePartBegin = pszTempCheck + 1;

                                    strncpy(szFileName, pszFilePartBegin, pszFilePartEnd - pszFilePartBegin);
                                    szFileName[pszFilePartEnd - pszFilePartBegin] = 0x00;

                                 }
                              }
                              else
                              {
                                 PSZ pszTempCheck;

                                 pszUserCommentEnd = pszSubjectBegin + 9;
                                 ulPart = atol(szFileParts);
                                 ulTotal = atol(szFilePartsTotal);

                                 pszTempCheck = pszFilePartBegin - 1;

                                 while ((*pszTempCheck != '\\') && (*pszTempCheck != '<') && (*pszTempCheck != '>') &&
                                    (pszTempCheck > pszUserCommentEnd))
                                    pszTempCheck--;
                                 if (pszTempCheck != pszUserCommentEnd)
                                    pszUserCommentEnd = pszTempCheck + 1;

                                 strncpy(szFileName, pszUserCommentEnd, (pszFilePartBegin - 1) - pszUserCommentEnd);
                                 szFileName[(pszFilePartBegin - 1) - pszUserCommentEnd] = 0x00;
                              }
                           }
                           else
                           {

                              pszTemp = pszFilePartBegin - 2;

                              while (((*pszTemp != ' ') || (*(pszTemp + 1) != ' ')) && (pszTemp > pszSubjectBegin)
                                    && (*pszTemp != '/'))
                                 pszTemp--;
                              if (*pszTemp == ' ')
                              {
                                 pszTemp += 2;

                                 ulPart = atol(szFileParts);
                                 ulTotal = atol(szFilePartsTotal);

                                 strncpy(szFileName, pszTemp, (pszFilePartBegin - 1) - pszTemp);
                                 szFileName[(pszFilePartBegin - 1) - pszTemp] = 0x00;
                              }
                              else
                              {

                                 if (*pszTemp == '/')
                                 {
                                    pszTemp ++;

/*                                    while (*pszTemp != ' ')
                                       *pszTemp++;

                                    while (*pszTemp == ' ')
                                       *pszTemp++;  */

                                    ulPart = atol(szFileParts);
                                    ulTotal = atol(szFilePartsTotal);

                                    strncpy(szFileName, pszTemp, (pszFilePartBegin - 1) - pszTemp);
                                    szFileName[(pszFilePartBegin - 1) - pszTemp] = 0x00;
                                 }
                                 else
                                 {
                                    if (pszTemp == pszSubjectBegin)
                                    {

                                       pszTemp = pszSubjectBegin + 9;
                                       ulPart = atol(szFileParts);
                                       ulTotal = atol(szFilePartsTotal);

                                       if (pszFilePartBegin > pszTemp)
                                       {

                                          strncpy(szFileName, pszTemp, (pszFilePartBegin - 1) - pszTemp);
                                          szFileName[(pszFilePartBegin - 1) - pszTemp] = 0x00;
                                       }
                                       else
                                          strcpy(szFileName, "Unknown");

                                    }
                                    else
                                       return(0);
                                 }
                              }
                           }
                        }

                     }
                     break;
                  }
                  case  NNTPPP2K25B:
                  {

                     CHAR szTotalFileParts[50] = "";
                     CHAR szTotalFiles[50] = "";
                     CHAR szFileParts[50] = "";
                     CHAR szFilePartsTotal[50] = "";
                     BOOL bNotFound = TRUE;
                     BOOL bNotEnd = TRUE;
                     PSZ pszTempSubjectBegin;

                     if (*(pszSubjectEnd - 1) == ')')
                     {
                        PSZ pszFilePartBegin;
                        PSZ pszFilePartEnd;
                        PSZ pszFileSeparator;
                        PSZ pszFileTotalBegin = NULL;
                        PSZ pszFileTotalEnd = NULL;

                        pszFilePartEnd = pszSubjectEnd - 1;
                        pszFilePartBegin = pszFilePartEnd - 1;
                        while (isdigit(*pszFilePartBegin))
                           pszFilePartBegin--;

                        if (*pszFilePartBegin == '/')
                        {
                           pszFileSeparator = pszFilePartBegin;
                           pszFilePartBegin--;
                           if (isdigit(*pszFilePartBegin))
                           {
                              while (isdigit(*pszFilePartBegin))
                                 pszFilePartBegin--;
                              if (*pszFilePartBegin == '(')
                              {
                                 strncpy(szFileParts,pszFilePartBegin + 1, pszFileSeparator - (pszFilePartBegin + 1));
                                 szFileParts[pszFileSeparator - (pszFilePartBegin + 1)] = 0x00;
                                 strncpy(szFilePartsTotal,pszFileSeparator + 1, pszFilePartEnd - (pszFileSeparator + 1));
                                 szFilePartsTotal[pszFilePartEnd - (pszFileSeparator + 1)] = 0x00;


                                 pszFileTotalBegin = SearchFor("[",pszSubjectBegin, (pszFilePartBegin - 1) - pszSubjectBegin, pszFileTotalBegin);
                                 if (pszFileTotalBegin == NULL)
                                    pszFileTotalBegin = SearchFor("(",pszSubjectBegin, (pszFilePartBegin - 1) - pszSubjectBegin, pszFileTotalBegin);
                                 if (pszFileTotalBegin != NULL)
                                 {
                                    CHAR szTotChar[5];

                                    szTotChar[0] = *pszFileTotalBegin;
                                    szTotChar[1] = 0x00;
                                    pszTempSubjectBegin = pszSubjectBegin;
                                    while (bNotFound && bNotEnd)
                                    {
                                       pszFileTotalBegin = SearchFor(szTotChar,pszTempSubjectBegin, (pszFilePartBegin - 1) - pszTempSubjectBegin, pszFileTotalBegin);
                                       if (pszFileTotalBegin != NULL)
                                       {
                                          if (szTotChar[0] == '[')
                                             pszFileTotalEnd = SearchFor("]",pszTempSubjectBegin, (pszFilePartBegin - 1) - pszTempSubjectBegin, pszFileTotalEnd);
                                          else
                                             pszFileTotalEnd = SearchFor(")",pszTempSubjectBegin, (pszFilePartBegin - 1) - pszTempSubjectBegin, pszFileTotalEnd);
                                          if (pszFileTotalEnd != NULL)
                                          {
                                             BOOL bFileSep = FALSE;

                                             pszFileTotalBegin = pszFileTotalEnd - 1;

                                             while ((isdigit(*pszFileTotalBegin)) || (*pszFileTotalBegin == '/'))
                                             {
                                                if (*pszFileTotalBegin == '/')
                                                   bFileSep = TRUE;
                                                pszFileTotalBegin--;
                                             }
                                             if (isdigit(*(pszFileTotalBegin + 1)))
                                             {
                                                if (bFileSep == FALSE)
                                                   pszFileTotalBegin = NULL;
                                             }
                                             else
                                                pszFileTotalBegin = NULL;
                                          }
                                       }
                                       if (pszFileTotalBegin)
                                       {
                                          PSZ pszTemp;

                                          pszTemp = pszFileTotalBegin;

                                          pszTemp++;

                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if (isdigit(*pszTemp))
                                             {
                                                PSZ pszTemp1;

                                                pszTemp1 = szTotalFileParts;

                                                while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                                   *(pszTemp1++) = *(pszTemp++);
                                                if (pszTemp < pszSubjectEnd)
                                                {
                                                   if (*pszTemp == '/')
                                                   {
                                                      *pszTemp1 = 0x00;

                                                      pszTemp++;

                                                      if (pszTemp < pszSubjectEnd)
                                                      {
                                                         pszTemp1 = szTotalFiles;

                                                         while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                                            *(pszTemp1++) = *(pszTemp++);
                                                         if (pszTemp < pszSubjectEnd)
                                                         {
                                                            if (((*pszTemp == ']') && (szTotChar[0] == '[')) || ((*pszTemp == ')') && (szTotChar[0] == '(')))
                                                            {
                                                               *pszTemp1 = 0x00;
                                                               bNotEnd = FALSE;
                                                               pszFileTotalEnd = pszTemp;
                                                               bNotFound = FALSE;
                                                            }
                                                            else
                                                            {
                                                               bNotFound = TRUE;
                                                               pszTempSubjectBegin = pszTemp;
                                                            }
                                                         }
                                                         else
                                                            bNotEnd = FALSE;
                                                      }
                                                      else
                                                         bNotEnd = FALSE;
                                                   }
                                                   else
                                                   {
                                                      bNotFound = TRUE;
                                                      pszTempSubjectBegin = pszTemp;
                                                   }
                                                }
                                                else
                                                   bNotEnd = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                 }

                                 if (pszFileTotalBegin != NULL)
                                 {
                                    strncpy(szFileName, pszFileTotalEnd + 4, (pszFilePartBegin - 1) - (pszFileTotalEnd + 4));
                                    szFileName[(pszFilePartBegin - 1) - (pszFileTotalEnd + 4)] = 0x00;
                                 }
                                 else
                                 {
                                    PSZ pszFileNameBegin;
                                    PSZ pszFileNameEnd;

                                    pszFileNameEnd = pszFilePartBegin - 1;
                                    pszFileNameBegin = pszFileNameEnd - 1;
                                    while ((*pszFileNameBegin != ' ') && (pszFileNameBegin > pszSubjectBegin))
                                       pszFileNameBegin--;
                                    if (pszFileNameBegin > pszSubjectBegin)
                                    {

                                       strncpy(szFileName, pszFileNameBegin + 1, pszFileNameEnd - (pszFileNameBegin + 1));
                                       szFileName[pszFileNameEnd - (pszFileNameBegin + 1)] = 0x00;
                                    }
                                 }

                                 ulPart = atol(szFileParts);
                                 ulTotal = atol(szFilePartsTotal);
                              }
                           }

                        }
                     }
                     else
                     {

                        CHAR szFileParts[50] = "";
                        CHAR szFilePartsTotal[50] = "";
                        PSZ pszFilePartBegin = NULL;
                        PSZ pszFilePartEnd = NULL;
                        BOOL bNotFound = TRUE;
                        BOOL bNotEnd = TRUE;
                        PSZ pszTempSubjectBegin;


                        pszTempSubjectBegin = pszSubjectBegin;
                        bNotFound = TRUE;
                        bNotEnd = TRUE;
                        while (bNotFound && bNotEnd)
                        {
                           pszFilePartBegin = SearchFor("(",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFilePartBegin);
                           if (pszFilePartBegin)
                           {
                              PSZ pszTemp;

                              pszTemp = pszFilePartBegin;

                              pszTemp++;

                              if (pszTemp < pszSubjectEnd)
                              {
                                 if (isdigit(*pszTemp))
                                 {
                                    PSZ pszTemp1;

                                    pszTemp1 = szFileParts;

                                    while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                       *(pszTemp1++) = *(pszTemp++);
                                    if (pszTemp < pszSubjectEnd)
                                    {
                                       if (*pszTemp == '/')
                                       {
                                          *pszTemp1 = 0x00;

                                          pszTemp++;

                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             pszTemp1 = szFilePartsTotal;

                                             while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                                *(pszTemp1++) = *(pszTemp++);
                                             if (pszTemp < pszSubjectEnd)
                                             {
                                                if (*pszTemp == ')')
                                                {
                                                   *pszTemp1 = 0x00;
                                                   bNotEnd = FALSE;
                                                   pszFilePartEnd = pszTemp;
                                                   bNotFound = FALSE;
                                                }
                                                else
                                                {
                                                   bNotFound = TRUE;
                                                   pszTempSubjectBegin = pszTemp;
                                                }
                                             }
                                             else
                                                bNotEnd = FALSE;
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                       {
                                          bNotFound = TRUE;
                                          pszTempSubjectBegin = pszTemp;
                                       }
                                    }
                                    else
                                       bNotEnd = FALSE;
                                 }
                                 else
                                 {
                                    bNotFound = TRUE;
                                    pszTempSubjectBegin = pszTemp;
                                 }
                              }
                              else
                                 bNotEnd = FALSE;
                           }
                           else
                              bNotEnd = FALSE;
                        }

                        if (pszFilePartEnd != NULL)
                        {
                           PSZ pszUserCommentBegin;
                           PSZ pszUserCommentEnd;

                           pszUserCommentBegin = SearchFor(" - ",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszUserCommentBegin);
                           if (pszUserCommentBegin != NULL)
                           {
                              ulPart = atol(szFileParts);
                              ulTotal = atol(szFilePartsTotal);
                              pszUserCommentEnd = SearchFor(" - ",pszUserCommentBegin + 1, pszSubjectEnd - (pszUserCommentBegin + 1), pszUserCommentEnd);
                              if (pszUserCommentEnd != NULL)
                                 pszUserCommentBegin = pszUserCommentEnd;

                              pszUserCommentBegin += 3;

                              strncpy(szFileName, pszUserCommentBegin, (pszFilePartBegin - 1) - pszUserCommentBegin);
                              szFileName[(pszFilePartBegin - 1) - pszUserCommentBegin] = 0x00;
                           }
                        }
                     }
                     break;
                  }
                  case  NNTPPP2K24:
                  {
                     CHAR szFileParts[50] = "";
                     CHAR szFilePartsTotal[50] = "";
                     PSZ pszFilePartBegin = NULL;
                     PSZ pszFilePartEnd = NULL;
                     BOOL bNotFound = TRUE;
                     BOOL bNotEnd = TRUE;
                     PSZ pszTempSubjectBegin;
                     USHORT usCommentCount = 0;

                     pszTempSubjectBegin = pszSubjectBegin;
                     bNotFound = TRUE;
                     bNotEnd = TRUE;
                     while (bNotFound && bNotEnd)
                     {
                        pszFilePartBegin = SearchFor("(",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFilePartBegin);
                        if (pszFilePartBegin)
                        {
                           PSZ pszTemp;

                           pszTemp = pszFilePartBegin;

                           pszTemp++;

                           if (pszTemp < pszSubjectEnd)
                           {
                              if (isdigit(*pszTemp))
                              {
                                 PSZ pszTemp1;

                                 pszTemp1 = szFileParts;

                                 while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                    *(pszTemp1++) = *(pszTemp++);
                                 if (pszTemp < pszSubjectEnd)
                                 {
                                    if (*pszTemp == '/')
                                    {
                                       *pszTemp1 = 0x00;

                                       pszTemp++;

                                       if (pszTemp < pszSubjectEnd)
                                       {
                                          pszTemp1 = szFilePartsTotal;

                                          while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                             *(pszTemp1++) = *(pszTemp++);
                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if (*pszTemp == ')')
                                             {
                                                *pszTemp1 = 0x00;
                                                bNotEnd = FALSE;
                                                pszFilePartEnd = pszTemp;
                                                bNotFound = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                    else
                                    {
                                       bNotFound = TRUE;
                                       pszTempSubjectBegin = pszTemp;
                                    }
                                 }
                                 else
                                    bNotEnd = FALSE;
                              }
                              else
                              {
                                 bNotFound = TRUE;
                                 pszTempSubjectBegin = pszTemp;
                              }
                           }
                           else
                              bNotEnd = FALSE;
                        }
                        else
                           bNotEnd = FALSE;
                     }

                     if (pszFilePartEnd != NULL)
                     {
                        PSZ pszUserCommentBegin;
                        PSZ pszUserCommentEnd;

                        pszUserCommentBegin = SearchFor(" - ",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszUserCommentBegin);
                        if (pszUserCommentBegin != NULL)
                        {
                           do
                           {
                              usCommentCount++;

                              pszUserCommentEnd = SearchFor(" - ",pszUserCommentBegin + 1, pszSubjectEnd - (pszUserCommentBegin + 1), pszUserCommentEnd);
                              if (pszUserCommentEnd != NULL)
                                 pszUserCommentBegin = pszUserCommentEnd;
                           }  while (pszUserCommentEnd != NULL);

                           pszUserCommentBegin += 3;

                           ulPart = atol(szFileParts);
                           ulTotal = atol(szFilePartsTotal);

                           strncpy(szFileName, pszUserCommentBegin, (pszFilePartBegin - 1) - pszUserCommentBegin);
                           szFileName[(pszFilePartBegin - 1) - pszUserCommentBegin] = 0x00;
                        }
                     }
                     break;
                  }
                  case  NNTPPP2K24C:
                  {
                     CHAR szTotalFileParts[50] = "";
                     CHAR szTotalFiles[50] = "";
                     CHAR szFileParts[50] = "";
                     CHAR szFilePartsTotal[50] = "";
                     PSZ pszFileTotalBegin = NULL;
                     PSZ pszFileTotalEnd = NULL;
                     PSZ pszFilePartBegin = NULL;
                     PSZ pszFilePartEnd = NULL;
                     BOOL bNotFound = TRUE;
                     BOOL bNotEnd = TRUE;
                     PSZ pszTempSubjectBegin;
                     PSZ pszTempFileBegin;
                     PSZ pszTempFileEnd;
                     PSZ pszCommentBegin;
                     PSZ pszCommentEnd;
                     PSZ pszCommentTemp;
                     HFILE hfOutFile;
                     CHAR szFileName[50];
                     USHORT sCount;
                     PSZ pszBodyBegin;
                     PSZ pszBodyEnd;
                     SHORT sDecoded = FALSE;



                     pszBodyBegin = SearchFor("begin",pszBody, ulBodyLen, pszBodyBegin);
                     if (pszBodyBegin != NULL)
                     {
                        pszBodyEnd = SearchFor("end\r\n",pszBody, ulBodyLen, pszBodyEnd);
                        if (pszBodyEnd != NULL)
                        {

                           if (bUUCodeDecoderNotLoaded == TRUE)
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
                                       rc = DosQueryProcAddr(MMod,0,"UUDecode",(PFN *)&UUDecode);
                                       if (rc == 0)
                                          bUUCodeDecoderNotLoaded = FALSE;
                                    }
                                 }
                              }
                           }
                           if (bUUCodeDecoderNotLoaded == FALSE)
                           {
                              CHAR szUUFileName[256];
                              ULONG ulFileSize;
                              LONG UURet;
                              PSZ pszBodyData;

                              rc = DosAllocMem((VOID *)&pszBodyData,ulBodyLen ,PAG_READ | PAG_WRITE | PAG_COMMIT);
                              if (rc == 0)
                              {

                                 memcpy(pszBodyData, pszBody, ulBodyLen);
   
                                 UURet = UUDecode(pszBodyData, ulBodyLen, szUUFileName, &ulFileSize);
                                 if (UURet == 0)
                                 {
                                    CHAR szTempFileName[256];
                                    PSZ pszExtension;
   
                                    sprintf(szTempFileName,"%s\\%s",pszNewsGroup,szUUFileName);
   
                                    rc = DosOpen(szTempFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    sCount = 1;
                                    while ((rc != 0) && (sCount < 32000))
                                    {
   
                                       strcpy(szTempFileName, pszNewsGroup);
                                       strcat(szTempFileName, "\\");
                                       pszExtension = SearchFor(".",szUUFileName, strlen(szUUFileName), pszExtension);
                                       if (pszExtension != NULL)
                                       {
                                          strncat(szTempFileName, szUUFileName, pszExtension - szUUFileName);
                                          sprintf(&szTempFileName[strlen(szTempFileName)], "%d", sCount++);
                                          strcat(szTempFileName, pszExtension);
                                       }
                                       else
                                       {
                                          strcat(szTempFileName, szUUFileName);
                                          sprintf(&szTempFileName[strlen(szTempFileName)], "%d", sCount++);
                                       }
                                       rc = DosOpen(szTempFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                          OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                                          OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    }
                                    if (rc == 0)
                                    {
                                       ULONG BytesWritten;
   
                                       DosWrite(hfOutFile,pszBodyData,ulFileSize, &BytesWritten);
                                       DosClose(hfOutFile);
                                       sDecoded = TRUE;
                                    }
                                    else
                                       rc = 1;
                                 }

                                 DosFreeMem(pszBodyData);
                              }
                           }
                        }
                     }

                     if (sDecoded == FALSE)
                     {

                        sCount = 1;
                        sprintf(szFileName, "NNTPPP2k24c%5d.out", sCount++);
                        pszCommentBegin = szFileName;
                        while (*pszCommentBegin != 0x00)
                        {
                           if (*pszCommentBegin == 0x20)
                              *pszCommentBegin = '0';
                           pszCommentBegin++;
                        }

                        rc = DosOpen(szFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                           OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                           OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                        while ((rc != 0) && (sCount < 32000))
                        {

                           sprintf(szFileName, "NNTPPP2k24c%5d.out", sCount++);
                           pszCommentBegin = szFileName;
                           while (*pszCommentBegin != 0x00)
                           {
                              if (*pszCommentBegin == 0x20)
                                 *pszCommentBegin = '0';
                              pszCommentBegin++;
                           }
                           rc = DosOpen(szFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                              OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                        }
                        if (rc == 0)
                        {

                           DosWrite(hfOutFile,pszHeader, ulHeaderLen, &BytesWritten);
                           DosWrite(hfOutFile,pszBody, ulBodyLen, &BytesWritten);

                           DosClose(hfOutFile);
                        }
                     }
                     break;


                     pszTempSubjectBegin = pszSubjectBegin;
                     while (bNotFound && bNotEnd)
                     {
                        pszFileTotalBegin = SearchFor("[",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFileTotalBegin);
                        if (pszFileTotalBegin == NULL)
                        {
                           pszFileTotalEnd = SearchFor("]",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFileTotalEnd);
                           if (pszFileTotalEnd != NULL)
                           {
                              BOOL bFileSep = FALSE;

                              pszFileTotalBegin = pszFileTotalEnd - 1;

                              while ((isdigit(*pszFileTotalBegin)) || (*pszFileTotalBegin == '/'))
                              {
                                 if (*pszFileTotalBegin == '/')
                                    bFileSep = TRUE;
                                 pszFileTotalBegin--;
                              }
                              if (isdigit(*(pszFileTotalBegin + 1)))
                              {
                                 if (bFileSep == FALSE)
                                    pszFileTotalBegin = NULL;
                              }
                              else
                                 pszFileTotalBegin = NULL;
                           }
                        }
                        if (pszFileTotalBegin)
                        {
                           PSZ pszTemp;

                           pszTemp = pszFileTotalBegin;

                           pszTemp++;

                           if (pszTemp < pszSubjectEnd)
                           {
                              if (isdigit(*pszTemp))
                              {
                                 PSZ pszTemp1;

                                 pszTemp1 = szTotalFileParts;

                                 while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                    *(pszTemp1++) = *(pszTemp++);
                                 if (pszTemp < pszSubjectEnd)
                                 {
                                    if (*pszTemp == '/')
                                    {
                                       *pszTemp1 = 0x00;

                                       pszTemp++;

                                       if (pszTemp < pszSubjectEnd)
                                       {
                                          pszTemp1 = szTotalFiles;

                                          while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                             *(pszTemp1++) = *(pszTemp++);
                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if (*pszTemp == ']')
                                             {
                                                *pszTemp1 = 0x00;
                                                bNotEnd = FALSE;
                                                pszFileTotalEnd = pszTemp;
                                                bNotFound = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                    else
                                    {
                                       bNotFound = TRUE;
                                       pszTempSubjectBegin = pszTemp;
                                    }
                                 }
                                 else
                                    bNotEnd = FALSE;
                              }
                              else
                              {
                                 bNotFound = TRUE;
                                 pszTempSubjectBegin = pszTemp;
                              }
                           }
                           else
                              bNotEnd = FALSE;
                        }
                        else
                           bNotEnd = FALSE;
                     }



                     pszTempSubjectBegin = pszSubjectBegin;


                     /* Nathan */

                     pszCommentBegin = SearchFor(" -- ",pszTempSubjectBegin, ulHeaderLen - (pszSubjectBegin - pszHeader), pszCommentBegin);
                     if (pszCommentBegin != NULL)
                     {

                        pszCommentEnd = pszCommentBegin + 2;
                        pszCommentTemp = pszCommentEnd;
                        do
                        {
                           pszCommentTemp = SearchFor("--",pszCommentEnd, pszSubjectEnd - pszCommentEnd, pszCommentTemp);
                           if (pszCommentTemp != NULL)
                           {
                              pszCommentTemp += 2;
                              pszCommentEnd = pszCommentTemp;
                           }

                        } while (pszCommentTemp != NULL);
                     }


                     pszFilePartBegin = SearchFor("(",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFilePartBegin);
                     bNotFound = TRUE;
                     bNotEnd = TRUE;
                     while (bNotFound && bNotEnd)
                     {
                        pszFilePartBegin = SearchFor("(",pszTempSubjectBegin, pszSubjectEnd - pszTempSubjectBegin, pszFilePartBegin);
                        if (pszFilePartBegin)
                        {
                           PSZ pszTemp;

                           pszTemp = pszFilePartBegin;

                           pszTemp++;

                           if (pszTemp < pszSubjectEnd)
                           {
                              if (isdigit(*pszTemp))
                              {
                                 PSZ pszTemp1;

                                 pszTemp1 = szFileParts;

                                 while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                    *(pszTemp1++) = *(pszTemp++);
                                 if (pszTemp < pszSubjectEnd)
                                 {
                                    if (*pszTemp == '/')
                                    {
                                       *pszTemp1 = 0x00;

                                       pszTemp++;

                                       if (pszTemp < pszSubjectEnd)
                                       {
                                          pszTemp1 = szFilePartsTotal;

                                          while ((pszTemp < pszSubjectEnd) && (isdigit(*pszTemp)))
                                             *(pszTemp1++) = *(pszTemp++);
                                          if (pszTemp < pszSubjectEnd)
                                          {
                                             if (*pszTemp == ')')
                                             {
                                                *pszTemp1 = 0x00;
                                                bNotEnd = FALSE;
                                                pszFilePartEnd = pszTemp;
                                                bNotFound = FALSE;
                                             }
                                             else
                                             {
                                                bNotFound = TRUE;
                                                pszTempSubjectBegin = pszTemp;
                                             }
                                          }
                                          else
                                             bNotEnd = FALSE;
                                       }
                                       else
                                          bNotEnd = FALSE;
                                    }
                                    else
                                    {
                                       bNotFound = TRUE;
                                       pszTempSubjectBegin = pszTemp;
                                    }
                                 }
                                 else
                                    bNotEnd = FALSE;
                              }
                              else
                              {
                                 bNotFound = TRUE;
                                 pszTempSubjectBegin = pszTemp;
                              }
                           }
                           else
                              bNotEnd = FALSE;
                        }
                        else
                           bNotEnd = FALSE;
                     }


                     ulPart = 0;
                     ulTotal = 0;
                     if ((pszFileTotalBegin != NULL) && (pszFilePartBegin != NULL))
                     {
                        ulPart = atol(szFileParts);
                        ulTotal = atol(szFilePartsTotal);

                        if (pszFileTotalBegin < pszFilePartBegin)
                        {
                           PSZ pszTempCopy;

                           pszTempCopy = pszFileTotalEnd + 1;

                           while ((*pszTempCopy == ' ') || (*pszTempCopy == '-'))
                              pszTempCopy++;

                           if ((pszFilePartBegin - 1) - pszTempCopy > 255)
                              pszTempCopy = pszFilePartBegin - 255;

                           strncpy(szFileName, pszTempCopy, (pszFilePartBegin - 1) - pszTempCopy);
                           szFileName[(pszFilePartBegin - 1) - pszTempCopy] = 0x00;
                        }
                        else
                        {
                           PSZ pszTempCopy;
                           PSZ pszTempCopy1;

                           pszTempCopy = pszSubjectBegin + 9;
                           if ((pszFilePartBegin - 2) - pszTempCopy > 255)
                              pszTempCopy = pszFilePartBegin - 255;


                           strncpy(szFileName, pszTempCopy, (pszFilePartBegin - 2) - pszTempCopy);
                           szFileName[(pszFilePartBegin - 2) - pszTempCopy] = 0x00;
                        }
                     }
                     else
                     {
                        if ((pszFileTotalBegin == NULL) && (pszFilePartBegin != NULL))
                        {
                           PSZ pszUserCommentBegin;
                           PSZ pszUserCommentEnd;
                           PSZ pszTemp;

                           pszUserCommentBegin = SearchFor("- ",pszSubjectBegin, pszSubjectEnd - pszSubjectBegin, pszUserCommentBegin);
                           if (pszUserCommentBegin != NULL)
                           {
                              PSZ pszTemp1;

                              pszTemp1 = pszUserCommentBegin;
                              do
                              {
                                 pszUserCommentEnd = SearchFor("- ",pszTemp1 + 1, pszSubjectEnd - (pszUserCommentBegin + 1), pszUserCommentEnd);
                                 if (pszUserCommentEnd != NULL)
                                    if (pszUserCommentEnd < pszFilePartBegin)
                                    {
                                       pszUserCommentBegin = pszUserCommentEnd;
                                       pszTemp1 = pszUserCommentEnd;
                                    }
                                    else
                                       pszTemp1 = pszUserCommentEnd;
                              } while (pszUserCommentEnd != NULL);

                              pszUserCommentBegin += 2;
                              ulPart = atol(szFileParts);
                              ulTotal = atol(szFilePartsTotal);


                              if (pszFilePartBegin > pszUserCommentBegin)
                              {
                                 if ((pszFilePartBegin - 1) - pszUserCommentBegin > 255)
                                    pszUserCommentBegin = pszFilePartBegin - 255;
                                 strncpy(szFileName, pszUserCommentBegin, (pszFilePartBegin - 1) - pszUserCommentBegin);
                                 szFileName[(pszFilePartBegin - 1) - pszUserCommentBegin] = 0x00;
                              }
                              else
                              {
                                 PSZ pszTemp;
                                 PSZ pszTempCopy;
                                 PSZ pszTempCopy1;

                                 pszTempCopy = pszSubjectBegin + 9;

/*                                 pszTempCopy1 = SearchFor("--",pszTempCopy, (pszFilePartBegin - 1) - pszTempCopy, pszTempCopy1);  */
                                 do
                                 {
                                    pszTempCopy1 = SearchFor("--",pszTempCopy, (pszFilePartBegin - 1) - pszTempCopy, pszTempCopy1);
                                    if (pszTempCopy1 != NULL)
                                       pszTempCopy = pszTempCopy1 + 2;

                                 } while (pszTempCopy1 != NULL);

                                 strncpy(szFileName, pszTempCopy, (pszFilePartBegin - 1) - pszTempCopy);
                                 szFileName[(pszFilePartBegin - 1) - pszTempCopy] = 0x00;

                                 pszTemp = &szFileName[strlen(szFileName)];
                                 while ((*pszTemp != '\\') && (*pszTemp != '<') && (*pszTemp != '>') &&
                                    (pszTemp > szFileName))
                                    pszTemp--;
                                 if (pszTemp != szFileName)
                                 {
                                    pszTemp++;
                                    strcpy(szFileName, pszTemp);
                                 }
                              }
                           }
                           else
                           {
                              pszTemp = pszSubjectBegin + 9;
/*                              ulPart = atol(szFileParts);
                              ulTotal = atol(szFilePartsTotal);  */
                              ulPart = 1;

                              if ( ((pszFilePartBegin - 1) - pszTemp) > 0)
                              {
                                 strncpy(szFileName, pszTemp, (pszFilePartBegin - 1) - pszTemp);
                                 szFileName[(pszFilePartBegin - 1) - pszTemp] = 0x00;
                              }
                              else
                                 strcpy(szFileName,"");
                           }
                        }
                     }
                     break;
                  }
               }

               if ((strlen(szFileName) > 0) && (ulPart > 0))
               {

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

/*                   printf("NNTP POWER-POST 2000\r\n"); */
/*                     printf("%s\r\n", szProgramVer);  */
                     if (ulTotal > 1)
                     {
                        sprintf(szNewFileName,"%s\\%s.NNTP2000%d",pszNewsGroup, szFileName, ulPart);
/*                        printf("Saving Multipart %s, part %d of %d\r\n",szFileName, ulPart,ulTotal);  */
                     }
                     else
                     {
                        if (ulPart > 0)
                        {
                           sprintf(szNewFileName,"%s\\%s",pszNewsGroup,szFileName);
/*                           printf("Saving %s ",szFileName);  */
                        }
                     }
                     fflush(NULL);

                     if (ulPart > 0)
                     {

                        rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                           OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                           OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                        if (rc != 0)
                        {
                           if (strlen(szNewFileName) > 4)
                           {
                              if (stricmp(&szNewFileName[strlen(szNewFileName) - 4], ".jpg") == 0)
                              {
                                 ULONG ulNewNum = 1;
                                 CHAR szNewNumFileName[256];

                                 szNewFileName[strlen(szNewFileName) - 4] = 0x00;
                                 while (rc != 0)
                                 {
                                    sprintf(szNewNumFileName,"%s-%d.jpg",szNewFileName, ulNewNum++);
                                    if (ulNewNum > 100)
                                       sprintf(szNewNumFileName,"%s\\Test-%d.jpg",pszNewsGroup,ulNewNum++);

                                    rc = DosOpen(szNewNumFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                       OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS,
                                       OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                    DosSleep(100);
                                    if (rc == 0)
                                       strcpy(szNewFileName, szNewNumFileName);
                                 }
                              }
                              else
                                 rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                    OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                           }
                           else
                              rc = DosOpen(szNewFileName,&hfOutFile,&action,0L,FILE_NORMAL,
                                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                        }
                        if (rc == 0)
                        {
                           PSZ pszDataBegin;
                           PSZ pszDataEnd;

                           pszDataBegin = SearchFor("\r\n",pszBody, ulBodyLen, pszDataBegin);
                           if (pszDataBegin != NULL)
                           {

                              pszDataBegin += 2;
                              pszDataEnd = SearchFor("\r\n.",pszDataBegin, ulBodyLen - (pszDataBegin - pszBody), pszDataEnd);

                              if (pszDataEnd != NULL)
                              {
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
                              CHAR szMergeFileName[256];

                              sprintf(szFindFile,"%s\\%s.NNTP2000*",pszNewsGroup,szFileName);

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

                              if (ulNumFiles == ulTotal)
                              {
                                 HFILE hfOutFile;

/*                                 printf("Merging NNTP POWER-POST 2000 file %s ", szFileName);  */
                                 fflush(NULL);
                                 sprintf(szMergeFileName,"%s\\%s",pszNewsGroup,szFileName);
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

                                       sprintf(szNewFileName,"%s\\%s.NNTP2000%d",pszNewsGroup,szFileName, x);
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
                                 }
                              }

                           }
                           else
                           {
                              USHORT rc;

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

/*                                 rc = DosOpen("NNTPTemp.dat",&hfOutFile,&action,0L,FILE_NORMAL,
                                    OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L);
                                 if (rc == 0)
                                 {
                                    DosWrite(hfOutFile,pszHeader, ulHeaderLen, &BytesWritten);
                                    DosWrite(hfOutFile,pszBody, ulBodyLen, &BytesWritten);
                                    DosClose(hfOutFile);

                                 }
                                 exit(0); */
                           }
                        }
/*                        else
                           printf("- Failed t open file\r\n");  */

                     }

                     if (bLogs)
                        SaveLog(szLogFileName,pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup), szFileName);
                     if (strcmp(pszNewsGroup, szGroup) != 0)
                        strcpy(szGroup, pszNewsGroup);

                  }
                  return(1);
               }
               else
               {
                  if (ulPart == 0)
                  {
/*                     printf("NNTP Text Message - Ignoring\r\n");  */
                     if (bLogs)
                        SaveLog(szLogFileName,pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup), szFileName);
                     return(0);
                  }
                  else
                  {
/*                     printf("Invalid file name NNTP\r\n");  */
                     if (bLogs)
                        SaveLog(szLogFileName,pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup), szFileName);
                     return(0);
                  }
               }
            }
/*            else
               printf("Reply message - Ignoring\r\n");  */
         }
      }
      return(0);
   }
   else
   {
      pszNEWSPoster = SearchFor("X-Newsposter: NNTP POWER-POST 2000GE",pszHeader, ulHeaderLen, pszNEWSPoster);
      if (pszNEWSPoster != NULL)
      {
/*         printf("NNTP GE Version - Saving\r\n");  */
         if (bLogs)
            SaveLog("NNTP2000GE.LOG",pszHeader, ulHeaderLen, pszNewsGroup, strcmp(pszNewsGroup, szGroup1),"No Filename,Power-Post 2000GE");
         if (strcmp(pszNewsGroup, szGroup1) != 0)
            strcpy(szGroup1, pszNewsGroup);

/*         return(1);  */
      }

      return(0);
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

int SaveLog(PSZ pszLogFileName, PSZ pszHeader, ULONG ulHeaderLen, PSZ pszGroup, SHORT sWriteGroup, PSZ pszFileName)
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
            DosWrite(hfLog," -- ",4L,&BytesWritten);
            if (strlen(pszFileName) > 0)
               DosWrite(hfLog,pszFileName,strlen(pszFileName),&BytesWritten);
            else
               DosWrite(hfLog,"!No Name",8,&BytesWritten);
            DosWrite(hfLog,"\r\n",2L,&BytesWritten);
            DosClose(hfLog);
            break;
      }
   }
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
                     rc = DosQueryProcAddr(MMod,0,"UUDecode",(PFN *)&UUDecode);
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




