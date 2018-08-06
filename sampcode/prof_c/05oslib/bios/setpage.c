/*
 *	setpage -- select "visual" screen page for viewing
 *	(the "active" page is the one being written to)
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>
#include <local\video.h>

int
setpage(pg)
int pg;	/* visual screen page number */
{
	union REGS inregs, outregs;

	/* check page number against table */
	if (Maxpage[Vmode] > 0 && (pg < 0 || pg >= Maxpage[Vmode]))
		return (-1);

	/* change the visual page */
	inregs.h.ah = SET_PAGE;
	inregs.h.al = pg;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
