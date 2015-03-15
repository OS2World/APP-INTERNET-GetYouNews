#define INCL_WIN
#define INCL_GPI
#define INCL_GPILCIDS
#define INCL_DEV
#define INCL_WINERRORS
#define INCL_ERRORS
#define EXTERN
#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOS
#define INCL_KBD
#define INCL_VIO
#define INCL_DOSDEVIOCTL
#define INCL_WINHOOKS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <time.h>

#include "gyn_de.h"


HAB  hAB;         /* Handle to the Anchor Block                     */
HMQ  hMQ;         /* Handle to the Message Queue                    */

int _System Setup()
{
   int ret;

   return(ret);
}


