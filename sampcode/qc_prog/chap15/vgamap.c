/*  vgamap.c  -- remaps the vga mode 19 palette        */
/* Program list: vgamap.c (for rand())                 */
#include <stdio.h>
#include <graph.h>
#include <conio.h>
#define ESC '\033'
#define PALSIZE 256
#define ROWS 16
#define COLS 16
#define MIDBLUE 0x190000L
long newpal[PALSIZE]; /* array of color values */

main()
{
    struct videoconfig vc;
    int mode = _MRES256COLOR;
    short xmax, ymax;
    short xcs[ROWS][COLS];
    short ycs[ROWS][COLS];
    short row, col;
    long colorval;       /* VGA color value */
    long index;          /* looping index   */
    short palval;        /* palette value */
    int c_base;  /* color base -- blue, green, or red */
    int ch;

    if (_setvideomode(mode) == 0)
        {
        fprintf(stderr, "%d mode not supported\n", mode);
        exit(1);
        }
    _getvideoconfig(&vc);
    xmax = vc.numxpixels - 1;
    ymax = vc.numypixels - 1;
    for (col = 0; col < COLS; col++)
        for (row = 0; row < ROWS; row++)
            {
            xcs[row][col] =  col * xmax / COLS + 5;
            ycs[row][col] =  row * ymax / ROWS + 5;
            }
    _setcolor(1);
    _rectangle(_GBORDER, 0, 0, xmax, ymax);
    for (col = 1; col < COLS ; col++)
        {
        _moveto(col * (xmax + 1) / COLS, 0);
        _lineto(col * (xmax + 1) / COLS, ymax);
        }
    for (row = 1; row < ROWS;  row++)
        {
        _moveto(0, row * (ymax + 1) / ROWS);
        _lineto(xmax, row * (ymax + 1) / ROWS);
        }

    for (col = 0; col < COLS; col++)
        for (row = 0; row < ROWS; row++)
            {
            _setcolor(row * ROWS + col);
            _floodfill(xcs[row][col], ycs[row][col],1);
             }
    getch();

    /*  initialize newpal[] to 64 shades of blue, 64
        shades of green, 64 shades of red, and 64 shades
        of magenta */
    for (index = 0; index < 64; index++)
        {
        newpal[index] = index << 16;
        newpal[index + 64] = index << 8;
        newpal[index + 128] = index;
        newpal[index + 192] = index | MIDBLUE;
        }
    _remapallpalette(newpal);
    getch();

    /* set squares and colors randomly -- ESC
       terminates loop, and other keystrokes toggle
       it on and off */
    do
        {
        while (!kbhit())
            {
            palval = rand() % PALSIZE;
            colorval = 0L;
            for (c_base = 0; c_base < 3; c_base++)
                colorval += ((long) rand() % 64) <<
                             (c_base * 8);
            _remappalette (palval, colorval);
            }
        ch = getch();
        if (ch != ESC)
            ch = getch();
        } while (ch != ESC);
    _setvideomode(_DEFAULTMODE);
}
