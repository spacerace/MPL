/* scape.c -- uses nondefault EGA colors              */
/* If you load graphics.qlb, no program list is needed.*/

#include <stdio.h>
#include <graph.h>
#include "egacolor.h"
#include <stdlib.h>
#include <conio.h>
#define SKY (b | B | g)
#define OCEAN b
#define SAND (R | g | b)
#define SUN (R | G | r | g)

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _ERESCOLOR;
    short xmax, ymax, sunx, suny, sunsizex, sunsizey;
    float ar;


    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
        {
        fprintf(stderr,"mode %d not supported\n", mode);
        exit(1);
        }
    _getvideoconfig(&vc);
    xmax = vc.numxpixels - 1;
    ymax = vc.numypixels - 1;
    sunx = 0.7 * xmax;
    suny = 0.2 * ymax;
    ar = (float)(10 * vc.numypixels) / (6.5 * vc.numxpixels);
    sunsizex = xmax / 30;
    sunsizey = ar * sunsizex;
    _remappalette(1, SKY);
    _remappalette(2, OCEAN);
    _remappalette(3, SAND);
    _remappalette(4, SUN);
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, 0, 0, xmax, 2 * ymax / 5);
    _setcolor(4);
    _ellipse(_GFILLINTERIOR, sunx - sunsizex, suny -
             sunsizey, sunx + sunsizex, suny + sunsizey);
    _setcolor(2);
    _rectangle(_GFILLINTERIOR, 0, 2 * ymax / 5, xmax,
               2 * ymax / 3);
    _setcolor(3);
    _rectangle(_GFILLINTERIOR, 0, 2 * ymax / 3, xmax, ymax);
    getch();
    _setvideomode(_DEFAULTMODE);
}
