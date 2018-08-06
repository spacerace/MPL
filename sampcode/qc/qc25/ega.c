/* EGA.C: Demonstrates EGA palettes */
#include <stdio.h>
#include <conio.h>
#include <graph.h>

main()
{
   _setvideomode( _ERESCOLOR );
   _setcolor( 4 );
   _rectangle( _GFILLINTERIOR, 50, 50, 200, 200 );

   _settextposition( 1, 1 );
   printf( "Normal palette\n" );
   printf( "Press a key" );
   getch();

   _remappalette( 4, _BLUE );

   _settextposition( 1, 1 );
   printf( "Remapped palette\n" );
   printf( "Press a key" );
   getch();

   _remappalette( 4, _RED );

   _settextposition( 1, 1 );
   printf( "Restored palette\n" );
   printf( "Press a key to clear the screen" );
   getch();

   _clearscreen( _GCLEARSCREEN );
   _setvideomode( _DEFAULTMODE );
}
