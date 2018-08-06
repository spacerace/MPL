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
 *	pp. 35-59.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define A0 	-0.64124943423745581147e+2
#define A1 	+0.16383943563021534222e+2
#define A2 	-0.78956112887491257267e+0
#define A(w) 	((A2*w A1)*w A0)

#define B0 	-0.76949932108494879777e+3
#define B1 	+0.31203222091924532844e+3
#define B2 	-0.35667977739034646171e+2
#define B3	+1.0
#define B(w)	(((w B2)*w B1)*w B0)

#define C1 	0.693359375			/* = 355 / 512		*/
#define C2 	(-2.121944400546905827679e-4)	/* C1 + C2 = log(2)	*/

/*\p*********************************************************************/
	double
log(x)			/* return natural log (base e) of x		*/

/*----------------------------------------------------------------------*/
double x;
{
	double r, f, z, w, znum, zden, xn;
	int n;
	
	if (x <= 0.0)
	{	errno = EDOM;
		return -INFINITY;
	}
	f = frexp(x, &n);
	if (f > ROOTHALF)
	{	znum = (f - 0.5) - 0.5;
		zden = ldexp(f, -1) + 0.5;
	}
	else
	{	--n;
		znum = f - 0.5;
		zden = ldexp(znum, -1) + 0.5;
	}
	z = znum / zden;
	w = z * z;
	r = z + z * (w *
		 A(w)
		 /B(w));
	xn = n;
	return (xn * C2 + r) + xn * C1;
}
