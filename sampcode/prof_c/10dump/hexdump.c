/*
 *	hexdump -- read data from an open file and "dump"
 *	it in side-by-side hex and ASCII to standard output
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <local\std.h>

#define LINEWIDTH	80
#define NBYTES		16
#define WORD		0xFFFF
#define RGHTMARK	179
#define LEFTMARK	179
#define DEL		0x7F

int hexdump(fd, strip)
int fd;
BOOLEAN strip;
{
	unsigned char i;
	int n;			/* bytes per read operation */
	unsigned long offset;	/* bytes from start of file */
	char inbuf[BUFSIZ + 1], outbuf[LINEWIDTH + 1];
	char hexbuf[5];
	register char *inp, *outp;

	extern char *byte2hex(unsigned char, char *);
	extern char *word2hex(unsigned int, char *);

	offset = 0;
	while ((n = read(fd, inbuf, BUFSIZ)) != 0) {
		if (n == -1)
			return FAILURE;
		inp = inbuf;
		while (inp < inbuf + n) {
			outp = outbuf;

			/* offset in hex */
			outp += sprintf(outp, "%08lX",
				offset + (unsigned long)(inp - inbuf));
			*outp++ = ' ';

			/* block of bytes in hex */
			for (i = 0; i < NBYTES; ++i) {
				*outp++ = ' ';
				strcpy(outp, byte2hex(*inp++, hexbuf));
				outp += 2;
			}
			*outp++ = ' ';
			*outp++ = ' ';
			*outp++ = (strip == TRUE) ? '|' : LEFTMARK;

			/* same block of bytes in ASCII */
			inp -= NBYTES;
			for (i = 0; i < NBYTES; ++i) {
				if (strip == TRUE && (*inp < ' ' || *inp >= DEL))
					*outp = '.';
				else if (iscntrl(*inp))
					*outp = '.';
				else
					*outp = *inp;
				++inp;
				++outp;
			}
			*outp++ = (strip == TRUE) ? '|' : RGHTMARK;
			*outp++ = '\n';
			*outp = '\0';
			fputs(outbuf, stdout);
		}
		offset += n;
	}
	return SUCCESS;
}
