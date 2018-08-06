/*
 *	drawbox -- create a box with IBM line-drawing characters
 */

#include <local\video.h>

int
drawbox(top, lft, btm, rgt, pg)
int top, lft, btm, rgt, pg;
{
	int i;
	int x;	/* interior line length for top and bottom segments */

	x = rgt - lft - 1;

	/* draw the top row */
	putcur(top, lft, pg);
	put_ch(ULC11, pg);
	writec(HBAR1, x, pg);
	putcur(top, rgt, pg);
	put_ch(URC11, pg);

	/* draw the sides */
	for (i = 1; i < btm - top; ++i)
	{
		putcur(top + i, lft, pg);
		put_ch(VBAR1, pg);
		putcur(top + i, rgt, pg);
		put_ch(VBAR1, pg);
	}

	/* draw the bottom row */
	putcur(btm, lft, pg);
	put_ch(LLC11, pg);
	writec(HBAR1, x, pg);
	putcur(btm, rgt, pg);
	put_ch(LRC11, pg);
}
