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
 *	pp. 174-193.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define P0 	+1.0
#define P1 	-0.27368494524164255994e+2
#define P2 	+0.57208227877891731407e+2
#define P3 	-0.39688862997504877339e+2
#define P4 	+0.10152522233806463645e+2
#define P5 	-0.69674573447350646411
#define P(g) 	((((P5*g P4)*g P3)*g P2)*g P1)

#define Q0 	-0.16421096714498560795e+3
#define Q1 	+0.41714430248260412556e+3
#define Q2 	-0.38186303361750149284e+3
#define Q3 	+0.15095270841030604719e+3
#define Q4 	-0.23823859153670238830e+2
#define Q(g) 	(((((g Q4)*g Q3)*g Q2)*g Q1)*g Q0)

LOCAL double asincos();

/************************************************************************/
	double
asin(x)			/* return the trigonometric arc-sine of x	*/

/*----------------------------------------------------------------------*/
double x;
{
	return asincos(x, 0);
}

/*\p*********************************************************************/

double acos(x)		/* return the trigonometric arc-cosine of x	*/

/*----------------------------------------------------------------------*/
double x;
{
	return asincos(x, 1);
}

/************************************************************************/
	LOCAL double
asincos(x, flg)		/* return arc sin or cos, depending on flg	*/

/*----------------------------------------------------------------------*/
double x;
{
	double y, g, r;
	FAST int i;
	LOCAL double a[2] = { 0.0, PIover4};
	LOCAL double b[2] = { PIover2, PIover4};

	y = ABS(x);
	i = flg;
	if (y < FADEOUT)
		r = y;
	else
	{	if (y > 0.5)
		{	i = 1 - i;
			if (y > 1.0)
			{	errno = EDOM;
				return 0.0;
			}
			g = ldexp((0.5 - y) + 0.5, -1);
			y = -ldexp(sqrt(g), 1);
		} else
			g = y * y;
		r = y + y *
			((P(g) * g)
			/ Q(g));
	}
	if (flg)
	{	if (x < 0.0)
			r = (b[i] + r) + b[i];
		else
			r = (a[i] - r) + a[i];
	}
	else
		r = (a[i] + r) + a[i];
	return (x < 0.0 ? -r : r);
}

