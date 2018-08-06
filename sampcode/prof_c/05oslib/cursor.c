/*
 *	cursor -- interactively set cursor shape
 */

#include <dos.h>
#include <stdlib.h>
#include <local\video.h>
#include <local\keydefs.h>

/* additional drawing characters (others are defined in video.h) */
#define DOT	254
#define NO_DOT	196
#define D_POINT	31
#define R_POINT 16
#define L_POINT 17

/* dimensions of the help frame */
#define BOX_Y	6
#define BOX_X	30

/* upper-left row and column of big cursor */
int Ulr;
int Ulc;
int Mid;

/* cursor scan-line-selection modes */
typedef enum { STARTSCAN, ENDSCAN } CMODE;

int
main()
{
	int i, j;
	int ch;
	int start, end;
	int height, width;
	static char spoint[] = { "Start\020" }; /* contains right pointer */
	static char epoint[] = { "\021Stop" };	/* contains left pointer */
	static char title[] = { "CURSOR: Control cursor shape (V1.0)" };
	unsigned char
		oldattr,	/* video attribute upon entry */
		headattr,	/* video attribute of header */
		attr, 		/* primary video attribute */
		standout;	/* highlighting video attribute */
	CMODE mode;

	static void drawdspy(int, int, int, int, int);
	static void drawstart(int, char *);
	static void drawend(int, int, char *);
	static void drawactive(int, int, CMODE);
	static void showhelp(int, int);

	/* get video information and initialize */
	getstate();
	Mid = Vwidth / 2;
	readca(&ch, &oldattr, Vpage);	/* preserve user's video attribute */
	getctype(&start, &end, Vpage);	/* and cursor shape */
	headattr = (WHT << 4) | BLK;

	/* set parameters based on video mode (default = CGA) */
	height = width = 8;	/* use an 8 by 8 block character cell */
	attr = (BLU << 4) | CYAN | BRIGHT;
	standout = YEL;
	if (Vmode == MDA_M80) {
		/* uses a 14 by 9 dot block character cell */
		height = 14;
		width = 9;
		attr = NORMAL;
		standout = BWHT;
	}
	setctype(height + 1, height + 1);	/* cursor off */

	/* basic text and layout */
	Ulr = 2;
	Ulc = Mid - width / 2;
	clrscrn(attr);
	putcur(0, 0, Vpage);
	writeca(' ', headattr, Vwidth, Vpage);
	putcur(0, Mid - strlen(title) / 2, Vpage);
	writestr(title, Vpage);
	showhelp(Ulr + height + 1, Mid - BOX_X / 2);

	/* interactively select cursor shape */
	mode = STARTSCAN;
	drawdspy(start, end, standout, width, height);
	drawstart(start, spoint);
	drawend(end, width, epoint);
	drawactive(height, width, mode);
	while (1) {
		switch (ch = getkey()) {
		case K_UP:
			/* move up one scan line */
			if (mode == STARTSCAN)
				drawstart(start--, "      ");
			else
				drawend(end--, width, "     ");
			break;
		case K_DOWN:
			/* move down one scan line */
			if (mode == STARTSCAN)
				drawstart(start++, "      ");
			else
				drawend(end++, width, "     ");
			break;
		case K_LEFT:
			/* starting scan-line-selection mode */
			mode = STARTSCAN;
			drawactive(height, width, mode);
			continue;
		case K_RIGHT:
			/* ending scan-line-selection mode */
			mode = ENDSCAN;
			drawactive(height, width, mode);
			continue;
		case K_RETURN:
			/* set the new cursor shape */
			setctype(start, end);
			clrscrn(oldattr);
			putcur(0, 0, Vpage);
			exit(0);
		}

		/* make corrections at cursor image boundaries */
		if (start < 0)
			start = 0;
		else if (start > height)
			start = height;
		if (end < 0)
			end = 0;
		else if (end >= height)
			end = height - 1;

		/* show updated cursor shape and pointers */
		drawdspy(start, end, standout, width, height);
		drawstart(start, spoint);
		drawend(end, width, epoint);
	}

	exit(0);
} /* end main() */


/*
 *	drawdspy -- draw a magnified image of a cursor with the
 *	currently active scan lines depicted as a sequence of dots
 *	and inactive lines depicted as straight lines
 */

static void
drawdspy(s, e, a, w, h)
int s;	/* starting scan line */
int e;	/* ending scan line */
int a;	/* video attribute */
int w;	/* width */
int h;	/* height */
{
	int i;

	/* display an exploded image of each scan line */
	for (i = 0; i < h; ++i) {
		putcur(Ulr + i, Ulc, Vpage);
		if (s >= h)
			/* cursor is effectively off */
			writeca(NO_DOT, a, w, Vpage);
		else if ((s <= e && i >= s && i <= e) || /* a full block */
			(s > e && (i <= e || i >= s)))	 /* a split block */
			writeca(DOT, a, w, Vpage);
		else
			/* outside start/end range */
			writeca(NO_DOT, a, w, Vpage);
	}
} /* end drawdspy() */


/*
 *	drawstart -- display a pointer to the displayed starting
 *	scan line in the magnified cursor image
 */

static void
drawstart(s, sp)
int s;		/* starting scan line number */
char *sp;	/* visual pointer to the displayed starting scan line */
{
	putcur(Ulr + s, Ulc - strlen(sp), Vpage);
	putstr(sp, Vpage);
} /* end drawstart() */


/*
 *	drawend -- display a pointer to the displayed ending
 *	scan line in the magnified cursor image
 */

static void
drawend(e, w, ep)
int e;		/* ending scan line number */
int w;		/* width of the cursor image */
char *ep;	/* visual pointer to the displayed ending scan line */
{
	putcur(Ulr + e, Ulc + w, Vpage);
	putstr(ep, Vpage);
} /* end drawend() */

static void
drawactive(h, w, m)
int h, w;
CMODE m;
{
	int col;

	/* clear active selector row */
	putcur(Ulr - 1, Ulc, Vpage);
	writec(' ', w, Vpage);

	/* point to active selector */
	col = (m == STARTSCAN) ? 0 : w - 1;
	putcur(Ulr - 1, Ulc + col, Vpage);
	writec(D_POINT, 1, Vpage);
} /* end drawactive() */

/*
 *	showhelp -- display a set of instructions about the
 *	use of the cursor program in a fine-ruled box
 */

static void
showhelp(r, c)
int r, c;	/* upper-left corner of help frame */
{
	static char title[] = { " Instructions " };
	extern int drawbox(int, int, int, int, int);

	/* fine-ruled box */
	clrw(r, c, r + BOX_Y, c + BOX_X, (WHT << 4) | GRN | BRIGHT);
	drawbox(r, c, r + BOX_Y, c + BOX_X, Vpage);

	/* centered title */
	putcur(r, c + (BOX_X - strlen(title)) / 2, Vpage);
	putstr(title, Vpage);

	/* display symbols and text using brute-force positioning */
	putcur(r + 2, c + 2, Vpage);
	put_ch(LEFTARROW, Vpage);
	put_ch(RIGHTARROW, Vpage);
	putstr("  Change selection mode", Vpage);
	putcur(r + 3, c + 2, Vpage);
	put_ch(UPARROW, Vpage);
	put_ch(DOWNARROW, Vpage);
	putstr("  Select scan lines", Vpage);
	putcur(r + 4, c + 2, Vpage);
	put_ch(L_POINT, Vpage);
	put_ch(LRC11, Vpage);
	putstr("  Set shape and exit", Vpage);
} /* end showhelp() */
