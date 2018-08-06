/*
 *	spaces -- send spaces (blanks) to the output stream
 */

#include <stdio.h>
#include <stdlib.h>

int
spaces(n, fp)
int n;
FILE *fp;
{
	register int i;

	for (i = 0; i < n; ++i)
		if (putc(' ', fp) == EOF && ferror(fp))
			break;

	/* return number of spaces emitted */
	return (i);
}
