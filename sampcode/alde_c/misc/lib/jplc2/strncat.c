/* 1.0  07-06-84
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
strncat(s1, s2, n)	/* concatenate string s2 onto s1, but at most
			   n characters. Return pointer s1.		*/
/*----------------------------------------------------------------------*/
STRING s1, s2;
int n;
{
	STRING p;

	p = s1;
	while (*s1 AND n > 0)
	{	s1++;
		n--;
	}
	strncpy(s1, s2, n);
	return p;
}
