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
strcpy(s, t)	/* copy string t into s, return pointer s		*/

/*----------------------------------------------------------------------*/
STRING s, t;
{
	STRING p;

	p = s;
	while (*s++ = *t++)
		;
	return p;
}
