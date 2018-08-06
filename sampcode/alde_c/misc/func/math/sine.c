
/***********************************************************
 *               The TULSA IBM C BOARD                     *
 *                   918-664-8737                          *
 *             300/1200 XMODEM, 24 Hours                   *
 **********************************************************/


#include "math.h"
#include "errno.h"

double cos(x)
double x;
{
        double sincos();

        return sincos(x, fabs(x) + 1.57079632679489661923, 0);
}

double sin(x)
double x;
{
        double sincos();

        if (x < 0.0)
                return sincos(x,-x,1);
        else
                return sincos(x,x,0);
}

#define R1 -0.16666666666666665052e+00
#define R2 +0.83333333333331650314e-02
#define R3 -0.19841269841201840457e-03
#define R4 +0.27557319210152756119e-05
#define R5 -0.25052106798274584544e-07
#define R6 +0.16058936490371589114e-09
#define R7 -0.76429178068910467734e-12
#define R8 +0.27204790957888846175e-14

#define YMAX 6.7465e09

static double sincos(x,y,sgn)
double x,y;
{
        double f, xn, r, g;
        extern int errno;

        if (y >= YMAX) {
                errno = ERANGE;
                return 0.0;
        }
        if (modf(y * 0.31830988618379067154, &xn) >= 0.5)
                ++xn;
        if ((int)xn & 1)
                sgn = !sgn;
        if (fabs(x) != y)
                xn -= 0.5;
        g = modf(fabs(x), &x);          /* break into fraction and integer parts */
        f = ((x - xn*3.1416015625) + g) + xn*8.9089102067615373566e-6;
        if (fabs(f) > 2.3283e-10) {
                g = f*f;
                r = (((((((R8*g R7)*g R6)*g R5)*g
                                R4)*g R3)*g R2)*g R1)*g;
                f += f*r;
        }
        if (sgn)
                f = -f;
        return f;
}

