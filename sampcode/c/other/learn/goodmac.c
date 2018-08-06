/* GOODMAC.C: Parentheses in macro arguments. */
#include <stdio.h>

#define FOURX(arg)  ( (arg) * 4 )

main()
{
   int val;
   val = FOURX( 2 + 3 );
   printf( "val = %d\n", val );
}
