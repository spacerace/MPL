/* 1.0  03-05-85 */
/************************************************************************
 *                      Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, California 91009	1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"

/*----------------------------------------------------------------------*
	This algorithm utilizes Simpson's extended rule, as published in
HANDBOOK OF MATHEMATICAL FUNCTIONS, U. S. Department of Commerce, 
National Bureau of Standards, Applied Mathematics Series 55, June 1964,
formula 25.4.6 on page 886.  The error is (n*h**5/90)*f''''(x) for some
x in (lowlim, uplim), where h = (uplim - lowlim)/2n.			*/

/************************************************************************/
	double
simpson(func, lowlim, uplim, n)	/* return the integral of func from
				   lowlim to uplim using Simpson's rule
				   on 2n + 1 points.			*/
/*----------------------------------------------------------------------*/

double (*func)(), lowlim, uplim;
{
	double x, y, odds, evens, h, h2, ends;
	int i;

	if (n <= 0)
	{	errno = EDOM;
		return 0.0;
	}
	odds = evens = 0.0;
	ends = (*func)(lowlim) + (*func)(uplim);
	h2 = (uplim - lowlim) / n;
	h = h2 / 2.0;
	x = lowlim + h;		/* sum upward on odd points, downward	*/
	y = uplim - h2;		/* on the even points.			*/
	for (i = 1; i < n; i++, x += h2, y -= h2)
	{	odds += (*func)(x);
		evens += (*func)(y);
	}
	odds += (*func)(x);
	return (h * (ends + 4.0 * odds + 2.0 * evens) / 3);
}

