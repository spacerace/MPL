/*
 *	clrscrn -- clear the "visual" screen page
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>
#include <local\video.h>

int
clrscrn(a)
unsigned int a;	/* video attribute for new lines */
{
	union REGS inregs, outregs;

	inregs.h.ah = SCROLL_UP;
	inregs.h.al = 0;		/* blank entire window */
	inregs.h.bh = a;		/* use specified attribute */
	inregs.h.bl = 0;
	inregs.x.cx = 0;		/* upper left corner */
	inregs.h.dh = Maxrow[Vmode] - 1;/* bottom screen row */
	inregs.h.dl = Maxcol[Vmode] - 1;/* rightmost column */
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
