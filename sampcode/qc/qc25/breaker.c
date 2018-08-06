/* BREAKER.C: Demonstrate break statement. */

#include <stdio.h>
#include <conio.h>

main()
{
   char ch;
   printf( "Press any key. Press Tab to quit.\n" );
   while( 1 )
   {
      ch = getche();
      if( ch == '\t' )
      {
	 printf( "\a\nYou pressed Tab\n" );
	 break;
      }
   }
}
