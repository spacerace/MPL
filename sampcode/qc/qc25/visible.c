/* VISIBLE.C: Demonstrate local visibility. */

#include <stdio.h>
void be_bop( void );

main()
{
   int val = 10;
   be_bop();
}

void be_bop( void )
{
   printf( "val = %d", val ); /* Error! */
}
