/* SWITCH.C: Demonstrate switch statement. */

#include <stdio.h>
#include <conio.h>

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getch();
   switch( ch )
   {
      case 'b':
         printf( "Beep!\a\n" );
         break;
      case '\r':
         printf( "Enter\n" );
         break;
      default:
         printf( "Bye bye" );
         break;
   }
}
