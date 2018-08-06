/* PRTESC.C: Prints escape characters \",\n, and \t. 
*/

#include <stdio.h>
#include <string.h>

main()
{
   char b[80];
   int i,j;

   strcpy( b, "and seven years ago\n" );
   printf( "\"Four score\n" );
   printf( b );
   printf( "\tone tab\n\t\ttwo tabs\n\t\t\tthree tabs\n" );
   i = sizeof( b );
   j = strlen( b );
   printf( "Size is %d\nLength is %d.\n", i, j );
}
