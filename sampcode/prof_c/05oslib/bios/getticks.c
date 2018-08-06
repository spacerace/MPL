/*
 *	getticks -- get the current bios clock ticks value
 */

#include <dos.h>
#include <local\bioslib.h>

long
getticks()
{
	long count;
	union REGS inregs, outregs;

	/* get BIOS time of day as no. of ticks since midnight */
	inregs.h.ah = 0;
	int86(TOD, &inregs, &outregs);

	/* correct for possible rollover at 24 hours */
	count = (outregs.h.al != 0) ? 0x01800B0L : 0;

	/* add current day ticks */
	count += (outregs.x.dx + (outregs.x.cx << 16));

	return (count);
}
