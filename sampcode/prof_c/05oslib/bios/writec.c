/*
 *	writec -- write a character only
 *	(leave attribute undisturbed)
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
writec(ch, count, pg)
unsigned char ch;	/* character */
int count;		/* repetitions */
int pg;			/* screen page for writes */
{
	union REGS inregs, outregs;

	inregs.h.ah = WRITE_CHAR;
	inregs.h.al = ch;
	inregs.h.bh = pg;
	inregs.x.cx = count;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
