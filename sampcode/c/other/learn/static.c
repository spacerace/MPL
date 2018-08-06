/* STATIC.C: Demonstrate static variables. */

#include <stdio.h>

void add_ten( int val );

main()
{
   int val = 10;
   add_ten( val++ );
   add_ten( val );
}

void add_ten( int value )
{
   static int methuselah;
   if( value == 10 )
      methuselah = 0;
   methuselah = methuselah + value;
   printf( "methuselah = %d\n", methuselah );
}
