/*
 *	mx -- control Epson MX-series printer
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>
#include <local\printer.h>

extern PRINTER prt;	/* printer data */

main(argc, argv)
int argc;
char **argv;
{
	int ch, font;
	BOOLEAN errflag;	/* option error */
	BOOLEAN clrflag;	/* clear special fonts */
	BOOLEAN rflag;		/* hardware reset */
	BOOLEAN tflag;		/* top-of-form */
	FILE *fout;
	static char pgm[MAXNAME + 1] = { "mx" };

	extern void fatal(char *, char *, int);
	extern char *getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern char *optarg;
	extern int optind, opterr;
	extern int setprnt();
	extern int clrprnt(FILE *);
	extern int setfont(int, FILE *);

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	if (setprnt() == -1) {
		fprintf(stderr, "%s: Bad printer configuration\n", pgm);
		exit(1);
	}

	/* interpret command line */
	errflag = clrflag = rflag = tflag = FALSE;
	font = 0;
	fout = stdprn;
	while ((ch = getopt(argc, argv, "bcdefino:prtu")) != EOF) {
		switch (ch) {
		case 'b':
			/* set bold */
			font |= EMPHASIZED;
			break;
		case 'c':
			/* set compressed */
			font |= CONDENSED;
			break;
		case 'd':
			/* set double strike */
			font |= DOUBLE;
			break;
		case 'e':
			/* set double strike */
			font |= EXPANDED;
			break;
		case 'i':
			/* set italic */
			font |= ITALICS;
			break;
		case 'n':
			/* set normal (clear all special fonts) */
			clrflag = TRUE;
			break;
		case 'o':
			/* use specified output stream */
			if ((fout = fopen(optarg, "w")) == NULL)
				fatal(pgm, "cannot open output stream", 1);
			break;
		case 'p':
			/* preview control strings on stdout */
			fout = stdout;
			break;
		case 'r':
			/* hardware reset */
			rflag = TRUE;
			break;
		case 't':
			/* top of form */
			tflag = TRUE;
			break;
		case 'u':
			/* set underline */
			font |= UNDERLINE;
			break;
		case '?':
			/* unknown option */
			errflag = TRUE;
			break;
		}
	}

	/* report errors, if any */
	if (errflag == TRUE || argc == 1) {
		fprintf(stderr, "Usage: %s -option\n", pgm);
		fprintf(stderr,
			"b=bold, c=compressed, d=double strike, e=expanded\n");
		fprintf(stderr,
			"i=italic, n=normal, o file=output to file\n");
		fprintf(stderr,
			"p=preview, r=reset, t=top-of-form, u=underline\n");
		exit(2);
	}

	/* do hardware reset and formfeed first */
	if (rflag == TRUE)
		fputs(prt.p_init, fout);
	else if (tflag == TRUE)
		fputc('\f', fout);

	/* clear or set the aggregate font */
	if (clrflag == TRUE)
		clrprnt(fout);
	else if (setfont(font, fout) == FAILURE) {
		fprintf(stderr, "%s: Bad font spec\n", pgm);
		exit(3);
	}

	exit(0);
}
