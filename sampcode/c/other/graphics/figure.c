/* FIGURE.C illustrates graphics drawing functions including:
 *      _setpixel   _lineto     _moveto     _rectangle      _ellipse
 *      _arc        _pie
 *
 * Window versions of graphics drawing functions (such as _rectangle_w,
 * _ellipse_wxy, and _lineto_w) are illustrated in WINDOW.C and GEDIT.C.
 */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

main()
{
    short x, y;
    short mode = _VRES16COLOR;

    while( !_setvideomode( mode ) )     /* Find best graphics mode   */
        mode--;
    if( mode == _TEXTMONO )
        exit( 1 );                      /* No graphics available     */

    for( x = 10, y = 50; y < 90; x += 2, y += 3 )/* Draw pixels      */
        _setpixel( x, y );
    getch();

    for( x = 60, y = 50; y < 90; y += 3 )        /* Draw lines       */
    {
        _moveto( x, y );
        _lineto( x + 20, y );
    }
    getch();

    x = 110; y = 70;                             /* Draw rectangles  */
    _rectangle( _GBORDER,       x - 20, y - 20, x, y );
    _rectangle( _GFILLINTERIOR, x + 20, y + 20, x, y );
    getch();

    x = 160;                                     /* Draw ellipses    */
    _ellipse( _GBORDER,       x - 20, y - 20, x, y );
    _ellipse( _GFILLINTERIOR, x + 20, y + 20, x, y );
    getch();

    x = 210;                                     /* Draw arcs        */
    _arc( x - 20, y - 20, x, y, x, y - 10, x - 10, y );
    _arc( x + 20, y + 20, x, y, x + 10, y + 20, x + 20, y + 10 );
    getch();

    x = 260;                                    /* Draw pies        */
    _pie( _GBORDER,       x - 20, y - 20, x, y, x, y - 10, x - 10, y );
    _pie( _GFILLINTERIOR, x + 20, y + 20, x, y,
                          x + 10, y + 20, x + 20, y + 10 );
    getch();

    exit( !_setvideomode( _DEFAULTMODE ) );
}
