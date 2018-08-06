/* 1.0  11-19-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Integration step for N first-order ordinary differential equations
 *	using the 4th-order Runge-Kutta formulas, given in
 *
 *	HANDBOOK OF MATHEMATICAL FUNCTIONS, U. S. Dept. of Commerce,
 *	National Bureau of Standards Applied Mathematics Series No. 55,
 *	formula 25.5.10, page 896, June, 1964,
 *
 *	on each equation y'[i] = f(i, t, y[i]), i = 0, ... , N-1, as time
 *	progresses.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	VOID
rknstep(N, f, h, t, y1, y2, dy, k, ys)
		/* Store y2[i] = y1[i](t+h), for y1[i]' = f(i, t, y1),
		   using 4th-order Runge-Kutta method, i = 0, ... , N.	*/
/*----------------------------------------------------------------------*/
double (*f)(), h, t, y1[], y2[], dy[], k[], ys[];
/*
 *	Note: dy[] is a scratch array needed to hold intermediate 
 *	calculations accumulating the dy value; k[] holds coefficients
 *	k1, k2, k3, and k4, successively; and the ys[] is for y approximants,
 *	for each i.
 */
{
	int i;
	double h2, ldexp();

	h2 = ldexp(h, -1);
	for (i = 0; i < N; i++)		/* i is equation index.	*/
	{	dy[i] = h * (*f)(i, t, y1);		/* k1	*/
		ys[i] = y1[i] + dy[i] / 2;
	}
	for (i = 0; i < N; i++)
	{	k[i] = h * (*f)(i, t + h2, ys);		/* k2	*/
		dy[i] += 2 * k[i];
	}
	for (i = 0; i < N; i++)
		ys[i] = y1[i] + k[i] / 2.0;
	for (i = 0; i < N; i++)
	{	k[i] = h * (*f)(i, t + h2, ys);		/* k3	*/
		dy[i] += 2 * k[i];
	}
	for (i = 0; i < N; i++)
		ys[i] = y1[i] + k[i];
	for (i = 0; i < N; i++)
		dy[i] += h * (*f)(i, t + h, ys);	/* k4	*/
				/* dy is (k1 + 2 k2 + 2 k3 + k4) */
	for (i = 0; i < N; i++)
		y2[i] = y1[i] + dy[i] / 6.0;
}
