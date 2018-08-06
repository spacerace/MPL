/* 1.0  05-13-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
stradd(n, s, args) /* concatenate n strings args to s. return pointer s	*/

/*----------------------------------------------------------------------*/
STRING s, args;
{
	STRING p, t, *q;

	p = s;
	while (*s)
		s++;
	for (q = &args; n-- > 0; q++)
	{	t = *q;
		while (*t)
			*s++ = *t++;
	}
	*s = NULL;
	return p;
}
