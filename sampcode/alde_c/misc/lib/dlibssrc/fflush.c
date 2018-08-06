#include <stdio.h>

int fflush(fp)
register FILE *fp;
/*
 *	Flush the output buffer of the stream <fp>.  The buffer is
 *	automatically flushed when it is full, the stream is closed,
 *	or the program terminates through exit().  This function has
 *	no effect if the stream in unbuffered.
 */
{
	register int f, rv = 0;

	f = fp->F_stat;
	if(!(f & F_BUFFER))			/* file not buffered */
		return(0);
	if(!(f & (F_READ | F_WRITE)))		/* file not open! */
		return(0);
	if((f & F_IODIR) && (fp->F_cnt)) {	/* have data to be written? */
		if(write(fp->F_h, fp->F_buf, fp->F_cnt) < 0)
			fp->F_stat |= F_ERROR;
	}
	fp->F_bp = fp->F_buf;
	fp->F_cnt = 0;
	fp->F_unc1 = fp->F_unc2 = '\0';
}
