/* ARRAY.C: Demonstrate one-dimensional array. */
#include <stdio.h>

main()
{
   int j;
   int i_array[3];

   i_array[0] = 176;
   i_array[1] = 4069;
   i_array[2] = 303;

   printf( "--- Values --------     --- Addresses -------\n\n" );

   for( j = 0; j < 3; j = j + 1 )
   {
      printf( "i_array[%d] = %d", j, i_array[j] );
      printf( "\t&i_array[%d] = %u\n", j, &i_array[j] );
   }

}
