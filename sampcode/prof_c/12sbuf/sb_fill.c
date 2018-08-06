/*
 * 	sb_fill -- fill region routines
 */

#include <local\sbuf.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

/*
 *	sb_fill -- set all cells in a specified region
 *	to the same character/attribute value
 */

int
sb_fill(win, ch, attr)
struct REGION *win;
unsigned char ch;	/* fill character */
unsigned char attr;	/* fill attribute */
{
	register int i, j;
	unsigned short ca;

	ca = (attr << 8) | ch;
	for (i = win->sr0; i <= win->sr1; ++i) {
		for (j = win->sc0; j <= win->sc1; ++j)
			Scrnbuf[i][j].cap = ca;
		if (win->sc0 < Sbuf.lcol[i])
			Sbuf.lcol[i] = win->sc0;
		if (win->sc1 > Sbuf.rcol[i])
			Sbuf.rcol[i] = win->sc1;
	}
	Sbuf.flags |= SB_DELTA;

	return SB_OK;
}


/*
 *	sb_fillc -- set all cells in a specified region
 *	to the same character value; leave attributes undisturbed
 */

int
sb_fillc(win, ch)
struct REGION *win;
unsigned char ch;	/* fill character */
{
	register int i, j;

	for (i = win->sr0; i <= win->sr1; ++i) {
		for (j = win->sc0; j <= win->sc1; ++j)
			Scrnbuf[i][j].b.ch = ch;
		if (win->sc0 < Sbuf.lcol[i])
			Sbuf.lcol[i] = win->sc0;
		if (win->sc1 > Sbuf.rcol[i])
			Sbuf.rcol[i] = win->sc1;
	}
	Sbuf.flags |= SB_DELTA;

	return SB_OK;
}


/*
 *	sb_filla -- set all cells in a specified region
 *	to the same attribute value; leave characters undisturbed
 */

int
sb_filla(win, attr)
struct REGION *win;
unsigned char attr;	/* fill attribute */
{
	register int i, j;

	for (i = win->sr0; i <= win->sr1; ++i) {
		for (j = win->sc0; j <= win->sc1; ++j)
			Scrnbuf[i][j].b.attr = attr;
		if (win->sc0 < Sbuf.lcol[i])
			Sbuf.lcol[i] = win->sc0;
		if (win->sc1 > Sbuf.rcol[i])
			Sbuf.rcol[i] = win->sc1;
	}
	Sbuf.flags |= SB_DELTA;

	return SB_OK;
}
