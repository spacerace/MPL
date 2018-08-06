#include <stdio.h>

extern	char	_numstr[];		/* string of hexadecimal digits */

char *ctlcnv(string)
char *string;
/*
 *	Convert \<char> notation in <string> to actual characters.  This
 *	is useful for reading strings from a stream when you want to allow
 *	insertion of control character or other characters that may have
 *	special meaning otherwise, or may not otherwise be allowed.  The
 *	following formats are supported:
 *		\n		newline or linefeed
 *		\r		carriage return
 *		\0		null character (value 0)
 *		\b		backspace
 *		\t		horizontal tab
 *		\v		vertical tab
 *		\f		form feed
 *		\a		alarm (bell)
 *		\\		backslash
 *		\'		single quote
 *		\"		double quote
 *		\NNN		octal constant
 *		\xNN		hexadecimal constant
 *		\<nl>		"folded" line (both characters removed)
 */
{
	register char *p, *q, c;
	register int i, n;

	p = q = string;
	while(*q) {
		if(*q == '\\') {
			switch(*++q) {
				case 'n':	/* newline or linefeed */
					*p++ = '\n';
					break;
				case 'r':	/* carriage return */
					*p++ = '\r';
					break;
				case 'b':	/* backspace */
					*p++ = '\b';
					break;
				case 't':	/* horizontal tab */
					*p++ = '\t';
					break;
				case 'v':	/* vertical tab */
					*p++ = '\v';
					break;
				case 'f':	/* form feed */
					*p++ = '\f';
					break;
				case 'a':	/* alarm (bell) */
					*p++ = '\a';
					break;
				case '0':
				case '1':
				case '2':
				case '3':	/* octal constant */
					c = (*q - '0');
					i = 0;
					n = 3;
					do {
						i = (i<<3) + c;
						c = (*++q - '0');
					} while(--n & (c >= 0) && (c < 8));
					*p++ = i;
					--q;
					break;
				case 'x':	/* hexadecimal constant */
					i = 0;
					n = 2;
					while(n-- & ((c = strpos(_numstr,
					    toupper(*++q))) >= 0))
						i = (i << 4) + c;
					--q;
					break;
				case '\n':	/* "folded" line */
					break;
				default:
					*p++ = *q;
			}
		}
		else
			*p++ = *q;
		++q;
	}
	*p = '\0';
	return(string);
}
