/*
 *	notes2 -- add a date/time stamped entry to a
 *	"notes" data file.  Allow user to optionally
 *	edit the data file upon completion of the entry.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <process.h>
#include <local\std.h>

/* length of date/time string */
#define DT_STR	26

main(argc, argv)
int argc;
char **argv;
{
	int n;		/* number of lines added */
	int exitcode = 0;
	FILE *fp;
	static char notesfile[MAXPATH + 1] = { "notes.txt" };
	static char editname[MAXPATH + 1] = { "edlin" };
	char ch;
	char dt_stamp[DT_STR];
	char *s;
	long ltime;
	static char pgm[MAXNAME + 1] = { "notes3" };

	extern void fatal(char *, char *, int);
	extern void getpname(char *, char *);
	static int addtxt(FILE *, FILE *);

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* locate the "notes" database file and open it */
	if (argc > 1)
		strcpy(notesfile, *++argv);
	else if (s = getenv("NOTESFILE"))
		strcpy(notesfile, s);
	if ((fp = fopen(notesfile, "a")) == NULL)
		fatal(pgm, notesfile, 1);

	/* disable Ctrl-Break interrupt */
	if (signal(SIGINT, SIG_IGN) == (int(*)())-1)
		perror("Cannot set signal");

	/* append a header and date/time tag */
	ltime = time(NULL);
	strcpy(dt_stamp, ctime(&ltime));
	fprintf(stderr, "Appending to %s: %s", notesfile, dt_stamp);
	fprintf(fp, "\n%s", dt_stamp);

	/* add text to notes file */
	if ((n = addtxt(stdin, fp)) == 0) {
		fputs("No new text", stderr);
		if (fclose(fp))
			fatal(pgm, notesfile, 2);
		exit(0);
	}
	else
		fprintf(stderr, "%d line(s) added to %s\n", n, notesfile);
	if (fclose(fp))
		fatal(pgm, notesfile, 2);

	/* optionally edit text in the notes file */
	fprintf(stderr, "E + ENTER to edit; ENTER alone to quit: ");
	while ((ch = tolower(getchar())) != '\n')
		if (ch = 'e') {
			if (s = getenv("EDITOR"))
				strcpy(editname, s);
			if ((exitcode = spawnlp(P_WAIT, editname, editname,
				notesfile, NULL)) == -1)
				fatal(pgm, editname, 3);
		}
	exit(exitcode);
}

/*
 *	addtxt -- append new text to notes file
 */
static int addtxt(fin, fout)
FILE *fin, *fout;
{
	int ch;
	int col = 0;	/* column */
	int n = 0;	/* number of lines added */

	while ((ch = fgetc(fin)) != EOF) {
		if (ch == '.' && col == 0) {
			ch = fgetc(fin); /* trash the newline */
			break;
		}
		fputc(ch, fout);
		if (ch == '\n') {
			col = 0;
			++n;
		}
		else
			++col;
	}
	return (n);
}
