/* 1.0  01-22-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *	Integration step of first-order ordinary differential equation
 *	using the 4th-order Runge-Kutta formulas, given in
 *
 *	HANDBOOK OF MATHEMATICAL FUNCTIONS, U. S. Dept. of Commerce,
 *	National Bureau of Standards Applied Mathematics Series No. 55,
 *	formula 25.5.10, page 896, June, 1964.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	VOID
rkstep(f, h, t, yp)	/* Store approximate y(t+h), for y' = f(t, y),
			   where y = *yp, using 4th-order Runge-Kutta
			   method.					*/
/*----------------------------------------------------------------------*/
double (*f)(), h, t, *yp;
{
	double k1, k2, k3, k4, h2, y, ldexp();

	h2 = ldexp(h, -1);
	y = *yp;
	k1 = h * (*f)(t, y);
	k2 = h * (*f)(t + h2, y + ldexp(k1, -1));
	k3 = h * (*f)(t + h2, y + ldexp(k2, -1));
	k4 = h * (*f)(t + h, y + k3);
	*yp += (k1 + ldexp(k2 + k3, 1) + k4) / 6.0;
}
