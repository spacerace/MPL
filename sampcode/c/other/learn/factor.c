/* FACTOR.C: Demonstrate functions.
 *  NOTE:  a value > 16 will overflow a long integer.
 */

#include <stdio.h>

long factor( int param );

main()
{
   int num;
   long result;
   /* Display a prompt */
   printf( "Type a number: " );
   /* Input a numeric value, assign to num */
   scanf( "%d", &num );
   result = factor( num );
   printf( "Factorial of %d is %ld\n", num, result );
}

long factor( int param )
{
   long value = 1;
   while( param > 1 )
   {
      value = value * param;
      param = param - 1;
   }
   return value;
}
