/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "ctype.h"

/************************************************************************/
	BOOL
isupper(c)		/* returns true if c is 'A' to 'Z'		*/

/*----------------------------------------------------------------------*/
int c;
{
	return (ISUPPER(c));
}
