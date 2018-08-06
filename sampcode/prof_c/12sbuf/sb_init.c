/*
 *	sb_init -- initialize the buffered screen interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\sbuf.h>

/* global data declarations */
struct BUFFER Sbuf;			/* control information */
union CELL Scrnbuf[SB_ROWS][SB_COLS];	/* screen buffer array */

int
sb_init()
{
	int i;
	char *um;	/* update mode */

	/* set initial parameter values */
	Sbuf.bp = &Scrnbuf[0][0];
	Sbuf.row = Sbuf.col = 0;
	for (i = 0; i < SB_ROWS; ++i) {
		Sbuf.lcol[i] = SB_COLS;
		Sbuf.rcol[i] = 0;
	}
	Sbuf.flags = 0;

	/* set screen update mode */
	um = strupr(getenv("UPDATEMODE"));
	if (um == NULL || strcmp(um, "BIOS") == 0)
		Sbuf.flags &= ~SB_DIRECT;
	else if (strcmp(um, "DIRECT") == 0)
		Sbuf.flags |= SB_DIRECT;
	else
		return SB_ERR;

	return SB_OK;
}
