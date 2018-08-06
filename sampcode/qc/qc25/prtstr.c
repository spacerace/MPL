/* PRTSTR.C: Prints strings. */
#include <stdio.h>
#include <string.h>

main()
{
   char aline[80], more[80];
   char *strptr;

   /* aline = "Another line."; */
   /* Note: This causes a compiler error */

   strcpy( aline, "Another line." );
   strcpy( more, aline );
   strptr = aline;
   strcat( aline, "dog" );
   printf( "A line of text." );
   printf( aline );
   printf( more );
   printf( strptr );
}
