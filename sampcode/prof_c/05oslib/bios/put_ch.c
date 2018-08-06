/*
 *	put_ch -- display a character in the prevailing video
 *	attribute and advance the cursor position
 */

#include <local\video.h>

int
put_ch(ch, pg)
register char ch;
int pg;
{
	int r, c, c0;

	readcur(&r, &c, pg);
	writec(ch, 1, pg);
	putcur(r, ++c, pg);
	return (1);
}
