/* 1.2  01-08-86						(fseek.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
fseek(fp, offset, where)	/* Position the FILE fp at offset bytes
				   from where. Return EOF on error.	*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
long offset;
{
	long lseek();
	METACHAR fflush();

	fp->_flags &= ~_EOF;
	if (fp->_flags & _DIRTY)	/* if something in the write buffer */
	{	if (fflush(fp))
			return EOF;
	}
	else if (where IS CURPOS AND fp->_bptr)	/* or read buffer	*/
		offset -= (fp->_bend - fp->_bptr);
	fp->_bptr = fp->_bend = NULL;
	if (lseek(fp->_unit, offset, where) < 0)
		return EOF;

	return NULL;
}
