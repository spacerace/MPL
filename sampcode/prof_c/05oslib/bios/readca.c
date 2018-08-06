/*
 *	readca -- read character and attribute at current position
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int readca(ch, attr, pg)
unsigned char *ch;
unsigned char *attr;
unsigned int pg;	/* screen page for reads */
{
	union REGS inregs, outregs;

	inregs.h.ah = READ_CHAR_ATTR;
	inregs.h.bh = pg;		/* display page */

	int86(VIDEO_IO, &inregs, &outregs);

	*ch = outregs.h.al;		/* character */
	*attr = outregs.h.ah;		/* attribute */

	/* return the value in AX register */
	return (outregs.x.cflag);
} /* end readca() */
