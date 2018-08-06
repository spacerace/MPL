/*
	HEADER:		CUG236;
	TITLE:		List Head of Text Files;
	DATE:		04/04/1987;
	DESCRIPTION:	"Lists the first n lines of each of a list of files.  n
			is variable as is the tab increment of the listing.";
	VERSION:	2.0;
	FILENAME:	HEAD.C;
	COMPILERS:	vanilla;
	AUTHORS:	W. C. Colley III, E. H. Mallory;
*/

/*
	Utility to Print the First n Lines of a List of Files

	Based upon Eugene H. Mallory's HEAD.C (copyright 1983)

	Rewrite by William C. Colley, III -- 4 APR 1987

The purpose of this rewrite is to get a portable version of HEAD.C as the
original was coded in BDS C.  This involves some loss of functionality as the
BDS C function wildexp() is irrevokably CP/M dependent.	 The differences are:

	1)  Wild-carded file names cannot be expanded unless the host operating
	    system provides this facility as part of command-line argument
	    passing.  UNIX does, MSDOS and CP/M do not.

	2)  I punted the sorting of the argument list.	This allows the -
	    options to apply to particular files.  Thus, if you want to see the
	    first 10 lines of FOO.C and the first 20 lines of BAR.C, you can
	    do it with the following command:

		HEAD -L10 FOO.C -L20 BAR.C

	3)  The -H option has been dropped to simplify things.	If you type
	    the command HEAD with no arguments, you will still get the usage
	    reminder.

	4)  Only the first WIDTH characters of a line are displayed.  The rest
	    are sent to the bit bucket.	 The usual procedure would be to set
	    WIDTH to the width of your terminal or printer.

	5)  Controllable tab expansion size has been added with the -T option.
	    Thus, you can expand FOO.C's tabs on 4-space intervals with the
	    following command:

		HEAD -T4 FOO.C

	6)  Bit 7 is stripped on all displayed characters so that WordStar
	    images and the like don't generate funny graphics junk on some
	    terminals.
*/

#include <stdio.h>

/*  Uncomment the following #define if you are using an AZTEC C compiler.  */

/*
#define getc(x)		agetc(x)
*/

/*  User-adjustable default settings:					*/

#define WIDTH		79	/* Max length of line to display.	*/
#define HEADLEN		10	/* Default length of head to display.	*/
#define TABSIZE		8	/* Default tab size.			*/

int main(argc,argv)
int argc;
char **argv;
{
    char *bp, buf[WIDTH + 1];
    int i, j;
    FILE *f;
    static char opts[] = "[ -Lnn | -Tnn | <filename> ]";
    static int err = 0, headlen = HEADLEN, tabsize = TABSIZE;
    int atoi();

    if (argc < 2) {
	printf("Usage:\tHEAD %s %s*\n",opts,opts);
	err = !0;
    }
    else while (--argc) {
	if (**++argv == '-') {
	    i = *(*argv + 1);
	    switch (i) {
		case 'l':
		case 'L':   if ((i = atoi(*argv + 2)) < 1) {
				printf("ERROR:  Head length < 1\n\n");
				err = !0;
			    }
			    else headlen = i;
			    break;

		case 't':
		case 'T':   if ((i = atoi(*argv + 2)) < 1) {
				printf("ERROR:  Tab size < 1\n\n");
				err = !0;
			    }
			    else tabsize = i;
			    break;

		default:    printf("ERROR:  Illegal option %s\n\n",*argv);
			    err = !0;  break;
	    }
	}
	else {
	    if (!(f = fopen(*argv,"r"))) {
		printf("ERROR:  Cannot find file %s\n\n",*argv);
		err = !0;
	    }
	    else {
		printf("FILE:   %s\n\n",*argv);
		for (i = headlen; i && !feof(f); --i) {
		    for (bp = buf; ; ) {
			if ((j = getc(f)) == EOF || (j &= 0177) == '\n') break;
			if (j == '\t')
			    while (bp < buf + WIDTH) {
				*bp++ = ' ';
				if (!((bp - buf) % tabsize)) break;
			    }
			else if (bp < buf + WIDTH) *bp++ = j;
		    }
		    *bp = '\0';	 printf("%s\n",buf);
		}
		printf("\n");  fclose(f);
	    }
	}
    }
    return err;
}
