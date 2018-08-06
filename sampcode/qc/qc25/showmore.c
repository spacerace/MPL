/* SHOWMORE.C: Demonstrate passing by value. */
#include <stdio.h>

void showme( int any, int old, int name );

main()
{
   int x = 10, y = 20, z = 30;
   showme( z, y, x );
   printf( "  z=%d   y=%d    x=%d\n", z, y, x );
}

void showme( int any, int old, int name )
{
   printf( "any=%d old=%d name=%d\n", any, old, name );
   any = 55;
   old = 66;
   name = 77;
   printf( "any=%d old=%d name=%d\n", any, old, name );
}
