/*
 * Bob Denny 28-Aug-82	Move stdio dependencies to lexerr(), lexget(),
 *					    lexech() and mapch(). This is one of 4 modules 
 *						in lexlib which depend upon the standard I/O package.
 * Scott Guthery 20-Nov-83  Adapt for IBM PC & DeSmet C
 */

#include <stdio.h>
#include <lex.h>

extern int yyline;

lexerror(s, arg1, arg2, arg3)
char *s;
int arg1, arg2, arg3;
{
        if (yyline)
                fprintf(stderr, "%d: ", yyline);
        fprintf(stderr, s, arg1, arg2, arg3);
}
