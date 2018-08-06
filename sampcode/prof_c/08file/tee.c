/*
 *	tee -- a "pipe fitter" for DOS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>

main(argc, argv)
int argc;
char *argv[];
{
	register int ch, n;
	static char openmode[] = { "w" };
	static char pgm[MAXPATH + 1] = { "tee" };
	FILE *fp[_NFILE];	/* array of file pointers */

	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;
	extern void getpname(char *, char *);

	/* check for an alias */
	if (_osmajor >= 3)
		getpname(argv[0], pgm);

	/* process command-line options, if any */
	while ((ch = getopt(argc, argv, "a")) != EOF)
		switch (ch) {
		case 'a':
			strcpy(openmode, "a");
			break;
		case '?':
			break;
		}
	n = argc -= optind;
	argv += optind;

	/* check for errors */
	if (argc > _NFILE) {
		fprintf(stderr, "Too many files (max = %d)\n", _NFILE);
		exit(1);
	}

	/* open the output file(s) */
	for (n = 0; n < argc; ++n) {
		if ((fp[n] = fopen(argv[n], openmode)) == NULL) {
			fprintf(stderr, "Cannot open %s\n", argv[n]);
			continue;
		}
	}

	/* copy input to stdout plus opened file(s) */
	while ((ch = getchar()) != EOF) {
		putchar(ch);
		for (n = 0; n < argc; ++n)
			if (fp[n] != NULL)
				fputc(ch, fp[n]);
	}

	/* close file(s) */
	if (fcloseall() == -1) {
		fprintf(stderr, "Error closing a file\n");
		exit(2);
	}

	exit(0);
}
