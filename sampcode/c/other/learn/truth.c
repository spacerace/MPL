/* TRUTH.C: Demonstrate true and false values. */

#include <stdio.h>

main()
{
   printf( "C generates %d for true\n", 2 == 2 );
   printf( "C generates %d for false\n", 2 == 4 );
   if( -33 )
      printf( "C recognizes any nonzero value as true\n" );
}
