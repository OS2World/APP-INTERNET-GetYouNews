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

#define DEC(c)  (((c) - ' ') & 077)             /* single character decode */

INT main(int argc, char *argv[]);

int UUDecodeLine(PSZ pszFileLine, ULONG ulLineLength, PSZ pszDecodedLine, ULONG *ulLineSize);

