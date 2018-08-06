/* PARRAY1.C: Compact version of PARRAY.C. */

#include <stdio.h>

int i_array[] = { 25, 300, 2, 12 };


main()
{
   int count;
   int *ptr = i_array;
   for( count = 0; count < 4 ; count++ )
      printf( "array[%d] = %d\n", count, *ptr++ );
}
