/* 1.0  12-17-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
rewind(fp)		/* Set the FILE fp to the ORIGIN. Return EOF on	
			   error.					*/
/*----------------------------------------------------------------------*/
FILE *fp;
{
	METACHAR fseek();

	return fseek(fp, 0L, ORIGIN);
}
