/* VISIBLE.C: Demonstrate visibility of variables. 
*/

#include <stdio.h>
void be_bop( void );

main()
{
   int var1 = 10;
   be_bop();
}

void be_bop( void )
{
   printf( "var1 = %d", var1 ); /* Error! */
}
