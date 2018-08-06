/* ELSE.C: Demonstrate else clause. */

#include <stdio.h>
#include <conio.h>

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getch();
   if( ch == 'b' )
      printf( "Beep!\a\n" );
   else
      printf( "Bye bye\n" );
}
