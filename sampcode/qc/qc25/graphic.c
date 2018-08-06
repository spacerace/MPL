/* GRAPHIC.C: Displays every graphics mode */
#include <stdio.h>
#include <graph.h>
#include <conio.h>

struct videoconfig screen;
int modes[12] =
{
   _MRES4COLOR, _MRESNOCOLOR, _HRESBW,
   _HERCMONO,
   _MRES16COLOR, _HRES16COLOR, _ERESNOCOLOR, _ERESCOLOR,
   _VRES2COLOR, _VRES16COLOR, _MRES256COLOR, _ORESCOLOR
};

void print_menu( void );
void show_mode( char );

main()
{
   char key;
   print_menu();
   while( (key = getch()) != 'x' )
      show_mode( key );
}

void print_menu( void )
{
   _setvideomode( _DEFAULTMODE );
   _clearscreen( _GCLEARSCREEN );
   printf( "Please choose a graphics mode\nType 'x' to exit.\n\n" );
   printf( "0 _MRES4COLOR\n1 _MRESNOCOLOR\n2 _HRESBW\n" );
   printf( "3 _HERCMONO\n4 _MRES16COLOR\n5 _HRES16COLOR\n" );
   printf( "6 _ERESNOCOLOR\n7 _ERESCOLOR\n" );
   printf( "8 _VRES2COLOR\n9 _VRES16COLOR\na _MRES256COLOR\n" );
   printf( "b _ORESCOLOR\n" );
}

void show_mode( char which )
{
   int nc, i;
   int height, width;
   int mode = which;

   if( mode < '0' || mode > '9' )
      if( mode == 'a' )
         mode = '9' + 1;
      else if( mode == 'b' )
         mode = '9' + 2;
      else
         return;

   if( _setvideomode( modes[mode - '0'] ) )
   {
      _getvideoconfig( &screen );
      nc = screen.numcolors;
      width = screen.numxpixels/nc;
      height = screen.numypixels/2;
      for( i = 0; i < nc; i++ )
      {
         _setcolor( i );
         _rectangle( _GFILLINTERIOR, i * width, 0, (i + 1) * width, height );
      }
   }
   else
   {
      printf( " \nVideo mode %c is not available.\n", which);
      printf( "Please press a key.\n" );
   }
   getch();
   _setvideomode( _DEFAULTMODE );
   print_menu();
}
