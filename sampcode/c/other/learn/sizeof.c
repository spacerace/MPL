/* SIZEOF.C: Demonstrate sizeof operator. */

#include <stdio.h>

char string[] = "Hello, world";

main()
{
   int val;
   printf( "An int is %d bytes long\n", sizeof( int ) );
   printf( "The string is %d bytes long\n", sizeof string );
}
