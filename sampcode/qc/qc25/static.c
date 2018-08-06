/* STATIC.C: Demonstrate static variables. */

#include <stdio.h>

void add_val( int value );

main()
{
   add_val( 1 );
   add_val( 5 );
   add_val( 20 );
}

void add_val( int value )
{
   static int methuselah;
   if( value == 1 )
      methuselah = 0;
   methuselah = methuselah + value;
   printf( "methuselah = %d\n", methuselah );
}
