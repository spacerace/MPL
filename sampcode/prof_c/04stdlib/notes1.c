/*
 *	notes1 -- add an entry to a "notes" text file
 *
 *	version 1: appends new data to NOTES.TXT in the
 *	current directory -- uses local date/time stamp
 *	as a header for each new entry
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <local\std.h>

main()
{
	FILE *fp;
	static char notesfile[MAXPATH + 1] = { "notes.txt" };
	char ch;
	long ltime;
	static char pgm[MAXNAME + 1] = { "notes1" };

	extern void fatal(char *, char *, int);

	/* try to open notes file in current directory */
	if ((fp = fopen(notesfile, "a")) == NULL)
		fatal(pgm, notesfile, 1);

	/* append a header and date/time tag */
	ltime = time(NULL);
	fprintf(stderr, "Appending to %s: %s",
		 notesfile, ctime(&ltime));
	fprintf(fp, "%s", ctime(&ltime));

	/* append new text */
	while ((ch = getchar()) != EOF)
		putc(ch, fp);

	/* clean up */
	if (fclose(fp))
		fatal(pgm, notesfile, 2);
	exit(0);
}
