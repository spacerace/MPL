
/***********************************************************
 *               The TULSA IBM C BOARD                     *
 *                   918-664-8737                          *
 *             300/1200 XMODEM, 24 Hours                   *
 **********************************************************/

#include "math.h"
#include "errno.h"

double arcsine();

double asin(x)
double x;
{
        return arcsine(x,0);
}

double acos(x)
double x;
{
        return arcsine(x,1);
}

#define P1 -0.27368494524164255994e+2
#define P2 +0.57208227877891731407e+2
#define P3 -0.39688862997504877339e+2
#define P4 +0.10152522233806463645e+2
#define P5 -0.69674573447350646411
#define Q0 -0.16421096714498560795e+3
#define Q1 +0.41714430248260412556e+3
#define Q2 -0.38186303361750149284e+3
#define Q3 +0.15095270841030604719e+3
#define Q4 -0.23823859153670238830e+2

#define P(g) ((((P5*g P4)*g P3)*g P2)*g P1)
#define Q(g) (((((g Q4)*g Q3)*g Q2)*g Q1)*g Q0)

double arcsine(x,flg)
double x;
{
        double y, g, r;
        register int i;
        extern int errno;
        static double a[2] = { 0.0, 0.78539816339744830962 };
        static double b[2] = { 1.57079632679489661923, 0.78539816339744830962 };

        y = fabs(x);
        i = flg;
        if (y < 2.3e-10)
                r = y;
        else {
                if (y > 0.5) {
                        i = 1-i;
                        if (y > 1.0) {
                                errno = EDOM;
                                return 0.0;
                        }
                        g = (0.5-y)+0.5;
                        g = ldexp(g,-1);
                        y = sqrt(g);
                        y = -(y+y);
                } else
                        g = y*y;
                r = y + y*
                                ((P(g)*g)
                                /Q(g));
        }
        if (flg) {
                if (x < 0.0)
                        r = (b[i] + r) + b[i];
                else
                        r = (a[i] - r) + a[i];
        } else {
                r = (a[i] + r) + a[i];
                if (x < 0.0)
                        r = -r;
        }
        return r;
}

