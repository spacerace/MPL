/*
	HEADER:		CUG236;
	TITLE:		Multi-Column Output Stacker;
	DATE:		05/23/87;
	DESCRIPTION:	"Formats text into one or more columns.  Has several
			command line parameters that can be set.";
	KEYWORDS:	Software tools, Text filters, Text formatters;
	FILENAME:	2UP.C;
	WARNINGS:	"The author claims copyrights and authorizes
			non-commercial use only.";
	AUTHORS:	Eugene H. Mallory, William C. Colley III;
	COMPILERS:	vanilla;
*/

/*********************************************************************
*				2UP				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*								     *
* Hacked from BDS C to portable C 23 MAY 1987 by W. C. Colley, III.  *
* This involved removing the -w option as it relies upon a non-	     *
* portable direct console I/O call.  In the process, I also reworked *
* the thing into standard C puncutation and optimized various	     *
* sections of code so that even a stupid compiler can make a good    *
* executable from it.						     *
*								     *
*********************************************************************/

#include <stdio.h>

#define MAXW		132

/*  Portability Note:  8-bit systems often don't have header file
    ctype.h.  If your system doesn't have it, uncomment the #define
    NO_CTYPE_H.								*/

/*
#define	 NO_CTYPE_H
*/

/*  Portability Note:  Some older compilers don't know the new data
    type void.	If yours is one of these compilers, uncomment the
    following #define:							*/

/*
#define void		int
*/

#ifdef	 NO_CTYPE_H
int isdigit();
#else
#include <ctype.h>
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char string[MAXW + 2];
char array[90][MAXW + 2];
int ncol = 0, col = 0, len = 0, margin = 0, space = 0, width;
int bm = 0, doneflag = FALSE, tm = 0;
void exit();

int main(argc,argv)
int argc;
char **argv;
{
    int i, j, k, atoi();
    void error(), strmov();

/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/

    char c, *ss;
    int ii, jj, optionerr;

    optionerr = 0;
    for (ii = argc - 1; ii > 0; --ii)
	if (argv[ii][0] == '-') {
	    for (ss = &argv[ii][1]; *ss != '\0';) {
		c = *ss++;
		switch (toupper(c)) {
		    case 'L':	len = atoi(ss);	 break;

		    case 'C':	col= atoi(ss);	break;

		    case 'M':	margin = atoi(ss);  break;

		    case 'T':	tm = atoi(ss);	break;

		    case 'B':	bm = atoi(ss);	break;

		    case 'S':	space = atoi(ss);  break;

		    case 'N':	ncol= atoi(ss);	 break;

		    case 'H':	optionerr = TRUE;  break;

		    default:	fprintf(stderr,"2UP: Illegal option %c.\n",c);
				optionerr = TRUE;  break;
		}
		while (isdigit(*ss)) ss++;
	    }
	    for (jj = ii; jj < (argc-1); ++jj) argv[jj] = argv[jj + 1];
	    argc--;
	}
    if (optionerr) {
	fprintf(stderr,"2UP:   Legal options are:\n");
	fprintf(stderr,"-Ln    Page length.\n");
	fprintf(stderr,"-Cn    Column width .\n");
	fprintf(stderr,"-Mn    Margin.\n");
	fprintf(stderr,"-Tn    Top margin.\n");
	fprintf(stderr,"-Bn    Bottom margin.\n");
	fprintf(stderr,"-Sn    Space between columns.\n");
	fprintf(stderr,"-Nn    Number of columns.\n");
	return !0;
    }
    if (ncol == 0) ncol = 2;
    if (len == 0) len = 66;
    if (col == 0) col = (80 - margin - space * (ncol - 1)) / ncol;
    if (len > 88) error("2UP: Too long a page, 88 lines is maximum.");
    if ((margin + ncol * col + (ncol - 1) * space) > MAXW)
	error("2UP: Total width exceeds 132.");
    len = len - tm - bm;
    if (len < 1) error("2UP: Insufficient length.");

/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/
/*********************************/
/*	MAIN LOOP		 */
/*********************************/
    width = margin + ncol * col + (ncol - 1) * space;
    do {
	for (i = 1; i <= len; ++i)
	    for (j = 1;j <= MAXW + 1; ++j) array[i][j] = ' ';
	for (k = 0; k < ncol; ++k) {
	    for (i = 1; i <= len; ++i) {
		if (gets(string))
		    strmov(&array[i][margin+k*col+k*space+1],string,col);
		else {
		    if (!k && i == 1) return 0;
		    doneflag = TRUE;  goto end_of_file;
		}
	    }
	}
end_of_file:
	for (i = 1; i <= tm; ++i) puts("\n");
	for (i = 1; i <= len; ++i) {
	    for (j = MAXW + 1; j; --j)
		if (array[i][j] != ' ' || j == 1) {
		    array[i][j+1] = 0;	break;
		}
	    puts(&array[i][1]);
	}
	for (i = bm; i; --i) puts("\n");
    } while (!doneflag);
    return 0;
}

void strmov(s1,s2,n)
char *s1,*s2;
int n;
{
    char c;

    while (n--) {
	if (!(c = *s2++) || c == '\n') break;
	*s1++ = c;
    }
}

void error(msg)
char *msg;
{
    fprintf(stderr,"%s\n",msg);
    exit(!0);
}
