/* pixels.c -- create shapes       */
/*             from random pixels  */

#include <graph.h> /* for graphics */

main()
{
    int pixels, xpos, ypos;
    /* window coordinates */
    int xmin = 100, xmax = 540;
    int ymin = 50,  ymax = 150;

    srand(0);               /* init random nums */
    _setvideomode(_HRESBW); /* CGA 640 x 200    */
    _setcolor(1);           /* white foreground */

    /* generate random pixel locations      */
    for (pixels = 1; pixels < 10000; pixels++)
        {
        xpos = rand() % 639;
        ypos = rand() % 199;

        /* set pixel if within window */
        if ((xpos > xmin && xpos < xmax) &&
             (ypos > ymin && ypos < ymax))
           _setpixel(xpos, ypos);
        }
    getch(); /* freeze screen until key pressed */
             /* restore original video mode */
    _setvideomode(_DEFAULTMODE);
}
