/*
 *	scroll -- scroll a region of the "visual" screen
 *	page up or down by n rows (0 = initialize region)
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
scroll(t, l, b, r, n, a)
int t;	/* top row of scroll region */
int l;	/* left column */
int b;	/* bottom row */
int r;	/* right column */
int n;	/* number of lines to scroll */
	/* sign indicates direction to scroll */
	/* 0 means scroll all lines in the region (initialize) */
unsigned char a;/* attribute for new lines */
{
	union REGS inregs, outregs;

	if (n < 0) {
		/* scroll visual page down n lines */
		inregs.h.ah = SCROLL_DN;
		inregs.h.al = -n;
	}
	else {
		/* scroll visual page up n lines */
		inregs.h.ah = SCROLL_UP;
		inregs.h.al = n;
	}
	inregs.h.bh = a;	/* attribute of blank lines */
	inregs.h.bl = 0;
	inregs.h.ch = t;	/* upper-left of scroll region */
	inregs.h.cl = l;
	inregs.h.dh = b;	/* lower-right of scroll region */
	inregs.h.dl = r;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
