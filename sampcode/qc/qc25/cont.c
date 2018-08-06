/* CONT.C: Demonstrate continue statement. */
#include <stdio.h>

main()
{
   int count;
   for( count = 0; count < 10; count++ )
   {
      if( count > 3 )
         continue;
      printf( "count = %d\n", count );
   }
   printf( "Done!\n" );
}
