/*
 *	st -- screen test using cpblk function
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <local\std.h>
#include <local\video.h>

#define CG_SEG		0xB800
#define MONO_SEG	0xB000
#define NBYTES		0x1000
#define PAGESIZ		(NBYTES / 2)
#define PG0_OS		0
#define PG1_OS		PG0_OS + NBYTES
#define ESC		27
#define MAXSCAN		14

main(argc, argv)
int argc;
char *argv[];
{
	int i;
	int k;				/* user command character */
	int ca;				/* character/attribute word */
	int ch;				/* character value read */
	int row, col;			/* cursor position upon entry */
	int c_start, c_end;		/* cursor scan lines */
	unsigned char attr;		/* saved video attribute */
	unsigned dseg;			/* destination buffer segment */
	unsigned os;			/* page offset in bytes */
	static unsigned sbuf[NBYTES];	/* screen buffer */
	unsigned *bp;			/* screen element pointer */
	unsigned sseg;			/* source segment */
	int special;			/* use special copy routine */
	int apg, vpg;			/* active and visual display pages */

	/* segment register values */
	struct SREGS segregs;

	extern void swap_int(int *, int *);

	static char pgm[] = { "st" };	/* program name */

	/* save user's current video state */
	getstate();
	readcur(&row, &col, Vpage);
	putcur(row - 1, 0, Vpage);
	readca(&ch, &attr, Vpage);
	getctype(&c_start, &c_end, Vpage);
	setctype(MAXSCAN, c_end);
	clrscrn(attr);
	putcur(1, 1, Vpage);

	/* initialize destination segment */
	special = 1;
	fputs("ScreenTest (ST): ", stderr);
	if (Vmode == CGA_C80 || Vmode == CGA_M80) {
		dseg = CG_SEG;
		fprintf(stderr, "Using CGA mode %d", Vmode);
	}
	else if (Vmode == MDA_M80) {
		dseg = MONO_SEG;
		fprintf(stderr, "Using MDA (mode %d)", Vmode);
		special = 0;
	} else
		fprintf(stderr, "%s: Requires 80-column text mode\n", pgm);

	/* process command-line arguments */
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [x]\n", pgm);
		exit(2);
	}
	else if (argc == 2)
		special = 0;	/* bypass special block move */

	/* get data segment value */
	segread(&segregs);
	sseg = segregs.ds;

	/* set up "active" and "visual" display pages */
	apg = 1;	/* page being written to */
	vpg = 0;	/* page being viewed */

	/* display buffers on the user's command */
	fprintf(stderr, " -- Type printable text; Esc=exit");
	while ((k = getkey()) != ESC) {
		if (isascii(k) && isprint(k)) {
			/* fill the buffer */
			ca = ((k % 0xEF) << 8) | k;
			for (bp = sbuf; bp - sbuf < PAGESIZ; ++bp)
				*bp = ca;
			if (Vmode == MDA_M80)
				os = 0;
			else
				os = (apg == 0) ? PG0_OS : PG1_OS;
			if (special)
				cpblk(sbuf, sseg, os, dseg);
			else
				movedata(sseg, sbuf, dseg, os, NBYTES);
			if (Vmode != MDA_M80) {
				swap_int(&apg, &vpg);
				setpage(vpg);
			}
		}
		else {
			clrscrn(attr);
			putcur(0, 0, Vpage);
			writestr(" Type printable text; Esc = exit ", vpg);
		}
	}

	/* restore user's video conditions and return to DOS */
	setpage(Vpage);
	clrscrn(attr);
	putcur(0, 0, Vpage);
	setctype(c_start, c_end);
	exit(0);
}
