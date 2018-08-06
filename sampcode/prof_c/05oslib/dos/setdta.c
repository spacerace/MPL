/*
 *	setdta -- tell DOS where to do disk transfers
 */

#include <dos.h>
#include <local\doslib.h>

void
setdta(bp)
char *bp;	/* pointer to byte-aligned disk transfer area */
{
	union REGS inregs, outregs;

	inregs.h.ah = SET_DTA;
	inregs.x.dx = (unsigned int)bp;
	(void)intdos(&inregs, &outregs);
}
