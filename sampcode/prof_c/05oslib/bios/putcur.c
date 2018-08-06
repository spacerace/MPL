/*
 *	putcur -- put cursor at specified position (row, col) 
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

putcur(r, c, pg)
unsigned int
	r,	/* row */
	c,	/* column */
	pg;	/* screen page for writes */
{
	union REGS inregs, outregs;

	inregs.h.ah = CUR_POS;
	inregs.h.bh = pg & 0x07;
	inregs.h.dh = r & 0xFF;
	inregs.h.dl = c & 0xFF;

	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
} /* end putcur() */
