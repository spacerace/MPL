
/*

Figure 8
========

*/

/*
 * usage()
 *
 * DESCRIPTION: Display an informative usage message using the
 *  actual program name, which may have been changed by the user.
 *
 * ENTRY: A pointer to the program name.
 */

#include <stdio.h>

void
usage(pname)
char *pname;
{
    fprintf(stderr, "Usage: %s [-c] [-v] file ...\n", pname);
    fprintf(stderr, "\t-c  Do not create any files\n");
    fprintf(stderr, "\t-v  Verbose mode -- report activities\n");
}
