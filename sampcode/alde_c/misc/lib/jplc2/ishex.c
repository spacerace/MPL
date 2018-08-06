/* 1.0  07-26-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "ctype.h"

/************************************************************************/
	BOOL
ishex(c)		/* returns true if c is a hex number, 0-9, a-f.	*/

/*----------------------------------------------------------------------*/
int c;
{
	c = TOUPPER(c);
	return (ISDIGIT(c) OR ('A' <= c AND c <= 'F'));
}
