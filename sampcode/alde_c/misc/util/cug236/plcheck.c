/*  LCHECK by Richard Conn

LCHECK displays to the user the nesting level number of each BEGIN/END ({/})
group, thereby helping him to identify problem areas in his C programs.	 It
recognizes quoted material and comments and ignores { and } within these.

08/01/84 - Fixed bug that caused program to treat an \' or \" as a "live"
	   quote.  (Therefore, a statement such as "#define QUOTE '\''"
	   would screw up the level counter in the program.  Changed version
	   number to 1.2A.  (Don McCrady)

05/16/87 - Conversion from BDS C to portable C.	 (William C. Colley, III)
*/

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)		agetc(f)
#define putc(c,f)	aputc(c,f)
*/

#define VERS	"2.0"  /* Version Number */

#include <stdio.h>

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	(!0)
#endif

#ifndef EOF
#define EOF	-1
#endif

#define TWIDTH	80	/* Terminal width = 80 chars */
#define YES	'Y'
#define NO	'N'
#define ovfl	YES	/* Line Overflow */
#define noovfl	NO	/* No Line Overflow */

FILE *iobuf;
int level = 0, chval, pos, nroutines = 0;

int main(argc,argv)
int argc;
char **argv;
{
    int done;
    void getit(), prlevel();

    if (argc == 1) {
	printf("LCHECK, Version %s\n",VERS);
	printf("Format of Command Line is --\n");
	printf("  LCHECK filename.typ");
	return TRUE;
	}
    if (!(iobuf = fopen(argv[1],"r"))) {
	printf("Cannot Find File %s\n",argv[1]);
	return TRUE;
	}
    printf("LCHECK, Version %s -- File:  %s\n",VERS,argv[1]);
    prlevel();	/* Print level number */
    do {
	getit();    /* Get next char */
	if (chval == '\'') do { /* If single quote, flush to end quote */
		getit();
		if (chval == '\\') {	    /* check for \'  */
		    getit();
		    if (chval == '\'')	    /* Ignore it and */
			getit();	    /* get next char */
		}
	    } while (chval != '\'');
	if (chval == '"') do {	/* If double quote, flush to end quote */
		getit();
		if (chval == '\\') {	    /* check for \" */
		    getit();
		    if (chval == '"')	/* Ignore it and */
			getit();	    /* get next char */
		}
	    } while (chval != '"');
	if (chval == '/') {	    /* Possible comment */
	    getit();
	    if (chval == '*') {	    /* Yes, it is a comment */
		getit();
		done = FALSE;
		do {
		    if (chval == '*') {	    /* End comment? */
			getit();
			if (chval == '/')   /* Yes */
			    done = TRUE;
			}
		    else getit();
		} while (!done);
		}
	    }
	if (chval == '{') level++;  /* BEGIN */
	if (chval == '}') {	    /* END */
	    level--;
	    if (level == 0) {
		nroutines++;
		printf("\n** Routine %d **", nroutines);
		}
	    }
    } while (chval != EOF);

    printf("\nProgram Level Check is ");
    if (level == 0) printf("OK");
    else printf("NOT OK");
    printf("\nNumber of Routines Encountered: %d",--nroutines);
    fclose(iobuf);
    return level != 0;
}

void getit()  /* Get and Echo Character */
{
    void echo(), prlevel();

    chval = getc(iobuf);
    if ((pos >= TWIDTH) & (chval != '\n')) prlevel(ovfl);
    if (chval != EOF) echo(chval);
}

void echo(chval)  /* Echo Char with tabulation */
char chval;
{
    void prlevel();

    switch (chval) {
    case '\t' : putchar(' '); pos++;
		while (pos%9 != 0) {
		    putchar(' ');
		    pos++;
		    }
		break;
    case '\b' : putchar('\b');
		pos--;
		break;
    case '\n' : prlevel(noovfl);
		break;
    default  :	if (chval >= ' ') {
		putchar(chval);
		pos++;
	}
	break;
    }
}

void prlevel(ovfl_flag)	 /* Print Level Number and Set Col Count */
char ovfl_flag;
{
    putchar('\n');
    if (level < 10) printf(" %d",level);
    else printf("%d",level);
    if (ovfl_flag == YES) putchar('-');
    else putchar(':');
    putchar(' ');  pos = 5;
}
