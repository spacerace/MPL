/*
 *	cpblk -- copy a block of characters and attributes
 *	while eliminating "snow" on a standard CGA display
 */

#include <conio.h>
#include <memory.h>

#define BLKCNT	10
#define VSTAT	0x3DA
#define VRBIT	8
#define WRDCNT	200
#define NBYTES	(2 * WRDCNT)

/* macro to synchronize with vertical retrace period */
#define VSYNC	while ((inp(VSTAT) & VRBIT) == VRBIT); \
		while ((inp(VSTAT) & VRBIT) != VRBIT)

int
cpblk(src_os, src_seg, dest_os, dest_seg)
unsigned int src_os, src_seg, dest_os, dest_seg;
{
	register int i;
	int n;
	register int delta;

	n = 0;
	delta = 0;
	for (i = 0; i < BLKCNT ; ++i) {
		/* copy a block of words during vertical retrace */
		VSYNC;
		movedata(src_seg, src_os + delta,
			dest_seg, dest_os + delta, NBYTES);
		n += WRDCNT;

		/* adjust buffer offset */
		delta += NBYTES;
	}

	return (n);
}
