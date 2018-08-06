/* CONVERT.C: Demonstrate type conversions. */

#include <stdio.h>

main()
{
   char c_val = 10;
   int i_val = 20;
   long l_val = 64000;
   float f_val = 3.1;
   int result;

   result = c_val + i_val + l_val + f_val;  /* Error! */

   printf( "%d\n", result );
}
