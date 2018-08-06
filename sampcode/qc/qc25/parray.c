/* PARRAY.C: Demonstrate pointer to array. */
#include <stdio.h>

int i_array[] = { 25, 300, 2, 12 };

main()
{
   int *ptr;
   int count;
   ptr = &i_array[0];
   for( count = 0; count < 4 ; count++ )
   {
      printf( "i_array[%d] = %d\n", count, *ptr );
      ptr++;
   }
}
