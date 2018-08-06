/* SHOWME.C: Demonstrate passing by value. */
#include <stdio.h>

void showme( int a, int b, int c );

main()
{
   int x = 10, y = 20, z = 30;
   showme( z, y, x );
}

void showme( int a, int b, int c )
{
   printf( "a=%d b=%d c=%d", a, b, c );
}
