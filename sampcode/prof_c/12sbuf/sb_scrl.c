/*
 *	sb_scrl -- scrolling routines
 */

#include <local\sbuf.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];


/*
 *	sb_scrl -- scroll the specified window
 *	n lines (direction indicated by sign)
 */

int
sb_scrl(win, n)
struct REGION *win;
short n;		/* number of rows to scroll */
{
	register short r, c;

	if (n == 0)
		/* clear the entire region to spaces */
		sb_fillc(win, ' ');
	else if (n > 0) {
		/* scroll n rows up */
		for (r = win->sr0; r <= win->sr1 - n; ++r) {
			for (c = win->sc0; c <= win->sc1; ++c)
				Scrnbuf[r][c] = Scrnbuf[r + n][c];
			if (win->sc0 < Sbuf.lcol[r])
				Sbuf.lcol[r] = win->sc0;
			if (win->sc1 > Sbuf.rcol[r])
				Sbuf.rcol[r] = win->sc1;
		}
		for ( ; r <= win->sr1; ++r) {
			for (c = win->sc0; c <= win->sc1; ++c)
				Scrnbuf[r][c].b.ch = ' ';
			if (win->sc0 < Sbuf.lcol[r])
				Sbuf.lcol[r] = win->sc0;
			if (win->sc1 > Sbuf.rcol[r])
				Sbuf.rcol[r] = win->sc1;
		}
	}
	else {
		/* scroll n rows down */
		n = -n;
		for (r = win->sr1; r >= win->sr0 + n; --r) {
			for (c = win->sc0; c <= win->sc1; ++c)
				Scrnbuf[r][c] = Scrnbuf[r - n][c];
			if (win->sc0 < Sbuf.lcol[r])
				Sbuf.lcol[r] = win->sc0;
			if (win->sc1 > Sbuf.rcol[r])
				Sbuf.rcol[r] = win->sc1;
		}
		for ( ; r >= win->sr0; --r) {
			for (c = win->sc0; c <= win->sc1; ++c)
				Scrnbuf[r][c].b.ch = ' ';
			if (win->sc0 < Sbuf.lcol[r])
				Sbuf.lcol[r] = win->sc0;
			if (win->sc1 > Sbuf.rcol[r])
				Sbuf.rcol[r] = win->sc1;
		}
	}
	Sbuf.flags |= SB_DELTA;
	return SB_OK;
} /* end sb_scrl() */


/*
 *	sb_set_scrl -- set the scroll region boundaries
 */

int
sb_set_scrl(win, top, left, bottom, right)
struct REGION *win;	/* window pointer */
short top, left;	/* upper-left corner */
short bottom, right;	/* lower-left corner */
{
	if (top < 0 || left < 0 ||
		bottom > win->r1 - win->r0 || right > win->c1 - win->c0)
		return SB_ERR;
	win->sr0 = win->r0 + top;
	win->sc0 = win->c0 + left;
	win->sr1 = win->r0 + bottom - 1;
	win->sc1 = win->c0 + right - 1;
	return SB_OK;
} /* end sb_set_scrl() */
