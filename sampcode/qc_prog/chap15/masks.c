/*  masks.c -- illustrates _setfillmask() and         */
/*             _floodfill()                           */
/* Program list: masks.c                              */
/* If you load graphics.qlb, no program list is needed*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
unsigned char Inversemask[8];
unsigned char Masks[3][8] = {
            {0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00},
            {0xff,0x80,0x80,0x80,0xff,0x08,0x08,0x08},
            {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa}};
main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _MRES4COLOR;
    short xc, yc;
    short box, i;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        fprintf(stderr,"Can't set mode %d\n", mode);
        exit(1);
    }
    _getvideoconfig(&vc);
    xc = vc.numxpixels / 2;
    yc = vc.numypixels / 2;
    for (i = 0; i < 8; i++)
        Inversemask[i] = ~Masks[1][i];
    _setlogorg(xc, yc);
    _selectpalette(0);
    _setcolor(1);
    _rectangle(_GBORDER, -xc + 1, -yc + 1, xc - 1, yc - 1);
    _moveto(-xc + 1, -yc / 3);
    _lineto(xc -1, -yc / 3);
    _moveto(-xc + 1, yc / 3);
    _lineto(xc -1, yc / 3);
    for (box = 0; box < 3; box++)
    {
         _setcolor(box + 1);
         _setfillmask(Masks[box]);
         _floodfill(0, (box - 1) * yc / 2, 1);
    }
    _settextposition(5, 10);
    _outtext("Press a key to continue");
    getch();
    _setcolor(3);
    _setfillmask(Inversemask);
    _floodfill (0, 0, 1);
    _setcolor(2);
    _setfillmask(Masks[0]);
    _floodfill(0, yc / 2, 1);
    _settextposition(5, 10);
    _outtext("Press a key to terminate");
    getch();
    _setvideomode(_DEFAULTMODE);
}
