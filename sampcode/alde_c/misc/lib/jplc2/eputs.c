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
eputs(s)		/* Put the string s, followed by newline, to the
			   stderr device. Return newline if ok, or
			   EOF if not.					*/
/*----------------------------------------------------------------------*/
FAST STRING s;
{
	while (*s)
		if (eputc(*s++) IS EOF)
			return EOF;

	return eputc('\n');
}
