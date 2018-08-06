/*
 *	clrw -- clear specified region of "visual" screen page
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
clrw(t, l, b, r, a)
int t;		/* top row of region to clear */
int l;		/* left column */
int b;		/* bottom row */
int r;		/* right column */
unsigned char a;/* attribute for cleared region */
{
	union REGS inregs, outregs;

	inregs.h.ah = SCROLL_UP;/* scroll visual page up */
	inregs.h.al = 0;	/* blank entire window */
	inregs.h.bh = a;	/* attribute of blank lines */
	inregs.h.bl = 0;
	inregs.h.ch = t;	/* upper left of scroll region */
	inregs.h.cl = l;
	inregs.h.dh = b;	/* lower right of scroll region */
	inregs.h.dl = r;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
