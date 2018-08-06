#include <stdio.h>
#include <io.h>

int setvbuf(fp, bp, bmode, size)
register FILE *fp;
char *bp;
int bmode;
unsigned int size;
{
	fp->F_cnt = 0;
	fp->F_unc1 = '\0';
	fp->F_unc2 = '\0';
	if((bmode == _SVBn) || (bp == NULL)) {		/* unbuffered */
		fp->F_stat &= ~F_BUFFER;
		fp->F_buf = NULL;
		fp->F_bsiz = 0;
	}
	else {
		fp->F_stat |= F_BUFFER;
		fp->F_buf = bp;
		fp->F_bsiz = size;
	}
	fp->F_bp = fp->F_buf;
	return(0);
}
