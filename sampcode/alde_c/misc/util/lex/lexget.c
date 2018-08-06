/*
 * lexget.c
 *
 * Bob Denny	 28-Aug-82	Move stdio dependencies to lexerr(), lexget(),
 *							lexech() and mapch(). This is one of 4 modules
 *							in lexlib which depend upon the standard I/O package.
 *
 * Scott Guthery 20-Nov-83  Adapt for IBM PC & DeSmet C.  Endow DeSmet getc
 *						    with ungetc capabilities.	
 */

#include <stdio.h>
#include <lex.h>

lexgetc()
{
        return(unix_getc(lexin));
}

static char getbuf[30], *getbufptr = getbuf;

unix_getc(iop)
FILE iop;
{
	if(getbufptr == getbuf)
		return(getc(iop));
	else
		return(*--getbufptr);
}

ungetc(c, iop)
char c;
FILE iop; /* WARNING: iop ignored ... ungetc's are multiplexed!!! */
{
	*getbufptr++ = c;
}