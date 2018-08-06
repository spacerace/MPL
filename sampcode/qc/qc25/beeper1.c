/* BEEPER1.C: Demonstrate passing arguments */
#include <stdio.h>

void beep( int num_beep );

main()
{
   printf( "Time to beep\n" );
   beep( 5 );
   printf( "All done\n" );
}

void beep( int num_beep )
{
   while( num_beep > 0  )
   {
      printf( "Beep!\a\n" );
      num_beep = num_beep - 1;
   }
}
