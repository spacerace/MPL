/* 1.0  05-07-85				     	     (redirbuf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	VOID
redirbuf(fd)		/* Change the buffer allocation for stdin and 
			   stdout for redirected i/o.			*/
/*----------------------------------------------------------------------*/
{
	if (fd ISNT STDIN AND fd ISNT STDOUT)
		return;

	IObuffs[fd]._buflen = BUFSIZ;
}
