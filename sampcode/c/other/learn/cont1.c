/* CONT1.C: Demonstrate alternative to continue. */

#include <stdio.h>

main()
{
   int count;
   for( count = 0; count < 10; count++ )
   {
      if( count < 4 )
	 printf( "count = %d\n", count );
   }
   printf( "Done!\n" );
}
