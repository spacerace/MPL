/* VISIBLE2.C: Demonstrate visibility of variables. 
*/

#include <stdio.h>

void be_bop( int param );

main()
{
   be_bop( var1 ); /* Error! */
}

int var1 = 10;

void be_bop( int param )
{
   printf( "var1 = %d\n", param );
}
