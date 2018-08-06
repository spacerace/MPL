/* SWITCH.C: Demonstrate switch statement. */

#include <stdio.h>
#include <conio.h>
#define  B_KEY  'b'
#define  ENTER_KEY  '\r'

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getch();
   switch( ch )
   {
      case B_KEY:
	 printf( "Beep!\a\n" );
	 break;
      case ENTER_KEY:
	 printf( "What a boring selection...\n" );
	 break;
      default:
	 printf( "Bye bye." );
	 break;
   }
}
