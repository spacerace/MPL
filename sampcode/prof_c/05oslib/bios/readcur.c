/*
 *	readcur	-- pass back the cursor position (row, col) 
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

unsigned int readcur(row, col, pg)
unsigned int *row;	/* current row */
unsigned int *col;	/* current column */
unsigned int pg;	/* screen page */
{
	union REGS inregs, outregs;

	inregs.h.ah = GET_CUR;
	inregs.h.bh = pg;

	int86(VIDEO_IO, &inregs, &outregs);

	*col = outregs.h.dl;		/* col */
	*row = outregs.h.dh;		/* row */

	return (outregs.x.cflag);
} /* end readcur() */
