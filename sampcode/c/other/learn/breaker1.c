/* BREAKER1.C: Break only exits one loop. */

#include <stdio.h>
#include <conio.h>

main()
{
   char ch;
   printf( "Press any key. Press RETURN to quit.\n" );
   do
   {
      while( ( ch = getche() ) != '\r' )
      {
	 if( ch == '\t' )
	 {
	    printf( "\a\nYou pressed TAB.\n" );
	    break;
	 }
      }
   } while( ch != '\r' );
   printf( "\nBye bye." );
}
