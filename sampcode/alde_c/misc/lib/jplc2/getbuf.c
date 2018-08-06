/* 1.3  01-08-86  						(getbuf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/*----------------------------------------------------------------------*

	 	STANDARD I/O STREAM DEFINITIONS:			*/

FILE 	IObuffs[MAXSTREAM] = 	/* i/0 file stream table		*/
{	{0, 0, BUFSIZ,	0, 0, _BUSY, STDIN},
	{0, 0,	1,	0, 0, _BUSY, STDOUT},
	{0, 0,	1,      0, 0, _BUSY, STDERR},
	{0, 0,	0,      0, 0, 	0, 	0},
	{0, 0,	0,      0, 0, 	0,	0},
	{0, 0,	0,      0, 0, 	0,	0},
	{0, 0,	0,      0, 0, 	0, 	0},
	{0, 0,	0,      0, 0, 	0, 	0},
	{0, 0,	0,      0, 0, 	0, 	0},
	{0, 0,	0,      0, 0, 	0,	0}	/* MAXSTREAM elements here */
};

/************************************************************************/
	VOID
getbuf(fp)		/* Get a std buffer either from allot(), or use
			   the bytbuf if none.	Attach to the FILE fp.	*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	BUFFER buffer, allot();

	if ((fp->_buflen < sizeof(HEADER))
	   OR ((buffer = allot(fp->_buflen)) IS NULL))
	{	fp->_buff = &fp->_bytbuf;
		fp->_buflen = 1;
		return;
	}

	fp->_flags |= _ALLBUF;		/* indicate an allocated buffer	*/
	fp->_buff = buffer;
	return;
}
