/*
 *	writea -- write attribute only to screen memory (faked by
 *	reading char and attr and writing back the original
 *	character and the new attribute at each position)
 */

#include <local\std.h>

int writea(a, n, pg)
unsigned char a;/* video attribute */
int n;		/* number of positions to write */
int pg;		/* screen page */
{
	int i;
	int status;
	unsigned short chx, attrx;
	unsigned short r, c;

	/* get starting (current) position */
	status = 0;
	status = readcur(&r, &c, pg);
	for (i = 0; i < n; ++i) {
		status += putcur(r, c + i, pg);
		status += readca(&chx, &attrx, pg);
		status += writeca(chx, a, 1, pg);
	}

	/* restore cursor position */
	status += putcur(r, c, pg);

	return (status);
}
