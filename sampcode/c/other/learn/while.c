/* WHILE.C: Demonstrate while loop. */

#include <stdio.h>

main()
{
   int test = 10;

while( test > 0 )
   {
      printf( "test = %d\n", test );
      test -= 2;
   }
}
