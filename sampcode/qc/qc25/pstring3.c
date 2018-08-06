/* PSTRING3.C: Strings and pointer notation.  */
#include <stdio.h>
#include <string.h>

main()
{
   int count;
   char name[] = "john";
   for( count = 0; count < strlen( name ); count++ )
      printf( "*(name+%d) = %c\n", count, *(name+count) );
}
