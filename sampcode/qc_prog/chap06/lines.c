/* lines.c -- calls line() with */
/*            five parameters   */

#include <graph.h>

main()
{
    void line (x1, y1, x2, y2, color);

    int x1, x2, y1, y2, i, color;

    _setvideomode(_MRES16COLOR); /* 320 x 200  16 col. */
    srand(2);                    /* new random seed    */
    for (i = 0; i < 100; i++)
        {
        x1 = rand() % 319;       /* random coordinates */
        x2 = rand() % 319;
        y1 = rand() % 199;
        y2 = rand() % 199;
        color = (rand() % 14) + 1; /* random color 1-15 */
        line(x1, y1, x2, y2, color); /* draw a line   */
        }
    while(!kbhit() ); /* wait for key to be hit */

    _setvideomode(_DEFAULTMODE); /* restore video mode */
}

void line (x1, y1, x2, y2, color)
int x1, y1, x2, y2, color;
{
    _moveto(x1, y1); /* position at first endpoint   */
    _setcolor(color);
    _lineto(x2, y2); /* draw line to second endpoint */
}
