/*
        PROTOL.C: a template for a line-oriented filter.

        Ray Duncan, June 1987.
*/

#include <stdio.h>

static char input[256];                 /* buffer for input line */
static char output[256];                /* buffer for output line */

main(argc,argv)
int argc;
char *argv[];
{       while( gets(input) != NULL )    /* get a line from input stream */
                                        /* perform any necessary translation
                                           and possibly write result */
        {       if (translate()) puts(output);
        }
        exit(0);                        /* terminate at end of file */
}

/*
        Perform any necessary translation on input line, leaving
        the resulting text in output buffer.  Value of function
        is 'true' if output buffer should be written to standard output
        by main routine, 'false' if nothing should be written.
*/

translate()
{       strcpy(output,input);           /* template action is copy input */
        return(1);                      /* line and return true flag */
}
