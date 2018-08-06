/* 1.1  01-08-86 						 (atoi.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/

atoi(s)		/* return integer value of string s up to first
		   non-digit						*/
/*----------------------------------------------------------------------*/
STRING s;
{
	int i;
	STRING astoi();

	astoi(s, &i);
	return i;
}

/************************************************************************/
	STRING
astoi(s, val)	/* store into val the value of the string s up to the
		   first non-digit.  return pointer to that char.	*/
/*----------------------------------------------------------------------*/
STRING s;
int *val;
{
	int c, v, base;
	BOOL minus;
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