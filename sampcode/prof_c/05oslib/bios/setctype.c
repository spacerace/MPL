/*
 *	setctype -- set the cursor start and end raster scan lines
 */

#include <dos.h>
#include <local\bioslib.h>

#define LO_NIBBLE	0x0F
#define CURSOR_OFF	0x2
#define MAXSCANLN	15

int
setctype(start, end)
int start;	/* starting raster scan line */
int end;	/* ending raster scan line */
{
	union REGS inregs, outregs;

	inregs.h.ah = CUR_TYPE;
	inregs.h.ch = start & LO_NIBBLE;
	inregs.h.cl = end & LO_NIBBLE;
	if (start >= MAXSCANLN) {
		inregs.h.ah |= CURSOR_OFF;
		inregs.h.al = MAXSCANLN;
	}
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
