#include <osbind.h>
#include <stdio.h>
#include <io.h>

extern	FILE	_iob[];

FILE *fdopen(h, mode)
register int h;
register char *mode;
/*
 *	Associates a stream with the already open file <h>.  The <mode>
 *	values are the same as for fopen(), but MUST be compatible with
 *	the mode in which <h> was open()ed.  This functions allows use
 *	of a file opened with the low level open()/creat() calls to be
 *	used as a buffered/translated stream.  A pointer to a FILE struct
 *	is returned, or NULL for errors.
 */
{
	register FILE *fp = NULL;
	register char *p = NULL;
	register int i, j = FALSE, f = 0;
	char *malloc();

	for(i=0; (!fp && (i < MAXFILES)); ++i)
		if(!(_iob[i].F_stat & (F_READ | F_WRITE)))   /* empty slot? */
			fp = &_iob[i];
	if(!fp)
		return(NULL);
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
	if((p=malloc(BUFSIZ)) != NULL)	/* set up file buffer */
		f |= F_BUFFER;
	if(j)				/* append mode. move to eof. */
		lseek(h, 0L, _LSKend);
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
