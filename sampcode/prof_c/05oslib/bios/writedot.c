/*
 *	writedot -- display a dot at the specified position
 */

#include <dos.h>
#include <local\std.h> 
#include <local\bioslib.h>

int
writedot(r, c, color)
int r, c;	/* row and column cordinate */
int color;	/* dot (pixel) color */
{
	union REGS inregs, outregs;

	inregs.h.ah = WRITE_DOT;
	inregs.h.al = color;
	inregs.x.cx = c;
	inregs.x.dx = r;
	int86(VIDEO_IO, &inregs, &outregs);

	return (outregs.x.cflag);
}
