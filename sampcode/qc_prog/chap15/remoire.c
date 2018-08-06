/* remoire.c -- adds palette remapping to moire.c      */
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#define ESC '\033'
#define MAXCOLORS 64
#define PALCOLORS 16
long Ega_to_vga(int);

main (argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    unsigned int col, row;
    long colors[MAXCOLORS];
    long palette[PALCOLORS];
    int index;
    int shift = 1;
    int firstcol, firstrow, lastrow, lastcol;
    int mode = _ERESCOLOR;


    if (argc > 1)
        mode = atoi(argv[1]);

    if (_setvideomode(mode) == 0)
        {
        printf("Can't do that mode.\n");
        exit(1);
        }
    /* Create array of all 64 color values. */
    for (index = 0; index < MAXCOLORS; index++)
        colors[index] = Ega_to_vga(index);
    /* Create array of 16 palette choices. */
    for (index = 0; index < PALCOLORS; index++)
        palette[index] = colors[index];
    _remapallpalette(palette);
    _getvideoconfig(&vc);
    firstcol = vc.numxpixels / 5;
    firstrow = vc.numypixels / 5;
    lastcol = 4 * vc.numxpixels / 5;
    lastrow = 4 * vc.numypixels / 5;

    for (col = firstcol; col <= lastcol; ++col)
        {
        for (row = firstrow; row <= lastrow; ++row)
            {
            _setcolor(((row * row + col * col) / 10)
                       % vc.numcolors);
            _setpixel(col, row);
            }
        }
    _settextposition(1, 1);
    _outtext("Type a key to stop or start.");
    _settextposition(2, 1);
    _outtext("Type [Esc] while paused to quit.");
    do
        {
        while (!kbhit())
            {
            /*  Set palette array to new color values. */
            for (index = 1; index < PALCOLORS; index++)
                palette[index] = (colors[(index + shift)
                                  % MAXCOLORS]);
            _remapallpalette(palette);
            shift++;
             }
         getch();  /* pause until key is typed */
        }  while (getch() != ESC);

    _setvideomode(_DEFAULTMODE);  /* reset orig. mode */
}

long Ega_to_vga(egacolor)
int egacolor;       /* ega color value */
{
    static long vgavals[6] = {0x2A0000L, 0x002A00L, 0x00002AL,
                              0x150000L, 0x001500L, 0x000015L};
    long vgacolor = 0L; /* vga color value */
    int bit;

    for (bit = 0; bit < 6; bit++)
        vgacolor += ((egacolor >> bit) &1) * vgavals[bit];
    return (vgacolor);
}
