/*  rect.c -- illustrates logical coordinates,         */
/*            the _rectangle() and _setlinestyle()     */
/*            functions                                */
/* If you load graphics.qlb, no program list is needed */

#include <stdio.h>
#include <graph.h>
#include <conio.h>
#define STYLES 5
short Linestyles[STYLES] = {0xFFFF, 0x8888, 0x7777,
                            0x00FF, 0x8787};

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _MRES4COLOR;
    int xcent, ycent;
    int xsize, ysize;
    int i;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        printf("Can't open that mode.\n");
        exit(1);
    }
    _getvideoconfig(&vc);
    xcent = vc.numxpixels / 2 - 1;
    ycent = vc.numypixels / 2 - 1;
    _setlogorg(xcent, ycent);
    xsize = 0.9 * xcent;
    ysize = 0.9 * ycent;
    _selectpalette(1);
    _setcolor(3);
    _rectangle(_GBORDER, -xsize, -ysize, xsize, ysize);
    xsize *= 0.9;
    ysize *= 0.9;
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, -xsize, -ysize, xsize, ysize);
    for (i = 0; i < 16; i++)
    {
        _setcolor(((i % 2) == 0) ? 2 : 3);
        _setlinestyle(Linestyles[ i % 5 ]);
        xsize *= 0.9;
        ysize *= 0.9;
        _rectangle(_GBORDER, -xsize, -ysize, xsize, ysize);
    }
    getch();      /* Type a key to terminate. */
    _setvideomode(_DEFAULTMODE);
}
