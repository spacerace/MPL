/*
 *	lines -- send newlines to the output stream
 */

#include <stdio.h>
#include <stdlib.h>

int
lines(n, fp)
int n;
FILE *fp;
{
	register int i;

	for (i = 0; i < n; ++i)
		if (putc('\n', fp) == EOF && ferror(fp))
			break;

	/* return number of newlines emitted */
	return (i);
}
