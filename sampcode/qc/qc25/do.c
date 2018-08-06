/* DO.C: Demonstrate do loop. */

#include <stdio.h>

main()
{
   int test = 10;
   do
   {
      printf( "test = %d\n", test );
      test = test - 2;
   }  while( test > 0 );
}
