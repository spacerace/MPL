/* HORIZON.C: VGA graphics with cycling of 256 colors */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

#define RED 0x0000003FL
#define GRN 0x00003F00L
#define BLU 0x003F0000L
#define WHT 0x003F3F3FL
#define STEP 21

struct videoconfig screen;
long int rainbow[512];

main()
{
   int i;
   long int col, gray;

   if( _setvideomode( _MRES256COLOR ) == 0 )
   {
      printf("This program requires a VGA card.\n" );
      exit( 0 );
   }
   for( col = 0; col < 64; col++ )
   {
      gray = col | (col << 8) | (col << 16);
      rainbow[col] = rainbow[col + 256] = BLU & gray;
      rainbow[col + 64] = rainbow[col + 64 + 256] = BLU | gray;
      rainbow[col + 128] = rainbow[col + 128 + 256] = RED | (WHT & ~gray);
      rainbow[col + 192] = rainbow[col + 192 + 256] = RED & ~gray;
   }
   _setvieworg( 160, 85 );

   for( i = 0; i < 255; i++ )
   {
      _setcolor( 255 - i );
      _moveto( i, i - 255 );
      _lineto( -i, 255 - i );
      _moveto( -i, i - 255 );
      _lineto( i, 255 - i );
      _ellipse( _GBORDER, -i, -i / 2, i, i / 2 );
   }
   for( i = 0; !kbhit(); i += STEP, i %= 256 )
      _remapallpalette( &(rainbow[i]) );

   _setvideomode( _DEFAULTMODE );
}
