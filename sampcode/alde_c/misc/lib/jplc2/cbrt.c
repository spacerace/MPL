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

/*
Reference: See J. F. Hart, E. W. Cheney, C. L. Lawson, H. J. Maehly,
C. K. Mesztenyi, J. R. Rice, H. C. Thacher, Jr., and C. Witzgall,
COMPUTER APPROXIMATIONS, John Wiley & Sons, Inc., New York, NY, 1968.
*/

/************************************************************************/
	double
cbrt(x)			/* return cube root of x			*/

/*----------------------------------------------------------------------*/
double x;
{
	double fx, yn, frexp(), ldexp();
	int n, ex, sgn;
	
	if (x IS 0.0)
		return x;

	sgn = (x < 0.0 ? 1 : 0);
	if (sgn)
		x = -x;
	fx = frexp(x, &ex);		/* frac x and exp x		*/
	yn = 0.59130052 + 0.41533799 * fx;
		/* See Chapter 7, Table 640 of Reference. Has err=2.1 D */
	yn = ldexp((yn + 1.5 * fx / (yn * yn + ldexp(fx, -1) / yn)), -1);
		/* See Newton iteration formula 6.1.13 of Reference,	*/
	yn -= ldexp((yn - 1.5 * fx / (yn * yn + ldexp(fx, -1) / yn)), -1);
		/* after two iterations, err = 19 D, without roundoff	*/

	n = (ex % 3);
	ex -= n;		/* ex now divisible by 3		*/
	if (n < 0)		/* must compensate if negative		*/
	{	n += 3;
		ex -= 3;
	}
	if (n IS 1)
		yn *= CBRTTWO;
	else if (n IS 2)
		yn *= CBRTFOUR;
	yn = ldexp(yn, ex/3);
	return (sgn ? -yn : yn);
}
