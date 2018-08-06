/* GALAX.C -- creates an ellipse by selecting */
/*              from random pixels            */

#include <graph.h> /* for graphics */
#include <math.h>  /* for sqrt() */
#include <conio.h> /* for kbhit() */

main()
{
    int pixels, radius = 50;
    double center_x = 320, center_y = 100,
        xpos, ypos;
    srand(0);
    _setvideomode(_HRESBW);
    _setcolor(1);
    for (pixels = 1; pixels < 25000; pixels++)
        {
        /* draws filled ellipse, due */
        /* to dimensions of hires screen */
        /* generate random location */
        xpos = rand() % 639;
        ypos = rand() % 199;
        if (sqrt /* is distance within radius? */
              ((xpos - center_x) * (xpos - center_x)
              + (ypos - center_y) * (ypos - center_y))
              < radius)
           _setpixel(xpos, ypos);
        if (kbhit() )
             break; /* exit if key pressed */
        }
    getch(); /* freeze screen until key pressed */
    _setvideomode(_DEFAULTMODE);
}
