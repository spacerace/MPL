/*  PCJUNK.C     Misc. procedures */

#include "stdio.h"
#include "sdbio.h"
#define ESC 27
#define BS 8
#define CR 13
#define LF 10

/*    Replaces JUNK.C for the IBM/PC version            dns */


char *alloc(n)
int n;
{
   return (char*) malloc(n);
}
/* commented out because it doesn't work right replaced with the
   getcx from the original sdb
*/
int getcx(fp)
  FILE *fp;
{
    static char buf[LINEMAX] = {0};
    static int  bufptr = 0;
    static int  buflen = 0;
    int ch, i, j;

    if (fp!=stdin)
       if ((ch = fgetc(fp)) == '\n')
          return(fgetc(fp));
       else
          return(ch);

    if (buflen > 0 && bufptr < buflen)
      if (bufptr == buflen-1)
        {
        i = bufptr ;
        bufptr = 0 ;
        buflen = 0 ;
        return (buf[i]) ;
        }
      else
        {
        return (buf[bufptr++]);
        } ;

    bufptr = 0 ;
    buflen = 0 ;
    for (bufptr = 0; (ch = getch() ) != -1; )
        if (bufptr < LINEMAX)
          switch (ch)
            {
            case ESC:
              bufptr=0;
              buflen = 0 ;
              printf("\\\n     ");
              fflush(stdout);
              break ;
            case BS:
              if (bufptr>0)
                {
                bufptr-- ;
                buflen-- ;
                putch(ch) ;
                putch(32) ;
                putch(ch) ;
                }
              break ;
            case CR:
              putch(13) ;
              putch(10) ;
              if (buflen == 0)
                return(10) ;
              else
                {
                buf[bufptr] = 10 ;
                buflen++ ;
                bufptr = 1 ;
                return(buf[0]) ;
                }
              break ;
            default:
              buf[bufptr++] = ch;
              buflen++ ;
              putch(ch) ;
              break ;
            }
        else {
            printf("*** line too long ***\nRetype> ");
            fflush( stdout ) ;
            buflen = 0;
            bufptr = 0;
        };
 }
 int strncmp( str1, str2, len)
 char *str1, *str2 ;
 int  len ;
 {
   char  newstr1[255], newstr2[255] ;
   int  i ;

   for ( i = 0 ; i < len ; i++ )
     {
       newstr1[i] = str1[i] ;
       newstr2[i] = str2[i] ;
     }
   return (strcmp( newstr1, newstr2 )) ;
}
