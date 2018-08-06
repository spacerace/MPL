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
	long
ftell(fp)		/* Return offset in bytes relative to ORIGIN of
			   FILE fp.					*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	long offset, lseek();

	offset = lseek(fp->_unit, 0L, CURPOS);	/* locate current position */
	if (fp->_flags & _DIRTY)		/* add that in the buffer  */
		offset += (fp->_bptr - fp->_buff); /* not yet written.	   */
	else if (fp->_bptr)			/* or in read buffer	   */
		offset -= (fp->_bend - fp->_bptr);
	return offset;
}
