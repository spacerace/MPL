/*
 *	sb_show -- copy the screen buffer to display memory
 */

#include <stdio.h>
#include <dos.h>
#include <memory.h>
#include <local\sbuf.h>
#include <local\video.h>

#define	MDA_SEG	0xB000
#define	CGA_SEG	0xB800
#define NBYTES	(2 * SB_COLS)

/* macro to synchronize with vertical retrace period */
#define VSTAT	0x3DA
#define VRBIT	8
#define VSYNC	while ((inp(VSTAT) & VRBIT) == VRBIT); \
		while ((inp(VSTAT) & VRBIT) != VRBIT)

extern struct BUFFER Sbuf;
extern union CELL Scrnbuf[SB_ROWS][SB_COLS];

int
sb_show(pg)
short pg;
{
	register short r, c;
	short n;
	short count, ncols;
	unsigned int src_os, dest_os;
	struct SREGS segregs;

	if ((Sbuf.flags & SB_DIRECT) == SB_DIRECT) {
		/* use the direct-screen interface */
		segread(&segregs);

		/* determine extent of changes */
		n = 0;
		for (r = 0; r < SB_ROWS; ++r)
			if (Sbuf.lcol[r] <= Sbuf.rcol[r])
				++n;
		src_os = dest_os = 0;
		if (n <= 2)
			/* copy only rows that contain changes */
			for (r = 0; r < SB_ROWS; ++r) {
				if (Sbuf.lcol[r] <= Sbuf.rcol[r]) {
					/* copy blocks during vertical retrace */
					VSYNC;
					movedata(segregs.ds,
						(unsigned)&Scrnbuf[0][0] + src_os,
						CGA_SEG, dest_os, NBYTES);
					Sbuf.lcol[r] = SB_COLS;
					Sbuf.rcol[r] = 0;
				}
				src_os += SB_COLS;
				dest_os += NBYTES;
			}
		else {
			/* copy the entire buffer */
			count = 3 * NBYTES;
			ncols = 3 * SB_COLS;
			for (r = 0; r < SB_ROWS - 1; r += 3) {
				VSYNC;
				movedata(segregs.ds, (unsigned)&Scrnbuf[0][0]
					+ src_os, CGA_SEG, dest_os, count);
				src_os += ncols;
				dest_os += count;
			}
			VSYNC;
			movedata(segregs.ds, (unsigned)&Scrnbuf[0][0] + src_os,
				CGA_SEG, dest_os, NBYTES);
			for (r = 0; r < SB_ROWS; ++r) {
				Sbuf.lcol[r] = SB_COLS;
				Sbuf.rcol[r] = 0;
			}
		}
	}
	else
		/* use the BIOS video interface */
		for (r = 0; r < SB_ROWS; ++r)
			/* copy only changed portions of lines */
			if (Sbuf.lcol[r] < SB_COLS && Sbuf.rcol[r] > 0) {
				for (c = Sbuf.lcol[r]; c <= Sbuf.rcol[r]; ++c) {
					putcur(r, c, pg);
					writeca(Scrnbuf[r][c].b.ch,
						Scrnbuf[r][c].b.attr, 1, pg);
				}
				Sbuf.lcol[r] = SB_COLS;
				Sbuf.rcol[r] = 0;
			}

	/* the display now matches the buffer -- clear flag bit */
	Sbuf.flags &= ~SB_DELTA;

	return SB_OK;
}
