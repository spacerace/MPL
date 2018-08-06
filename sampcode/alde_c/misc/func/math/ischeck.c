
/* ischeck()  program to check Microsoft C's issomething matrix.
 *            it has no other purpose.
 *            1 to 1 correspondence between 0 to 127 and 128 to 255!!!
 *            WHRauser   10-4-83     Microsoft C  Version 1.04
 */

#include  <ctype.h>

main()
{
     int  i;

     printf("   dec   char   hex    XBCP SNLU    ");
     printf("   dec   char   hex    XBCP SNLU\n\n");

     for (i=0; i<128; i++) {
          printit(i);
          printit(i+128);
          printf("\n");
     }
}/*main
--------------------------------------------*/

printit(i)
     int i;
{
     int  t;
     int  alpha;
     int  x,b,c,p, s,n,l,u;     /* bit map */

     if isprint(i)
           alpha = i;
     else  alpha = ' ';

     t = _ctype[i+1];    /*Stupid, don't they know C is zero origin?*/

     x = t & 128 ? 1 : 0;    /*   X 128        hexadecimal flag */
     b = t &  64 ? 1 : 0;    /*   B 64         blank flag */
     c = t &  32 ? 1 : 0;    /*   C 32         control character flag */
     p = t &  16 ? 1 : 0;    /*   P 16         punctuation flag */
     s = t &   8 ? 1 : 0;    /*   S 8          space flag */
     n = t &   4 ? 1 : 0;    /*   N 4          number flag */
     l = t &   2 ? 1 : 0;    /*   L 2          lower case flag */
     u = t &   1 ? 1 : 0;    /*   U 1          upper case flag */

     printf("  %3d     %c    %3x     %1d%1d%1d%1d %1d%1d%1d%1d    ",
             i,alpha,t, x,b,c,p, s,n,l,u);
}

