/* 1.1  07-10-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "mathcons.h"

/************************************************************************/
	double
frexp(x, np)		/* returns mantissa of x and *np contains the
			   characteristic of x (i.e., log2(x)).		*/
/*----------------------------------------------------------------------*/
double x;
int *np;
{
	double ldexp();
	int chrstc();

	return ldexp(x, -(*np = chrstc(x)));
}

/*\p*********************************************************************/
	double
ldexp(x, m)	/* return value = x * (2^m)				*/

/*----------------------------------------------------------------------*/
double x;
int m;
{
	FAST n;
	int chrstc();
	TBITS *xp;
	LOCAL double raiseto[8] =
		{1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0};

	if (x IS 0.0)
		return 0.0;

	xp = (char *) &x;

#ifdef ECOSOFT
	n = chrstc(x) + m;
	*xp = (n = (n > 0x7f ? 0x7f : n) + 0x80);
#endif

#ifdef AZTEC
	n = m / 8;
	if ((m %= 8) < 0)
	{	n--;
		m += 8;
	}
	if ((n += (*xp & 0x7f)) > 0x7f)
		n = 0x7f;
	*xp = n | (*xp & 0x80);
	x *= raiseto[m];
#endif

#ifdef CPM86
	n = chrstc(x) + m;
	n = (n > 0x400 ? 0x400 : n) + 0x3fe;
	xp[7] = (xp[7] & 0x80) | ((n >> 4) & 0x7f);
	xp[6] = (xp[6] & 0x0f) | (n << 4);
#endif

#ifdef UNIVERSE
	n = chrstc(x) + m;
	n = (n > 0x7f ? 0x7f : n) + 0x80;
	*xp = (*xp++ & 0x80) | ((n >> 1) & 0x7f);
	*xp = (*xp & 0x7f) | (n & 1 ? 0x80 : 0);
#endif
/*\p*/

#ifdef SUN
	n = chrstc(x) + m + 0x3fe;
	if (n >= 0x7ff)
		return INFINITY;

	*xp = (*xp & 0x80) | (n>>4);
	xp[1] = (xp[1] & 0xf) | ((n & 0xf)<<4);
#endif

	if (n < 1)
		return 0.0;
	else
		return x;
}

