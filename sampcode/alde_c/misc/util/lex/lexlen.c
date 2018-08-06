/*
 * Bob Denny	 28-Aug-82  Remove reference to stdio.h
 * Scott Guthery 20-Nov-83	Adapt for IBM PC & DeSmet C
 */

#include <lex.h>

extern char *llend, llbuf[];

lexlength()
/*
 * Return the length of the current token
 */
{
        return(llend - llbuf);
}
