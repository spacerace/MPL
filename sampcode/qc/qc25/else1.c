/* ELSE1.C: Demonstrate else-if construct. */

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
      if( ch == '\r' )
         printf( "Enter\n" );
   else
      printf( "Bye bye\n" );
}
