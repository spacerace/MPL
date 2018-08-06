/*
 *	sb_read -- read character/attribute data
 */

#include <local\sbuf.h>

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

unsigned char
sb_ra(win)
struct REGION *win;	/* window pointer */
{
	return (Scrnbuf[win->r0 + win->row][win->c0 + win->col].b.attr);
} /* end sb_ra() */


/*
 *	sb_rc -- read character from current location in screen buffer
 */

unsigned char
sb_rc(win)
struct REGION *win;	/* window pointer */
{
	return (Scrnbuf[win->r0 + win->row][win->c0 + win->col].b.ch);
} /* end sb_rc() */


/*
 *	sb_rca -- read character/attribute pair from current
 *	location in screen buffer
 */

unsigned short
sb_rca(win)
struct REGION *win;	/* window pointer */
{
	return (Scrnbuf[win->r0 + win->row][win->c0 + win->col].cap);
} /* end sb_rca() */
