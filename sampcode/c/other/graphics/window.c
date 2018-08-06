/* WINDOW.C illustrates windows and coordinate systems, using the
 * following functions:
 *    _setviewport   _setvieworg    _setcliprgn     _setwindow
 *    _rectangle     _rectangle_w   _rectangle_wxy  _clearscreen
 *    _ellipse       _ellipse_w     _ellipse_wxy
 *
 * Although not all illustrated here, functions ending in _w
 * are similar to _rectangle_w and _ellipse_w; functions ending
 * in _wxy are similar to _rectangle_wxy and _ellipse_wxy.
 */

#include <conio.h>
#include <graph.h>
#include <stdlib.h>

main()
{
    short xhalf, yhalf, xquar, yquar;
    struct _wxycoord upleft, botright;
    struct videoconfig vc;
    short mode = _VRES16COLOR;

    while( !_setvideomode( mode ) )     /* Find best graphics mode   */
        mode--;
    if( mode == _TEXTMONO )
        exit( 1 );                      /* No graphics available     */
    _getvideoconfig( &vc );

    xhalf = vc.numxpixels / 2;
    yhalf = vc.numypixels / 2;
    xquar = xhalf / 2;
    yquar = yhalf / 2;

    /* First window - integer physical coordinates */
    _setviewport( 0, 0, xhalf - 1, yhalf - 1 );
    _rectangle( _GBORDER, 0,  0, xhalf - 1, yhalf - 1 );
    _ellipse( _GFILLINTERIOR, xquar / 4, yquar / 4,
                              xhalf - (xquar / 4), yhalf - (yquar / 4) );
    getch();
    _clearscreen( _GVIEWPORT );
    _rectangle( _GBORDER, 0,  0, xhalf - 1, yhalf - 1 );

    /* Second window - integer world coordinates with clip region */
    _setcliprgn( xhalf, 0, vc.numxpixels, yhalf );
    _setvieworg( xhalf + xquar - 1, yquar - 1 );
    _rectangle( _GBORDER, -xquar + 1, -yquar + 1, xquar, yquar );
    _ellipse( _GFILLINTERIOR, (-xquar * 3) / 4, (-yquar * 3) / 4,
                              (xquar * 3) / 4, (yquar * 3) / 4 );
    getch();
    _clearscreen( _GVIEWPORT );
    _rectangle( _GBORDER, -xquar + 1, -yquar + 1, xquar, yquar );

    /* Third window */
    _setviewport( xhalf, yhalf, vc.numxpixels - 1, vc.numypixels - 1 );
    _setwindow( 0, -4.0, -5.0, 4.0, 5.0 );
    _rectangle_w( _GBORDER, -4.0, -5.0, 4.0, 5.0 );
    _ellipse_w( _GFILLINTERIOR, -3.0, -3.5, 3.0, 3.5 );
    getch();
    _clearscreen( _GVIEWPORT );
    _rectangle_w( _GBORDER, -4.0, -5.0, 4.0, 5.0 );

    /* Fourth window */
    _setviewport( 0, yhalf, xhalf - 1, vc.numypixels - 1 );
    _setwindow( 0, -4.0, -5.0, 4.0, 5.0 );
    upleft.wx = -4.0;
    upleft.wy = -5.0;
    botright.wx = 4.0;
    botright.wy = 5.0;
    _rectangle_wxy( _GBORDER, &upleft, &botright );
    upleft.wx = -3.0;
    upleft.wy = -3.5;
    botright.wx = 3.0;
    botright.wy = 3.5;
    _ellipse_wxy( _GFILLINTERIOR, &upleft, &botright );

    getch();
    exit( !_setvideomode( _DEFAULTMODE ) );
}
