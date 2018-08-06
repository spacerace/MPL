/*
 *	writetty -- write to screen using TTY interface
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
writetty(ch, attr, pg)
unsigned char ch;	/* character */
unsigned char attr;	/* video attribute */
int pg;			/* screen page for writes */
{
	union REGS inregs, outregs;

	inregs.h.ah = WRITE_TTY;
	inregs.h.al = ch;
	inregs.h.bl = attr;
	inregs.h.bh = pg;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
