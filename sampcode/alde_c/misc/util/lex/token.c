/*
 * Bob Denny	 28-Aug-82  Remove reference to stdio.h
 * Scott Guthery 20-Nov-83	Adapt for IBM PC & DeSmet C
 */

#include <lex.h>

extern char *llend, llbuf[];

char *token(cpp)
char **cpp;
{
        if (cpp)
                *cpp = llend;
        return(llbuf);
}
