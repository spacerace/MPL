/* SAMPLER.C: Display sample text in various fonts. */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <graph.h>
#include <string.h>

#define NFONTS 6

main()
{
  static unsigned char *text[2*NFONTS] =
  {
      "COURIER",        "courier",
      "HELV",           "helv",
      "TMS RMN",        "tms rmn",
      "MODERN",         "modern",
      "SCRIPT",         "script",
      "ROMAN",          "roman"
  };
  static unsigned char *face[NFONTS] =
  {
      "t'courier'",
      "t'helv'",
      "t'tms rmn'",
      "t'modern'",
      "t'script'",
      "t'roman'"
  };
  static unsigned char list[20];
  struct videoconfig vc;
  int mode = _VRES16COLOR;
  register i;

  /*  Read header info from all .FON files in
   *  current directory   */
  if(_registerfonts( "*.FON" )<0 )
  {
     _outtext("Error:  can't register fonts");
     exit( 0 );
  }

  /*   Set highest available video mode */
  while( !_setvideomode( mode ) )
     mode--;
  if( mode == _TEXTMONO )
     exit ( 0 );

  /*   Copy video configuration into structure vc */
  _getvideoconfig( &vc );

  /*   Display six lines of sample text */
  for( i = 0; i<NFONTS; i++ )
  {
     strcpy( list, face[i] );
     strcat( list, "h30w24b" );

     if( !_setfont( list ) )
     {
         _setcolor( i + 1 );
         _moveto( 0, (i * vc.numypixels) / NFONTS );
         _outgtext( text[i * 2] );
         _moveto( vc.numxpixels / 2,
                     (i * vc.numypixels) / NFONTS );
         _outgtext( text[(i * 2) + 1] );
     }
     else
     {
         _setvideomode( _DEFAULTMODE );
         _outtext( "Error:  can't set font" );
         exit( 0 );
     }
  }
  getch();
  _setvideomode( _DEFAULTMODE );

  /* Return memory when finished with fonts */
  _unregisterfonts();
  exit( 0 );
}
