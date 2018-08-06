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
 *	pp. 217-238.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define P0 	-0.35181283430177117881e+6
#define P1 	-0.11563521196851768270e+5
#define P2 	-0.16375798202630751372e+3
#define P3 	-0.78966127417357099479e+0
#define P(z) 	(((P3*z P2)*z P1)*z P0)

#define Q0 	-0.21108770058106271242e+7
#define Q1 	+0.36162723109421836460e+5
#define Q2 	-0.27773523119650701667e+3
#define Q3	+1.0
#define Q(z) 	(((z Q2)*z Q1)*z Q0)

#define LNv	0.69316101074218750000
#define Vm2	0.24999308500451499336
#define Vov2m1	0.13830277879601902638e-4

/*\p*********************************************************************/
	double
sinh(x)			/* return hyperbolic sine of x			*/

/*----------------------------------------------------------------------*/
double x;
{
	double y, w, z;
	FAST int sgn;
	
	if (x < 0.0)
	{	y = -x;
		sgn = 1;
	}
	else
	{	y = x;
		sgn = 0;
	}
	if (y > 1.0)
	{	w = y - LNv;
		if (w > LOGINFINITY)
		{	errno = ERANGE;
			z = INFINITY;
		}
		else
		{	z = exp(w);
			if (w < PRECISION)
				z -= Vm2 / z;
			z += Vov2m1 * z;
		}
		if (sgn)
			z = -z;
	}
	else if (y < FADEOUT)
		z = x;
	else
	{	z = x * x;
		z = x + x * z *
			P(z)
			/Q(z);
	}
	return z;
}
