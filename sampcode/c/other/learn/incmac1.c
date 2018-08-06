/* INCMAC1.C: Increment operator in the for statement. 
*/

#include <stdio.h>
#define ABS(value)  ( (value) >= 0 ? (value) : -(value) )

main()
{
   static int array[4] = {3, -20, -555, 6};
   int *ptr = array;
   int val, count;
   for( count = 0; count < 4; count++, ptr++ )
   {
      val = ABS(*ptr);
      printf( "abs of array[%d] = %d\n", count, val );
   }
}
