
/***********************************************************
 *               The TULSA IBM C BOARD                     *
 *                   918-664-8737                          *
 *             300/1200 XMODEM, 24 Hours                   *
 **********************************************************/


#include "math.h"
#include "errno.h"

double sqrt(x)
double x;
{
        double f, y;
        int n;
        extern int errno;

        if (x == 0.0)
                return x;
        if (x < 0.0) {
                errno = EDOM;
                return 0.0;
        }
        f = frexp(x, &n);
        y = 0.41731 + 0.59016 * f;
        y = (y + f/y);
        y = ldexp(y,-2) + f/y;  /* fast calculation of y2 */
        y = ldexp(y + f/y, -1);
        y = ldexp(y + f/y, -1);

        if (n&1) {
                y *= 0.70710678118654752440;
                ++n;
        }
        return ldexp(y,n/2);
}

