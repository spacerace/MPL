/*
 *	palette	-- set graphics color values or border color
 */

#include <dos.h>
#include <local\bioslib.h>

int
palette(id, color)
unsigned int id, color;
{
	union REGS inregs, outregs;

	inregs.h.ah = PALETTE;
	inregs.h.bh = id;
	inregs.h.bl = color;
	int86(VIDEO_IO, &inregs, &outregs);

	return(outregs.x.cflag);
}
