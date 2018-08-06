/*
 *	prtstr -- send text string(s) to standard printer
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

main(argc, argv)
int argc;
char **argv;
{
	int ch;
	BOOLEAN errflag, lineflag;
	static char pgm[MAXNAME + 1] = { "prtstr" };
	FILE *fout;

	extern char *getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* process options, if any */
	errflag = FALSE;
	lineflag = TRUE;
	fout = stdprn;
	while ((ch = getopt(argc, argv, "np")) != EOF)
		switch (ch) {
		case 'n':
			/* don't emit the trailing newline */
			lineflag = FALSE;
			break;
		case 'p':
			/* preview on stdout */
			fout = stdout;
			break;
		case '?':
			/* bad option */
			errflag = TRUE;
			break;
		}
	if (errflag == TRUE) {
		fprintf(stderr, "Usage: %s [-np] [string...]\n");
		exit(1);
	}

	/* print the string(s) */
	argc -= optind;
	argv += optind;
	while (argc-- > 1 ) {
		fputs(*argv++, fout);
		fputc(' ', fout);
	}
	fputs(*argv++, fout);
	if (lineflag == TRUE)
		fputc(' ', fout);
	if (lineflag == TRUE)
		fputc('\n', fout);
	
	exit(0);
}
