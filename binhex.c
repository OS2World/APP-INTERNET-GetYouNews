Newsgroups: comp.sys.mac.comm
Subject: Re: I need to UNHQX in unix? Any pointers??
From: root@conexch.uucp (Larry Dighera)
Date: Fri, 16 Jun 1995 16:42:27 GMT
Lines: 913

In article <3rl9vr$d7r@news.xmission.com> robobob@xmission.xmission.com (Friendly Human Person) writes:
>Michael Coburn (s9409498@yallara.cs.rmit.EDU.AU) wrote:
>: Hi all,
>
>: I need to UNHQX a file on a unix system. I've been told that there is a
>: version of the source around that can be recompiled for Unix systems.
>: Does anybodyknow where this lives??
>
>I dont know where it lives, but archie should =), its called 'mcvert', if
>you cant find it with archie, let me know and I will give you a copy of
>the one I have.


#ifndef lint
static char version[] = "xbin.c Version 2.3 09/30/85";
#endif lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#ifdef MAXNAMLEN        /* 4.2 BSD */
#define FNAMELEN MAXNAMLEN
#else
#define FNAMELEN DIRSIZ
#endif

#ifdef BSD
#include <sys/time.h>
#include <sys/timeb.h>
#define search_last rindex
extern char *rindex();
#else
#include <time.h>
extern long timezone;
#define search_last strrchr
extern char *strrchr();
#endif

/* Mac time of 00:00:00 GMT, Jan 1, 1970 */
#define TIMEDIFF 0x7c25b080

#define DATABYTES 128

#define BYTEMASK 0xff
#define BYTEBIT 0x100
#define WORDMASK 0xffff
#define WORDBIT 0x10000

#define NAMEBYTES 63
#define H_NLENOFF 1
#define H_NAMEOFF 2

/* 65 <-> 80 is the FInfo structure */
#define H_TYPEOFF 65
#define H_AUTHOFF 69
#define H_FLAGOFF 73

#define H_LOCKOFF 81
#define H_DLENOFF 83
#define H_RLENOFF 87
#define H_CTIMOFF 91
#define H_MTIMOFF 95

#define H_OLD_DLENOFF 81
#define H_OLD_RLENOFF 85

#define F_BUNDLE 0x2000
#define F_LOCKED 0x8000

struct macheader {
        char m_name[NAMEBYTES+1];
        char m_type[4];
        char m_author[4];
        short m_flags;
        long m_datalen;
        long m_rsrclen;
        long m_createtime;
        long m_modifytime;
} mh;

struct filenames {
        char f_info[256];
        char f_data[256];
        char f_rsrc[256];
} files;

int pre_beta;   /* options */
int listmode;
int verbose;

int compressed; /* state variables */
int qformat;
FILE *ifp;

/*
 * xbin -- unpack BinHex format file into suitable
 * format for downloading with macput
 * Dave Johnson, Brown University Computer Science
 *
 * (c) 1984 Brown University
 * may be used but not sold without permission
 *
 * created ddj 12/16/84
 * revised ddj 03/10/85 -- version 4.0 compatibility, other minor mods
 * revised ddj 03/11/85 -- strip LOCKED bit from m_flags
 * revised ahm 03/12/85 -- System V compatibility
 * revised dba 03/16/85 -- (Darin Adler, TMQ Software)  4.0 EOF fixed,
 *                         4.0 checksum added
 * revised ddj 03/17/85 -- extend new features to older formats: -l, stdin
 * revised ddj 03/24/85 -- check for filename truncation, allow multiple files
 * revised ddj 03/26/85 -- fixed USG botches, many problems w/multiple files
 * revised jcb 03/30/85 -- (Jim Budler, amdcad!jimb), revised for compatibility
 *                         with 16-bit int machines
 * revised dl  06/16/85 -- (Dan LaLiberte, liberte@uiucdcs) character
 *                         translation speedup
 * revised ddj 09/30/85 -- fixed problem with run of RUNCHAR
 */
char usage[] = "usage: \"xbin [-v] [-l] [-o] [-n name] [-] filename\"\n";

main(ac, av)
char **av;
{
        char *filename, *macname;

        filename = ""; macname = "";
        ac--; av++;
        while (ac) {
                if (av[0][0] == '-') {
                        switch (av[0][1]) {
                        case '\0':
                                filename = "-";
                                break;
                        case 'v':
                                verbose++;
                                break;
                        case 'l':
                                listmode++;
                                break;
                        case 'o':
                                pre_beta++;
                                break;
                        case 'n':
                                if (ac > 1) {
                                        ac--; av++;
                                        macname = av[0];
                                        filename = "";
                                        break;
                                }
                                else
                                        goto bad_usage;
                        default:
                                goto bad_usage;
                        }
                }
                else
                        filename = av[0];
                if (filename[0] != '\0') {
                        setup_files(filename, macname);
                        if (listmode) {
                                print_header();
                        }
                        else {
                                process_forks();
                                /* now that we know the size of the forks */
                                forge_info();
                        }
                        if (ifp != stdin)
                                fclose(ifp);
                        macname = "";
                        ifp = NULL;             /* reset state */
                        qformat = 0;
                        compressed = 0;
                }
                ac--; av++;
        }
        if (*filename == '\0') {
bad_usage:
                fprintf(stderr, usage);
                exit(1);
        }
}

static char *extensions[] = {
        ".hqx",
        ".hcx",
        ".hex",
        "",
        NULL
};

setup_files(filename, macname)
char *filename;         /* input file name -- extension optional */
char *macname;          /* name to use on the mac side of things */
{
        char namebuf[256], *np;
        char **ep;
        int n;
        struct stat stbuf;
        long curtime;

        if (filename[0] == '-') {
                ifp = stdin;
                filename = "stdin";
        }
        else {
                /* find input file and open it */
                for (ep = extensions; *ep != NULL; ep++) {
                        sprintf(namebuf, "%s%s", filename, *ep);
                        if (stat(namebuf, &stbuf) == 0)
                                break;
                }
                if (*ep == NULL) {
                        perror(namebuf);
                        exit(-1);
                }
                ifp = fopen(namebuf, "r");
                if (ifp == NULL) {
                        perror(namebuf);
                        exit(-1);
                }
        }
        if (ifp == stdin) {
                curtime = time(0);
                mh.m_createtime = curtime;
                mh.m_modifytime = curtime;
        }
        else {
                mh.m_createtime = stbuf.st_mtime;
                mh.m_modifytime = stbuf.st_mtime;
        }
        if (listmode || verbose) {
                fprintf(stderr, "%s %s%s",
                        listmode ? "\nListing" : "Converting",
                        namebuf, listmode ? ":\n" : " ");
        }

        qformat = find_header(); /* eat mailer header &cetera, intuit format */

        if (qformat)
                do_q_header(macname);
        else
                do_o_header(macname, filename);

        /* make sure host file name doesn't get truncated beyond recognition */
        n = strlen(mh.m_name);
        if (n > FNAMELEN - 2)
                n = FNAMELEN - 2;
        strncpy(namebuf, mh.m_name, n);
        namebuf[n] = '\0';

        /* get rid of troublesome characters */
        for (np = namebuf; *np; np++)
                if (*np == ' ' || *np == '/')
                        *np = '_';

        sprintf(files.f_data, "%s.data", namebuf);
        sprintf(files.f_rsrc, "%s.rsrc", namebuf);
        sprintf(files.f_info, "%s.info", namebuf);
        if (verbose)
                fprintf(stderr, "==> %s.{info,data,rsrc}\n", namebuf);
}

/* print out header information in human-readable format */
print_header()
{
        char *ctime();

        printf("macname: %s\n", mh.m_name);
        printf("filetype: %.4s, ", mh.m_type);
        printf("author: %.4s, ", mh.m_author);
        printf("flags: 0x%x\n", mh.m_flags);
        if (qformat) {
                printf("data length: %ld, ", mh.m_datalen);
                printf("rsrc length: %ld\n", mh.m_rsrclen);
        }
        if (!pre_beta) {
                printf("create time: %s", ctime(&mh.m_createtime));
        }
}

process_forks()
{
        if (qformat) {
                /* read data and resource forks of .hqx file */
                do_q_fork(files.f_data, mh.m_datalen);
                do_q_fork(files.f_rsrc, mh.m_rsrclen);
        }
        else
                do_o_forks();
}

/* write out .info file from information in the mh structure */
forge_info()
{
        static char buf[DATABYTES];
        char *np;
        FILE *fp;
        int n;
        long tdiff;
        struct tm *tp;
#ifdef BSD
        struct timeb tbuf;
#else
        long bs;
#endif

        for (np = mh.m_name; *np; np++)
                if (*np == '_') *np = ' ';

        buf[H_NLENOFF] = n = np - mh.m_name;
        strncpy(buf + H_NAMEOFF, mh.m_name, n);
        strncpy(buf + H_TYPEOFF, mh.m_type, 4);
        strncpy(buf + H_AUTHOFF, mh.m_author, 4);
        put2(buf + H_FLAGOFF, mh.m_flags & ~F_LOCKED);
        if (pre_beta) {
                put4(buf + H_OLD_DLENOFF, mh.m_datalen);
                put4(buf + H_OLD_RLENOFF, mh.m_rsrclen);
        }
        else {
                put4(buf + H_DLENOFF, mh.m_datalen);
                put4(buf + H_RLENOFF, mh.m_rsrclen);

                /* convert unix file time to mac time format */
#ifdef BSD
                ftime(&tbuf);
                tp = localtime(&tbuf.time);
                tdiff = TIMEDIFF - tbuf.timezone * 60;
                if (tp->tm_isdst)
                        tdiff += 60 * 60;
#else
                /* I hope this is right! -andy */
                time(&bs);
                tp = localtime(&bs);
                tdiff = TIMEDIFF - timezone;
                if (tp->tm_isdst)
                        tdiff += 60 * 60;
#endif
                put4(buf + H_CTIMOFF, mh.m_createtime + tdiff);
                put4(buf + H_MTIMOFF, mh.m_modifytime + tdiff);
        }
        fp = fopen(files.f_info, "w");
        if (fp == NULL) {
                perror("info file");
                exit(-1);
        }
        fwrite(buf, 1, DATABYTES, fp);
        fclose(fp);
}

/* eat characters until header detected, return which format */
find_header()
{
        int c, at_bol;
        char ibuf[BUFSIZ];

        /* look for "(This file ...)" line */
        while (fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (strncmp(ibuf, "(This file", 10) == 0)
                        break;
        }
        at_bol = 1;
        while ((c = getc(ifp)) != EOF) {
                switch (c) {
                case '\n':
                case '\r':
                        at_bol = 1;
                        break;
                case ':':
                        if (at_bol)     /* q format */
                                return 1;
                        break;
                case '#':
                        if (at_bol) {   /* old format */
                                ungetc(c, ifp);
                                return 0;
                        }
                        break;
                default:
                        at_bol = 0;
                        break;
                }
        }

        fprintf(stderr, "unexpected EOF\n");
        exit(2);
        /* NOTREACHED */
}

static unsigned int crc;

short get2q();
long get4q();

/* read header of .hqx file */
do_q_header(macname)
char *macname;
{
        char namebuf[256];              /* big enough for both att & bsd */
        int n;
        unsigned int calc_crc, file_crc;

        crc = 0;                        /* compute a crc for the header */
        q_init();                       /* reset static variables */

        n = getq();                     /* namelength */
        n++;                            /* must read trailing null also */
        getqbuf(namebuf, n);            /* read name */
        if (macname[0] == '\0')
                macname = namebuf;

        n = strlen(macname);
        if (n > NAMEBYTES)
                n = NAMEBYTES;
        strncpy(mh.m_name, macname, n);
        mh.m_name[n] = '\0';

        getqbuf(mh.m_type, 4);
        getqbuf(mh.m_author, 4);
        mh.m_flags = get2q();
        mh.m_datalen = get4q();
        mh.m_rsrclen = get4q();

        comp_q_crc(0);
        comp_q_crc(0);
        calc_crc = crc;
        file_crc = get2q();
        verify_crc(calc_crc, file_crc);
}

do_q_fork(fname, len)
char *fname;
register long len;
{
        FILE *outf;
        register int c, i;
        unsigned int calc_crc, file_crc;

        outf = fopen(fname, "w");
        if (outf == NULL) {
                perror(fname);
                exit(-1);
        }

        crc = 0;        /* compute a crc for a fork */

        if (len)
                for (i = 0; i < len; i++) {
                        if ((c = getq()) == EOF) {
                                fprintf(stderr, "unexpected EOF\n");
                                exit(2);
                        }
                        putc(c, outf);
                }

        comp_q_crc(0);
        comp_q_crc(0);
        calc_crc = crc;
        file_crc = get2q();
        verify_crc(calc_crc, file_crc);
        fclose(outf);
}

/* verify_crc(); -- check if crc's check out */
verify_crc(calc_crc, file_crc)
unsigned int calc_crc, file_crc;
{
        calc_crc &= WORDMASK;
        file_crc &= WORDMASK;

        if (calc_crc != file_crc) {
                fprintf(stderr, "CRC error\n---------\n");
                fprintf(stderr, "CRC in file:\t0x%x\n", file_crc);
                fprintf(stderr, "calculated CRC:\t0x%x\n", calc_crc);
                exit(3);
        }
}

static int eof;
static char obuf[3];
static char *op, *oend;

/* initialize static variables for q format input */
q_init()
{
        eof = 0;
        op = obuf;
        oend = obuf + sizeof obuf;
}

/* get2q(); q format -- read 2 bytes from input, return short */
short
get2q()
{
        register int c;
        short value = 0;

        c = getq();
        value = (c & BYTEMASK) << 8;
        c = getq();
        value |= (c & BYTEMASK);

        return value;
}

/* get4q(); q format -- read 4 bytes from input, return long */
long
get4q()
{
        register int c, i;
        long value = 0L;

        for (i = 0; i < 4; i++) {
                c = getq();
                value <<= 8;
                value |= (c & BYTEMASK);
        }
        return value;
}

/* getqbuf(); q format -- read n characters from input into buf */
/*              All or nothing -- no partial buffer allowed */
getqbuf(buf, n)
register char *buf;
register int n;
{
        register int c, i;

        for (i = 0; i < n; i++) {
                if ((c = getq()) == EOF)
                        return EOF;
                *buf++ = c;
        }
        return 0;
}

#define RUNCHAR 0x90

/* q format -- return one byte per call, keeping track of run codes */
getq()
{
        register int c;

        if ((c = getq_nocrc()) == EOF)
                return EOF;
        comp_q_crc((unsigned)c);
        return c;
}

getq_nocrc()
{
        static int rep, lastc;
        int c;

        if (rep) {
                rep--;
                return lastc;
        }
        if ((c = getq_raw()) == EOF) {
                return EOF;
        }
        if (c == RUNCHAR) {
                if ((rep = getq_raw()) == EOF)
                        return EOF;
                if (rep != 0) {
                        /* already returned one, about to return another */
                        rep -= 2;
                        return lastc;
                }
                else {
                        lastc = RUNCHAR;
                        return RUNCHAR;
                }
        }
        else {
                lastc = c;
                return c;
        }
}

/* q format -- return next 8 bits from file without interpreting run codes */
getq_raw()
{
        char ibuf[4];
        register char *ip = ibuf, *iend = ibuf + sizeof ibuf;
        int c;

        if (op == obuf) {
                for (ip = ibuf; ip < iend; ip++) {
                        if ((c = get6bits()) == EOF)
                                if (ip <= &ibuf[1])
                                        return EOF;
                                else if (ip == &ibuf[2])
                                        eof = 1;
                                else
                                        eof = 2;
                        *ip = c;
                }
                obuf[0] = (ibuf[0] << 2 | ibuf[1] >> 4);
                obuf[1] = (ibuf[1] << 4 | ibuf[2] >> 2);
                obuf[2] = (ibuf[2] << 6 | ibuf[3]);
        }
        if ((eof) & (op >= &obuf[eof]))
                return EOF;
        c = *op++;
        if (op >= oend)
                op = obuf;
        return (c & BYTEMASK);
}

/*
char tr[] = "!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";
             0 123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
             0                1               2               3
trlookup is used to translate by direct lookup.  The input character
is an index into trlookup.  If the result is 0xFF, a bad char has been read.
Added by:  Dan LaLiberte, liberte@uiucdcs.Uiuc.ARPA, ihnp4!uiucdcs!liberte
*/
char trlookup[83] = {   0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFF, 0xFF,
                        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0xFF,
                        0x14, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
                        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0xFF,
                        0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0xFF,
                        0x2C, 0x2D, 0x2E, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xFF,
                        0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0xFF, 0xFF,
                        0x3D, 0x3E, 0x3F };

/* q format -- decode one byte into 6 bit binary */
get6bits()
{
        register int c;
        register int tc;

        while (1) {
                c = getc(ifp);
                switch (c) {
                case '\n':
                case '\r':
                        continue;
                case ':':
                case EOF:
                        return EOF;
                default:
                        tc = ((c-' ') < 83) ? trlookup[c-' '] : 0xff;
/*                      fprintf(stderr, "c = '%c'  tc = %4x\n", c, tc); */
                        if (tc != 0xff)
                                return (tc);
                        fprintf(stderr, "bad char: '%c'\n", c);
                        return EOF;
                }
        }
}


#define CRCCONSTANT 0x1021

comp_q_crc(c)
register unsigned int c;
{
        register int i;
        register unsigned long temp = crc;

        for (i=0; i<8; i++) {
                c <<= 1;
                if ((temp <<= 1) & WORDBIT)
                        temp = (temp & WORDMASK) ^ CRCCONSTANT;
                temp ^= (c >> 8);
                c &= BYTEMASK;
        }
        crc = temp;
}

/* old format -- process .hex and .hcx files */
do_o_header(macname, filename)
char *macname, *filename;
{
        char namebuf[256];              /* big enough for both att & bsd */
        char ibuf[BUFSIZ];
        int n;

        /* set up name for output files */
        if (macname[0] == '\0') {
                strcpy(namebuf, filename);

                /* strip directories */
                macname = search_last(namebuf, '/');
                if (macname == NULL)
                        macname = namebuf;
                else
                        macname++;

                /* strip extension */
                n = strlen(macname);
                if (n > 4) {
                    n -= 4;
                    if (macname[n] == '.' && macname[n+1] == 'h'
                                            && macname[n+3] == 'x')
                            macname[n] = '\0';
                }
        }
        n = strlen(macname);
        if (n > NAMEBYTES)
                n = NAMEBYTES;
        strncpy(mh.m_name, macname, n);
        mh.m_name[n] = '\0';

        /* read "#TYPEAUTH$flag"  line */
        if (fgets(ibuf, BUFSIZ, ifp) == NULL) {
                fprintf(stderr, "unexpected EOF\n");
                exit(2);
        }
        n = strlen(ibuf);
        if (n >= 7 && ibuf[0] == '#' && ibuf[n-6] == '$') {
                if (n >= 11)
                        strncpy(mh.m_type, &ibuf[1], 4);
                if (n >= 15)
                        strncpy(mh.m_author, &ibuf[5], 4);
                sscanf(&ibuf[n-5], "%4hx", &mh.m_flags);
        }
}

do_o_forks()
{
        char ibuf[BUFSIZ];
        int forks = 0, found_crc = 0;
        unsigned int calc_crc, file_crc;
        extern long make_file();


        crc = 0;        /* calculate a crc for both forks */

        /* create empty files ahead of time */
        close(creat(files.f_data, 0666));
        close(creat(files.f_rsrc, 0666));

        while (!found_crc && fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (forks == 0 && strncmp(ibuf, "***COMPRESSED", 13) == 0) {
                        compressed++;
                        continue;
                }
                if (strncmp(ibuf, "***DATA", 7) == 0) {
                        mh.m_datalen = make_file(files.f_data, compressed);
                        forks++;
                        continue;
                }
                if (strncmp(ibuf, "***RESOURCE", 11) == 0) {
                        mh.m_rsrclen = make_file(files.f_rsrc, compressed);
                        forks++;
                        continue;
                }
                if (compressed && strncmp(ibuf, "***CRC:", 7) == 0) {
                        found_crc++;
                        calc_crc = crc;
                        sscanf(&ibuf[7], "%x", &file_crc);
                        break;
                }
                if (!compressed && strncmp(ibuf, "***CHECKSUM:", 12) == 0) {
                        found_crc++;
                        calc_crc = crc & BYTEMASK;
                        sscanf(&ibuf[12], "%x", &file_crc);
                        file_crc &= BYTEMASK;
                        break;
                }
        }

        if (found_crc)
                verify_crc(calc_crc, file_crc);
        else {
                fprintf(stderr, "missing CRC\n");
                exit(3);
        }
}

long
make_file(fname, compressed)
char *fname;
/*
 * This file is part of uudeview, the simple and friendly multi-part multi-
 * file uudecoder  program  (c) 1994-2001 by Frank Pilhofer. The author may
 * be contacted at fp@fpx.de
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SYSTEM_WINDLL
#include <windows.h>
#endif
#ifdef SYSTEM_OS2
#include <os2.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <uudeview.h>
#include <fptools.h>
#include <uufnflt.h>

/* For the sake of getcwd() in DOS */
#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif

/*
 * the progress meter is only displayed if stderr is a terminal.
 * take care of systems where we can't check this condition.
 */

#ifdef HAVE_ISATTY
#define UUISATTY(f)  (isatty(fileno(f)))
#else
#define UUISATTY(f)  (1)
#endif

#define SHOW_INFO (1)   /* For more () */
#define SHOW_FILE (2)

/*
 * local definitions
 */

typedef struct _extlist {
  char extension[8];
  struct _extlist *NEXT;
} extlist;

/*
 * Operational Flags
 */

static int incext    = 0, stdinput = 0;
static int overwrite = 0, interact = 1;
static int quiet     = 0, decoall  = 0;
static int nobar     = 0, autoren  = 0;

/*
 * Global Variables
 */

static extlist *extensions = NULL;
static char savepath[512];

/*
 * some statistics
 */

static int decodedok     = 0;
static int decodedfailed = 0;
static int decodedmisp   = 0;
static int inputerrors   = 0;
static int inputfiles    = 0;

/*
 * version string
 */

static char uuversion[256] = VERSION "pl" PATCH;

/*
 * cvs version
 */
char * uudeview_id = "$Id: uudeview.c,v 1.25 2002/03/06 13:52:46 fp Exp $";

static int  addexts     _ANSI_ARGS_((char *));
static int  work_file   _ANSI_ARGS_((char *));
static int  proc_stdin  _ANSI_ARGS_((void));
static void loadparfile _ANSI_ARGS_((char *));
static int  work_comline   _ANSI_ARGS_((int, char *[]));
static int  moreCB      _ANSI_ARGS_((void *, char *));
static void more     _ANSI_ARGS_((uulist *, int));
static void DumpFileList   _ANSI_ARGS_((void));
static int  process_one _ANSI_ARGS_((uulist *));
static int  process_files  _ANSI_ARGS_((void));
static void sighandler  _ANSI_ARGS_((int));
static void usage    _ANSI_ARGS_((char *));

/*
 * Header lines starting with these texts will be stripped when
 * printing a file's info
 */

static char *hateheaders[] = {
  "Path:", "Newsgroups:", /*"Organization:",*/ "Lines:",
  "Message-ID:", "NNTP-Posting-Host:", "Xref:",
  "References:", "X-Newsreader:", "Distribution",
  /*"Sender:",*/ "Nntp-Posting-Host:", /*"Reply-To:",*/
  /*"Approved:",*/ "Mime-Version:", "Content-Type:",
  "Content-Transfer-Encoding:", "X-Posting-Software:",
  NULL
};

/*
 * Busy Callback
 */

static int
BusyCallback (void *param, uuprogress *progress)
{
  char stuff[26];
  int count, pcts;
  char *ptr;

  /*
   * Display progress meter only if stderr is a terminal
   */

  if (!UUISATTY(stderr) || nobar)
    return 0;

  if ((ptr = _FP_strrchr (progress->curfile, DIRSEPARATOR[0])) == NULL)
    ptr = progress->curfile;
  else
    ptr++;

  if (!quiet && progress->action == 1) {
    fprintf (stderr, "scanned %3d%% of %-50s\r",
        progress->percent, ptr);
    fflush  (stderr);
  }
  else if (!quiet && progress->action == 2) {
    pcts = (int)((100*progress->partno+progress->percent-100) /
       progress->numparts);
    for (count=0, stuff[25]='\0'; count<25; count++)
      stuff[count] = (count<pcts/4)?'#':'.';
    fprintf (stderr, "decoding part%3d of%3d %s\r",
        progress->partno, progress->numparts,
        stuff);
    fflush  (stderr);
  }
  else if (!quiet && progress->action == 3) {
    for (count=0, stuff[25]='\0'; count<25; count++)
      stuff[count] = (count<progress->percent/4)?'#':'.';
    fprintf (stderr, "copying target file      %s\r",
        stuff);
    fflush  (stderr);
  }
  return 0;
}

/*
 * Message Callback
 */

static void
MessageCallback (void *param, char *message, int level)
{
  if (!quiet || level >= UUMSG_WARNING)
    fprintf (stderr, "%s\n", message);
}

/*
 * local functions
 */

static void
killext (extlist *data)
{
  extlist *iter=data, *ptr;

  while (iter) {
    ptr = iter->NEXT;
    _FP_free (iter);
    iter = ptr;
  }
}

static char *
filemode (int mode)
{
  static char result[11];
  static char rwxkey[8][4] =
    { "---", "--x", "-w-", "-wx",
      "r--", "r-x", "rw-", "rwx" };

  result[0] = '-';
  result[1] = rwxkey[((mode >> 6) & 0x07)][0];
  result[2] = rwxkey[((mode >> 6) & 0x07)][1];
  result[3] = rwxkey[((mode >> 6) & 0x07)][2];
  result[4] = rwxkey[((mode >> 3) & 0x07)][0];
  result[5] = rwxkey[((mode >> 3) & 0x07)][1];
  result[6] = rwxkey[((mode >> 3) & 0x07)][2];
  result[7] = rwxkey[  mode       & 0x07 ][0];
  result[8] = rwxkey[  mode       & 0x07 ][1];
  result[9] = rwxkey[  mode       & 0x07 ][2];
  result[10]= '\0';

  return result;
}

/*
 * check if we want the header displayed
 */

static int
IsHeaderLine (char *text)
{
  char **hiter = hateheaders;

  if (text == NULL || hiter == NULL)
    return 0;

  while (*hiter) {
    if (_FP_strnicmp (text, *hiter, strlen (*hiter)) == 0)
      return 1;
    hiter++;
  }
  return 0;
}

/*
 * get a character from stdin (the keyboard), or accept default
 */

static char
getyn (char *theline, char def)
{
  char line[256], *ptr=0;
  int isdebug;

  if (interact && !feof (stdin)) {
    fflush (stdout);
    if ((ptr = _FP_fgets ((theline)?theline:line, 255, stdin)) == NULL) {
      if (theline) *theline='\0';
      return def;
    }
    if (ptr[0]=='4'&&ptr[1]=='2') {
      UUGetOption (UUOPT_DEBUG, &isdebug, NULL, 0);
      UUSetOption (UUOPT_DEBUG, !isdebug, NULL);
      if (!isdebug)
   printf ("*** Debugging Mode On\n");
      else
   printf ("*** Debugging Mode Off\n");
      ptr+=2;
    }
    while (isspace (*ptr) && *ptr!='\n')
      ptr++;
    if (*ptr == '\n') {
      if (theline) *theline='\0';
      return def;
    }
  }
  else {
    if (theline) *theline='\0';
    printf ("%c\n", def);
    return def;
  }
  return tolower(*ptr);
}

/*
 * check whether a file exists or not. This check is used when we decide
 * if a file would be overwritten. We also overwrite files with zero
 * length.
 */

static int
exists (char *filename)
{
  struct stat finfo;

  if (filename == NULL || *filename == '\0')
    return 0;

  if (stat (filename, &finfo) != 0)
    return 0;

  if ((long) finfo.st_size == 0)
    return 0;

  return 1;
}

/*
 * Find a new unique filename by appending a dot and a unique number
 * to the name.
 */

int
findAlternateName (char *originalName, char *newName)
{
  int i;

  for (i=1; i<32768; i++) {
    sprintf (newName, "%s.%d", originalName, i);
    if (!exists (newName)) {
      return 1;
    }
  }

  return 0;
}

/*
 * Add another extension we shall or shall not decode
 */

static int
addexts (char *elist)
{
  extlist *enew;
  int iter;

  while (*elist) {
    if (*elist != '.') {
      elist++;
      continue;
    }
    if (*++elist == '\0')
      break;

    if ((enew = (extlist *) malloc (sizeof (extlist))) == NULL) {
      fprintf (stderr, "ERROR: Out of memory in addexts()\n");
      return 0;
    }
    iter = 0;

    while (*elist != '.' && *elist != '\0' && iter < 7)
      enew->extension[iter++] = *elist++;

    enew->extension[iter] = '\0';

    enew->NEXT = extensions;
    extensions = enew;
  }
  return 1;
}

/*
 * looks at the extension of a file and decides whether we want to decode
 * it or not, based on our extensions list
 */

static int
work_file (char *filename)
{
  extlist *iter = extensions;
  char *ptr;

  if (filename == NULL)
    return 0;

  if ((ptr = _FP_strrchr (filename, '.')) == NULL)
    return incext ? 0 : 1;

  ptr++;

  while (iter) {
    if (_FP_stricmp (ptr, iter->extension) == 0)
      return incext ? 1 : 0;

    iter = iter->NEXT;
  }

  return incext ? 0 : 1;
}

/*
 * Handle standard input. Dump it into a temporary file and then add
 * this temp file to our list of command-line files
 */

static int
proc_stdin (void)
{
  static char buffer[1024];
  char *stdfile;
  FILE *target;
  size_t bytes;
  int res;

  if (stdinput) {
    fprintf (stderr, "proc_stdin: cannot process stdin twice\n");
    return 0;
  }

  if ((stdfile = tempnam (NULL, "uu")) == NULL) {
    fprintf (stderr, "proc_stdin: cannot get temporary file\n");
    return 0;
  }

  if ((target = fopen (stdfile, "wb")) == NULL) {
    fprintf (stderr, "proc_stdin: cannot open temp file %s for writing: %s\n",
        stdfile, strerror (errno));
    _FP_free (stdfile);
    return 0;
  }

  while (!feof (stdin)) {
    bytes = fread (buffer, 1, 1024, stdin);

    if (bytes == 0)
      break;

    if (ferror (stdin)) {
      fprintf (stderr, "proc_stdin: error reading from stdin: %s\n",
          strerror (errno));
      break;
    }

    if (fwrite (buffer, 1, bytes, target) != bytes) {
      fprintf (stderr, "proc_stdin: cannot write to temp file %s: %s\n",
          stdfile, strerror (errno));
      break;
    }

    if (ferror (target)) {
      fprintf (stderr, "proc_stdin: error writing to temp file %s: %s\n",
          stdfile, strerror (errno));
      break;
    }
  }

  if (ferror (stdin) || ferror (target)) {
    fclose (target);
    unlink (stdfile);
    _FP_free (stdfile);
    return 0;
  }
  fclose (target);

  inputfiles++;
  if ((res = UULoadFile (stdfile, NULL, 1)) != UURET_OK) {
    fprintf (stderr, "ERROR: while reading from copied standard input %s: %s %s\n",
        stdfile, UUstrerror (res),
        (res==UURET_IOERR)?
        strerror (UUGetOption (UUOPT_ERRNO, NULL, NULL, 0)) : "");
    inputerrors = 1;
  }
  _FP_free (stdfile);
  stdinput = 1;

  return 1;
}

/*
 * extract parameters from "command line"
 */

static void
makeparams (char *comline)
{
  char *p1, *argv[32];
  int argc=2, quote=0;

  if (comline==NULL || *comline=='\0')
    return;

  p1 = comline;

  while (*p1 == ' ' || *p1 == '\t') {
    p1++;
  }

  argv[1] = p1;

  while (*p1 && argc < 31) {
    switch (*p1) {
    case ' ':
    case '\t':
      if (!quote) {
   if ((*argv[argc-1] == '"' || *argv[argc-1] == '\'') &&
       p1-1 != argv[argc-1] &&
       *(p1-1) == *argv[argc-1]) {
     *(p1-1) = '\0';
     argv[argc-1] += 1;
   }
   *p1++ = '\0';
   while (*p1 == ' ' || *p1 == '\t')
     p1++;
   argv[argc++] = p1;
      }
      else {
   p1++;
      }
      break;

    case '"':
    case '\'':
      if (!quote) {
   quote = *p1++;
      }
      else if (quote == *p1++) {
   quote = 0;
      }
      break;

    default:
      p1++;
    }
  }

  if (!quote) {
    if ((*argv[argc-1] == '"' || *argv[argc-1] == '\'') &&
   p1-1 != argv[argc-1] &&
   *(p1-1) == *argv[argc-1]) {
      *(p1-1) = '\0';
      argv[argc-1] += 1;
    }
  }

  work_comline (argc, argv);
}

/*
 * Load a file with command-line parameters (given with @)
 */

static void
loadparfile (char *filename)
{
  char line[256], c;
  FILE *pfile;

  if ((pfile = fopen (filename+1, "r")) == NULL) {
    fprintf (stderr, "Couldn't load parameter file %s: %s (ignored)\n",
        filename+1, strerror (errno));
    return;
  }

  while (!feof (pfile)) {
    if (_FP_fgets (line, 256, pfile) == NULL)
      break;

    if (ferror (pfile))
      break;

    line[strlen(line)-1] = '\0';

    if (strlen (line) == 0)
      continue;

    if (line[strlen(line)-1] == 0x0a || line[strlen(line)-1] == 0x0d)
        line[strlen(line)-1] = '\0';

    if (line[0] == '\0' || line[0] == '#')
        continue;

    makeparams (line);

    c = fgetc (pfile);
    if (feof (pfile))
      break;
    else
      ungetc (c, pfile);
  }

  fclose (pfile);

  /*
   * command line files are always removed
   */

  unlink (filename+1);
}

/*
 * process command line parameters
 */

static int
work_comline (int argc, char *argv[])
{
  int number, res;

  for (number=1; number<argc; number++) {
    if (*argv[number] == '@')
      loadparfile (argv[number]);
    else if (*argv[number] != '-' && *argv[number] != '+') {
      inputfiles++;
      if ((res = UULoadFile ((*argv[number]=='|')?argv[number]+1:argv[number],
              NULL,
              (*argv[number]=='|')?1:0)) != UURET_OK) {
   fprintf (stderr, "ERROR: while reading from %s: %s %s\n",
       (*argv[number]=='|')?argv[number]+1:argv[number],
       UUstrerror (res),
       (res==UURET_IOERR)?
       strerror (UUGetOption (UUOPT_ERRNO, NULL, NULL, 0)) : "");
   inputerrors++;
      }
    }
    else switch (*(argv[number] + 1)) {
    case '\0':
      interact = 0;
      if (overwrite == 0) {
   overwrite = 1;
      }
      proc_stdin ();
      break;
    case 'a':
      autoren = 1;
      break;
    case 'b':
      if (argv[number][2] == '0' || argv[number][2] == '(')
   UUSetOption (UUOPT_BRACKPOL, 0, NULL);
      else if (argv[number][2] == '1' || argv[number][2] == '[')
   UUSetOption (UUOPT_BRACKPOL, 1, NULL);
      else if (argv[number][2] == '\0')
   UUSetOption (UUOPT_BRACKPOL, 1, NULL);
      /* '])' Emacs feels happier if I close these brackets */
      break;
    case 'c':
      UUSetOption (UUOPT_REMOVE, 1, NULL);
      break;
    case 'd':
      UUSetOption (UUOPT_DESPERATE, 1, NULL);
      UUSetOption (UUOPT_TINYB64,   1, NULL);
      break;
    case 'e':
      if (number+1 < argc && *argv[number+1] == '.') {
        incext = (*argv[number] == '+') ? 1 : 0;
        addexts (argv[++number]);
      }
      break;
    case 'f':
      UUSetOption (UUOPT_FAST, 1, NULL);
      break;
    case 'h':
      usage (argv[0]);
      exit (99);
      break;
    case 'i':
      if (*argv[number] == '+' && stdinput) {
   fprintf (stderr, "WARNING: cannot interact when reading from stdin\n");
      }
      else {
   interact  = (*argv[number] == '+') ? 1 : 0;
   if (overwrite == 0 && *argv[number] == '-') {
     overwrite = 1;
   }
      }
      break;
    case 'm':
      UUSetOption (UUOPT_IGNMODE, 1, NULL);
      break;
    case 'n':
      nobar = 1;
      break;
    case 'o':
      if (*argv[number] == '-') {
   overwrite = 1;
      }
      else {
   overwrite = -1;
      }
      break;
    case 'p':
      if (number+1 < argc) {
        strcpy (savepath, argv[++number]);
   if (strlen (savepath)) {
     if (savepath[strlen(savepath)-1]!=DIRSEPARATOR[0])
       strcat (savepath, DIRSEPARATOR);
   }
      }
      break;
    case 'q':
      UUSetOption (UUOPT_VERBOSE, 0, NULL);
      quiet = 1;
      nobar = 1;
      break;
    case 'r':
      UUSetOption (UUOPT_IGNREPLY, 1, NULL);
      break;
    case 's':
      if (argv[number][2] == '\0')
   UUSetOption (UUOPT_DUMBNESS,
           UUGetOption (UUOPT_DUMBNESS, NULL, NULL, 0) + 1,
           NULL);
      else
   UUSetOption (UUOPT_DUMBNESS, 42, NULL);
      break;
    case 't':
      UUSetOption (UUOPT_USETEXT, 1, NULL);
      break;
    case 'v':
      UUSetOption (UUOPT_VERBOSE, 0, NULL);
      break;
    case 'V':
      fprintf (stdout, "uudeview %spl%s compiled on %s\n",
          VERSION, PATCH, __DATE__);
      exit (0);
      break;
    case 'z':
      if (argv[number][2] >= '0' && argv[number][2] <= '9') {
   UUSetOption (UUOPT_MOREMIME, argv[number][2]-'0', NULL);
      }
      else if (argv[number][2] == 'z') {
   UUSetOption (UUOPT_MOREMIME, 2, NULL);
      }
      else {
   res = UUGetOption (UUOPT_MOREMIME, NULL, NULL, 0);
   UUSetOption (UUOPT_MOREMIME, res+1, NULL);
      }
      break;
    default:
      usage (argv[0]);
      exit (99);
      break;
    }
  }

  return 1;
}

/*
 * list a file callback
 */

struct mCBparm {
  int aline, lines;
  int quitit, cols;
};

static int
moreCB (void *param, char *string)
{
  struct mCBparm *data = (struct mCBparm *) param;

  if (IsHeaderLine (string))
    return 0;

  if (data->aline+2 >= data->lines) {
    data->aline = 0;
    if (interact) {
      fprintf (stdout, "<<MORE -- (q) to quit>>");
      if (getyn (NULL, 'y') == 'q') {
   data->quitit = 1;
   return 1;
      }
    }
  }

  while (strlen(string) &&
    (string[strlen(string)-1] == '\012' ||
     string[strlen(string)-1] == '\015'))
    string[strlen(string)-1] = '\0';

  if (data->cols > 0)
    string[data->cols-1] = '\0';

  fprintf (stdout, "%s\n", string);
  data->aline += 1;

  return 0;
}

static void
more (uulist *uin, int type)
{
  struct mCBparm data;
  FILE *inpfile = NULL;
  char text[256], *p;
  int res;

  data.aline  = 0;
  data.quitit = 0;

  if ((p = getenv ("LINES")) != NULL) {
    if ((data.lines = atoi(p)) < 5)
      data.lines = 24;
  }
  else
    data.lines = 24;

  if ((p = getenv ("COLUMNS")) != NULL) {
    if ((data.cols = atoi(p)) < 30)
      data.cols = 80;
  }
  else
    data.cols = 80;

  if (uin == NULL || uin->thisfile == NULL) {
    printf ("\tError -- (?)\n");
    return;
  }
  if (type == SHOW_INFO) {
    UUInfoFile (uin, &data, moreCB);
  }
  else {
    if ((res = UUDecodeToTemp (uin)) != UURET_OK) {
      fprintf (stderr, "ERROR: while decoding %s (%s): %s\n",
          uin->filename, (uin->subfname) ? uin->subfname : "",
          UUstrerror(res));
      return;
    }
    if (UUISATTY(stderr) && !nobar) {
      fprintf (stderr, "%70s\r", ""); /* clear progress information */
      fflush  (stderr);
    }

    if ((inpfile = fopen (uin->binfile, "r")) == NULL) {
      fprintf (stderr, "ERROR: could not open %s: %s\n",
          uin->binfile, strerror (errno));
      return;
    }

    while (!feof (inpfile)) {
      if (_FP_fgets (text, data.cols, inpfile) == NULL)
   break;

      if (ferror (inpfile)) {
   fprintf (stderr, "ERROR: while reading from %s: %s\n",
       uin->binfile, strerror (errno));
   break;
      }

      if (moreCB (&data, text))
   break;
    }
    fclose (inpfile);
  }

  if (interact && data.aline+2 >= data.lines) {
    fprintf (stdout, "<<END -- hit return>>");
    getyn   (NULL, '?');
  }
  else
    fprintf (stdout, "<<END>>");
}

static void
DumpFileList (void)
{
  int count, printed=0, index=0;
  uulist *iter;

  while ((iter=UUGetFileListItem(index))) {
    if (iter->state & UUFILE_NODATA) {
      index++;
      continue;
    }

    if (UUISATTY(stderr) && !nobar) {
      fprintf (stderr, "\r%70s\r", "");
      fflush  (stderr);
    }

    if (!printed++)
      printf ("\n");

    printf ("Found '%s' State %d %s Parts ",
       (iter->filename) ? iter->filename :
       (iter->subfname) ? iter->subfname : "???",
       iter->state,
       (iter->uudet == UU_ENCODED) ? "UUdata" :
       (iter->uudet == B64ENCODED) ? "Base64" :
       (iter->uudet == XX_ENCODED) ? "XXdata" :
       (iter->uudet == BH_ENCODED) ? "Binhex" :
       (iter->uudet == YENC_ENCODED) ? "yEnc" : "Text");

    if (iter->haveparts) {
      printf ("%s%d%s ",
         (iter->begin && iter->begin==iter->haveparts[0]) ? "begin " : "",
         iter->haveparts[0],
         (iter->end   && iter->end == iter->haveparts[0]) ? " end" : "");
      for (count=1; iter->haveparts[count]; count++) {
   printf ("%s%d%s ",
      (iter->begin==iter->haveparts[count]) ? "begin " : "",
      iter->haveparts[count],
      (iter->end == iter->haveparts[count]) ? " end"   : "");
      }
    }
    if (iter->state & UUFILE_OK) {
      printf ("OK");
    }
    printf ("\n");
    index++;
  }
  if (printed)
    printf ("\n");
}

/*
 * Returning  1 skips to next file
 *           -1 skips to prev file
 *            0 quits program
 */

static int
process_one (uulist *iter)
{
  char targetname[1024], renamedname[1024];
  char line[1024], command[256], tmp, *ptr1, *ptr2;
  int res, escflag;
  char *name;

  while (42) {
    if ((name = UUFNameFilter (iter->filename)) == NULL) {
      fprintf (stderr, "ERROR: couldn't get filename of %s (%s)\n",
          (iter->filename)?iter->filename:"(null)",
          (iter->subfname)?iter->subfname:"(null)");
      break;
    }

    if (interact && !decoall) {
      printf ("  %s %-15s is %s   [d] (?=help) ",
         filemode((int)iter->mode),
         (iter->filename) ? (iter->filename) : "",
         (iter->state&UUFILE_OK) ? "OK" : "in error (DESPERATE MODE)");
      tmp = getyn (line, 'd');
    }
    else {
      line[0] = '\0';
      tmp = 'd';
    }

    for (ptr1=line; *ptr1 && *ptr1 != tmp; ptr1++);
    if (*ptr1++==tmp) {
      while (isspace(*ptr1)) ptr1++;
    }

    if (tmp == 'n')
      return 1;
    if (tmp == 'b') {
      if (iter->PREV == NULL)
   printf ("*** Already at the beginning of the list\n");
      else
   return -1;
      continue;
    }
    else if (tmp == 'a') {
      decoall = 1;
      continue;
    }
    else if (tmp == 'c') {
      printf ("\n\n-------------------------------------------------------------------------------\n\n");
      printf ("\tYou are now using UUDEVIEW, the uudecoder by Frank Pilhofer\n\n");
      printf ("\tThe program  is  distributed  under  the  terms  of the GNU\n");
      printf ("\tGeneral Public License.  You should have received a copy of\n");
      printf ("\tthe GPL with the uudeview program. In particular this means\n");
      printf ("\tthat the program is distributed without any warranty.\n\n");
      printf ("\tIf you like uudeview, you are encouraged to send the author\n");
      printf ("\ta postcard (find  the address  in the  README  file), or at\n");
      printf ("\tleast an email.\n\n");
      printf ("\tCheck out uudeview's home page:\n");
      printf ("\t\thttp://www.fpx.de/fp/Software/UUDeview/\n\n");
      printf ("\t            Frank Pilhofer (fp@fpx.de)\n\n");
      printf ("-------------------------------------------------------------------------------\n\n");
      continue;
    }
    else if (tmp == 'q') {
      return 0;
    }
    else if (tmp == '?') {
      printf ("\n\tYou can ...\n");
      printf ("\t  - (d)ecode the file\n");
      printf ("\t  - (y)es - same as (d)\n");
      printf ("\t  - e(x)tract - same as (d)\n");
      printf ("\t  - decode (a)ll files\n");
      printf ("\t  - go to (n)ext file\n");
      printf ("\t  - show file (i)nfo\n");
      /*
       * Can't execute anything in QuickWin
       */
#ifndef SYSTEM_QUICKWIN
      printf ("\t  - (e)xecute a command\n");
#endif
      printf ("\t  - (l)ist textfile\n");
      printf ("\t  - (r)ename file\n");
      printf ("\t  - decode (a)ll files\n");
      printf ("\t  - go (b)ack to the previous file\n");
      printf ("\t  - set decode (p)ath: %s\n", savepath);
      printf ("\t  - (q)uit program\n");
      printf ("\t  - display (c)opyright\n\n");
      continue;
    }
    else if (tmp == 'r') {
      if (strlen(ptr1) <= 1) {
   printf ("Enter new filename: "); fflush (stdout);
   if (_FP_fgets (line, 250, stdin) == NULL) {
     printf ("\nERROR: Could not get filename: %s\n",
        strerror (errno));
     line[0] = '\0';
   }
   ptr1 = line;
      }
      if (strlen (ptr1) > 1) {
   if (ptr1[strlen(ptr1)-1] == '\n')
     ptr1[strlen(ptr1)-1] = '\0';
   UURenameFile (iter, ptr1);
      }
      continue;
    }
    else if (tmp == 'p') {
      if (strlen(ptr1) <= 1) {
   printf ("Enter new path: "); fflush (stdout);
   if (_FP_fgets (line, 250, stdin) == NULL) {
     printf ("\nERROR: Could not get path: %s\n", strerror (errno));
     line[0] = '\0';
   }
   ptr1 = line;
      }
      if (strlen (ptr1) > 1) {
   if (ptr1[strlen(ptr1)-1] == '\n')
     ptr1[strlen(ptr1)-1] = '\0';
   strcpy (savepath, ptr1);
   if (strlen (savepath)) {
     if (savepath[strlen(savepath)-1]!=DIRSEPARATOR[0])
       strcat (savepath, DIRSEPARATOR);
   }
      }
      continue;
    }
    else if (tmp == 'i') {
      printf ("\nFile info ...\n\n");
      printf ("-------------------------------------------------------------------------------\n");
      more   (iter, SHOW_INFO);
      printf ("\n-------------------------------------------------------------------------------\n\n");
      continue;
    }
    /*
     * for the following menu items, we need the file decoded
     */
    if ((res = UUDecodeToTemp (iter)) != UURET_OK) {
      fprintf (stderr, "ERROR: while decoding %s (%s): %s\n",
          (iter->filename) ? iter->filename : "",
          (iter->subfname) ? iter->subfname : "",
          UUstrerror(res));
      decodedfailed++;
      break;
    }
    if (!quiet && UUISATTY(stderr) && !nobar) {
      fprintf (stderr, "%70s\r", ""); /* clear progress information */
      fflush  (stderr);
    }
    if (iter->binfile == NULL) {
      fprintf (stderr, "ERROR: Ooops. Decoded %s but no binfile??\n",
          (iter->filename) ? iter->filename : "");
      decodedfailed++;
      break;
    }

    if (tmp=='d' || tmp=='y' || tmp=='x' || tmp=='\0' || tmp=='\n') {
      /*
       * Set Target directory
       */
    make_target:
      sprintf (targetname, "%s%s%s",
          savepath,
          (strlen(savepath) &&
      savepath[strlen(savepath)-1]==DIRSEPARATOR[0]) ?
          "":DIRSEPARATOR,
          name);

      /*
       * check whether this file exists
       */

    retryexcheck:
      if (exists (targetname) && overwrite!=1 && autoren &&
     findAlternateName (targetname, renamedname)) {
   printf ("*** Target File %s exists - autorenamed to %s\n",
      targetname, renamedname);
   strcpy (targetname, renamedname);
      }

      if (exists (targetname) && overwrite == -1) {
   printf ("*** Target File %s exists! (file not decoded)\n",
      targetname);
   return 1;
      }

      if (exists (targetname) && !overwrite) {
   printf ("*** Target File %s exists! Options:\n", targetname);
   res = ' ';

   printf ("\
*** (O)verwrite, Overwrite (A)ll, Overwrite Non(e),\n\
*** (R)ename, Au(t)o-Rename, (N)ext, (P)ath [o] ");
   fflush (stdout);

   /*
    * Ask the user for her/his choice. If 'r' or 'p', and there
    * is more data on the line, use it as reply
    */

   res = getyn (line, stdinput ? 'n' : 'o');

   for (ptr1=line; *ptr1 && *ptr1 != res; ptr1++);
   if (*ptr1++==res) {
     while (isspace(*ptr1)) ptr1++;
   }

   switch (res) {
   case 'n': return 1;
   case 'a': overwrite=1; goto retryexcheck;
   case 'o': goto noexcheck;
   case 'e': overwrite=-1; return 1;
   case 't': autoren=1; goto retryexcheck;
   case 'r':
     if (strlen(ptr1) <= 1) {
       printf ("Enter new filename: "); fflush (stdout);
       if (_FP_fgets (line, 250, stdin) == NULL) {
         printf ("\nERROR: Could not get filename: %s\n",
            strerror (errno));
         line[0] = '\0';
       }
       ptr1 = line;
     }
     if (strlen (ptr1) > 1) {
       if (ptr1[strlen(ptr1)-1] == '\n')
         ptr1[strlen(ptr1)-1] = '\0';
       UURenameFile (iter, ptr1);
     }
     if ((name = UUFNameFilter (iter->filename)) == NULL) {
       fprintf (stderr, "ERROR: couldn't get filename of %s (%s)\n",
           (iter->filename)?iter->filename:"(null)",
           (iter->subfname)?iter->subfname:"(null)");
       return 1;
     }
     goto make_target;

   case 'p':
     if (strlen(ptr1) <= 1) {
       printf ("Enter new path: "); fflush (stdout);
       if (_FP_fgets (line, 250, stdin) == NULL) {
         printf ("\nERROR: Could not get path: %s\n",
            strerror (errno));
         line[0] = '\0';
       }
       ptr1 = line;
     }
     if (strlen (ptr1) > 1) {
       if (ptr1[strlen(ptr1)-1] == '\n')
         ptr1[strlen(ptr1)-1] = '\0';
       strcpy (savepath, ptr1);
       if (strlen(savepath)) {
         if (savepath[strlen(savepath)-1]!=DIRSEPARATOR[0])
      strcat (savepath, DIRSEPARATOR);
       }
     }
     goto make_target;

   default:
     goto retryexcheck;
   }
      }

    noexcheck:
      if ((res = UUDecodeFile (iter, targetname)) != UURET_OK) {
   if (UUISATTY(stderr) && !nobar)
     fprintf (stderr, "%70s\r", ""); /* clear progress information */
   fprintf (stderr, "ERROR: while writing %s (%s): %s\n",
       targetname, (iter->subfname) ? iter->subfname : "",
       UUstrerror(res));
   decodedfailed++;
   break;
      }
      if (!quiet) {
   if (UUISATTY(stderr) && !nobar)
     fprintf (stderr, "%70s\r", ""); /* clear progress information */
   printf  ("    File successfully written to %s\n", targetname);
      }
      decodedok++;
      break;
    }
    else if (tmp == 'l') {
      printf ("\nContents of file ...\n\n");
      printf ("------------------------------------------------------------------------------\n");
      more   (iter, SHOW_FILE);
      printf ("\n------------------------------------------------------------------------------\n\n");
    }
    /*
     * Can't do that in QuickWin, since we'd need system()
     */
#ifndef SYSTEM_QUICKWIN
    else if (tmp == 'e') {
      printf ("Enter command line ($) for file: "); fflush (stdout);
      if (_FP_fgets (line, 256, stdin) == NULL)
   printf ("\nERROR: Could not get Command line: %s\n",
      strerror (errno));
      else if (strlen (line) > 1) {
   ptr1 = line; ptr2 = command; escflag = 0;

   while (*ptr1 && *ptr1 != '\012' && *ptr1 != '\015') {
     if (!escflag && *ptr1 != '\\') {
       if (*ptr1 == '$') {
         strcpy (ptr2, iter->binfile);
         ptr2  += strlen (iter->binfile);
         ptr1++;
       }
       else
         *ptr2++ = *ptr1++;
     }
     else if (escflag) {
       *ptr2++ = *ptr1++;
       escflag = 0;
     }
     else
       escflag = 1;
   }
   *ptr2 = '\0';

   printf ("------------------------------------------------------------------------------\n");
   system (command);
   printf ("------------------------------------------------------------------------------\n\n");
      }
    }
#endif   
    else {
      printf ("ERROR: unknown action '%c'. Enter ? for list of options.\n",
         tmp);
    }
  }

  return 1;
}

static int
process_files (void)
{
  int res, index=0, desp;
  uulist *iter;
  char *ptr;

  while ((iter=UUGetFileListItem(index))) {
    if (iter->filename == NULL) {
      index++;
      continue;
    }
    if (!(work_file (iter->filename))) {
      if (interact && !quiet)
        printf ("  %s %s ignored.\n",
                filemode((int)iter->mode),
      (iter->filename)?iter->filename:"");

      index++;
      continue;
    }
    if (iter->state & UUFILE_OK)
      ptr = "State is OK";
    else if (iter->state & UUFILE_NODATA)
      ptr = NULL;
    else if (iter->state & UUFILE_NOBEGIN)
      ptr = "No Begin found";
    else if (iter->state & UUFILE_MISPART)
      ptr = "Missing Part(s)";
    else if (iter->state & UUFILE_NOEND)
      ptr = "No End found";
    else
      ptr = "Unknown State";

    if (iter->state & UUFILE_NODATA) {
      index++;
      continue;
    }

    UUGetOption (UUOPT_DESPERATE, &desp, NULL, 0);

    if (iter->state & UUFILE_OK || desp) {
      res = process_one (iter);
    }
    else {
      if (ptr && iter->filename)
        printf ("ERROR: File %s (%s): %s (%d)\n",
                (iter->filename) ? iter->filename : "",
      (iter->subfname) ? iter->subfname : "",
      ptr, iter->state);
      decodedmisp++;
      res  = 1;
    }

    if (res == 0)
      break;
    else if (res == -1) {
      if (index==0)
   printf ("*** Already at beginning of list\n");
      else {
   index--;
   while ((!(iter->state & UUFILE_OK || desp) ||
      iter->state == UUFILE_NODATA) && index)
     index--;
      }
    }
    else {
      index++;
    }
  }
  return 0;
}

static void
sighandler (int signo)
{
  printf ("\nReceived Signal (%d), cleaning up temp files.\n", signo);

  UUCleanUp ();
  exit (99);
}

/*
 * usage
 */

static void
usage (char *argv0)
{
  printf ("\n\
  UUDEVIEW %s%s%s - the nice and friendly decoder - (w) 1994 Frank Pilhofer\n",
     VERSION, (PATCH[0]>'0')?"pl":"", (PATCH[0]>'0')?PATCH:"");
  printf ("  usage:\n");
  printf ("    uudeview [options] [file ...]\n\n");
  printf ("  Options:\n");
  printf ("\t-i      Disable interactivity (do not ask, decode everything)\n");
  printf ("\t-a      Autorename (rename file if it exists on disk)\n");
  printf ("\t-m      Ignore the file mode of uuencoded files\n");
  printf ("\t+e/-e   Include or exclude extensions exclusively\n");
  printf ("\t-d      Sets 'desperate' mode (process incomplete files)\n");
  printf ("\t-f      Fast mode. Only if each file holds no more than one part\n");
  printf ("\t-o/+o   -o OK to overwrite files, +o never overwrite files\n");
  printf ("\t-b1     Select alternate bracket policy\n");
  printf ("\t-p path Sets path where to save decoded binaries to\n");
  printf ("\t-c      Autoclear files that were successfully decoded.\n");
  printf ("\t-s      Be more strict on MIME adherance.\n");
  printf ("\t-q      Quiet. Do not emit progress messages.\n\n");
#if defined(SYSTEM_DOS) || defined(SYSTEM_QUICKWIN)
  printf ("  See Manual for more details\n\n");
#else
  printf ("  See uudeview(1) for more details.\n\n");
#endif
  printf ("  Example:\n");
  printf ("    uudeview +e .jpg.gif -i newsfile\n");
  printf ("\tThis decodes all .jpg or .gif files encountered in <newsfile>\n");
  printf ("\twithout asking.\n\n");
}

/*
 * uudeview main function
 */

int
main (int argc, char *argv[])
{
  int res;
#ifdef SYSTEM_QUICKWIN
  struct _wsizeinfo ws;
#endif

  /*
   * No Signal handler in QuickWin
   */
#ifndef SYSTEM_QUICKWIN
  signal (SIGINT, sighandler);
#endif
  /*
   * In QuickWin, set the about() Box, and give more space to scroll
   */
#ifdef SYSTEM_QUICKWIN
  ws._version = _QWINVER;
  ws._type    = _WINSIZEMAX;
  (void) _wabout        ("UUdeview for Windows\n(c) 1995 Frank Pilhofer\nfp@fpx.de");
  (void) _wsetscreenbuf (fileno(stdout), 16384);
  (void) _wsetscreenbuf (fileno(stderr), 16384);
  (void) _wsetsize      (fileno(stdout), &ws);
#endif

  /*
   * Check where we are and set the save directory
   */
#ifdef HAVE_GETCWD
  if (getcwd (savepath, 255) == NULL)
#endif
    strcpy (savepath, "./");

  /*
   * in DOS, set the DOS Filename Filter
   */

#if defined(SYSTEM_DOS) || defined(SYSTEM_QUICKWIN)
  UUSetFNameFilter (NULL, UUFNameFilterDOS);
#else
  UUSetFNameFilter (NULL, UUFNameFilterUnix);
#endif

  UUSetBusyCallback (NULL, BusyCallback, 100);

  /*
   * If we were called as uudecode, be quiet and don't interact
   */
  if (_FP_stristr (argv[0], "uudecode") != NULL) {
    interact    = 0;
    decoall     = 1;
    quiet       = 1;
    overwrite   = 1;
  }
  else if (argc < 2) {
    usage (argv[0]);
    return 99;
  }

  /*
   * Setup Callback
   */

  UUSetMsgCallback (NULL, MessageCallback);

  if (UUInitialize () != UURET_OK) {
    fprintf (stderr, "oops: could not initialize decoding library\n");
    return 99;
  }

  /*
   * use options from UUDEVIEW environment variable
   */

  if ((getenv ("UUDEVIEW")) != NULL) {
    makeparams (getenv ("UUDEVIEW"));
  }

  if (argc < 2) {
    /*
     * can only be in uudecode compatibility mode
     */
    proc_stdin ();
  }
  else {
    work_comline (argc, argv);
  }

  if (strlen(savepath)) {
    if (savepath[strlen(savepath)-1] != DIRSEPARATOR[0])
      strcat (savepath, DIRSEPARATOR);
  }

  if (!stdinput && !quiet && UUGetOption (UUOPT_VERBOSE, NULL, NULL, 0)) {
    DumpFileList ();
  }

  /*
   * try merging thrice with increased tolerance
   */
  UUSmerge (0);
  UUSmerge (1);
  UUSmerge (99);

  res = process_files ();

  /*
   * clear info
   */
  if (UUISATTY(stderr) && !nobar) {
    fprintf (stderr, "\r%70s\r", "");
    fflush  (stderr);
  }

#ifndef SYSTEM_QUICKWIN
  signal   (SIGINT, SIG_DFL);
#endif
  UUCleanUp();
  killext  (extensions);

  /*
   * Without user interaction, or if the user has quit
   * the proggy, kill myself in QuickWin
   */
#ifdef SYSTEM_QUICKWIN
  if (!interact || res==1)
    _wsetexit (_WINEXITNOPERSIST);
  else {
    printf ("\n\
No more Programs to decode.\n\
Select File-Exit to close window\n\
\n");
  }
#endif

  if (UUGetOption (UUOPT_VERBOSE, NULL, NULL, 0) && !quiet &&
      inputfiles && (decodedok || decodedfailed || decodedmisp)) {
    printf ("%d file%s decoded from %d input file%s, %d failed\n",
       decodedok, (decodedok==1)?"":"s",
       inputfiles, (inputfiles==1)?"":"s",
       decodedfailed+decodedmisp);
  }

  /*
   * determine return value
   */

  if (decodedfailed || decodedmisp) {
    return ((decodedok?1:0) | ((decodedmisp||decodedfailed)?2:0));
  }

  return 0;
}

int compressed;
{
        char ibuf[BUFSIZ];
        FILE *outf;
        register long nbytes = 0L;

        outf = fopen(fname, "w");
        if (outf == NULL) {
                perror(fname);
                exit(-1);
        }

        while (fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (strncmp(ibuf, "***END", 6) == 0)
                        break;
                if (compressed)
                        nbytes += comp_to_bin(ibuf, outf);
                else
                        nbytes += hex_to_bin(ibuf, outf);
        }

        fclose(outf);
        return nbytes;
}

comp_c_crc(c)
unsigned char c;
{
        crc = (crc + c) & WORDMASK;
        crc = ((crc << 3) & WORDMASK) | (crc >> 13);
}

comp_e_crc(c)
unsigned char c;
{
        crc += c;
}

#define SIXB(c) (((c)-0x20) & 0x3f)

comp_to_bin(ibuf, outf)
char ibuf[];
FILE *outf;
{
        char obuf[BUFSIZ];
        register char *ip = ibuf;
        register char *op = obuf;
        register int n, outcount;
        int numread, incount;

        numread = strlen(ibuf);
        ip[numread-1] = ' ';            /* zap out the newline */
        outcount = (SIXB(ip[0]) << 2) | (SIXB(ip[1]) >> 4);
        incount = ((outcount / 3) + 1) * 4;
        for (n = numread; n < incount; n++)     /* restore lost spaces */
                ibuf[n] = ' ';

        n = 0;
        while (n <= outcount) {
                *op++ = SIXB(ip[0]) << 2 | SIXB(ip[1]) >> 4;
                *op++ = SIXB(ip[1]) << 4 | SIXB(ip[2]) >> 2;
                *op++ = SIXB(ip[2]) << 6 | SIXB(ip[3]);
                ip += 4;
                n += 3;
        }

        for (n=1; n <= outcount; n++)
                comp_c_crc((unsigned)obuf[n]);

        fwrite(obuf+1, 1, outcount, outf);
        return outcount;
}

hex_to_bin(ibuf, outf)
char ibuf[];
FILE *outf;
{
        register char *ip = ibuf;
        register int n, outcount;
        int c;

        n = strlen(ibuf) - 1;
        outcount = n / 2;
        for (n = 0; n < outcount; n++) {
                c = hexit(*ip++);
                comp_e_crc((unsigned)(c = (c << 4) | hexit(*ip++)));
                fputc(c, outf);
        }
        return outcount;
}

hexit(c)
int c;
{
        if ('0' <= c && c <= '9')
                return c - '0';
        if ('A' <= c && c <= 'F')
                return c - 'A' + 10;

        fprintf(stderr, "illegal hex digit: %c", c);
        exit(4);
        /* NOTREACHED */
}

put2(bp, value)
char *bp;
short value;
{
        *bp++ = (value >> 8) & BYTEMASK;
        *bp++ = value & BYTEMASK;
}

put4(bp, value)
char *bp;
long value;
{
        register int i, c;

        for (i = 0; i < 4; i++) {
                c = (value >> 24) & BYTEMASK;
                value <<= 8;
                *bp++ = c;
        }
}
--
USPS: The Consultants' Exchange, PO Box 12100, Santa Ana, CA  92712
EMAIL: larry@mtndew.com  or  root@conexch.uucp  or  uunet!usa2002!conexch!root
TELE: (714) 842-6348; (714) 842-5851: Public guest & bbs logins
UUCP: conexch Any ACU 9600 17148426348 ogin:-""-ogin:-""-ogin: nuucp
