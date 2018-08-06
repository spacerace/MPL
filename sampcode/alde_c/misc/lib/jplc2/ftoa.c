/* 1.3  10-08-85						(ftoa.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "ctype.h"
#include "stdtyp.h"
#include "mathcons.h"

/************************************************************************/
	STRING
ftoa(s, x, pr, mode)	/* Convert x to ascii string s with precision pr
			   in mode 'e', 'f', or 'g' format.
			   Return pointer to s.				*/
/*----------------------------------------------------------------------*/
STRING s;
double x;
{
	BOOL minus;
	STRING p, astoi();
	int d;
	double normalize();

	p = s;		/* save string pointer for returns	*/
	*s = NULL;
	if (pr > DPRECISION)
		pr = DPRECISION;
	if (minus = ((x < 0.0) ? TRUE : FALSE)) /* work with pos. numbers */
		x = -x;
	x = normalize(x, &d);
	*s++ = (minus ? '-' : ' ');	/* put sign into string	*/
	switch (tolower(mode))
	{	case 'e':
		case 0:
			expform(s, x, d, pr);
			break;
		case 'f':
		case 1:
			floatform(s, x, d, pr);
			break;
		case 'g':
		case 2:
			minform(s, x, d, pr);
			break;
		default:
			;
	}
	return p;
}

/*\p*********************************************************************/
	LOCAL double
normalize(x, d)		/* return x * 10^-d in [1, 10), with d set so
			   this condition is met.			*/
/*----------------------------------------------------------------------*/
double x;
int *d;
{
	/****************************************************************
	 * This array contains 2^n powers of 10 up to n = BIG10X-1	*
	 ****************************************************************/

	LOCAL double tentothe2tothe[BIG10X] =
	{	10., 100., 10000., 100000000., 10000000000000000.,
		100000000000000000000000000000000.
#ifdef AZTEC
		, 1.0e64, 1.0e128
#endif

#ifdef SUN
		, 1.0e64, 1.0e128, 1.0e256
#endif
	};

	/********************************************************
	 * Also, make this array be 2^n up to 2^(BIG10X-1)	*
	 ********************************************************/

	LOCAL int twoto[BIG10X] =
	{	1, 2, 4, 8, 16, 32
#ifdef AZTEC
		, 64, 128
#endif

#ifdef SUN
		, 64, 128, 256
#endif
	};

	int i;

	*d = 0;
	if (x ISNT 0.0)
		if (x < 1.0)
		{	for (i = 1; x < SYMLEAST; i++)
				x *= 10.0;	
			x = 10.0 / normalize(1.0 / x, d);
			*d = -*d - i;
			if (x >= 10.0)	/* compensate if on the edge.	*/
			{	x /= 10.0;
				++*d;
			}
		}	
		else
			for (i = BIG10X; x >= 10.0 AND --i >= 0; )
				if (x >= tentothe2tothe[i])
				{	x /= tentothe2tothe[i];
					*d += twoto[i];
				}
	return x;
}	

/*\p*********************************************************************/
	LOCAL VOID
floatform(s, x, d, pr)	/* put x * 10^d (precision pr) into string s
			   in f-format.					*/
/*----------------------------------------------------------------------*/
STRING s;
double x;
{
	double roundoff();
	int i;

	x = roundoff(x, &d, pr + ((d > 0) ? d : 0));
	if (d < 0)
		*s++ = '0';
	else
		while (d >= 0)
		{	*s++ = pickdigit(&x);
			d--;
		}
	if ((i = pr) >= 0)
		*s++ = '.';
	while (++d < 0 AND i-- > 0)
		*s++ = '0';
	while (i-- > 0)
		*s++ = pickdigit(&x);
	*s = NULL;
}

/************************************************************************/
	LOCAL VOID
expform(s, x, d, pr)	/* put x * 10^d (precision pr) into string s
			   in e-format					*/
/*----------------------------------------------------------------------*/
STRING s;
double x;
{
	double roundoff();
	int i;

	x = roundoff(x, &d, i = pr);
	*s++ = pickdigit(&x);
	*s++ = '.';
	while (i-- > 0)
		*s++ = pickdigit(&x);
	*s++ = 'e';
	itoa(s, d, 0);	/* minimum width */
}

/*\p*********************************************************************/
	LOCAL VOID
minform(s, x, d, pr)	/* put x * 10^d into string s in minimum format
			   with precision pr				*/
/*----------------------------------------------------------------------*/
STRING s;
double x;
{
	int i, j;

	if (d < 5)
	{	floatform(s, x, d, pr);
		i = strlen(s) - 1;
		while (s[i] IS '0')
			s[i--] = NULL;
		if (s[i] IS '.')
			s[i--] = NULL;
	}
	else
	{	expform(s, x, d, pr);
		j = strlen(s) - 1;
		while (s[j] ISNT 'e')
			j--;
		for (i = j - 1; s[i] IS '0'; i--)
			;
		strcpy(&s[++i], &s[j]);
	}
}

/************************************************************************/
	LOCAL
pickdigit(x)	/* return ascii leading digit of x, strip it off, and
		   prepare x for extraction of next digit.		*/
/*----------------------------------------------------------------------*/
double *x;
{
	int n;

	n = *x;			/* take integer part.			*/
	*x = 10 * ( *x - n);	/* strip it off, and renormalize x	*/
	return (n + '0');
}
/*\p*********************************************************************/
	LOCAL double
roundoff(x, d, p)	/* return x * 10^*d rounded to precision p	*/

/*----------------------------------------------------------------------*/
double x;
int *d;
{				   /*************************************/
	LOCAL double rounder[21] = /* NOTE: DPRECISION must be no larger*/
				   /* than the number of elements in	*/
				   /* this array.			*/
				   /*************************************/
	{	0.5,     0.5e-1,  0.5e-2,  0.5e-3,  0.5e-4,  0.5e-5,
		0.5e-6,  0.5e-7,  0.5e-8,  0.5e-9,  0.5e-10, 0.5e-11,
		0.5e-12, 0.5e-13, 0.5e-14, 0.5e-15, 0.5e-16, 0.5e-17,
		0.5e-18, 0.5e-19, 0.5e-20
	};
	double y;

	if ((y = x) ISNT 0.0)
		if (p <= DPRECISION)
			if ((x += rounder[p]) >= 10.0)
			{	x = y / 10.0 + rounder[p];
				++*d;
			}
	return x;
}
