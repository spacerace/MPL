/* FOR2.C: Demonstrate variations on for loop. */

#include <stdio.h>
#include <conio.h>

main()
{
   for( printf( "Type something\n" ); getche() != '\r'; )
      ; /* Null statement */
}
