/* 1.0  02-06-85 */
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
fgets(s, n, fp)		/* Get a string s from FILE fp, at most n chars,
			   up to newline or EOF. Newline is part of s.
			   Return s if ok, or NULL if empty and EOF.	*/
/*----------------------------------------------------------------------*/
STRING s;
FILE *fp;
{
	FAST c;
	STRING t;

	t = s;
	while (--n > 0 AND (c = getca(fp)) ISNT EOF)
	{	*s++ = c;
		if (c IS '\n')
			break;
	}
	*s = NULL;
	if (c IS EOF AND s IS t)
		return NULL;

	return t;
;
}

