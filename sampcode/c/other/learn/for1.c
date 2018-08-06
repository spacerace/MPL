/* FOR1.C: Demonstrate multiple expressions. */

#include <stdio.h>

main()
{
   int a, b;
   for( a = 256, b = 1; b < 512; a /= 2, b *= 2 )
      printf( "a = %d  b = %d\n", a, b );
}
