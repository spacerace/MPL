/* COMMA.C: Demonstrate comma operator. */

#include <stdio.h>

main()
{
   int val = 5, val1 = 666, temp;
   temp = val, val = val1, val1 = temp;
   printf( "val = %d  val1 = %d\n", val, val1 );
}
