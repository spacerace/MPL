/* 1.0  01-09-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *	Programmmed using the algorithm given in:
 *
 *	Coty, William J., Jr., and Waite, William, SOFTWARE MANUAL FOR
 *	THE ELEMENTARY FUNCTIONS, Prentice-Hall Series in Computational
 *	Mathematics, Prentice-Hall, Inc., Inglewood Cliffs, NJ, 1980,
 *	pp. 60-83.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define P0 	0.249999999999999993e+0
#define P1 	0.694360001511792852e-2
#define P2 	0.165203300268279130e-4
#define P(z)	((P2*z+P1)*z+P0)

#define Q0 	0.5
#define Q1 	0.555538666969001188e-1
#define Q2 	0.495862884905441294e-3
#define Q(z) 	((Q2*z+Q1)*z+Q0)

#define C1	0.693359375			/* = 355 / 512		*/
#define C2	2.121944400546905827697e-4	/* C1 - C2 = LOG2	*/

/*\p*********************************************************************/
	double
exp(x)				/* exponential function of x, e^x	*/

/*----------------------------------------------------------------------*/
double x;
{
	double x2, xn, g, r, z;
	int n;
	
	if (x > LOGINFINITY)
	{	errno = ERANGE;
		return INFINITY;
	}
	if (x < LOGLEAST)
	{	errno = ERANGE;
		return 0.0;
	}
	if (ABS(x) < WASHOUT)
		return 1.0;

	x2 = modf(x * LOG2e, &xn);
	if (x2 >= 0.5)
		++xn;
	else if (x2 <= -0.5)
		--xn;
	n = xn;
	x2 = modf(x, &x);		/* in case no guard digit (p. 68) */
	g = ((x - xn * C1) + x2) + xn * C2;
	z = g * g;
	r = P(z) * g;
	r = 0.5 + r / (Q(z) - r);
	return ldexp(r, n + 1);
}
