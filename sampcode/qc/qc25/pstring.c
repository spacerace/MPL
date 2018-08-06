/* PSTRING.C: Demonstrate pointer to a string. */

#include <stdio.h>

main()
{
   int count;
   static char name[] = "john";
   char *ptr = name;
   for( count = 0; count < 4; count++ )
   {
      printf( "name[%d]: %c\n", count, *ptr++ );
   }
}
