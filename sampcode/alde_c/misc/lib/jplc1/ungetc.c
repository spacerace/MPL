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
ungetc(c, fp)		/* Push c back onto FILE fp stream, and return c.
			   Return EOF if c is EOF.			*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	if ((c IS EOF) OR (fp->_bptr <= fp->_buff))
		return EOF;

	return (*--fp->_bptr = c);
}
