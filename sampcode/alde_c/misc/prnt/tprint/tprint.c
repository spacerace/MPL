
/****************************************************************
*								*
*			P R I N T				*
*								*
*		ASCII file transfer to PRN device. 		*
*								*
*	T. Jennings 6 Nov. 82					*
*	  created 23 Sept. 82					*
*								*
****************************************************************/

#include <stdio.h>
#include <ctype.h>

#define	NUL	0x00
#define	CR	0x0d
#define	LF	0x0a
#define	HT	0x09
#define	FF	0x0c
#define	DEL	0x7f

#define	PAGELEN	66
#define	BOTTOM	58
FILE *printfile, *fopen();
FILE *printer;
char c;
int line,column;
int tabwidth;
int formfeed;
int number,lineno;
int brackets,brkt_level;
int pageno;
int printable;
int titles;
int margin;
int i;
char filename[12];

main(argc,argv)
int argc;
char **argv;
{
	fprintf(stderr,"\nFile Print Utility 6 Nov. 82\n");
	number =0;		/* dont number lines */
	titles= 0;		/* no top of page title */
	margin= 0;		/* no left margin tab */
	brackets =0;		/* dont count brackets */
	brkt_level =0;
	column =0;
	line =0; 
	tabwidth =8;
	formfeed =1;
	lineno =0;		/* current line number, */
	pageno =1;
	printable =0;

	if (--argc) {
		if ((printfile=fopen(*++argv,"r")) ==NULL) {
			fprintf(stderr,"PRINT: Can't find file '%s'.\n",*argv);
			exit();
		}
		strcpy(filename,*argv);		/* save filename for title */

		if ((printer= fopen("PRN","w")) ==NULL) {
			fprintf(stderr,"PRINT: Can't open LST device\n");
			exit();
		}
		while (--argc) {		/* look for options */
			c= **++argv;
			switch(tolower(c)) {
			case 't':
				fprintf(stderr,"\t-Page headers\n");
				titles= 1;
				break;
			case 'n':		/* line numbers */
				fprintf(stderr,"\t-Line numbers\n");
				number =1;
				break;
			case 'c':		/* C source */
				fprintf(stderr,"\t-'C' source\n");
				brackets =1;
				break;
			case 'm':
				fprintf(stderr,"\t-Left margin\n");
				margin= 1;
				break;
			default:
				fprintf(stderr,"Illegal option: '%s'\n",*argv);
				break;
			}
		}
		title();
		printchar(LF);
		while ((i=fgetc(printfile)) !=EOF) {
			c=i;			/* type conversion, */
			printchar(c);		/* print character, */
			if (c =='{')		/* count bracket levels, */
				++brkt_level;
			if (c == '}') {		/* if closing bracket, and */
				if ((--brkt_level ==0) && brackets) {
					printchar(FF); /* top level, formfeed */
				}
			}
		}
		fputc(LF,printer);
		fputc(FF,printer);		/* do a formfeed, */
		exit();
	}
	fprintf(stderr,"\nYou must specify a file, followed by any options");
	fprintf(stderr,"\n\tT\tTop of page headers");
	fprintf(stderr,"\n\tN\tLine numbers");
	fprintf(stderr,"\n\tC\tC source");
	fprintf(stderr,"\n\tM\tLeft margin");
	fprintf(stderr,"\nExample:  PRINT <filename> N C T M");
	exit();
}
/* Print character <C> to the printer. */

printchar(c)
char c;
{
	switch (c) {
	case LF:
		++lineno;
		fputc(CR,printer);
		fputc(c,printer);		/* then line feed, */
		if (++line >= BOTTOM) 		/* if bottom margin, */
			printchar(FF);		/* formfeed, */
		if (number) 
			fprintf(printer,"%3d: \t",lineno);
		if (margin)
			printchar(HT);		/* left margins */
		break;
	case FF:
		fputc(LF,printer);
		fputc(c,printer);
		line =0; ++pageno;
		title();
		break;
	default:
		fputc(c,printer);
		break;
	}
	return;
}
/* If the title flag is set, type the title across the top of the page. */

title() {
	if (titles) {
		fprintf(printer,"File: %-12s%58sPage %d\n",filename," ",pageno);
		++line;
	}
	return;
}
