/* FORLOOP.C: Demonstrate for loop. */

#include <stdio.h>

main()
{
   int test;
   for( test = 10; test > 0; test = test - 2 )
      printf( "test = %d\n", test );
}
