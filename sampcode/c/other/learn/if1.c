/* IF.C: Demonstrate if statement. */

#include <stdio.h>
#include <conio.h>
#define  B_KEY  'b'

main()
{
   char ch;
   printf( "Press the b key to hear a bell.\n" );
   ch = getche();
   if( ch == B_KEY )
      printf( "Beep!\a\n" );
}
