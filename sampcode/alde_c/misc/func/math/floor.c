
/***********************************************************
 *               The TULSA IBM C BOARD                     *
 *                   918-664-8737                          *
 *             300/1200 XMODEM, 24 Hours                   *
 **********************************************************/


#include "math.h"

double floor(d)
double d;
{
        if (d < 0.0)
                return -ceil(-d);
        modf(d, &d);
        return d;
}

double ceil(d)
double d;
{
        if (d < 0.0)
                return -floor(-d);
        if (modf(d, &d) > 0.0)
                ++d;
        return d;
}

