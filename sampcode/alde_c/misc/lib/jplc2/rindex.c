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
rindex(s, c)	/* return pointer to last occurrence of char c in string
		   s, or NULL otherwise.				*/
/*----------------------------------------------------------------------*/
STRING s;
char c;
{
	STRING p;

	for (p = s; *s ; s++)
		;
	while (c ISNT *s AND s >= p)
		s--;
	return (s < p ? NULL : s);
}
