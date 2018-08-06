/*
 *	pr_file -- process each filename or standard input
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include "print.h"

int
pr_file(pname, ac, av)
char *pname;
int ac;
char **av;
{
	int ch, errcount = 0;
	FILE *fin, *fout;
	extern PRINT pcnf;

	extern void fatal(char *, char *, int);
	extern int pr_cpy(FILE *, FILE *, char *);

	/* open output stream only if not already open */
	if (*pcnf.p_dest == '\0' || strcmp(pcnf.p_dest, "CON") == 0)
		fout = stdout;
	else if (strcmp(pcnf.p_dest, "PRN") == 0)
		fout = stdprn;
	else if (strcmp(pcnf.p_dest, "AUX") == 0)
		fout = stdaux;
	else
		if ((fout = fopen(pcnf.p_dest, "w")) == NULL)
			fatal(pname, "Error open destination stream", 1);

	/* prepare input stream */
	if (ac == 0)
		pr_cpy(stdin, fout, "");
	else {
		for (; ac > 0; --ac, ++av) {
			if ((fin = fopen(*av, "r")) == NULL) {
				fprintf(stderr, "%s: Error opening %s",
					pname, *av);
				continue;
			}
			if (pr_cpy(fin, fout, *av) == -1) {
				fprintf(stderr, "%s: Cannot stat %s",
					pname, *av);
				continue;
			}
			if (fclose(fin) == EOF)
				fatal(pname, "Error closing input file", 2);
		}
	}

	return (errcount);
}
