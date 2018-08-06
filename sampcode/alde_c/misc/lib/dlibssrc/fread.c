#include <osbind.h>
#include <stdio.h>

int fread(data, size, count, fp)
register char *data;
int size;
int count;
register FILE *fp;
/*
 *	Read <count> items of <size> characters each from stream <fp>.
 *	Data is stored in the buffer pointed to by <data>.  The number of
 *	full items actually read is returned, or a negative error code.
 */
{
	register long n, lsiz;

	lsiz = ((long) size);
	n = ((long) count) * lsiz;
	n = Fread(fileno(fp), n, data);
	return((n > 0) ? (n / lsiz) : n);
}
