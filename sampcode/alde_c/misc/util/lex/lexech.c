/*
 * lexech.c
 *
 * Bob Denny 28-Aug-82	Move stdio dependencies to lexerr(), lexget(),
 *					    lexech() and mapch(). This is one of 4 modules
 *					    in lexlib which depend upon the standard I/O package.
 * Scott Guthery 20-Nov-83  Adapt for IBM PC & DeSmet C
 */

#include <stdio.h>
#include <lex.h"

extern char *llend, llbuf[];

lexecho(fp)
register FILE fp;
{
        register char *lp;

        for (lp = llbuf; lp < llend;)
                putc(*lp++, fp);
}
