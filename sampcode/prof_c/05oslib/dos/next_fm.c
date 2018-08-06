/*
 *	next_fm - find next file match in work directory
 */

#include <dos.h>
#include <local\doslib.h>

int
next_fm()
{
	union REGS inregs, outregs;

	/* find next matching file */
	inregs.h.ah = FIND_NEXT;
	(void)intdos(&inregs, &outregs);

	return (outregs.x.cflag);
}
