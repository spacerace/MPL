/* BEEPER.C: Demonstrate simple function */
#include <stdio.h>

void beep( void);

main()
{
   printf( "Time to beep\n" );
   beep();
   printf( "All done\n" );
}

void beep( void )
{
   printf( "Beep!\a\n" );
}
