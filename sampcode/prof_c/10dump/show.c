/*
 *	show -- a filter that displays the contents of a file
 *	in a way that is guaranteed to be displayable
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <local\std.h>

main(argc, argv)
int argc;
char **argv;
{
	int ch;
	FILE *fp;
	BOOLEAN sflag = FALSE;	/* strip non-ASCII characters */
	BOOLEAN vflag = FALSE;	/* verbose mode */
	BOOLEAN wflag = FALSE;	/* filter typical word processing codes */
	BOOLEAN errflag = FALSE;
	static char pgm[] = { "show" };

	extern int getopt(int, char **, char *);
	extern char *optarg;
	extern int optind, opterr;
	extern int showit(FILE *, BOOLEAN, BOOLEAN);
	extern void fatal(char *, char *, int);

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	while ((ch = getopt(argc, argv, "svw")) != EOF) {
		switch (ch) {
		case 's': /* strip non-ASCII characters */
			sflag = TRUE;
			break;
		case 'v': /* verbose */
			vflag = TRUE;
			break;
		case 'w': /* use word-processing conventions */
			wflag = sflag = TRUE;
			break;
		case '?':
			errflag = TRUE;
			break;
		}
	}

	/* check for errors */
	if (errflag == TRUE) {
		fprintf(stderr, "Usage: %s [-sv] [file...]\n", pgm);
		exit(1);
	}

	/* if no file names, use standard input */
	if (optind == argc) {
		if (setmode(fileno(stdin), O_BINARY) == -1)
			fatal(pgm, "Cannot set binary mode on stdin", 2);
		showit(stdin, sflag, wflag);
		exit(0);
	}

	/* otherwise, process remainder of command line */
	for ( ; optind < argc; ++optind) {
		if ((fp = fopen(argv[optind], "rb")) == NULL) {
			fprintf(stderr,
				"%s: Error opening %s\n", pgm, argv[optind]);
			perror("");
			continue;
		}
		if (vflag == TRUE)
			fprintf(stdout, "\n%s:\n", argv[optind]);
		if (showit(fp, sflag, wflag) != 0) {
			fprintf(stderr,
				"%s: Error reading %s\n", pgm, argv[optind]);
			perror("");
		}
		if (fclose(fp) == EOF)
			fatal(pgm, "Error closing input file", 2);
	}
	
	exit(0);
}
