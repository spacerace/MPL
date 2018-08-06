#include <stdio.h>
#include <io.h>

void setbuf(fp, buf)
register FILE *fp;
char *buf;
/*
 *	If <buf> is NULL, make <fp> unbuffered, else <buf> points to a buffer
 *	of BUFSIZ characters to be used as the stream buffer for <fp>.
 */
{
	fp->F_cnt = 0;
	fp->F_unc1 = '\0';
	fp->F_unc2 = '\0';
	fp->F_bp = buf;
	if(fp->F_buf = buf) {
		fp->F_stat |= F_BUFFER;
		fp->F_bsiz = BUFSIZ;
	}
	else {
		fp->F_stat &= ~F_BUFFER;
		fp->F_bsiz = 0;
	}
}
