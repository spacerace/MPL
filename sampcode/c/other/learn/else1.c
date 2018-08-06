/* ELSE1.C: Demonstrate else-if construct. */

#include <stdio.h>
#include <conio.h>
#define  B_KEY  'b'
#define  ENTER_KEY '\r'

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getch();
   if( ch == B_KEY )
      printf( "Beep!\a\n" );
   else
      if( ch == ENTER_KEY )
	 printf( "What a boring selection...\n" );
   else
      printf( "Bye bye.\n" );
}
