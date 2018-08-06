/* 1.2  05-08-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	VOID
setbuf(fp, buffer)	/* If a buffer is assigned to FILE fp, liberate
			   it.  Assign buffer (assumed BUFSIZ bytes) if
			   not NULL; otherwise, assign the 1-byte buffer. */
/*----------------------------------------------------------------------*/
FAST FILE *fp;
BUFFER buffer;
{
	if (fp->_buff AND fp->_flags & _ALLBUF)
		liberate(fp->_buff, fp->_buflen);
	if (buffer)
	{	fp->_buff = buffer;
		fp->_buflen = BUFSIZ;
	}
	else
	{	fp->_buff = &fp->_bytbuf;
		fp->_buflen = 1;
	}
}
