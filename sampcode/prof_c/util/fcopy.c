/*
 *	fcopy -- copy input stream (fin) to output stream
 *	(fout) and return an indication of success or failure
 */

#include <stdio.h>

#define BUFLEN	1024

int
fcopy(fin, fout)
FILE *fin, *fout;
{
	int errcount = 0;
	char line[BUFLEN];
	register char *s;

	while ((s = fgets(line, BUFLEN, fin)) != NULL)
		if (fputs(s, fout) == EOF)
			++errcount;
	if (ferror(fin))
		++errcount;
	return (errcount);	/* 0 if all went well */
}
