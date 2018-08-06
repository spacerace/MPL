#include <stdio.h>

void fputs(data, fp)
register char *data;
register FILE *fp;
/*
 *	Write the characters in <data> to the stream <fp>.  A newline
 *	WILL NOT be added.
 */
{
	while(*data)
		fputc(*data++, fp);
}

void puts(data)
char *data;
/*
 *	Write the characters in <data> to stdout.  A newline WILL be
 *	written after the data.
 */
{
	fputs(data, stdout);
	fputc('\n', stdout);
}
