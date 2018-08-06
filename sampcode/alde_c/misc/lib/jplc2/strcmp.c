/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/

strcmp(s, t)	/* return 0 if strings s and t are the same, a neg no. if
		   s < t, and a pos. no. if s > t.			*/
/*----------------------------------------------------------------------*/
STRING s, t;
{
	for ( ; *s IS *t; s++, t++)
		if (*s IS NULL)
			return (NULL);
	return (*s - *t);
}
