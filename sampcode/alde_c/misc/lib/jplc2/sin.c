/* 1.1  07-08-85						(sin.c) 
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *	Programmmed using the algorithms given in:
 *
 *	Coty, William J., Jr., and Waite, William, SOFTWARE MANUAL FOR
 *	THE ELEMENTARY FUNCTIONS, Prentice-Hall Series in Computational
 *	Mathematics, Prentice-Hall, Inc., Inglewood Cliffs, NJ, 1980, 
 *	pp. 125-149.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define R0	1.0
#define R1	0.16666666666666665052e+00
#define R2	0.83333333333331650314e-02
#define R3	0.19841269841201840457e-03
#define R4	0.27557319210152756119e-05
#define R5	0.25052106798274584544e-07
#define R6	0.16058936490371589114e-09
#define R7	0.76429178068910467734e-12
#define R8	0.27204790957888846175e-14

#define C1	3.1416015625			/* C1 - C2 = PI		*/
#define C2	8.9089102067615373566e-6;

LOCAL double sincos();

/*\p*********************************************************************/
	double
sin(x)				/* return trigonometric sine of x	*/

/*----------------------------------------------------------------------*/
double x;
{
	if (x < 0.0)
		return sincos(x, -x, 1);
	else
		return sincos(x, x, 0);
}

/************************************************************************/
	double
cos(x)				/* return trigonometric cosine of x	*/

/*----------------------------------------------------------------------*/
double x;
{
	return sincos(x, ABS(x) + PIover2, 0);
}

/************************************************************************/
	LOCAL double
sincos(x, y, sgnflag)		/* compute both sine and cosine		*/

/*----------------------------------------------------------------------*/
double x, y;
{
	double f, xn, r, g;

	if (y >= MAXANGLE)
	{	errno = ERANGE;
		return 0.0;
	}
	if (modf(y * INVPI, &xn) >= 0.5)
		++xn;
	if ((int)xn & 1)
		sgnflag = !sgnflag;
	if (y ISNT ABS(x))
		xn -= 0.5;
	g = modf(ABS(x), &x);
	f = ((x - xn * C1) + g) + xn * C2;
	if (ABS(f) > FADEOUT)
	{	g = f * f;
		r = (((((((R8*g-R7)*g+R6)*g-R5)*g
			+R4)*g-R3)*g+R2)*g-R1)*g;
		f += f * r;
	}
	return (sgnflag ? -f : f);
}
