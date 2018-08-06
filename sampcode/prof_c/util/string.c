/*
 *	string -- emit a sequence of n repetitions of the
 *	character ch 
 */

#include <stdio.h>
#include <stdlib.h>

int
string(n, ch, fp)
int n;
char ch;
FILE *fp;
{
	register int i;

	for (i = 0; i < n; ++i)
		if (fputc(ch, fp) == EOF && ferror(fp))
			break;

	/* return number of actual repetitions */
	return (i);
}
