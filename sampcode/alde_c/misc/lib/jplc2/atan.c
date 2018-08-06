/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Programmmed using the algorithms given in:
 *
 *	Coty, William J., Jr., and Waite, William, SOFTWARE MANUAL FOR
 *	THE ELEMENTARY FUNCTIONS, Prentice-Hall Series in Computational
 *	Mathematics, Prentice-Hall, Inc., Inglewood Cliffs, NJ, 1980,
 *	pp. 194-216.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define P0 	-0.13688768894191926929e+2
#define P1 	-0.20505855195861651981e+2
#define P2 	-0.84946240351320683534e+1
#define P3 	-0.83758299368150059274e+0
#define gP(g) 	((((P3*g P2)*g P1)*g P0)*g)

#define Q0 	+0.41066306682575781263e+2
#define Q1 	+0.86157349597130242515e+2
#define Q2 	+0.59578436142597344465e+2
#define Q3 	+0.15024001160028576121e+2
#define Q4	+1.0
#define Q(g) 	((((g Q3)*g Q2)*g Q1)*g Q0)

/*\p*********************************************************************/
	double
atan2(v, u)		/* computes atan(v, u) = atan(v / u)		*/

/*----------------------------------------------------------------------*/
double u,v;
{
	double f;
	FAST int c;

	if (u IS 0.0)
	{	if (v IS 0.0)
		{	errno = EDOM;
			return 0.0;
		}
		return PIover2;
	}
	c = chrstc(v) - chrstc(u);	/* characteristic of v/u	*/
	if (c > MAXEXPG)
		f = PIover2;		/* overflow			*/
	else if (c < MINEXPG)
		f = 0.0;		/* underflow			*/
	else
	{	f = v / u;
		f = atan(ABS(f));
		if (u < 0.0)
			f = PI - f;
	}
	return (v < 0.0 ? -f : f);
}

/*\p*********************************************************************/
	double
atan(x)			/* return trigonometric arctangent of x		*/

/*----------------------------------------------------------------------*/
double x;
{
	double f, r, g;
	FAST int n;
	LOCAL double A[4] =
	{	0.0,
		PIover6,
		PIover2,
		PIover3,
	};
	
	f = ABS(x);
	if (f > 1.0)
	{	f = 1.0 / f;
		n = 2;
	}
	else
		n = 0;
	if (f > TWOmSQRT3)
	{	f = (((SQRT3m1 * f - 0.5) - 0.5) + f) / (SQRT3 + f);
		++n;
	}
	if (ABS(f) < FADEOUT)
		r = f;
	else
	{	g = f * f;
		r = f + f *
			gP(g)
			/Q(g);
	}
	if (n > 1)
		r = -r;
	r += A[n];
	return (x < 0.0 ? -r : r);
}
