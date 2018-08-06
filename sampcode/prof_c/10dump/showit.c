/*
 *	showit -- make non-printable characters in
 *	the stream fp visible (or optionally strip them)
 */

#include <stdio.h>
#include <ctype.h>
#include <local\std.h>

int
showit(fp, strip, wp)
FILE *fp;
BOOLEAN strip;	/* strip non-ASCII codes */
BOOLEAN wp;	/* filter typical word processing codes */
{
	int ch;
	
	clearerr(fp);
	while ((ch = getc(fp)) != EOF)
		if (isascii(ch) && (isprint(ch) || isspace(ch)))
			switch (ch) {
			case '\r':
				if (wp == TRUE) {
					if ((ch = getc(fp)) != '\n')
						ungetc(ch, fp);
					putchar('\r');
					putchar('\n');
				}
				else
					putchar(ch);
				break;
			default:
				putchar(ch);
				break;
			}
		else if (strip == FALSE)
			printf("\\%02X", ch);
		else if (wp == TRUE && isprint(ch & ASCII))
			putchar(ch & ASCII);
	return (ferror(fp));
}
