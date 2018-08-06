#include <stdio.h>

int getw(fp)
FILE *fp;
/*
 *	Get a 2-byte value from the stream <fp>.  The high-order byte is
 *	read first.
 */
{
	register int n, c;

	if((c = fgetc(fp)) == EOF)
		return(EOF);
	n = (c << 8);
	if((c = fgetc(fp)) == EOF)
		return(EOF);
	n |= (c & 0xFF);
}

int putw(n, fp)
register int n;
FILE *fp;
/*
 *	Put the 2-byte value <n> to the stream <fp>.  The high-order byte
 *	is written first.
 */
{
	register int m;

	m = (n >> 8);
	fputc((m & 0xFF), fp);
	fputc((n & 0xFF), fp);
}
