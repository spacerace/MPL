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
isalpha(c)		/* returns true if c is alphabetic.		*/

/*----------------------------------------------------------------------*/
int c;
{
	return (ISALPHA(c));
}
