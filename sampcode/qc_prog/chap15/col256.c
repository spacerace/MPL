/*  col256.c  -- show 256 colors in mode 19             */
/* If you load graphics.qlb, no program list is needed. */

#include <stdio.h>
#include <graph.h>
#include <conio.h>
#define ESC '\033'
#define ROWS 16
#define COLS 16
main()
{
    struct videoconfig vc;
    int mode = _MRES256COLOR;
    short xmax, ymax;           /* screen size */
    short xcs[ROWS][COLS];      /* coordinates of the */
    short ycs[ROWS][COLS];      /* 256 rectangles     */
    short row, col;


    if (_setvideomode(mode) == 0)
        {
        fprintf(stderr, "%d mode not supported\n", mode);
        exit(1);
        }
    _getvideoconfig(&vc);

    xmax = vc.numxpixels - 1;
    ymax = vc.numypixels - 1;

    /* Compute an interior point for each rectangle. */
    for (col = 0; col < COLS; col++)
        for (row = 0; row < ROWS; row++)
            {
            xcs[row][col] =  col * xmax / COLS + 5;
            ycs[row][col] =  row * ymax / ROWS + 5;

            }

    /* draw outside boundary */
    _setcolor(1);
    _rectangle(_GBORDER, 0, 0, xmax, ymax);

    /* draw gridwork */
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

    /*  fill in rectangles with palette colors */
    for (col = 0; col < COLS; col++)
        for (row = 0; row < ROWS; row++)
            {
            _setcolor(row * ROWS + col);
            _floodfill(xcs[row][col], ycs[row][col],1);
            }

    /* terminate program */
    getch();
    _setvideomode(_DEFAULTMODE);
}
