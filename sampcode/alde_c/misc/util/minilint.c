/****************************************************************
*								*
*			CC (C Checker)				*
*								*
*	     C Source Paren, bracket and comment Checker 	*
*								*
*	         T. Jennings  -- Sometime in 1983		*
*		Slightly tweaked and renamed MINILINT		*
*			KAB Aug 84				*
*								*
*								*
****************************************************************/

#include <stdio.h>
#include <ctype.h>

/* Very crude but very effective C source debugger. Counts the numbers of
matching braces, parenthesis and comments, and displays them at the left edge
of the screen. The best way to see what it does is to do it; try

	MINILINT MINILINT.C		/* C Check MINILINT.C */

Properly handles parens and brackets inside comments; they are ignored. 
*/

main(argc,argv)
	int argc;
	char **argv;
{
int file;	
char c,lastc;
int parens,brackets,comments;
int line, col;
char hdr[40];

	file= open(argv[1],0x8000);
	if (file == -1) {
		cputs("File missing.\r\nTry MINILINT <filename.ext> \r\n");
		exit();
	}
	brackets= parens= comments= 0;
	line= 0; col= 0;
	lastc= '\0';

	while (read(file,&c,1)) {
		if (c == 0x1a) break;			/* stop if ^Z */

		if (col == 0) {
			sprintf(hdr,"%d: {%d} (%d) /*%d*/",line,brackets,parens,comments);
			while (strlen(hdr) < 23)	/* gross but works */
				strcat(hdr," ");	/* to hell with elegant, */
			cprintf("%s|",hdr);		/* we got a job to do !*/
		}				/* (real programmers dont ...) */

/* Dont count parens and brackets that are inside comments. This of course
assumes that comments are properly matched; in any case, that will be the
first thing to look for. */

		if (comments <= 0) {
			if (c == '{') ++brackets;
			if (c == '(') ++parens;
			if (c == '}') --brackets;
			if (c == ')') --parens;
		}

/* Now do comments. This properly handles nested comments;
whether or not the compiler does is your responsibility */

		if ((c == '*') && (lastc == '/')) ++comments;
		if ((c == '/') && (lastc == '*')) --comments;

		++col;
		if (c == 0x0a) {		/* newline == New Line */
			col= 0;			/* set column 0 */
			++line;
		}
		putchar(c);			/* display text */
		lastc= c;			/* update last char */
	}
	cputs("\r\n\r\n");
	if (brackets) cputs("Unbalanced brackets\r\n");
	if (parens) cputs("Unbalanced parenthesis\r\n");
	if (comments) cputs("Unbalanced comments\r\n");
}

/* eof */*/

#include <stdio.h>
#include <ctype.h>

/* Very crude but very effective C source debugger. Counts the numbers of
match                                                                                                                                