/* arrow.c -- fill inside and outside of a line       */
/*            drawing                                 */
/* If you load graphics.qlb, no program list is needed*/

#include <stdio.h>
#include <graph.h>
#include <conio.h>
#define ESC '\033'
#define BKCOLS 16   /* use 16 background colors */
long Bkcolors[BKCOLS] = {_BLACK, _BLUE, _GREEN, _CYAN,
                 _RED, _MAGENTA, _BROWN, _WHITE,
                 _GRAY, _LIGHTBLUE, _LIGHTGREEN,
                 _LIGHTCYAN, _LIGHTRED, _LIGHTMAGENTA,
                 _LIGHTYELLOW,_BRIGHTWHITE };
char Mask[8] = {0x90, 0x68, 0x34, 0x19, 0x19, 0x34, 0x68,
                0x90};
char Outmask[8] = {0xff, 0x80, 0x80, 0x80, 0xff, 0x08, 0x08,
                   0x08};
main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _MRES4COLOR;
    float x1, y1, x2, y2, x3, y3, y4, x5, y5;
    long bk = _BLUE;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
        {
        printf("Can't set mode %d.\n", mode);
        exit(1);
        }
    _getvideoconfig(&vc);

    x1 = 0.1 * vc.numxpixels;
    x2 = 0.7 * vc.numxpixels;
    x3 = 0.6 * vc.numxpixels;
    x5 = 0.9 * vc.numxpixels;
    y1 = 0.45 * vc.numypixels;
    y2 = 0.55 * vc.numypixels;
    y3 = 0.3 * vc.numypixels;
    y4 = 0.7 * vc.numypixels;
    y5 = 0.5 * vc.numypixels;
    _selectpalette(0);
    _setcolor(1);
    _moveto(x1, y1);
    _lineto(x2, y1);
    _lineto(x3, y3);
    _lineto(x5, y5);
    _lineto(x3, y4);
    _lineto(x2, y2);
    _lineto(x1, y2);
    _lineto(x1, y1);
    _setcolor(2);
    _setfillmask(Mask);
    _floodfill(x2, y5, 1) ;
    _setcolor(3);
    _setfillmask(Outmask);     /* restores default mask */
    _floodfill(5, 5, 1) ;
    _settextcolor(1);
    _settextposition(23, 0);
    _outtext("Press <enter> to change background.");
    _settextposition(24, 0);
    _outtext("Press <esc> to end.");
    while (getch() != ESC)
        _setbkcolor(Bkcolors[++bk % BKCOLS]);
    _setvideomode(_DEFAULTMODE);
}
