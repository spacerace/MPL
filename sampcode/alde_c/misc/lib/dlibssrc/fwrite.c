#include <osbind.h>
#include <stdio.h>

int fwrite(data, size, count, fp)
register char *data;
int size;
int count;
register FILE *fp;
/*
 *	Write <count> items of <size> characters each to stream <fp>.
 *	Data is read from the buffer pointed to by <data>.  The number of
 *	full items actually written is returned, or a negative error code.
 */
{
	register long n, lsiz;

	lsiz = ((long) size);
	n = ((long) count) * lsiz;
	n = Fwrite(fileno(fp), n, data);
	return((n > 0) ? (n / lsiz) : n);
}
