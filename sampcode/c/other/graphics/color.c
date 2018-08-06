 /* COLOR.C: Sets a medium resolution mode with maximum color choices */ 

#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <conio.h>
struct videoconfig vc;

main()
{
   if( _setvideomode( _MRES256COLOR ) )
      ;
   else if( _setvideomode( _MRES16COLOR ) )
      ;
   else if( _setvideomode( _MRES4COLOR ) )
      ;
   else   {
      printf( "Error: No color graphics capability\n" );
      exit( 0 );
   }

   _getvideoconfig( &vc );

   printf( "%d available colors\n", vc.numcolors );
   printf( "%d horizontal pixels\n", vc.numxpixels );
   printf( "%d vertical pixels\n", vc.numypixels );

   getch();
   _clearscreen( _GCLEARSCREEN );
   _setvideomode( _DEFAULTMODE );
}
