/* DECRMENT.C: Demonstrate prefix and postfix operators. 
*/

#include <stdio.h>
main()
{
   int val, sample = 3, proton = 3;
   val = sample--;
   printf( "val = %d  sample = %d\n", val, sample );
   val = --proton;
   printf( "val = %d  proton = %d\n", val, proton );
}
