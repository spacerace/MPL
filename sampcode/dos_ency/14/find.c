/*
        FIND.C          Searches text stream for a string.

        Usage:          FIND "pattern" [< source] [> destination]

        by Ray Duncan, June 1987

*/

#include <stdio.h>

#define TAB     '\x09'                  /* ASCII tab character (^I) */
#define BLANK   '\x20'                  /* ASCII space character */

#define TAB_WIDTH 8                     /* columns per tab stop */

static char input[256];                 /* buffer for line from input */
static char output[256];                /* buffer for line to output */
static char pattern[256];               /* buffer for search pattern */

main(argc,argv)
int argc;
char *argv[];
{       int line=0;                     /* initialize line variable */

        if ( argc < 2 )                 /* was search pattern supplied? */
        {       puts("find: missing pattern.");
                exit(1);                /* abort if not */
        }
        strcpy(pattern,argv[1]);        /* save copy of string to find */
        strupr(pattern);                /* fold it to uppercase */      
        while( gets(input) != NULL )    /* read a line from input */
        {       line++;                 /* count lines */
                strcpy(output,input);   /* save copy of input string */
                strupr(input);          /* fold input to uppercase */
                                        /* if line contains pattern */
                if( strstr(input,pattern) )
                                        /* write it to standard output */
                        writeline(line,output);
        }
        exit(0);                        /* terminate at end of file */
}

/*
        WRITELINE: Write line number and text to standard output,
        expanding any tab characters to stops defined by TAB_WIDTH.
*/

writeline(line,p)
int line;
char *p;
{       int i=0;                        /* index to original line text */
        int col=0;                      /* actual output column counter */
        printf("\n%4d: ",line);         /* write line number */
        while( p[i]!=NULL )             /* while end of line not reached */
        {       if(p[i]==TAB)           /* if current char = tab, expand it */
                {       do putchar(BLANK);
                        while((++col % TAB_WIDTH) != 0);
                }
                else                    /* otherwise just send character */
                {       putchar(p[i]);
                        col++;          /* count columns */
                }
                i++;                    /* advance through output line */
        }
}
