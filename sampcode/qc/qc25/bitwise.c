/* BITWISE.C: Demonstrate bitwise operators. */
#include <stdio.h>

main()
{
   printf( "255 & 15 = %d\n", 255 & 15 );
   printf( "255 | 15 = %d\n", 255 | 15 );
   printf( "255 ^ 15 = %d\n", 255 ^ 15 );
   printf( "2 << 2   = %d\n", 2 << 2 );
   printf( "16 >> 2  = %d\n", 16 >> 2 );
   printf( "~2       = %d\n", ~2 );
}
