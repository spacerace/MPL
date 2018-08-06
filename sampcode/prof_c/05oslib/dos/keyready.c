/*
 *	keyready -- non-zero if the keyboard buffer
 *	has any codes waiting
 */

#include <dos.h>
#include <local\doslib.h>

int
keyready()
{
	union REGS inregs, outregs;

	inregs.h.ah = CH_READY;
	intdos(&inregs, &outregs);

	return (outregs.h.al);
}
