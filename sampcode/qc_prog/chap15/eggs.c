/*  eggs.c -- draws colorful eggs                                      */
/*  This program illustrates use of the video configuration            */
/*  structure, the _ellipse() function, the effect of over-            */
/*  lapping solid figures, and a the use of logical coordinates.       */
/*  If you load graphics.qlb, no program list is needed.               */

#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <conio.h>
#define ESC '\033'

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _MRES4COLOR;
    short xcent[3], ycent[3]; /* egg centers */
    short xsize, ysize;       /* egg limits  */
    int egg;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        printf("Can't open mode %d\n", mode);
        exit(1);
    }
    _getvideoconfig(&vc);
    xsize = 0.3 * vc.numxpixels;
    ysize = 0.3 * vc.numypixels;
    xcent[0] = 0.3 * vc.numxpixels;
    xcent[1] = 0.5 * vc.numxpixels;
    xcent[2] = 0.7 * vc.numxpixels;
    ycent[0] = ycent[2] = 0.4 * vc.numypixels;
    ycent[1] = 0.6 * vc.numypixels;

    _selectpalette(0);
    _setbkcolor(_MAGENTA);
    for (egg = 0; egg < 3; egg++)
    {
        _setlogorg(xcent[egg], ycent[egg]);
        _setcolor(egg + 1);
        _ellipse(_GFILLINTERIOR, -xsize, -ysize, xsize, ysize);
    }
    _settextposition(24, 0);
    _settextcolor(1);
    _outtext("Strike any key to terminate.");
    getch();
    _setvideomode(_DEFAULTMODE);
}