/* ELSE.C: Demonstrate else clause. */

#include <stdio.h>
#include <conio.h>
#define  B_KEY  'b'

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getch();
   if( ch == B_KEY )
      printf( "Beep!\a\n" );
   else
      printf( "Bye bye.\n" );
}
