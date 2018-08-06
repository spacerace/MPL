/* VISIBLE2.C: Demonstrate external visibility.
*/

#include <stdio.h>

void be_bop( int param );

main()
{
   be_bop( val ); /* Error! */
}

int val = 10;

void be_bop( int param )
{
   printf( "val = %d\n", param );
}
