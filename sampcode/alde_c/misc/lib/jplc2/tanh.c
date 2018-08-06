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
 *	pp. 239-255.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define LN3ov2	0.54930614433405484570

#define P0 	-0.16134119023996228053e+4
#define P1 	-0.99225929672236083313e+2
#define P2 	-0.96437492777225469787e+0
#define gP(g) 	(((P2*g P1)*g P0)*g)

#define Q0 	+0.48402357071988688686e+4
#define Q1 	+0.22337720718962312926e+4
#define Q2 	+0.11274474380534949335e+3
#define Q3	+1.0
#define Q(g) 	(((g Q2)*g Q1)*g Q0)

/*\p*********************************************************************/
	double
tanh(x)			/* return hyperbolic tangent of x		*/

/*----------------------------------------------------------------------*/
double x;
{
	double f,g,r;
	
	if ((f = ABS(x)) > TANHXBIG)
		r = 1.0;
	else if (f > LN3ov2)
		r = ldexp(0.5 - 1.0 / (exp(ldexp(f, 1)) + 1.0), 1);
	else if (f < FADEOUT)
		r = f;
	else
	{	g = f * f;
		r = f + f*
			(gP(g)
			/Q(g));
	}
	return (x < 0.0 ? -r : r);
}
