/* 1.2  01-08-86						(putc.c) 
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
putc(c, fp)		/* Put character c on FILE fp.  Return c or EOF
			   on error condition.				*/
/*----------------------------------------------------------------------*/
METACHAR c;
FAST FILE *fp;
{
	METACHAR flush();

	_rowcol(c, fp);
	if (fp->_bptr >= fp->_bend)
		return flush(fp, c & 0xff);

	return (*fp->_bptr++ = c) & 0xff;
}
/*\p*********************************************************************/
	METACHAR
flush(fp, c)		/* Write any already buffered characters onto
			   FILE fp.  If c is EOF, return NULL on success,
			   EOF on failure.  Otherwise, if i/o is buffered,
			   put c in the buffer, and return c.  If io isn't
			   buffered, write the char, return c if ok or EOF
			   if write error. 				*/
/*----------------------------------------------------------------------*/
METACHAR c;
FAST FILE *fp;
{
	FAST int siz;
	char cc;

	cc = c;
	if (fp->_flags & _IOERR)
		return EOF;

	if (fp->_flags & _DIRTY)
	{	siz = fp->_bptr - fp->_buff;
		if (write(fp->_unit, fp->_buff, siz) ISNT siz)
		{	fp->_flags |= _IOERR;
			fp->_bend = fp->_bptr = NULL;
			return EOF;
		}
	}
	if (c IS EOF)
	{	fp->_flags &= ~_DIRTY;
		fp->_bend = fp->_bptr = NULL;
		return NULL;
	}
	if (fp->_buff IS NULL)		/* if no buffer yet assigned.	*/
		getbuf(fp);
	if (fp->_buflen IS 1)			/* unbuffered I/O	*/
	{	if (write(fp->_unit, &cc, 1) ISNT 1)
		{	fp->_flags |= _IOERR;
			fp->_bend = fp->_bptr = NULL;
			return EOF;
		}
		return c & 0xff;
	}
	fp->_bptr = fp->_buff;
	fp->_bend = fp->_buff + fp->_buflen;
	fp->_flags |= _DIRTY;
	return (*fp->_bptr++ = c) & 0xff;
}
/*\p*********************************************************************/
	VOID
fclosall()		/* close all stream files			*/

/*----------------------------------------------------------------------*/
{
	FILE *fp;

	for (fp = IObuffs; fp < IObuffs + MAXSTREAM; fp++)
		if (fp->_flags)
			fclose(fp);
	return;

}

/*************************************************************************/
	METACHAR
putca(c, fp)		/* Put ASCII character c on FILE fp.  Return c or
			   EOF on error condition.  Add a '\r' to the
			   output when c = '\n', as file fp is assumed
			   to be text.					*/
/*----------------------------------------------------------------------*/
METACHAR c;
FILE *fp;
{
#ifdef CROUTADD
	if (c IS '\n')
		if (putc('\r', fp) IS EOF)
			return EOF;
#endif
	return putc(c, fp);
}
