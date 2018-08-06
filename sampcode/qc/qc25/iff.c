/* IFF.C: Demonstrate if statement. */

#include <stdio.h>
#include <conio.h>

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getche();
   if( ch == 'b' )
      printf( "Beep!\a\n" );
}
