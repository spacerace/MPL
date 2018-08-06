/* 1.1  06-27-85						(chrstc.c) 
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathcons.h"

/************************************************************************/
	int
chrstc(x)		/* return the characteristic (2's exponent) of
			   double x. Mantissa is in (.5, 1).		*/
/*----------------------------------------------------------------------*/
double x;
{
	char *xp;
	int n, norm;

	if (x IS 0.0)
	{	errno = EDOM;
		return -INFINITY;
	}
	xp = (char *) &x;

#ifdef ECOSOFT
	return (((int) *xp) - 0x80);
#endif

#ifdef AZTEC
	n = (((int) (*xp++ & 0x7f)) - 0x40) * 8;
	norm = *xp;
	while (NOT(norm & 0x80))
	{	n--;
		norm = (norm << 1) | 1; 
	}
	return n;
#endif

#ifdef CPM86
	return (((int) (((xp[7] << 4) + (xp[6] >> 4)) & 0x7ff)) - 0x3fe);
#endif

#ifdef UNIVERSE
	return (((int)(((*xp << 1) + (xp[1] & 0x80 ? 1 : 0)) & 0xff)) - 0x80);
#endif

#ifdef SUN
	return (int)(((*xp & 0x7f) << 4) + ((xp[1] >> 4) & 0xf) - 0x3fe);
#endif
}
