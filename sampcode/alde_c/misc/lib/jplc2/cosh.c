/* 1.0  05-16-84 */
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

#define LNv	0.69316101074218750000
#define Vm2	0.24999308500451499336
#define Vov2m1	0.13830277879601902638e-4

/************************************************************************/
	double
cosh(x)				/* return hyperbolic cosine of x	*/

/*----------------------------------------------------------------------*/
double x;
{
	double y, w, z;
	
	if ((y = ABS(x)) > 1.0)
	{	w = y - LNv;
		if (w > LOGINFINITY)
		{	errno = ERANGE;
			return INFINITY;
		}
		z = exp(w);
		if (w < PRECISION)
			z += Vm2 / z;
		z += Vov2m1 * z;
	}
	else
	{	z = exp(y);
		z = ldexp(z,-1) + 0.5 / z;
	}
	return z;
}
