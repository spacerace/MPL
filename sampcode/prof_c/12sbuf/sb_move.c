/*
 *	sb_move -- move the screen buffer "cursor"
 */

#include <local\sbuf.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

int
sb_move(win, r, c)
struct REGION *win;	/* window pointer */
register short r, c;	/* buffer row and column */
{
	/* don't change anything if request is out of range */
	if (r < 0 || r > win->r1 - win->r0 || c < 0 || c > win->c1 - win->c0)
		return SB_ERR;
	win->row = r;
	win->col = c;
	Sbuf.row = r + win->r0;
	Sbuf.col = c + win->c0;
	return SB_OK;
}
