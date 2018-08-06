/* INCMAC.C: Increment operator in macro argument. 
*/

#include <stdio.h>
#define ABS(value)  ( (value) >= 0 ? (value) : -(value) )

main()
{
   static int array[4] = {3, -20, -555, 6};
   int *ptr = array;
   int val, count;
   for( count = 0; count < 4; count++ )
   {
      val = ABS(*ptr++); /* Error! */
      printf( "abs of array[%d] = %d\n", count, val );
   }
}
