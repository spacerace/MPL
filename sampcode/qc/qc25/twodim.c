/* TWODIM.C: Demonstrate multidimensional arrays. */

#include <stdio.h>

main()
{
   int j, k;
   int i_array[2][3] = { { 176, 4069, 303 }, { 6, 55, 777 } };

   printf( "--- Values --------     --- Addresses -------\n\n" );

   for( j = 0; j < 2; j = j + 1 )
   {
      for( k = 0; k < 3; k = k + 1 )
      {
         printf( "i_array[%d][%d] = %d", j, k, i_array[j][k] );
         printf( "\t&i_array[%d][%d] = %u\n", j, k, &i_array[j][k] );
      }
      printf( "\n" );
   }

}
