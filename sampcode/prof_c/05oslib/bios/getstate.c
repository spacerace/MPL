/*
 *	getstate -- update video state structure
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

/* current video state/mode information */
short Vmode;
short Vwidth;
short Vpage;

/*
 *	video tables -- these tables of video parameters use
 *	a value of -1 to indicate that an item is not supported
 *	and 0 to indicate that an item has a variable value.
 */

/* video limit tables */
short Maxrow[] = { 
	/* CGA modes */
	25, 25, 25, 25, 25, 25, 25,
	/* MDA mode */
	25,
	/* PCjr modes */
	25, 25, 25,
	/* not used */
	-1, -1,
	/* EGA modes */
	25, 25, 25, 25, 43
};

short Maxcol[] = {
	/* CGA modes */
	40, 40, 80, 80, 40, 40, 80,
	/* MDA mode */
	80,
	/* PCjr modes */
	-1, 40, 80,
	/* not used */
	-1, -1,
	/* EGA modes */
	80, 80, 80, 80
};

short Maxpage[] = {
	/* CGA modes */
	8, 8, 4, 4, 1, 1, 1,
	/* MDA mode */
	1,
	/* PCjr modes */
	0, 0, 0,
	/* not used */
	-1, -1,
	/* EGA modes */
	8, 4, 1, 1
};

int
getstate()
{
	union REGS inregs, outregs;

	inregs.h.ah = GET_STATE;
	int86(VIDEO_IO, &inregs, &outregs);

	Vmode = outregs.h.al;
	Vwidth = outregs.h.ah;
	Vpage = outregs.h.bh;

	return (outregs.x.cflag);
}
