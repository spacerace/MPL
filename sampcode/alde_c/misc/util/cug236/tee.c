/*
	HEADER:		CUG236;
	TITLE:		Portable TEE;
	DATE:		05/23/87;
	DESCRIPTION:	"Copies stdin to the specified file without changing
			stdout.";
	KEYWORDS:	Software tools, Text filters,tee, pipes;
	FILENAME:	TEE.C;
	WARNINGS:	"The author claims copyrights and authorizes
			non-commercial use only.";
	AUTHORS:	Michael M. Yokoyama;
	COMPILERS:	vanilla;
*/

#include <stdio.h>

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * #define if you are running AZTEC C:
 */

/*
#define putc(c,f)	aputc(c,f)
*/

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int main(argc, argv)
int argc;
char *argv[];
{
    int cin, correct;
    FILE *fout;

    correct = FALSE;
    if (argc == 2) {
	if ((fout = fopen(argv[1], "w")) == NULL) {
	    fprintf(stderr,"TEE:  cannot create %s\n", argv[1]);
	    return !0;
	}
	correct = TRUE;
    }
    else fprintf(stderr,"TEE Usage:  tee out_file\n");

    while ((cin = getchar()) != EOF) {
	putchar(cin);
	if (correct) putc(cin, fout);
    }
    if (correct) fclose(fout);
    return !correct;
}
