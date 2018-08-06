/*************************************************************************
 *                                                                       *
 *  ASCTBL.C                                                             *
 *   This program generates an ASCII lookup table for all displayable    *
 *   ASCII and extended IBM PC codes, leaving blanks for nondisplayable  *
 *   codes.                                                              *
 *                                                                       *
 ************************************************************************/

#include <ctype.h>
#include <stdio.h>

main()
{
int i, j, k;
       /* Print table title. */
       printf("\n\n\n                ASCII LOOKUP TABLE\n\n");

       /* Print column headers. */
       printf("    ");
       for (i = 0; i < 16; i++) 
               printf("%X  ", i);
       fputchar("\n");

       /* Print each line of the table. */
       for ( i = 0, k = 0; i < 16; i++)
               {
               /* Print first hex digit of symbols on this line. */
               printf("%X   ", i);
               /* Print each of the 16 symbols for this line. */
               for (j = 0; j < 16; j++)
                       {
                       /* Filter nonprintable characters. */
                       if ((k >= 7 && k <= 13) || (k >= 28 && k <= 31))
                               printf("   ");
                       else
                               printf("%c  ", k);
                       k++;
                       }
       fputchar("\n");
       }
}
