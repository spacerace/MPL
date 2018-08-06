/* ANIMATE.C illustrates animation functions including:
 *          _imagesize     _getimage     _putimage
 */

#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>

short action[5]  = { _GPSET,   _GPRESET, _GXOR,    _GOR,     _GAND    };
char *descrip[5] = { "PSET  ", "PRESET", "XOR   ", "OR    ", "AND   " };

main()
{
    char far *buffer;
    short i, x, y = 30;
    size_t imsize;
    short mode = _VRES16COLOR;

    while( !_setvideomode( mode ) )         /* Find best graphics mode    */
        mode--;
    if (mode == _TEXTMONO )
        exit( 1 );                          /* No graphics available      */
    _setcolor( 3 );
    for ( i = 0; i < 5; i++ )
    {
        x = 50; y += 40;
        _settextposition( 1, 1 );           /* Display action type        */
        _outtext( descrip[i] );
                                            /* Draw and measure ellipse   */
        _ellipse( _GFILLINTERIOR, x - 15, y - 15, x + 15, y + 15 );
        imsize = (size_t)_imagesize( x - 16, y - 16, x + 16, y + 16 );
        buffer = (char far *)_fmalloc( imsize );
        if ( buffer == (char far *)NULL )
            exit( 1 );
                                            /* Get master copy of ellipse */
        _getimage( x - 16, y - 16, x + 16, y + 16, buffer );
        while( x < 260 )                    /* Copy row of ellipses       */
        {                                   /*   with specified action    */
            x += 5;
            _putimage( x - 16, y - 16, buffer, action[i] );
        }
        _ffree( (char far *)buffer );       /* Free memory                */
        getch();
    }
    exit( !_setvideomode( _DEFAULTMODE ) );
}
