/* STRING.C: Demonstrate string arrays. */
#include <stdio.h>

main()
{
   int j;
   char c_array[] = "Hello";

   printf( "--- Values --------     --- Addresses -------\n\n" );

   for( j = 0; j < 6; j = j + 1 )
   {
      printf( "c_array[%d]   = %x %c", j, c_array[j], c_array[j] );
      printf( "\t&c_array[%d]    = %u\n", j, &c_array[j] );
   }
}
