/* 1.0  12-17-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	STRING
gets(line)		/* get line up to newline or EOF from stdin.
			   Return line if ok, NULL if empty and EOF.	*/
/*----------------------------------------------------------------------*/
STRING line;
{
	FAST STRING s;
	FAST int i;

	s = line;
	while (((i = getchar()) ISNT EOF) AND (i ISNT '\n'))
		*s++ = i;
	*s = NULL;
	if ((i IS EOF) AND (s IS line))
		return NULL;

	return line;
}
