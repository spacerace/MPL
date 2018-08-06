/* CGAPAL.C illustrates CGA palettes using:
 *      _selectpalette
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

long bkcolor[8] = { _BLACK,  _BLUE,     _GREEN,  _CYAN,
                    _RED,    _MAGENTA,  _BROWN,  _WHITE };
char *bkname [] = { "BLACK", "BLUE",    "GREEN", "CYAN",
                    "RED",   "MAGENTA", "BROWN", "WHITE" };
main()
{
    int i, j, k;

    if ( !_setvideomode( _MRES4COLOR ) )
        exit( 1 );
    for( i = 0; i < 4; i++ )
    {
        _selectpalette( i );
        for( k = 0; k < 8; k++ )
        {
            _setbkcolor( bkcolor[k] );
            for( j = 0; j < 4; j++ )
            {
                _settextposition( 1, 1 );
                printf( "Background color: %8s\n", bkname[k] );
                printf( "Palette: %d\nColor: %d\n", i, j );
                _setcolor( j );
                _rectangle( _GFILLINTERIOR, 160, 100, 320, 200 );
                getch();
            }
        }
    }
    exit( !_setvideomode( _DEFAULTMODE ) );
}
