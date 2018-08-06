/****************************************************************************
 *									    *
 *	COLOR.C 	Max R. DÅrsteler    12-7-83			    *
 *									    *
 *	Set screen colors in text mode. 				    *
 *									    *
 ****************************************************************************
 */
#include <stdio.h>
#include <ctype.h>
#include <graph.h>

#include "graph.h"

main(argc, argv)
	int argc;
	char **argv;
{
	int charcol = 11;
	int scrncol = 4;
	int bordcol = 4;
	int i;

	if (argc < 4) {
	      printf("Usage: color [forground ] [background] [border]\n");
	      printf(" e.g.:\ncolor 11 4 12\n");
	      printf(" or:\ncolor yellow blue lblue\n");
	      printf(" or:\ncolor y blu lblu\n\n");
	      printf(" DOS 2.0 color indices: [0..15], background [0..7]\n\n");
	      printf(" Available color names: \n");
	      printf("  bla<ck> <l>r<ed> <l>g<reen> y<ellow> br<own>\n");
	      printf("  <l>blu<e> <l>m<agenta> <l>c<yan> <l>w<hite>\n\n");
	      printf(" <x> : x=optional character");
	      printf("  l  : light (for foreground & border color only!)\n");
	      exit(0);
	}
	argv++;
	charcol = colnum(*argv++);
	scrncol = colnum(*argv++);
	bordcol = colnum(*argv);
	i = bordcol & 7;
	switch (i) {		/* Translation from DOS 2.0 color table */
		case 1: 	/* to IBM color card color table */
			i = 4;
			break;
		case 3:
			i = 6;
			break;
		case 4:
			i = 1;
			break;
		case 6:
			i = 3;
			break;
		default:
			i = bordcol;
			break;
	};
	bordcol = i | (bordcol & 8);
	_setvideomode(TEXTMOD);
	outp(COLREG, bordcol | 0x10);
	if (charcol & 8) printf("\033[1m\n");
	else printf("\033[0m\n");
	charcol = (charcol & 7) + 30;
	scrncol = (scrncol & 7) + 40;
	printf("\033[%d;%dm\n", charcol, scrncol);
	cls();
}
/*----------------------------------------------------------------------*/

colnum(colname)
	char *colname;
{
	int ncol;
	int intens;
	char *pc;

	pc = colname;
	intens = 0;
	if (isdigit(*pc)) {
		if(sscanf(pc,"%d", &ncol) == 1)
			return(ncol);
		else {
			fprintf(stderr,"what color number ? \n");
			exit(0);
		}
	}
	else if (isalpha(*pc)) {
		if (*pc == 'l' || *pc == 'L') {       /* light ? */
			intens = 8;
			pc++;
		}
		switch(*pc) {
			case 'b':
			case 'B':
				if (*(pc + 1) == 'r' || *(pc + 1) == 'R')
					return(3);	/* brown */
				else switch(*(pc + 2)) {
					case 'a':       /* black */
					case 'A':
						return(0 | intens);
					case 'u':       /* blue */
					case 'U':
						return(4 | intens);
				};
			case 'r':                       /* red */
			case 'R':
				return(1 | intens);
			case 'g':                       /* green */
			case 'G':
				return(2 | intens);
			case 'y':                       /* yellow */
			case 'Y':
				return(11);
			case 'm':        /* magenta */
			case 'M':
			case 'v':
			case 'V':
				return(5 | intens);
			case 'c':                       /* cyan */
			case 'C':
				return(6 | intens);
			case 'w':                       /* white */
			case 'W':
				return(7 | intens);
			default:
				fprintf(stderr,"what colour? \n");
				exit(0);
		} /* end switch (*pc) */
	} /* end else if (alpha) */
} /* end subroutine colnum */
/* end color.c */
