/*
 *	getxline -- get a line of text while expanding tabs,
 *	put text into an array, and return a pointer to the
 *	resulting null-terminated line
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <local\std.h>

char *
getxline(buf, size, fin)
char *buf;
int size;
FILE *fin;
{
	register int ch;	/* input character */
	register char *cp;	/* character pointer */

	extern BOOLEAN tabstop(int);

	cp = buf;
	while (--size > 0 && (ch = fgetc(fin)) != EOF) {
		if (ch == '\n') {
			*cp++ = ch;
			break;
		}
		else if (ch == '\t')
			do {
				*cp = ' ';
			} while (--size > 0 && (tabstop(++cp - buf) == FALSE));
		else
			*cp++ = ch & ASCII;
	}
	*cp = '\0';
	return ((ch == EOF && cp == buf) ? NULL : buf);
}
