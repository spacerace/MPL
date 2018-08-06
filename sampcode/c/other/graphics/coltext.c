/* COLTEXT.C: Displays text in color */
#include <stdio.h>
#include <conio.h>
#include <graph.h>

char buffer [80];

main()
{
   int blink,fgd;
   long bgd;

   _clearscreen( _GCLEARSCREEN );
   printf( "Text color attributes:\n" );

   for( blink=0; blink<=16; blink+=16 ) 
   {
      for( bgd=0; bgd<8; bgd++ ) 
      {
         _setbkcolor( bgd );
         _settextposition( bgd + ((blink / 16) * 9) + 3, 1 );
         _settextcolor( 7 );
         sprintf( buffer, "Bgd: %d Fgd:", bgd );
         _outtext( buffer );
         
         for( fgd=0; fgd<16; fgd++ ) 
         {
            _settextcolor( fgd+blink );
            sprintf( buffer, " %2d ", fgd+blink );
            _outtext( buffer );
         }
      }
   }
   getch();
   _setvideomode( _DEFAULTMODE );
}
