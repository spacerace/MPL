/* REALG.C: Example of real-coordinate graphics */
#include <stdio.h>
#include <conio.h>
#include <graph.h>

#define TRUE 1
#define FALSE 0

int four_colors( void );
void three_graphs( void );
void grid_shape( void );

int halfx, halfy;
struct videoconfig screen;
double bananas[] =
{   -0.3, -0.2, -0.224, -0.1, -0.5, +0.21, +2.9,
   +0.3, +0.2, 0.0, -0.885, -1.1, -0.3, -0.2,
   +.001, +.005, +0.14, 0.0, -0.9, -0.13, +0.3
};

main()
{
   if( four_colors() )
      three_graphs();
   else
      printf( "This program requires a CGA, EGA, or VGA graphics card.\n" );
}

int four_colors( void )
{
   _getvideoconfig( &screen );
   switch( screen.adapter )
   {
      case _CGA:
      case _OCGA:
         _setvideomode( _MRES4COLOR );
         break;
      case _EGA:
      case _OEGA:
         _setvideomode( _ERESCOLOR );
         break;
      case _VGA:
      case _OVGA:
         _setvideomode( _VRES16COLOR );
         break;
      default:
         return( FALSE );
   }
   _getvideoconfig( &screen );
   return( TRUE );
}

void three_graphs( void )
{
   int xwidth, yheight, cols, rows;
   struct _wxycoord upleft, botright;

   _clearscreen( _GCLEARSCREEN );
   xwidth = screen.numxpixels;
   yheight = screen.numypixels;
   halfx = xwidth/2;
   halfy = yheight/2;
   cols = screen.numtextcols;
   rows = screen.numtextrows;
   
   /* first window */
   _setviewport( 0, 0, halfx-1, halfy-1 );
   _settextwindow( 1, 1, rows/2, cols/2 );
   _setwindow( FALSE, -2.0, -2.0, 2.0, 2.0 );
   grid_shape();
   _rectangle( _GBORDER, 0, 0, halfx-1, halfy-1 );
   
   /* second window */
   _setviewport( halfx, 0, xwidth-1, halfy-1 );
   _settextwindow( 1, cols/2+1, rows/2, cols );
   _setwindow( FALSE, -3.0, -3.0, 3.0, 3.0 );
   grid_shape();
   _rectangle_w( _GBORDER, -3.0, -3.0, 3.0, 3.0 );   
   
   /* third window */
   _setviewport( 0, halfy, xwidth-1, yheight-1 );
   _settextwindow( rows/2+1, 1, rows, cols );
   _setwindow( TRUE, -3.0, -1.5, 1.5, 1.5 );
   grid_shape();
   upleft.wx = -3.0;
   upleft.wy = -1.5;
   botright.wx = 1.5;
   botright.wy = 1.5;
   _rectangle_wxy( _GBORDER, &upleft, &botright );
   
   getch();
   _setvideomode( _DEFAULTMODE );
}

void grid_shape( void )
{
   int i, numc, x1, y1, x2, y2;
   double x, y;
   char txt[80];
   
   numc = screen.numcolors;
   for( i = 1; i < numc; i++ )
   {
      _settextposition( i, 2 );
      _settextcolor( i );
      sprintf( txt, "Color %d", i );
      _outtext( txt );
   }
   
   _setcolor( 1 );
   _rectangle_w( _GBORDER, -1.0, -1.0, 1.0, 1.0 );
   _rectangle_w( _GBORDER, -1.02, -1.02, 1.02, 1.02 );
   
   for( x = -0.9, i = 0; x < 0.9; x += 0.1 )
   {
      _setcolor( 2 );
      _moveto_w( x, -1.0 );
      _lineto_w( x, 1.0 );
      _moveto_w( -1.0, x );
      _lineto_w( 1.0, x );
   
      _setcolor( 3 );
      _moveto_w( x - 0.1, bananas[i++] );
      _lineto_w( x, bananas[i] );
   }
   _moveto_w( 0.9, bananas[i++] );
   _lineto_w( 1.0, bananas[i] );
}
