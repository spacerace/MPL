/* 1.2  01-08-86						(atol.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	long
atol(s)		/* return long value of converted ascii string s.	*/

/*----------------------------------------------------------------------*/
STRING s;
{
	long i;
	STRING astol();

	astol(s, &i);
	return i;
}

/************************************************************************/
	STRING
astol(s, val)		/* convert string s to long value and return
			   pointer to next character not converted	*/
/*----------------------------------------------------------------------*/
STRING s;
long *val;
{
	long v;
	BOOL minus;
	int c, base;
	METACHAR tobase();

	while (isspace(*s))
		s++;
	if ((minus = (*s IS '-')) OR (*s IS '+'))
		s++;
	if (*s IS '0')
		if (tolower(*(++s)) IS 'x')
		{	base = 16;
			s++;
		}
		else
			base = 8;
	else
		base = 10;
	for (v = 0; (c = tobase(*s, base)) >= 0; s++)
		v = v * base + c;
	*val = (minus ? -v : v);
	return s;
}
