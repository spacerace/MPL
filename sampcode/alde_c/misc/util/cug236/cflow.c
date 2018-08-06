/*
	HEADER:		CUG236;
	TITLE:		Function Abstractor;
	DATE:		04/19/1987;
	DESCRIPTION:	"Abstracts C function calls and declarations from a C
			source and produces a listing of the program's calling
			hierarchy."
	VERSION:	3.0;
	KEYWORDS:	Flow Analysis, Flow Analyzer;
	FILENAME:	CFLOW.C;
	SEE-ALSO:	CFLOW.DOC;
	COMPILERS:	vanilla;
	AUTHORS:	W. C. Colley III, Mark Ellington;
*/

/*
**   CFLOW.C : find module call structure of c program
**   refer to cflow.doc for how to use
**					Mark Ellington
**					05-27-84
**
**   Ported to portable C.  Required the following changes:
**
**	1)  Stripped BDS C hooks.
**	2)  Stripped C/80 hooks.
**	3)  Allowed for presence/absence of header files "ctype.h"
**		and "string.h".
**	4)  Allowed for possible pre-definition of constants TRUE,
**		FALSE, and EOF.
**	5)  Made variable fptr type FILE * instead of int.
**	6)  Added a #define for the max line length.
**	7)  Made preprocessor directive rejection logic smarter.
**	8)  Removed name conflict between our fgets() and the std
**	    library fgets() by changing ours to get_source_line().
**					William C. Colley, III
**					04-19-87
*/

#include <stdio.h>

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)		agetc(f)
#define putc(c,f)	aputc(c,f)
*/

/*  Portability Note:  If you do not have a header file "ctype.h",
    uncomment the following #define so that the program will look for
    library support.							*/

/* #define	NO_CTYPE_H						*/

#ifdef	NO_CTYPE_H
extern int isalnum();
#else
#include <ctype.h>
#endif

/*  Portability Note:  If you do not have a header file "string.h",
    uncomment the following #define so that the program will look for
    library support.							*/

/* #define	NO_STRING_H						*/

#ifdef	NO_STRING_H
extern int strcmp();
#else
#include <string.h>
#endif

/*  Portability Note:  A few compilers don't know the additional type
    void.  If yours is one of these, uncomment the following #define.	*/

/* #define	void		int					*/

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef EOF
#define EOF	-1
#endif

#define LINE_LENGTH	256	/* Max line length program can handle.	*/

FILE *fptr;			/* input file pointer			*/
int level;			/* keep track of level of open "{"s	*/
char name[LINE_LENGTH];		/* module name buffer			*/
char ins[LINE_LENGTH];		/* source input line buffer		*/
int curchar;			/* current character in input line
					buffer array subscript		*/

/*  Fixed bug that makes _ characters lop off the beginning of function
    names.  WCC3.							*/

int isal_num(c)
int c;
{
    return isalnum(c) || c == '_';
}

int main(argc,argv)
int argc;
char *argv[];
{
	void modules();

	printf("\nCFLOW --> function declarations and calls in C source");
	printf("\n by Mark Ellington");

	if (argc != 2) {
		printf("\nUsage: cflow [infilename.ext] ");
		return TRUE;
	}

	if (!(fptr = fopen(argv[1],"r"))) {
		printf("\nCan't open %s\n",argv[1]);
		return TRUE;
	}
	printf("\nSource file: %s",argv[1]);

	modules();

	fclose(fptr);  return FALSE;
}


void modules()		/* find function declarations and calls		*/
{
	int j;
	char c;
	int incom;	/* comment flag					*/
	int decl;	/* module declaration line flag			*/
	int lastlin;	/* last line of file flag			*/
	int quoted;	/* within " quotes				*/
	int header;	/* within function header (before 1st '{')	*/
	int comment(), get_source_line(), modname(), skipline();
	void comout(), lookbak(), quotes();

	incom = lastlin = quoted  = header = FALSE;  level = 0;

	do {
		lastlin = get_source_line();	/* read a line of source */
		decl = FALSE;		/* assume nothing		*/
		curchar = 0;
					/* read for significant characters */
		while (curchar < LINE_LENGTH) {
			if (skipline()) break;
			quotes();	/* skip single quoted character */
			incom = comment(incom); /* true if in comment	*/
			c = ins[curchar];
			/* read for significant characters */
			if (!incom) {
			    /* skip double quoted strings */
			    if (c == '"') quoted = !quoted;
			    if (!quoted)
				switch(c) {
				    case '{' :	level++;  header = FALSE;
						break;

				    case '}' :	level--;
						break;

				/* "(" always follows function call */
					/* or declaration */

				    case '(' :	if (!isalnum(ins[curchar-1]))
						    break;
						lookbak(curchar);
						if (!(j = modname())) break;
						else decl = TRUE;
						if (j == 2) header = TRUE;
						break;

				    default :	break;
				}
			}
			++curchar;     /* next character */
		}
		/* display argument declarations */
		comout(ins);
		if (header && !decl) printf("%s",ins);
	} while (lastlin);    /*  = 0 if last line */
}

/* skip this line ? */

int skipline()
{
	char c;
	int i;

	if (!(c = ins[curchar])) return TRUE;	/* end of line		*/
	if (c == '#') {			/* skip preprocessor directives */
		for (i = curchar; i--; )
			if (ins[i] != ' ' && ins[i] != '\t') return FALSE;
		return TRUE;
	}
	return FALSE;				/* don't skip		*/
}


/* skip characters quoted (for instance '}' would throw off level count) */

void quotes()
{
	int flowchar();

	if (flowchar(ins[curchar]) &&		/* test critical chars only */
	    ins[curchar+1] == '\'' &&		/* next char single quote? */
	    curchar+2 < LINE_LENGTH)		/* don't pass end of string */
	    curchar = curchar + 2;		/* skip past quote	*/
}


/* return TRUE if entering comment, FALSE if exiting */

int comment(incom)
int incom;
{
	if (ins[curchar] == '/') {
		if (ins[curchar+1] == '*') return TRUE;
		if (curchar > 0 && ins[curchar-1] == '*') return FALSE;
	}
	return incom;				/* unchanged		*/
}


/* look back from position n in string.	 called with n indicating '('.
   determine function name						*/

void lookbak(n)
int n;
{
	int i;
	void comout();

	while (!isal_num(ins[n]) && n) --n;

	/* find leading blank */
	while (isal_num(ins[n-1]) && n) --n;

	/* save name */
	/* include variable declarations if module declaration */

	i = 0;
	if (level == 0)			/* full line if declaration	*/
		while (ins[n]) name[i++] = ins[n++];
	else				/* function call within function */
		while (isal_num(ins[n])) name[i++] = ins[n++];
	name[i] = '\0';
	comout(name);	/* remove comment from name string */
}


/* terminate string at comment */

void comout(s)
char *s;
{
	char c;

	while (c = *s++)
		if (c == '/')
			if (*s == '*') {
				*(s - 1) = '\n';
				*s = '\0';
				break;
			}
}



/* display module name with indentation according to { level */
/* returns 0 if not module, 1 if call within module, 2 if    */
/* module declaration  */

int modname()
{
	int j, unreserved();
	void comout();

	if (unreserved()) {		/* test if builtin like while */
		if (level == 0) {
			comout(ins);
			printf("\n\n\n**\n%s",ins);
			return 2;
		}
		else {
			printf("\n");
			for (j=0; j < level; ++j)
				putchar('\t');
			printf("%s()",name);
			return 1;
		}
	}
	return 0;
}

/* test for names that are operators not functions */

int unreserved()
{
	return	strcmp(name,"return") &&
		strcmp(name,"if") &&
		strcmp(name,"while") &&
		strcmp(name,"for") &&
		strcmp(name,"switch");
}


/* test if character is one that program tracks */

int flowchar(c)
char c;
{
	return c == '{' || c == '}' || c == '\"';
}


/* read a line of source */

int get_source_line()
{
	char *s;
	int ch;

	s = ins;
	while ((ch = getc(fptr)) != EOF) {
		if ((*s++ = ch) == '\n') {
			*s = '\0';
			return TRUE;
		}
	}
	*s = '\0';
	return FALSE;
}
