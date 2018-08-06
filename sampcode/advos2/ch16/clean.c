/*
    CLEAN.C     Filter to turn document files into 
                normal text files.

    Copyright 1988 Ray Duncan

    Compile:    C>CL CLEAN.C

    Usage:      C>CLEAN  <infile >outfile

    All text characters are passed through with high bit stripped
    off.  Form feeds, carriage returns, and line feeds are passed
    through.  Tabs are expanded to spaces.  All other control codes
    are discarded.
*/

#include <stdio.h>

#define TAB_WIDTH   8               /* width of a tab stop */

#define TAB     '\x09'              /* ASCII tab character */
#define LF      '\x0A'              /* ASCII line feed */
#define FF      '\x0C'              /* ASCII form feed */
#define CR      '\x0D'              /* ASCII carriage return */
#define BLANK   '\x20'              /* ASCII space code */
#define EOFMK   '\x1A'              /* Ctrl-Z end of file */


main(int argc, char *argv[])
{   
    char c;                         /* char. from stdin */
    int col = 0;                    /* column counter */

    while((c = getchar()) != EOF)   /* read input character */
    {   
        c &= 0x07F;                 /* strip high bit */    

        switch(c)                   /* decode character */
        {   
            case LF:                /* if line feed or */
            case CR:                /* carriage return, */
                col=0;              /* reset column count */

            case FF:                /* if form feed, carriage */
                wchar(c);           /* return, or line feed */
                break;              /* pass character through */

            case TAB:               /* if tab expand to spaces */
                do wchar(BLANK);
                while((++col % TAB_WIDTH) != 0);
                break;

            default:                /* discard other control */
                if(c >= BLANK)      /* characters, pass text */
                {                   /* characters through */
                    wchar(c);
                    col++;          /* bump column counter */
                }
                break;
        }
    }
    wchar(EOFMK);                   /* write end-of-file mark */
    exit(0);                    
}


/*  
    Write a character to the standard output.  If
    write fails, display error message and terminate.
*/

wchar(char c)
{   
    if((putchar(c) == EOF) && (c != EOFMK))
    {   
        fputs("clean: disk full",stderr);
        exit(1);
    }
}

