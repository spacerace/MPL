
/***********************************************************
 *               The TULSA IBM C BOARD                     *
 *                   918-664-8737                          *
 *             300/1200 XMODEM, 24 Hours                   *
 **********************************************************/

#include "math.h"
#include "errno.h"

double pow(a,b)
double a,b;
{
        double loga;
        extern int errno;

        if (a<=0.0) {
                if (a<0.0 || a==0.0 && b<=0.0) {
                        errno = EDOM;
                        return -HUGE;
                }
                else return 0.0;
        }
        loga = log(a);
        loga *= b;
        if (loga > LOGHUGE) {
                errno = ERANGE;
                return HUGE;
        }
        if (loga < LOGTINY) {
                errno = ERANGE;
                return 0.0;
        }
        return exp(loga);
}

