#define EXTERN

#include "Base64.h"
#include "gynplugin.h"


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



extern int _dllentry;

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


enum TOKENTYPE { NONE, BLANKS, PUNCT, TAG, NAME, CONTENT };

struct TOKEN {
                char *text;
                int  length;
                int  index;
                enum TOKENTYPE type;
             };



void fixname( char *name )
{
   while ( *name != '\0' )
   {

     if (ispunct( *name ) )
     {
        *name = '\0';
     }

     name++;
   }
}

void acquire_token( char *line, enum TOKENTYPE type, struct TOKEN *token )
{
   int doneflag=0, startflag=1;
   int index;
   enum TOKENTYPE nextstate=NONE;
   char blivit;

   if (token->type == NONE)
   {
      token->index = 0;
      token->length = 0;
   }

   index = token->index + token->length;

   token->text = 0;

   while ( doneflag == 0 )
   {
      blivit = line[index];
      if ( (blivit >= 'a') && (blivit <= 'z') )
      {
         blivit -= ' ';
      }

      switch (token->type)
      {
         case NONE:
         if ( blivit == ' ')
         {
            index++;
            token->index++;
         }
         else
         {
            token->type = TAG;
            nextstate = TAG;
         }
         break;

         case BLANKS:
         if      ( blivit == ' ')
         {
            index++;
         }
         else if ( ispunct( blivit ) )
         {
            token->type = PUNCT;
            token->index = index;
         }
         else
         {
            token->type = nextstate;
            token->index = index;
         }
         break;

         case PUNCT:
         if      ( blivit <  ' ')
         {
            doneflag = 1;
            token->type = NONE;
            token->index = index;
            token->text = line + index;
            token->length = 0;
         }
         else if ( blivit == ' ' )
         {
            token->type = BLANKS;
            token->index = index;
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

         }
         else if ( ispunct( blivit ) )
         {
            index++;
         }
         else
         {
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

            token->type = nextstate;
            token->index = index;
         }
         break;

         case TAG:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == TAG ) || ( type == NONE ) ) && !startflag)
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case NAME:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;

            if ( blivit != ';' )
            {
               nextstate = CONTENT;
            }
            else
            {
               nextstate = NAME;
            }

            if ( ( ( type == NAME ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case CONTENT:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == CONTENT ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;
      }
      startflag = 0;
   }
}

int compare_token( struct TOKEN *token, char *text )
{
   int index=0;
   int count;
   int result;
   char blivit1, blivit2;

   count = token->length;

   if ( count > 0 )
   {
      result = 1;
   }
   else
   {
      result = 0;
   }

   while ( (count > 0) && ( result != 0 ) )
   {
      blivit1 = token->text[index++];
      if ( (blivit1 >= 'a' ) && (blivit1 <= 'z') )
      {
         blivit1 -= ' ';
      }

      blivit2 = *text++;
      if ( (blivit2 >= 'a' ) && (blivit2 <= 'z') )
      {
         blivit2 -= ' ';
      }

      if ( blivit1 != blivit2 )
      {
         result = 0;
      }

      count--;
   }

   return result;
}

int cvt_ascii( unsigned char alpha )
{
   if      ( (alpha >= 'A') && (alpha <= 'Z') ) return (int)(alpha - 'A');
   else if ( (alpha >= 'a') && (alpha <= 'z') )
        return 26 + (int)(alpha - 'a');
   else if ( (alpha >= '0') && (alpha <= '9' ) )
        return 52 + (int)(alpha - '0');
   else if ( alpha == '+' ) return 62;
   else if ( alpha == '/' ) return 63;
   else if ( alpha == '=' ) return -2;
   else                     return -1;
}

int Base64Decode(PSZ pszData, ULONG ulDataLen, ULONG *ulDecoLen)
{
   int x;
   int n_options, n_files, index, jndex, shift, save_shift;
   enum { ENCODE, DECODE } whattodo = DECODE;
   int help_flag = 0, replace_flag = 0, perm_replace_flag = 0, quit = 0;
   int cycle_flag = 0;
   unsigned char blivit;
   unsigned long accum, value;
   char buf[80], dumname[80];
   char *cptr, *altptr;
   int decode_state;
   struct TOKEN token;
   int firsttime = 1;
   int skipflag = 0;
   int printmsg = 1;
   PSZ pszLineBegin;
   PSZ pszLineEnd;
   APIRET rc;
   PSZ pszDataPtr;


   x = _dllentry;
   pszDataPtr = pszData;
   *ulDecoLen = 0;

   pszLineBegin = pszData;
   pszLineEnd = pszLineBegin;

do {
   quit = 0;
   printmsg = 1;

   if ( whattodo == DECODE )
   {
      shift = 0;
      accum = 0;
      decode_state = 0;

      while ( ( pszLineEnd != NULL ) && (quit == 0) )
      {
/*         fgets( buf, 80, fin ); */
         pszLineEnd = SearchFor("\r\n",pszLineBegin, ulDataLen - (pszLineBegin - pszData), pszLineEnd);
         if ( pszLineEnd == NULL )
         {
            if ( ( dumname[0] != '\0' ) && ( shift != 0 ) )
            {
/*               printf( "Unexpected end of file encountered in %s\n"
                       "last few bytes may have been lost\n", dumname );*/
               quit = 1;
               decode_state = 1;
               pszLineBegin = pszLineEnd + 2;
               continue;
            }
            else if ( cycle_flag == 0 )
            {
               quit = 1;
               decode_state = 1;
               pszLineBegin = pszLineEnd + 2;
               continue;
            }
         }
         else
         {
            if ((pszLineEnd - pszLineBegin) > 79)
               pszLineEnd = pszLineBegin + 79;
            strncpy(buf, pszLineBegin, pszLineEnd - pszLineBegin);
            buf[pszLineEnd - pszLineBegin] = 0x00;
            cycle_flag = 1;

            if ( (decode_state == 1) &&
                 ( (buf[0] == '\n') || (buf[0] < '+') ) )
            {
               quit = 1;

               if ( shift != 0 )
               {
/*                  printf( "Unexpected end of section in %s\n"
                          "last few bytes may have been lost\n", dumname );  */
               }

               pszLineBegin = pszLineEnd + 2;
               continue;
            }
         }


         if ( decode_state == 0 )
         {
            for ( index = 0;
                  (buf[index] != '\n') && (buf[index] != '\0') &&
                  (decode_state >= 0);
                  index++ )
            {
               if ( ( (buf[index] >= 'A') && (buf[index] <= 'Z') ) ||
                    ( (buf[index] >= 'a') && (buf[index] <= 'z') ) ||
                    ( (buf[index] >= '0') && (buf[index] <= '9') ) ||
                    (buf[index] == '+') ||
                    (buf[index] == '/') ||
                    (buf[index] == '=') )
               {
                  decode_state = 1;
               }
               else
               {
                  decode_state = -2;
               }
            }

            if ( decode_state <= 0 )
            {

               decode_state = 0;
               token.type = NONE;

               acquire_token( buf, TAG, &token );
               if      ( compare_token( &token, "Content-Type") )
               {
                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "name" ) )
                     {
                        acquire_token( buf, CONTENT, &token );

                     }
                  } while ( token.type != NONE );
               }
               else if ( compare_token( &token, "Content-transfer-encoding" ) )
               {
                  skipflag = 1;

                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "base64" ) )
                     {
                        skipflag = 0;
                     }
                  } while ( token.type != NONE );
               }
               pszLineBegin = pszLineEnd + 2;
               continue;
            }
            else if ( skipflag != 0 )
            {
               pszLineBegin = pszLineEnd + 2;
               continue;
            }
         }

         if ( printmsg )
         {
/*            if ( skipflag )
            {
               printf( "Section %s not MIME base64\n", dumname );
            }  */

            printmsg = 0;
         }


         if ( pszLineEnd == NULL )
         {
            quit = 1;
         }

         if ( quit != 0 )
         {
            buf[0] = '\0';
         }

         for ( index = 0; (buf[index] != '\n') && (buf[index] != '\0'); index++)
         {
            value = cvt_ascii( buf[index] );

            if ( value < 64 )
            {
               accum <<= 6;
               shift += 6;
               accum |= value;
               if ( shift >= 8 )
               {
                  shift -= 8;
                  value = accum >> shift;
                  blivit = (unsigned char)value & 0xFFl;
                  *(pszDataPtr++) = blivit;
                  (*ulDecoLen)++;
/*                  fputc( blivit, fout ); */
               }
            }
            else
            {
               quit = 1;
               break;
            }
         }
         pszLineBegin = pszLineEnd + 2;
      }
   }

   firsttime = 0;
   dumname[0] = '\0';
   cycle_flag = 0;

   if (pszLineEnd != NULL)
   {
      pszLineBegin = pszLineEnd + 2;
      pszLineEnd = SearchFor("\r\n",pszLineBegin, ulDataLen - (pszLineBegin - pszData), pszLineEnd);
   }
} while ( pszLineEnd != NULL );

   return(0);
}



