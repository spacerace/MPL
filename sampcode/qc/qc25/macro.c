/* MACRO.C: Demonstrate macros. */

#include <stdio.h>
#define ABS(value)  ( (value) >= 0 ? (value) : -(value) )

main()
{
   int val = -20;
   printf( "result = %d\n", ABS(val) );
}
