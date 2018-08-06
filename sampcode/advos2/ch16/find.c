/*
	FIND.C

	Searches text stream for a string.

	Compile with:  C> cl find.c

	Usage is:  C> find "pattern" [<source] [>destination]

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>

#define TAB     '\x09'                  /* ASCII tab (^I) */
#define BLANK   '\x20'                  /* ASCII space */
#define TAB_WIDTH 8                     /* columns per tab stop */
#define BUF_SIZE  256

static char input[BUF_SIZE];            /* input line buffer */
static char output[BUF_SIZE];           /* output line buffer */
static char pattern[BUF_SIZE];          /* search pattern buffer */

void writeline(int, char *);            /* function prototype */

main(int argc, char *argv[])
{   
    int line = 0;                       /* initialize line variable */

    if(argc < 2)                        /* search pattern supplied? */
    {   
        puts("find: missing pattern");  /* abort if no search pattern */
        exit(1);
    }

    strcpy(pattern,argv[1]);            /* save copy of search pattern */
    strupr(pattern);                    /* fold it to uppercase */      

    while(gets(input) != NULL)          /* read a line from input */
    {   
        line++;                         /* count lines */

        strcpy(output, input);          /* save copy of input string */
        strupr(input);                  /* fold input to uppercase */

        if(strstr(input, pattern))      /* if line contains pattern */
            writeline(line, output);    /* write it to standard output */
    }
    exit(0);                            /* terminate at end of file */
}

/*
    WRITELINE: Write line number and text to standard output,
    expanding any tab characters to stops defined by TAB_WIDTH.
*/

void writeline(int line, char *p)
{   
    int i = 0;                          /* index to input line */
    int col = 0;                        /* output column counter */

    printf("\n%4d: ", line);            /* write line number */

    while(p[i] != NULL)                 /* while not end of line */
    {   
        if(p[i] == TAB)                 /* if tab, expand it */
        {   
            do putchar(BLANK);
            while((++col % TAB_WIDTH) != 0);
        }
        else                            /* otherwise leave it alone */
        {       
            putchar(p[i]);              /* send character */
            col++;                      /* count columns */
        }
        i++;                            /* advance through input */
    }
}
