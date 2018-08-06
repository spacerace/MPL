/*
 *	sb_box -- draw a box around the perimeter of a window
 *	using the appropriate IBM graphics characters
 */

#include <local\sbuf.h>
#include <local\video.h>
#include <local\box.h>

int
sb_box(win, type, attr)
struct REGION *win;
short type;
unsigned char attr;
{
	register short r;	/* row index */
	short x;		/* interior horizontal line length */
	short maxr, maxc;	/* maximum row and col values */
	BOXTYPE *boxp;		/* pointer to box drawing character struct */
	static BOXTYPE box[] = {
		'+',   '+',   '+',   '+',   '-',   '-',   '|',   '|',
		ULC11, URC11, LLC11, LRC11, HBAR1, HBAR1, VBAR1, VBAR1,
		ULC22, URC22, LLC22, LRC22, HBAR2, HBAR2, VBAR2, VBAR2,
		ULC12, URC12, LLC12, LRC12, HBAR1, HBAR1, VBAR2, VBAR2,
		ULC21, URC21, LLC21, LRC21, HBAR2, HBAR2, VBAR1, VBAR1,
		BLOCK, BLOCK, BLOCK, BLOCK, HBART, HBARB, BLOCK, BLOCK
	};

	boxp = &box[type];
	maxc = win->c1 - win->c0;
	maxr = win->r1 - win->r0;
	x = maxc - 1;

	/* draw top row */
	sb_move(win, 0, 0);
	sb_wca(win, boxp->ul, attr, 1);
	sb_move(win, 0, 1);
	sb_wca(win, boxp->tbar, attr, x);
	sb_move(win, 0, maxc);
	sb_wca(win, boxp->ur, attr, 1);

	/* draw left and right sides */
	for (r = 1; r < maxr; ++r) {
		sb_move(win, r, 0);
		sb_wca(win, boxp->lbar, attr, 1);
		sb_move(win, r, maxc);
		sb_wca(win, boxp->rbar, attr, 1);
	}

	/* draw bottom row */
	sb_move(win, maxr, 0);
	sb_wca(win, boxp->ll, attr, 1);
	sb_move(win, maxr, 1);
	sb_wca(win, boxp->bbar, attr, x);
	sb_move(win, maxr, maxc);
	sb_wca(win, boxp->lr, attr, 1);
	
	return SB_OK;
}
