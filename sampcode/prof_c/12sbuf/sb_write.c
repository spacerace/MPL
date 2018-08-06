/*
 *	sb_write -- screen buffer write routines
 */

#include <local\sbuf.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

/*
 *	sb_wa -- write an attribute to a region of the screen buffer
 */

int
sb_wa(win, attr, n)
struct REGION *win;	/* window pointer */
unsigned char attr;	/* attribute */
short n;		/* repetition count */
{
	short i;
	short row;
	short col;

	i = n;
	row = win->r0 + win->row;
	col = win->c0 + win->col;
	while (i--)
		Scrnbuf[row][col + i].b.attr = attr;

	/* marked the changed region */
	if (col < Sbuf.lcol[row])
		Sbuf.lcol[row] = col;
	if (col + n > Sbuf.rcol[row])
		Sbuf.rcol[row] = col + n;
	Sbuf.flags |= SB_DELTA;

	return (i == 0) ? SB_OK : SB_ERR;
} /* end sb_wa() */


/*
 *	sb_wc -- write a character to a region of the screen buffer
 */

int
sb_wc(win, ch, n)
struct REGION *win;	/* window pointer */
unsigned char ch;	/* character */
short n;		/* repetition count */
{
	short i;
	short row;
	short col;

	i = n;
	row = win->r0 + win->row;
	col = win->c0 + win->col;
	while (i--)
		Scrnbuf[row][col + i].b.ch = ch;

	/* marked the changed region */
	if (col < Sbuf.lcol[row])
		Sbuf.lcol[row] = col;
	if (col + n > Sbuf.rcol[row])
		Sbuf.rcol[row] = col + n;
	Sbuf.flags |= SB_DELTA;

	return (i == 0 ? SB_OK : SB_ERR);
} /* end sb_wc() */


/*
 *	sb_wca -- write a character/attribute pair to a region
 *	of the screen buffer
 */

int
sb_wca(win, ch, attr, n)
struct REGION *win;	/* window pointer */
unsigned char ch;	/* character */
unsigned char attr;	/* attribute */
short n;		/* repetition count */
{
	int i;
	short row;
	short col;

	i = n;
	row = win->r0 + win->row;
	col = win->c0 + win->col;
	while (i--)
		Scrnbuf[row][col + i].cap = (attr << 8) | ch;

	/* marked the changed region */
	if (col < Sbuf.lcol[row])
		Sbuf.lcol[row] = col;
	if (col + n > Sbuf.rcol[row])
		Sbuf.rcol[row] = col + n;
	Sbuf.flags |= SB_DELTA;

	return (i == 0 ? SB_OK : SB_ERR);
} /* end sb_wca() */
