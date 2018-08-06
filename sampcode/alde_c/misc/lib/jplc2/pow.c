/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Programmmed using the algorithm given in:
 *
 *	Coty, William J., Jr., and Waite, William, SOFTWARE MANUAL FOR
 *	THE ELEMENTARY FUNCTIONS, Prentice-Hall Series in Computational
 *	Mathematics, Prentice-Hall, Inc., Inglewood Cliffs, NJ, 1980,
 *	pp. 84-124.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/*----------------------------------------------------------------------*/

#define reduce(v) ldexp(fint(ldexp(v,4)),-4)
#define K	  0.44269504088896340736

#define P1	0.8333333333412136e-1
#define P2	0.1249999796500608e-1
#define P3	0.2233824352815418e-2
#define P(a)	(((P3*a+P2)*a+P1)*a)

#define Q1	0.693147180559937815e0
#define Q2	0.240226506956777522e0
#define Q3	0.555041084247568661e-1
#define Q4	0.961811769138724104e-2
#define Q5	0.133308101134082075e-2
#define Q6	0.150774061788142382e-3
#define Q(a)	((((((Q6*a+Q5)*a+Q4)*a+Q3)*a+Q2)*a+Q1)*a)

/*\p--------------------------------------------------------------------*/

LOCAL double A1[17] =		/* Converted from octal tables	*/
{	1.0,			/* in reference above by reading*/
	9.576032806985708e-01,	/* octal values to 45B, and	*/
	9.170040432046704e-01,	/* converting to decimal to give*/
	8.781260801866466e-01,	/* A1.  Then this decimal string*/
	8.408964152537131e-01,	/* was converted back to octal.	*/
	8.052451659746254e-01,	/* Error in this cycle was	*/
	7.711054127039674e-01,	/* added into the converted	*/
	7.384130729697489e-01,	/* octal value of the remaining	*/
	7.071067811865461e-01,	/* digits, giving A2.		*/
	6.771277734684453e-01,
	6.484197773255040e-01,
	6.209289060367418e-01,
	5.946035575013582e-01,
	5.693943173783431e-01,
	5.452538663326258e-01,
	5.221368912137052e-01,
	5.000000000000000e-01
};
LOCAL double A2[8] =
{	2.847357921552306e-15,
	3.151180748043254e-15,
	1.760954860069324e-15,
	7.597361444532664e-16,
	1.065461039677172e-15,
	2.314569550762763e-16,
	2.736854898096527e-15,
	1.721971773273086e-15
};
/*\p*********************************************************************/
	double
pow(x, y)		/* return power function x^y 			*/

/*----------------------------------------------------------------------*/
double x, y;
{
	double c, g, r, xm, u1, u2, v, w, w1, w2, y1, y2, z;
	FAST int iw, p;
	int m;

	if (x <= 0.0)
	{	if (x < 0.0 OR x IS 0.0 AND y <= 0.0)
		{	errno = EDOM;
			return -INFINITY;
		}
		else return 0.0;
	}
	g = frexp(x, &m);
	xm = m;
	p = 0;	/* The Ai[] tables are indexed one less than in reference. */
	if (g <= A1[8])
		p = 8;
	if (g <= A1[p + 4])
		p = p + 4;
	if (g <= A1[p + 2])
		p = p + 2;
	c = A1[++p];	/* p is now correct, index one less than ref.	*/
	z = ((g - c) - A2[p / 2]) / (ldexp(g, -1) + ldexp(c, -1));
	v = z * z;
	r = P(v) * z;
	r += K * r;
	u2 = (r + z * K) + z;
	u1 = ldexp(ldexp(xm, 4) - p, -4);
	y2 = y - (y1 = reduce(y));
	w1 = reduce(w = u2 * y + u1 * y2);
	w2 = w - w1;
	w1 = reduce(w = w1 + u1 * y1);
	w2 += (w - w1);
	iw = (int) ldexp(w1 + (w = reduce(w2)), 4);
	w2 -= w;
	if (iw > BIGX)
	{	errno = ERANGE;
		return INFINITY;
	}
	if (iw < SMALLX)
	{	errno = ERANGE;
		return 0.0;
	}
	if (w2 > 0)
	{	iw++;
		w2 -= 0.0625;
	}
	m = iw / 16 + (iw < 0 ? 0 : 1);
	p = 16 * m - iw;
	z = Q(w2);
	return ldexp(A1[p] + A1[p] * z, m); /* indices one less than ref. */
}
