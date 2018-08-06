/*
 *	pr_getln -- get a line of text while expanding tabs;
 *	put text into an array and return the length of the line
 *	including termination to the calling function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

int
pr_getln(s, lim, fin)
char *s;
int lim;
FILE *fin;
{
	int ch;
	register char *cp;

	extern int tabstop();	/* query tabstop array */

	cp = s;
	while (--lim > 0 && (ch = fgetc(fin)) != EOF && ch != '\n' && ch != '\f') {
		if (ch == '\t')
			/* loop and store spaces until next tabstop */
			do
				*cp++ = ' ';
			while (--lim > 0 && tabstop(cp - s) == 0);
		else
			*cp++ = ch;
	}
	if (ch == EOF && cp - s == 0)
		;
	else if (ch == EOF || ch == '\n')
		*cp++ = '\n';	/* assure correct line termination */
	else if (ch == '\f' && cp - s == 0) {
		*cp++ = '\f';
		fgetc(fin);	/* toss the trailing newline */
	}
	*cp = '\0';

	return (cp - s);
}
