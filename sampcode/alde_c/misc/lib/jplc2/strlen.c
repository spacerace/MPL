/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	int
strlen(s)	/* return length of string s				*/

/*----------------------------------------------------------------------*/
STRING s;
{
	STRING p;

	p = s;
	while (*p)
		p++;
	return (p - s);
}
