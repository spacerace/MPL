/*
 *	getctype -- pass back cursor type info (scan lines) 
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

#define LO_NIBBLE	0x0F

int
getctype(start_scan, end_scan, pg)
int *start_scan;/* starting scan line */
int *end_scan;	/* ending scan line */
int pg;		/* "visual" page */
{
	union REGS inregs, outregs;

	inregs.h.bh = pg;
	inregs.h.ah = GET_CUR;

	int86(VIDEO_IO, &inregs, &outregs);

	/* end_scan = low 4 bits of cl */
	*end_scan = outregs.h.cl & LO_NIBBLE;

	/* starting_scan = low 4 bits of ah */
	*start_scan = outregs.h.ch & LO_NIBBLE;

	return (outregs.x.cflag);
}
