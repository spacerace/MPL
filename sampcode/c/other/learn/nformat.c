/* NFORMAT.C: Prints numbers and a string. */

#include <stdio.h>
#include <string.h>

main()
{
   int    a = -765,
          b = 1,
          c = 44000,
          d = 33;
   float  e = 1.33E8,
          f = -0.1234567,
          g = 12345.6789,
          h = 1.0;
   char   i[80];

   strcpy( i, "word 1, word 2, word 3, word 4, word 5" );

   printf( "Unformatted:\n%d %d %d %d\n", a, b, c, d );
   printf( "%f %f %f %f\n", e, f, g, h );
   printf( "%s\n", i );
}
