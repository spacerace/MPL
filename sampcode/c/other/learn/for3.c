/* FOR3.C: Demonstrate similarity of for and while. 
*/

#include <stdio.h>

main()
{
   int count;

   for( count = 0; count < 10; count++ )
      printf( "count = %d\n", count );

   count = 0;
   while( count < 10 )
   {
      printf( "count = %d\n", count );
      count++;
   }

}
