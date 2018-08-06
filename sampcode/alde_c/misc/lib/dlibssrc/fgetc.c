#include <osbind.h>
#include <stdio.h>
#include <io.h>

static int	_fgetc2 = FALSE;	/* "2nd time in" flag */

int fgetc(fp)
register FILE *fp;
/*
 *	Get a character from <fp>.  Returns the character or EOF.
 */
{
	register int f, m;
	unsigned char c;

	f = fp->F_stat;
	if(!(f & F_READ))			/* stream opened for read? */
		return(EOF);
	m = (F_BUFFER | F_WRITE | F_IODIR);
	if((f & m) == m)			/* r/w direction switch */
		fflush(fp);
	fp->F_stat &= ~F_IODIR;
	if(f & (F_ERROR | F_EOF))		/* error/eof conditions */
		return(EOF);
	if(fp->F_unc1) {			/* ungotten character */
		c = fp->F_unc1;
		fp->F_unc1 = fp->F_unc2;
		fp->F_unc2 = '\0';
	}
	else if(f & F_BUFFER) {
		if(!((fp->F_cnt)--)) {
			if((m = read(fp->F_h, fp->F_buf, fp->F_bsiz)) < 0) {
				fp->F_stat |= F_ERROR;
				return(EOF);
			}
			if(m == 0) {
				fp->F_stat |= F_EOF;
				return(EOF);
			}
			fp->F_cnt = (m - 1);
			fp->F_bp = fp->F_buf;
		}
		c = *(fp->F_bp)++;
	}
	else {
		if((m = read(fp->F_h, &c, 1)) < 0) {
			fp->F_stat |= F_ERROR;
			return(EOF);
		}
		if(m == 0) {
			fp->F_stat |= F_EOF;
			return(EOF);
		}
	}
	if((c == '\r') && !_fgetc2 && !(f & F_BINARY)) {
		_fgetc2 = TRUE;
		if(f & F_DEVICE)			/* translated device */
			c = '\n';
		else if((c = fgetc(fp)) != '\n') {	/* translated stream */
			fungetc(c, fp);
			c = '\r';
		}
		_fgetc2 = FALSE;
	}
	else if((c == '\n') && !(f & F_BINARY) && (f & F_DEVICE))
		c = fgetc(fp);				/* filter <LF> on DEV */
	else if((c == 0x1A) && !(f & F_BINARY)) {	/* ^Z xlat to EOF */
		fp->F_stat |= F_EOF;
		return(EOF);
	}
	return(c);
}

int fungetc(c, fp)
char c;
register FILE *fp;
/*
 *	Push the character <c> back to be gotten by the next fgetc()
 *	call on <fp>.  Only 1 character may be ungotten at a time on
 *	each stream.  Subsequent calls to fungetc() will write over
 *	the currently saved character.
 */
{
	if(fp->F_stat & (F_ERROR | F_EOF))
		return(EOF);
	fp->F_unc2 = fp->F_unc1;
	return(fp->F_unc1 = c);
}
