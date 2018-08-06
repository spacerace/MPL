/* 1.1  02-10-86						(strncmp.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984, 86	*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/

strncmp(s, t, n)	/* compares s and t for max of n chars. Returns
			   0 if equal, a neg. no. if s<t, pos. if s>t	*/
/*----------------------------------------------------------------------*/
STRING s, t;
int n;
{
	for ( ; --n > 0 AND *s IS *t; s++, t++)
		if (*s IS NULL)
			return (0);
	return (*s - *t);
}
