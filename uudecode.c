#define INCL_PM
#define INCL_DOSFILEMGR   /* File Manager values */
#define INCL_DOSERRORS    /* DOS error values */
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXTERN
#define STDC_HEADERS
#define SYSTEM_OS2
#define PROTOTYPES
#define HAVE_IO_H
#define HAVE_FCNTL_H
#include "uudecode.h"

#define DEC(c)  (((c) - ' ') & 077)             /* single character decode */
extern int _dllentry;

BOOL main(void )
{

   return(0);
}

/* int UULoadFile() replacement in NEWSROVER.C */

/* int UUDecode(PSZ pszBody, ULONG ulBodyLen, PSZ pszFileName, ULONG *ulFileSize, ULONG *ulStartPos, ULONG *ulEndPos, ULONG *ulPart, ULONG *ulTotal, PSZ *pszNewData, ULONG *ulDecolLen) */
int UUDecode(PSZ pszBody, ULONG ulBodyLen, PSZ pszFileName, ULONG *ulFileSize)
{
   int x = _dllentry;
   PSZ pszUUDecodeBegin;
   USHORT usFileLen;
   PSZ pszBegin;
   USHORT NoError = 0;
   CHAR szTemp[256];
   PSZ pszTemp;
   USHORT usMode;
   BOOL bNotEndOfDecode = TRUE;

   pszBegin = pszBody;
   *ulFileSize = 0;

   pszUUDecodeBegin = SearchFor("begin ",pszBody, ulBodyLen, pszUUDecodeBegin);
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
         while ((*pszUUDecodeBegin != '\r') && (usFileLen++ < 256))
            *(pszFileName++) = *(pszUUDecodeBegin++);

         *pszFileName = 0x00;

         if (usFileLen < 256)
         {
            int n;
            char ch, *p;
            int mode, n1;
            int iLPlace;
            PSZ pszEndLine;


            pszUUDecodeBegin += 2;

            do
            {
               if (pszUUDecodeBegin < pszBody + ulBodyLen)
               {

                  n = DEC(*pszUUDecodeBegin);
                  if (n > 0)
                  {

/*                     pszEndLine = pszUUDecodeBegin + 1;
                     while ((*pszEndLine != 0x0D) && (*pszEndLine != 0x0A) && (pszEndLine < pszBody + ulBodyLen))
                        pszEndLine++;  */

                     for (++pszUUDecodeBegin; n > 0 /*, pszUUDecodeBegin <pszEndLine */; pszUUDecodeBegin += 4, n -= 3)
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
                        NoError = 1;
                  }
                  else
                     bNotEndOfDecode = FALSE;
               }
               else
                  NoError = 2;

            }  while ((pszUUDecodeBegin < pszBody + ulBodyLen) && (NoError == 0) && (bNotEndOfDecode));
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

/*
#include "crc32.c"
#include "fptools.c"
#include "uucheck.c"
#include "uuencode.c"
#include "uulib.c"
#include "uunconc.c"
#include "uuscan.c"
#include "uustring.c"
#include "uuutil.c"  */




#ifdef NEVER

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)uudecode.c  5.10 (Berkeley) 6/1/90";
#endif /* not lint */

/*
 * uudecode [file ...]
 *
 * create the specified file, decoding as you go.
 * used with uuencode.
 */
#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>

char *filename;

/* ARGSUSED */
main(argc, argv)
        int argc;
        char **argv;
{
        extern int errno;
        int rval;

        if (*++argv) {
                rval = 0;
                do {
                        if (!freopen(filename = *argv, "r", stdin)) {
                                (void)fprintf(stderr, "uudecode: %s: %s\n",
                                    *argv, strerror(errno));
                                rval = 1;
                                continue;
                        }
                        rval |= decode();
                } while (*++argv);
        } else {
                filename = "stdin";
                rval = decode();
        }
        exit(rval);
}

decode()
{
        extern int errno;
        struct passwd *pw;
        register int n;
        register char ch, *p;
        int mode, n1;
        char buf[MAXPATHLEN];

        /* search for header line */
        do {
                if (!fgets(buf, sizeof(buf), stdin)) {
                        (void)fprintf(stderr,
                            "uudecode: %s: no \"begin\" line\n", filename);
                        return(1);
                }
        } while (strncmp(buf, "begin ", 6));
        (void)sscanf(buf, "begin %o %s", &mode, buf);

        /* handle ~user/file format */
        if (buf[0] == '~') {
                if (!(p = index(buf, '/'))) {
                        (void)fprintf(stderr, "uudecode: %s: illegal ~user.\n",
                            filename);
                        return(1);
                }
                *p++ = NULL;
                if (!(pw = getpwnam(buf + 1))) {
                        (void)fprintf(stderr, "uudecode: %s: no user %s.\n",
                            filename, buf);
                        return(1);
                }
                n = strlen(pw->pw_dir);
                n1 = strlen(p);
                if (n + n1 + 2 > MAXPATHLEN) {
                        (void)fprintf(stderr, "uudecode: %s: path too long.\n",
                            filename);
                        return(1);
                }
                bcopy(p, buf + n + 1, n1 + 1);
                bcopy(pw->pw_dir, buf, n);
                buf[n] = '/';
        }

        /* create output file, set mode */
        if (!freopen(buf, "w", stdout) ||
            fchmod(fileno(stdout), mode&0666)) {
                (void)fprintf(stderr, "uudecode: %s: %s: %s\n", buf,
                    filename, strerror(errno));
                return(1);
        }

        /* for each input line */
        for (;;) {
                if (!fgets(p = buf, sizeof(buf), stdin)) {
                        (void)fprintf(stderr, "uudecode: %s: short file.\n",
                            filename);
                        return(1);
                }
#define DEC(c)  (((c) - ' ') & 077)             /* single character decode */
                /*
                 * `n' is used to avoid writing out all the characters
                 * at the end of the file.
                 */
                if ((n = DEC(*p)) <= 0)
                        break;
                for (++p; n > 0; p += 4, n -= 3)
                        if (n >= 3) {
                                ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
                                putchar(ch);
                                ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
                                putchar(ch);
                                ch = DEC(p[2]) << 6 | DEC(p[3]);
                                putchar(ch);
                        }
                        else {
                                if (n >= 1) {
                                        ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
                                        putchar(ch);
                                }
                                if (n >= 2) {
                                        ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
                                        putchar(ch);
                                }
                                if (n >= 3) {
                                        ch = DEC(p[2]) << 6 | DEC(p[3]);
                                        putchar(ch);
                                }
                        }
        }
        if (!fgets(buf, sizeof(buf), stdin) || strcmp(buf, "end\n")) {
                (void)fprintf(stderr, "uudecode: %s: no \"end\" line.\n",
                    filename);
                return(1);
        }
        return(0);
}

usage()
{
        (void)fprintf(stderr, "usage: uudecode [file ...]\n");
        exit(1);
}

#endif

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


