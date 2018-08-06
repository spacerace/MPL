/*
 *	sb_put -- routines to put characters and strings into the
 *	screen buffer; the cursor location is altered
 */

#include <local\sbuf.h>
#include <ctype.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

/*
 *	sb_putc -- put a character into a screen buffer window
 */

int
sb_putc(win, ch)
struct REGION *win;
unsigned char ch;
{
	short cmax, rmax;
	short lim;
	short noscroll = 0, puterr = 0;

	/* calculate screen buffer position and limits */
	cmax = win->c1 - win->c0;
	rmax = win->r1 - win->r0;
	Sbuf.row = win->r0 + win->row;
	Sbuf.col = win->c0 + win->col;

	/* process the character */
	switch (ch) {
	case '\b':
		/* non-destructive backspace */
		if (win->col > 0) {
			--win->col;
			Sbuf.col = win->c0 + win->col;
			return SB_OK;
		}
		else
			return SB_ERR;
	case '\n':
		/* clear trailing line segment */
		while (win->col < cmax)
			if (sb_putc(win, ' ') == SB_ERR)
				++puterr;
		break;
	case '\t':
		/* convert tab to required number of spaces */
		lim = win->col + 8 - (win->col & 0x7);
		while (win->col < lim)
			if (sb_putc(win, ' ') == SB_ERR)
				++puterr;
		break;
	default:
		/* if printable ASCII, place the character in the buffer */
		if (isascii(ch) && isprint(ch))
			Scrnbuf[Sbuf.row][Sbuf.col].b.ch = ch;
		if (Sbuf.col < Sbuf.lcol[Sbuf.row])
			Sbuf.lcol[Sbuf.row] = Sbuf.col;
		if (Sbuf.col > Sbuf.rcol[Sbuf.row])
			Sbuf.rcol[Sbuf.row] = Sbuf.col;
		break;
	}

	/* update the cursor position */
	if (win->col < cmax)
		++win->col;
	else if (win->row < rmax) {
		win->col = 0;
		++win->row;
	}
	else if ((win->wflags & SB_SCROLL) == SB_SCROLL) {
		sb_scrl(win, 1);
		win->col = 0;
		win->row = rmax;
	}
	else
		++noscroll;

	/* update screen buffer position */
	Sbuf.row = win->r0 + win->row;
	Sbuf.col = win->c0 + win->col;
	Sbuf.flags |= SB_DELTA;

	return ((noscroll || puterr) ? SB_ERR : SB_OK);
} /* end sb_putc() */


/*
 *	sb_puts -- put a string into the screen buffer
 */

int
sb_puts(win, s)
struct REGION *win;
unsigned char *s;
{
	while (*s)
		if (sb_putc(win, *s++) == SB_ERR)
			return SB_ERR;
	return SB_OK;
} /* end sb_puts() */
