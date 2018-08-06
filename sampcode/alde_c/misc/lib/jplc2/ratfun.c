/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathcons.h"

/************************************************************************/
	double
ratfun(x, P, Q, n, m)	/* return value of rational function P(x)/Q(x)
			   where n = deg(P), m = deg(Q)			*/
/*----------------------------------------------------------------------*/
double x, P[], Q[];
int n, m;
{
	double num, den;
	int abs(), chrstc();

	if ( P[n] IS 1.0 AND n > 0)
		num = x + P[--n];
	else
		num = P[n];
	for (n--; n >= 0; n--)
		num = num * x + P[n];
	if (Q[m] IS 1.0 AND m > 0)
		den = x + Q[--m];
	else
		den = Q[m];
	for (m--; m >= 0; m--)
		den = den * x + Q[m];
	if (num IS 0.0 AND den ISNT 0.0)
			return 0.0;

	if (den IS 0.0 OR abs(chrstc(num) - chrstc(den)) >= MAXEXP)
	{	errno = ERANGE;
		return (num < 0.0 ? -INFINITY : INFINITY);
	}
	errno = 0;
	return (num/den);
}
