/* 1.3  01-08-86 						(getc.c)
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
getc(fp)		/* Get and return next character from FILE fp.
			   Return EOF if unable to do so.		*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	FAST int siz, c;

	if (fp->_bptr >= fp->_bend)
	{	if (fp->_flags & (_EOF | _IOERR)) /* if past end of file */
			return EOF;

		fp->_flags &= ~_DIRTY;		/* clean the flush flag	*/
		if (fp->_buff IS NULL)		/* attach a buffer,	*/
			getbuf(fp);		/*   if not yet done.	*/
		if ((siz = read(fp->_unit, fp->_buff, fp->_buflen)) <= 0)
		{	fp->_flags |= (siz IS 0) ? _EOF : _IOERR;
			return EOF;
		}

		fp->_bend = (fp->_bptr = fp->_buff) + siz;
	}
	_rowcol(c = *fp->_bptr++ & 0xff, fp);
	return c;

	redirbuf();	/* a dummy call to be sure redirbuf is linked in
			   when a getc is made and input is redirected	*/
}

/*\p*********************************************************************/
	METACHAR
getca(fp)		/* Get and return next ASCII character from FILE
			   fp.  Return EOF if unable to do so. Suppress
			   '\r', file is assumed text.			*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	FAST int c;

	c = getc(fp);
#ifdef SYS_EOF
	if (c IS SYS_EOF)
	{	--fp->_bptr;
		return EOF;
	}
#endif
#ifdef CRINSUP
	if (c IS '\r' OR c IS NULL)
		c = getca(fp);
#endif
	return c;
}
