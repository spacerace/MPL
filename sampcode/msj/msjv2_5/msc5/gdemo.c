/*
Microsoft Systems Journal
Volume 2; Issue 5; November, 1987

Code Listings For:

	GDEMO
	pp. 67-76

Author(s): Augie Hansen
Title:     Microsoft C Optimizing Compiler 5.0 Offers Improved Speed and 
           Code Size





Figure 5
========

*/

/********************************************************************
* NAME: gdemo
*
* DESCRIPTION: This is a simple graphics demonstration program.  Its
*  purpose it to show how to select a graphics mode, do some simple
*  graphics work, and then return to DOS after restoring the user's
*  previous video environment.
********************************************************************/

#include <graph.h>
#include <math.h>

#define RHEIGHT         160
#define RWIDTH          300
#define MSG_ROW         25
#define MSG_COL         8
#define M_NOWAIT        0
#define M_WAIT          1

/*
 * Macro to plot points with the normal sense of the y axis.  The
 * graphics library default is inverted (y increases in value moving
 * down the screen).
 */

#define PLOT(x_, y_)    _setpixel((x_), -(y_))

extern void Message(short, short, char *, short);

int
main()
{
    struct videoconfig config;  /* video configuration data */
    short x_org, y_org;         /* coordinates of the origin */
    short x_ul, y_ul;           /* upper-left corner */
    short x_lr, y_lr;           /* lower-right corner */
    short x, y;

    static char prompt[] = { "Press a key to continue..." };

    /*
     * Set up the medium-resloution graphics screen and set
     * the logical origin to the center of the screen.
     */

    _setvideomode(_MRES16COLOR);
    _getvideoconfig(&config);
    x_org = config.numxpixels / 2 - 1;
    y_org = config.numypixels / 2 - 1;
    _setlogorg(x_org, y_org);

    /*
     * Draw a border rectangle and coordinate system.
     */

    x_ul = -RWIDTH / 2;
    y_ul = RHEIGHT / 2;
    x_lr = RWIDTH / 2;
    y_lr = -RHEIGHT / 2;
    _rectangle(_GBORDER, x_ul, y_ul, x_lr, y_lr);
    _moveto(x_ul, 0);
    _lineto(x_lr, 0);
    _moveto(0, y_ul);
    _lineto(0, y_lr);

    /*
     * Plot some curves.
     */

    for (x = -100; x <= 100; ++x) {
	y = x / 2;
	PLOT(x, y);
    }
    Message(11, 27, "y = x / 2", M_NOWAIT);
    for (x = -20; x <= 20; ++x) {
	y = (x * x / 3) - 75;
	PLOT(x, y);
    }
    Message(22, 3, "y = x**2 / 3 - 75", M_NOWAIT);

    /*
     * Wait for the user's command to continue.
     */
    Message(MSG_ROW, MSG_COL, prompt, M_WAIT);

    /*
     * Restore the original video mode.
     */
    _setvideomode(_DEFAULTMODE);

    return (0);
}
