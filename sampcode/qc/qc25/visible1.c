/* VISIBLE1.C: Demonstrate local visibility. */

#include <stdio.h>

void be_bop( int param );

main()
{
   int val = 10;
   be_bop( val );
}

void be_bop( int param )
{
   printf( "%d\n", param );
}
