/* BADSEMI.C: Misplaced semicolon. */

#include <stdio.h>

main()
{
   int count;
   for( count = 0; count < 500; count++ ); /* Error! */
   {
      printf( "count = %d\n", count );
      printf( "And the beat goes on...\n" );
   }
}
