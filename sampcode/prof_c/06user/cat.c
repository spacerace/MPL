/*
 *	cat -- concatenate files
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

main(argc, argv)
int argc;
char **argv;
{
	int ch;
	char *cp;
	FILE *fp;
	BOOLEAN errflag, silent;
	static char pgm[MAXNAME + 1] = { "cat" };

	extern void getpname(char *, char *);
	extern int fcopy(FILE *, FILE *);
	extern int getopt(int, char **, char *);
	extern int optind;
	extern char *optarg;

	/* use an alias if one is given to this program */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* process optional arguments, if any */
	errflag = FALSE;
	silent = FALSE;
	while ((ch = getopt(argc, argv, "s")) != EOF)
		switch (ch) {
		case 's':
			/* don't complain about non-existent files */
			silent = TRUE;
			break;
		case '?':
			/* say what? */
			errflag = TRUE;
			break;
		}
	if (errflag == TRUE) {
		fprintf(stderr, "Usage: %s [-s] file...\n", pgm);
		exit(1);
	}

	/* process any remaining arguments */
	argc -= optind;
	argv += optind;
	if (argc == 0)
		/* no file names -- use standard input */
		if (fcopy(stdin, stdout) != 0) {
			fprintf(stderr, "error copying stdin");
			exit(2);
		}
		else
			exit(0);

	/* copy the contents of each named file to standard output */
	for (; argc-- > 0; ++argv) {
		if ((fp = fopen(*argv, "r")) == NULL) {
			if (silent == FALSE)
				fprintf(stderr, "%s: can't open %s\n",
					pgm, *argv);
			continue;
		}
		if (fcopy(fp, stdout) != 0) {
			fprintf(stderr, "%s: Error while copying %s",
				pgm, *argv);
			exit(3);
		}
		if (fclose(fp) != 0) {
			fprintf(stderr, "%s: Error closing %s",
				pgm, *argv);
			exit(4);
		}
	}

	exit(0);
}
