/*
 *	writeca -- write character and attribute to the screen
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
writeca(ch, attr, count, pg)
unsigned char ch;	/* character */
unsigned char attr;	/* attribute */
int count;		/* number of repetitions */
int pg;			/* screen page for writes */
{
	union REGS inregs, outregs;

	inregs.h.ah = WRITE_CHAR_ATTR;
	inregs.h.al = ch;
	inregs.h.bh = pg;
	inregs.h.bl = attr;
	inregs.x.cx = count;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
