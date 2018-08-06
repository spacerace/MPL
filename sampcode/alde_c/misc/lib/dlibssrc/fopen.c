#include <osbind.h>
#include <stdio.h>
#include <io.h>

#define	LAST_DEVICE	-3		/* lowest character device handle # */

extern	FILE	_iob[];

static FILE *_fopen(filename, mode, fp)
char *filename;
register char *mode;
register FILE *fp;
/*
 *	INTERNAL FUNCTION.  Attempt to open <filename> in the given
 *	<mode> and attach it to the stream <fp>
 */
{
	register char *p = NULL;
	register int h, i, j = FALSE, f = 0;
	char *malloc();

	while(*mode) {
		switch(*mode++) {
			case 'r':
				f |= F_READ;
				break;
			case 'w':
				f |= F_WRITE;
				break;
			case 'a':
				f |= F_WRITE;
				j = TRUE;
				break;
			case '+':
				f |= (F_READ | F_WRITE);
				break;
			case 'b':
				f |= F_BINARY;
				break;
			case 't':
				f &= ~F_BINARY;
				break;
			default:
				fputs(stderr, "Illegal file mode.\n");
				return(NULL);
		}
	}
	if((i = (f & (F_READ | F_WRITE))) == 0)
		return(NULL);
	if(i == F_READ)
		h = open(filename, _OPNr);
	else if(!j && (i == F_WRITE))
		h = creat(filename, _CRErw);
	else {
		i = ((i == F_WRITE) ? _OPNw : _OPNrw);
		if ((h = open(filename, i)) < LAST_DEVICE)
			h = creat(filename, _CRErw);
	}
	if(h < 0) {
		if (h < LAST_DEVICE)
			return(NULL);		/* file open/create error */
		else
			f |= F_DEVICE;		/* character device handles */
	}
	else {
		if((p=malloc(BUFSIZ)) != NULL)	/* set up file buffer */
			f |= F_BUFFER;
		if(j)				/* append mode. move to eof. */
			lseek(h, 0L, _LSKend);
	}
	fp->F_h = h;			/* file handle */
	fp->F_stat = f;			/* file status flags */
	fp->F_buf = p;			/* base of file buffer */
	fp->F_bp = p;			/* current buffer pointer */
	fp->F_bsiz = (p ? BUFSIZ : 0);	/* buffer size */
	fp->F_cnt = 0;			/* # of bytes in buffer */
	fp->F_unc1 = '\0';		/* primary ungotten character */
	fp->F_unc2 = '\0';		/* secondary ungotten character */
	return(fp);
}

FILE *fopen(filename, mode)
char *filename;
char *mode;
/*
 *	Open <filename> as a stream file.  This is the normal open way
 *	to open a file.  The <mode> is a string specifying the mode(s)
 *	that are relevent to the file open.  Valid <mode> characters are:
 *		r		read mode
 *		w		write mode
 *		a		append mode
 *		b		binary mode
 *		t		text (translated) mode
 *	At least one of "r", "w" or "a" must be specified.  "t" is assumed
 *	and indicates that <nl> is translated to <cr><lf> on output and
 *	vica-versa on input.  "b" overides "t" and indicated that characters
 *	are not translated during i/o operations.  "a" represents append
 *	mode and means that the file pointer will initially be placed at
 *	the end of the file.  "w" mode will create a file if it doesn't
 *	exists, or zero an existing file.  If "r" is the only mode specified,
 *	the file must already exist.  A (FILE *) is returned if the open
 *	succeeds, or NULL if it fails.
 */
{
	register int i;
	register FILE *fp = NULL;

	for(i=0; (!fp && (i < MAXFILES)); ++i)
		if(!(_iob[i].F_stat & (F_READ | F_WRITE)))   /* empty slot? */
			fp = &_iob[i];
	if(fp)
		return(_fopen(filename, mode, fp));
	else
		return(NULL);
	/* return(fp ? _fopen(filename, mode, fp) : NULL); */
}

FILE *freopen(filename, mode, fp)
char *filename;
char *mode;
FILE *fp;
/*
 *	Closes the file associated with <fp> and opens the new file as with
 *	fopen(), except that a new FILE structure is not created.  The
 *	existing FILE structure pointed to by <fp> is re-initialized with
 *	the new file information.  This is typically used to redirect i/o
 *	to standard streams "stdin", "stdout", "stderr", "stdprn", "stdaux".
 *	<fp> is returned for success, or NULL for failure.
 */
{
	if(fclose(fp))
		return(NULL);
	else
		return(_fopen(filename, mode, fp));
	/* return(fclose(fp) ? NULL : _fopen(filename, mode, fp)); */
}

int fclose(fp)
register FILE *fp;
/*
 *	Close the stream <fp>, flushing the buffer.  Returns 0 on success.
 */
{
	if((fp->F_stat & (F_READ | F_WRITE)) == 0)
		return(ERROR);				/* file not open! */
	fflush(fp);
	if(fp->F_stat & F_BUFFER)
		free(fp->F_buf);			/* free buffer */
	fp->F_stat = 0;					/* clear status */
	fp->F_buf = NULL;
	fp->F_bsiz = 0;
	return(close(fp->F_h) ? EOF : 0);
}
