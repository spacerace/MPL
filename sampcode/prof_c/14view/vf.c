/*
 *	vf -- view a file using a full-screen window onto
 *	an in-memory text file buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include <local\video.h>
#include "vf.h"

extern int setctype(int, int);
int Startscan, Endscan;	/* cursor scan line range */
unsigned char Attr;	/* primary display attribute */
unsigned char Revattr;	/* reverse video for highlighting */
unsigned char Usrattr;	/* user's original attribute */

main(argc, argv)
int argc;
char **argv;
{
	int ch;
	unsigned char chr;
	BOOLEAN errflag;
	BOOLEAN numbers;
	int errcode;
	FILE *fp;
	extern char *optarg;
	extern int optind;
	static char pgm[MAXNAME + 1] = { "vf" };

	/* function prototypes */
	void clean();
	extern clrscrn(unsigned char);
	extern void getpname(char *, char *);
	extern void fixtabs(int);
	extern void initmsg(int, int, int, unsigned char, int);
	extern int getopt(int, char **, char *);
	extern int vf_cmd(FILE *, char *, BOOLEAN);
	extern int readca(unsigned char *, unsigned char *, unsigned int);
	extern int getctype(int *, int *, int);

	errcode = 0;
	fixtabs(TABWIDTH);

	/* get program name from DOS (version 3.00 and later) */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* be sure we have needed DOS support */
	if (_osmajor < 2) {
		fprintf(stderr, "%s requires DOS 2.00 or later\n", pgm);
		exit(1);
	}

	/* process optional arguments first */
	errflag = numbers = FALSE;
	while ((ch = getopt(argc, argv, "n")) != EOF)
		switch (ch) {
		case 'n':
			/* turn on line numbering */
			numbers = TRUE;
			break;
		case '?':
			/* bad option */
			errflag = TRUE;
			break;
		}

	/* check for command-line errors */
	argc -= optind;
	argv += optind;
	if (errflag == TRUE || argc == 0) {
		fprintf(stderr, "Usage: %s [-n] file...\n", pgm);
		exit(1);
	}

	/* get current video attribute and set VF attributes */
	getstate();
	readca(&chr, &Usrattr, Vpage);	/* save user's attribute settings */
	/*
	 * These colors are modified from those presented in the
	 * book so that VF will work on all monochrome and
	 * color systems the same way -- change them to suit
	 * your own shade of reality and operating conditions
	 */
	Attr = Usrattr;			/* basic text attributes */
	/* reverse video for highlighting */
	Revattr = ((Attr & 0x7) << 4) | (Attr >> 4);
	clrscrn(Attr);

	/* save user's cursor shape and turn cursor off */
	getctype(&Startscan, &Endscan, Vpage);
	setctype(MAXSCAN, MAXSCAN);

	/* set up the message line manager */
	initmsg(MSGROW, MSGCOL, Maxcol[Vmode] - MSGCOL, Attr, Vpage);

	/* display first screen page */
	putcur(0, 0, Vpage);
	putstr("ViewFile/1.0  H=Help Q=Quit Esc=Next", Vpage);
	putcur(1, 0, Vpage);
	writea(Revattr, Maxcol[Vmode], Vpage);

	for (; argc-- > 0; ++argv) {
		if ((fp = fopen(*argv, "r")) == NULL) {
			fprintf(stderr, "%s: cannot open %s -- ", pgm, *argv);
			perror("");
			++errcode;
			continue;
		}
		if (vf_cmd(fp, *argv, numbers) != 0)
			break;
		fclose(fp);
	}
	clean();
	exit(errcode);
}


/*
 *	clean -- restore the user's original conditions
 */

void
clean()
{
	/* set screen to user's attribute */
	clrscrn(Usrattr);
	putcur(0, 0, Vpage);

	/* restore user's cursor shape */
	setctype(Startscan, Endscan);
}
