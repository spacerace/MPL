/*  SINE.C: Illustrates basic graphics commands */
#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <math.h>
#include <conio.h>
#define PI 3.14159

void graphics_mode( void );
void draw_lines( void );
void sine_wave( void );
void draw_shapes( void );
void end_program( void );
int newx( int );
int newy( int );

struct videoconfig myscreen;
int maxx, maxy;
unsigned char diagmask[8] =
{ 0x93, 0xC9, 0x64, 0xB2, 0x59, 0x2C, 0x96, 0x4B };
unsigned char linemask[8] =
{ 0xFF, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00, 0xCC };

main()
{
   graphics_mode();
   draw_lines();
   sine_wave();
   draw_shapes();
   end_program();
}

void graphics_mode( void )
{
   _getvideoconfig( &myscreen );
   switch( myscreen.adapter )
   {
      case _CGA:
         _setvideomode( _HRESBW );
         break;
      case _OCGA:
         _setvideomode( _ORESCOLOR );
         break;
      case _EGA:
      case _OEGA:
         if( myscreen.monitor == _MONO )
            _setvideomode( _ERESNOCOLOR );
         else
            _setvideomode( _ERESCOLOR );
         break;
      case _VGA:
      case _OVGA:
      case _MCGA:
         _setvideomode( _VRES2COLOR );
         break;
      case _HGC:
         _setvideomode( _HERCMONO );
         break;
      default:
         printf( "This program requires a CGA, EGA, VGA, or Hercules card\n" );
         exit( 0 );
   }
   _getvideoconfig( &myscreen );
   maxx = myscreen.numxpixels - 1;
   maxy = myscreen.numypixels - 1;
}

int newx( int xcoord )
{
   int nx;
   float tempx;
   tempx = ((float) maxx)/ 1000.0;
   tempx = ((float) xcoord) * tempx + 0.5;
   return( (int) tempx );
}

int newy( int ycoord )
{
   int ny;
   float tempy;
   tempy = ((float) maxy)/ 1000.0;
   tempy = ((float) ycoord) * tempy + 0.5;
   return( (int) tempy );
}

void sine_wave( void )
{
   int locx, locy;
   double i, rad;

   for( i = 0.0; i < 1000.0; i += 3.0 )
   {
      rad = -sin( (PI * (float) i) / 250.0 );
      locx = newx( (int) i );
      locy = newy( (int) (rad * 250.0) );
      _setpixel( locx, locy );
   }
}

void draw_shapes( void )
{
   _setlinestyle( 0xFFFF );
   _setfillmask( diagmask );
   _rectangle( _GBORDER, newx(50), newy(-325), newx(200), newy(-425) );
   _rectangle( _GFILLINTERIOR, newx(550), newy(-325), newx(700), newy(-425) );

   _setfillmask( linemask );
   _ellipse( _GBORDER, newx(50), newy(325), newx(200), newy(425) );
   _ellipse( _GFILLINTERIOR, newx(550), newy(325), newx(700), newy(425) );
}

void end_program( void )
{
   getch();
   _setvideomode( _DEFAULTMODE );
}

void draw_lines( void )
{
   _rectangle( _GBORDER, 0, 0, maxx, maxy );
   /* _setcliprgn( 20, 20, maxx - 20, maxy - 20 ); */
   _setvieworg( 0, newy( 500 ) );

   _moveto( 0, 0 );
   _lineto( newx( 1000 ), 0 );
   _setlinestyle( 0xAA3C );
   _moveto( 0, newy( -250) );
   _lineto( newx( 1000 ), newy( -250 ) );

   _setlinestyle( 0x8888 );
   _moveto( 0, newy( 250 ) );
   _lineto( newx( 1000 ), newy( 250 ) );
}
