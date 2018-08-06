/*  rings.c -- shoots colored rings                   */
/*  This program illustrates _remapallpalette() and   */
/*  how it can be used to produce the appearance of   */
/*  motion. The program is intended for EGA modes 13, */
/*  14, and 16.                                       */
/* Program list: rings.c                              */
/* If you load graphics.qlb, no program list is needed*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#define ESC '\033'

long Colors[16] = {_BLACK, _BLUE, _GREEN, _CYAN,
                 _RED, _MAGENTA, _BROWN, _WHITE,
                 _GRAY, _LIGHTBLUE, _LIGHTGREEN,
                 _LIGHTCYAN, _LIGHTRED, _LIGHTMAGENTA,
                 _LIGHTYELLOW,_BRIGHTWHITE };

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    float aspect;
    short xmax, ymax;
    long int newpalette[16];
    long int temp;
    int index;
    int hot1 = 1;  /* first colored ring  */
    int hot2 = 8;  /* second colored ring */
    int mode = _ERESCOLOR;
    int ch;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (mode < 13)
    {
        fprintf(stderr,"Requires EGA or VGA mode\n");
        exit(1);
    }
    if (_setvideomode(mode) == 0)
    {
        fprintf(stderr,"% d mode unavailable\n", mode);
        exit(2);
    }
    _getvideoconfig(&vc);
    _setlogorg(vc.numxpixels / 2 - 1, vc.numypixels / 2 - 1);
    aspect = (10.0 * vc.numypixels) / (6.5 * vc.numxpixels);
    ymax = vc.numypixels / 2 - 2;
    xmax = ymax / aspect;
    for (index = 2; index < 16; index++)
        newpalette[index] = _LIGHTBLUE;
    newpalette[0] = _GRAY;
    newpalette[hot1] = _RED;
    newpalette[hot2] = _LIGHTRED;
    _remapallpalette(newpalette);  /* set initial palette */
    _setcolor(1);
    _ellipse(_GFILLINTERIOR, -xmax, -ymax, xmax, ymax);
    /* draw concentric circles */
    for (index = 2; index < 16; index++)
        {
        xmax /= 1.4;
        ymax /= 1.4;
        _setcolor(index);
        _ellipse(_GFILLINTERIOR, -xmax, -ymax, xmax, ymax);
        }
    do
        {
        while (!kbhit())
            {
            temp = newpalette[15];
            for(index = 15; index > 1; index--)
                newpalette[index] = newpalette[index - 1];
            newpalette[1] = temp;
            _remapallpalette(newpalette);
            hot1 = hot1 % 15 + 1;  /* index of colored ring */
            hot2 = hot2 % 15 + 1;
            }
        ch = getch();
        if (ch > '1' && ch < '8')  /* reassign colors */
            {
            newpalette[hot1] = Colors[ ch - '0'];
            newpalette[hot2] = Colors[ ch - '0' + 8];
            }
         } while (ch != ESC);
    _clearscreen(_GCLEARSCREEN);
    _setvideomode(_DEFAULTMODE);
}
