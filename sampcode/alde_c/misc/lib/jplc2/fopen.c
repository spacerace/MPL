/* 1.2  12-17-85						(fopen.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
fclose(fp)		/* flush the FILE fp buffer, and close the file.
			   Return EOF on error, NULL otherwise. 	*/
/*----------------------------------------------------------------------*/
FAST FILE *fp;
{
	int err;
	METACHAR fflush();

	err = NULL;
	if (fp->_flags)
	{	if (fp->_flags & _DIRTY)   /* if modifed, flush buffer 	*/
			err = fflush(fp);
		err |= close(fp->_unit);
		if (fp->_flags & _ALLBUF)	/* put the buffer back in */
			liberate(fp->_buff, BUFSIZ);	/* the free list. */
	}
	fp->_flags = NULL;
	return err;
}

/************************************************************************/
	METACHAR
fflush(fp)		/* Write characters in the FILE fp buffer onto
			   the file.  Return EOF on error.		*/
/*----------------------------------------------------------------------*/
FILE *fp;
{
	METACHAR flush();

	return flush(fp, EOF);
}
/*\p*********************************************************************/
	FILE *
fopen(name, type)	/* Open name file according to given type. Return
			   pointer to stream, or NULL on failure.	*/
/*----------------------------------------------------------------------*/
STRING name, type;
{
	FAST FILE *fp;
	FAST BOOL t;
	FAST int fd;

	for (fp = IObuffs; fp->_flags; ) /* look for a blank flag field. */
		if (++fp >= IObuffs + MAXSTREAM)
			return (NULL);

	t = (type[1] IS '+');
	switch (*type)
	{	case 'r':
			if ((fd = open(name, t? _RDWRIT : _RDONLY)) IS EOF)
				return (NULL);
			break;
		case 'w':
			if ((fd = creat(name, PMODE)) IS EOF)
				return (NULL);
			if (t)
			{	close(fd);
				fd = open(name, _RDWRIT);
			}
			break;
		case 'a':
			if ((fd = open(name, t? _RDWRIT : _WRONLY)) IS EOF)
			{	if ((fd = creat(name, PMODE)) IS EOF)
					return (NULL);

				close(fd);
				fd = open(name, t ? _RDWRIT : _WRONLY);
				break;
			}
			lseek(fd, 0L, 2);
			break;
		default:
			errno = EINVAL;
			return NULL;
	}
	fp->_unit = fd;
	fp->_flags = _BUSY;
	fp->_buflen = BUFSIZ;
	fp->_buff = fp->_bend = fp->_bptr = NULL;
	return (fp);
}
