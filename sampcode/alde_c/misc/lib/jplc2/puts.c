/* 1.0  12-10-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
puts(s)			/* Put the string s, followed by newline, to the
			   stdout device. Return newline if ok, EOF if not*/
/*----------------------------------------------------------------------*/
FAST STRING s;
{
	while (*s)
		if (putchar(*s++) IS EOF)
			return EOF;

	return putchar('\n');
}
