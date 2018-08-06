/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	BOOL
streql(s, t)		/* Return true if strings s and t are same.	*/

/*----------------------------------------------------------------------*/
STRING s, t;
{
	return (strcmp(s, t) IS NULL);
}
