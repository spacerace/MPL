/* 1.1  07-08-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *	Programmmed using the algorithms given in:
 *
 *	Coty, William J., Jr., and Waite, William, SOFTWARE MANUAL FOR
 *	THE ELEMENTARY FUNCTIONS, Prentice-Hall Series in Computational
 *	Mathematics, Prentice-Hall, Inc., Inglewood Cliffs, NJ, 1980,
 *	pp. 150-173.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define C1	1.57080078125			/* C1 - C2 = PIover2	*/
#define C2	4.454455103380768678308e-6

#define P0 	1.0
#define P1 	0.13338350006421960681e+0
#define P2 	0.34248878235890589960e-2
#define P3 	0.17861707342254426711e-4
#define P(g)	(((-P3*g+P2)*g-P1)*g)

#define Q0 	1.0
#define Q1 	0.46671683339755294240e+0
#define Q2 	0.25663832289440112864e-1
#define Q3 	0.31181531907010027307e-3
#define Q4 	0.49819433993786512270e-6
#define Q(g)	(((((Q4*g-Q3)*g+Q2)*g-Q1)*g+0.5)+0.5)

LOCAL double tancot();

/*\p*********************************************************************/
	double
tan(x)			/* return trigonometric tangent of x		*/

/*----------------------------------------------------------------------*/
double x;
{
	return tancot(x, ABS(x), 0);
}

/************************************************************************/
	double
cotan(x)		/* returns trigonometric cotangent of x		*/

/*----------------------------------------------------------------------*/
double x;
{
	double y;
	
	y = ABS(x);
	if (y < SYMLEAST)
	{	errno = ERANGE;
		if (x < 0.0)
			return -INFINITY;
		else
			return INFINITY;
	}
	return tancot(x, y, 1);
}

/*\p*********************************************************************/
	LOCAL double
tancot(x, y, flag)		/* process both tan and cot functions	*/

/*----------------------------------------------------------------------*/
double x,y;
BOOL flag;
{
	double f, g, x1, x2, xn;
	double xnum, xden;
	
	if (y > MAXANGLE)
	{	errno = ERANGE;
		return 0.0;
	}
	xn = round(TWOoverPI * x);
	x1 = fint(x);
	f = ((x1 - xn * C1) + (x - x1)) + xn * C2;
	if (ABS(f) < FADEOUT)
	{	xnum = f;
		xden = 1.0;
	}
	else
	{	g = f * f;
		xnum = P(g) * f + f;
		xden = Q(g);
	}
	if ((int) xn & 1)
	{	flag = !flag;	/* invert tan-cotan logic	*/
		xnum = -xnum;	/* Cases A and C, xn odd	*/
	}
	if (flag)
		return (xden / xnum);	/* inverted function	*/
	else		
		return (xnum / xden);	/* normal function	*/
}
