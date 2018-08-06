#include <osbind.h>
#include <stdio.h>
#include <io.h>

static int	_fputc2 = FALSE;	/* "2nd time in" flag */

int fputc(c, fp)
char c;
register FILE *fp;
/*
 *	Put the character <c> to the stream <fp>.
 */
{
	register int f, m;

	f = fp->F_stat;
	if(!(f & F_WRITE))			/* stream opened for write? */
		return(EOF);
	m = (F_BUFFER | F_READ | F_IODIR);
	if((f & m) == (F_BUFFER | F_READ))	/* r/w direction switch */
		fflush(fp);
	fp->F_stat |= F_IODIR;
	if(f & (F_ERROR | F_EOF))		/* error/eof conditions */
		return(EOF);
	if((c == '\n') && !_fputc2 && !(f & F_BINARY)) {
		_fputc2 = TRUE;
		m = fputc('\r', fp);
		_fputc2 = FALSE;
		if(m < 0)
			return(EOF);
	}
	if(f & F_BUFFER) {
		*(fp->F_bp)++ = c;
		if((++(fp->F_cnt)) >= fp->F_bsiz) {
			fp->F_cnt = 0;
			fp->F_bp = fp->F_buf;
			m = fp->F_bsiz;
			if(write(fp->F_h, fp->F_buf, m) != m) {
				fp->F_stat |= F_ERROR;
				return(EOF);
			}
		}
	}
	else {
		if(write(fp->F_h, &c, 1) != 1)
			return(EOF);
	}
	return(c);
}
