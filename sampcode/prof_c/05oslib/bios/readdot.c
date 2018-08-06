/*
 *	readdot -- read the value of a pixel
 *	(in graphics mode only)
 */

#include <dos.h>
#include <local\std.h> 
#include <local\bioslib.h>

int
readdot(row, col, dcolor)
int row, col;
int *dcolor;	/* pointer to dot color */
{
	union REGS inregs, outregs;

	inregs.h.ah = READ_DOT;
	inregs.x.cx = col;
	inregs.x.dx = row;
	int86(VIDEO_IO, &inregs, &outregs);
	*dcolor = outregs.h.al;

	return (outregs.x.cflag);
}
