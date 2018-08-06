#include <osbind.h>
#include <stdio.h>
#include <io.h>

long fseek(fp, offset, origin)
FILE *fp;
long offset;
int origin;
/*
 *	Operates like lseek(), except it works on streams.  Note that
 *	stream file positions may be misleading due to translation of
 *	<nl> characters during i/o.  ftell() may be used reliably with
 *	fseek() to reposition a file to a prior location.
 */
{
	fflush(fp);
	return(Fseek(offset, fileno(fp), origin));
}

void rewind(fp)
register FILE *fp;
/*
 *	Operates like fseek(f,0L,_LSKbeg), except it also clears the
 *	end-of-file and error flags for <fp>.  There is no return value.
 */
{
	fseek(fp, 0L, _LSKbeg);
	fp->F_stat &= ~(F_EOF|F_ERROR);
}
