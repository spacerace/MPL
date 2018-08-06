/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
strncpy(s1, s2, n)	/* copy leftmost n chars of s2 into s1,
			   return s1					*/
/*----------------------------------------------------------------------*/
STRING s1, s2;
int n;
{
	STRING s;

	for (s = s1; n-- > 0 AND (*s1 = *s2); s1++ , s2++)
		;
	*s1 = NULL;
	return s;
}

