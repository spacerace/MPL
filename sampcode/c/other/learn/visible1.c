/* VISIBLE1.C: Demonstrate visibility of variables. 
*/

#include <stdio.h>

void be_bop( int param );

main()
{
   int var1 = 10;
   be_bop( var1 );
}

void be_bop( int param )
{
   printf( "%d\n", param );
}
