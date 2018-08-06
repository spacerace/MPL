/* 1.2  10-08-85						(atof.c) 
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "mathcons.h"
#include "mathtyp.h"
#include "ctype.h"

/************************************************************************/
	STRING
astof(s, val)	/* convert value of string s in double/float format and 
		   store in val.  return pointer to next unused char.	*/
/*----------------------------------------------------------------------*/
STRING s;
double *val;
{
	STRING astoi();
	double v, fr, tenpow();
	int i;
	BOOL sign;
	char c;

	while (isspace(c = *s))
		s++;
	if (sign = (c IS '-'))
		s++;
	else if (c IS '+')
		s++;
	for (v = 0.0; isdigit(*s); v = 10. * v + (*s++ - '0'));
	if (*s IS '.')
		for (fr = 10.0, s++; isdigit(*s); fr *= 10.)
			v += (*s++ - '0') / fr;
	while (isspace(*s))
		s++;
	if (TOLOWER(*s) IS 'e')
	{	if ((i = ((c = *(++s)) IS '-')) OR c IS '+')
			s++;
		while (*s IS '0')
			s++;
		if (i)
			*(--s) = '-';
		s = astoi(s, &i);
		v *= tenpow(i);
	}
	*val = (sign ? -v : v);
	return (s);
}
/*\p*********************************************************************/
	double
atof(s)		/* return value of the string s in double/float format	*/

/*----------------------------------------------------------------------*/
STRING s;
{
	double x;
	STRING astof();

	astof(s, &x);
	return x;	
}

/************************************************************************/
	LOCAL double
tenpow(n)		/* Return 10.0^n, or -1 on error.		*/

/*----------------------------------------------------------------------*/
{
	/********************************************************
	 * This array contains 10^(2^n) up to n = BIG10X-1	*
	 ********************************************************/
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

	int i, minus;
	double d;

	if (minus = (n < 0) ? TRUE : FALSE)
		n = -n;
	for (d = 1.0, i = BIG10X; --i >= 0; )
		if (n >= twoto[i])
		{	d *= tentothe2tothe[i];
			n -= twoto[i];
		}
	return (minus) ? 1.0 / d : d;
}
