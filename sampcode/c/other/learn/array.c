/* ARRAY.C: Demonstrate arrays. */

#include <stdio.h>

main()
{
   int j, k;
   static int i_array[2][3] = { { 12, 2, 444 }, { 6, 55, 777 } };
   static char c_array[] = "Hello";

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

   for( j = 0; j < 6; j = j + 1 )
   {
      printf( "c_array[%d]   = %x %c", j, c_array[j], c_array[j] );
      printf( "\t&c_array[%d]    = %u\n", j, &c_array[j] );
   }
}
