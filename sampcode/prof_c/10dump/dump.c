/*
 *	dump -- display contents of non-ASCII files in hex byte and
 *	ASCII character forms (like the DOS debug dump option)
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <local\std.h>

#define STDINPUT	0
#define LINEWIDTH	80

main(argc,argv)
int argc;
char *argv[];
{
	int ch;
	BOOLEAN	sflag = FALSE,
		vflag = FALSE,
		errflag = FALSE;
	int fd;
	static char pgm[MAXNAME + 1] = { "dump" };

	extern int getopt(int, char **, char *);
	extern char *optarg;
	extern int optind, opterr;
	extern void getpname(char *, char *);
	extern int hexdump(int, BOOLEAN);
	extern void fatal(char *, char *, int);

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	while ((ch = getopt(argc, argv, "sv")) != EOF)
		switch (ch) {
		case 's': /* strip -- convert all non-ASCII to '.' */
			sflag = TRUE;
			break;
		case 'v': /* verbose -- tell user what's happening */
			vflag = TRUE;
			break;
		case '?': /* bad option */
			errflag = TRUE;
			break;
		}

	if (errflag == TRUE) {
		fprintf(stderr, "Usage: %s [-sv] [file...]\n", pgm);
		exit(1);
	}

	if (optind == argc) {
		if (setmode(STDINPUT, O_BINARY) == -1)
			fatal(pgm, "Cannot set binary mode", 2);
		hexdump(STDINPUT, sflag);
		exit(0);
	}

	for ( ; optind < argc; ++optind) {
		if ((fd = open(argv[optind], O_BINARY | O_RDONLY)) == -1) {
			fprintf(stderr,
				"%s: Error opening %s -- ", pgm, argv[optind]);
			perror("");
			continue;
		}
		if (vflag == TRUE)
			fprintf(stdout, "\n%s:\n", argv[optind]);
		if (hexdump(fd, sflag) == FAILURE) {
			fprintf(stderr,
				"%s: Error reading %s -- ", pgm, argv[optind]);
			perror("");
		}
		if (close(fd) == -1)
			fatal(pgm, "Error closing input file", 3);
	}

	exit(0);
}
