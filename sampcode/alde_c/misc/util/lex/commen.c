/*
 * Bob Denny 	 28-Aug-82  Remove reference to stdio.h
 * Scott Guthery 20-Nov-83	Adapt for IBM PC & DeSmet C
 */
#include <lex.h>

extern int yyline;

comment(mat)
char *mat;
{
        register c;
        register char *cp;
        int lno;

        lno = yyline;
        c = 1;
        for (cp = mat; *cp && c>=0;) {
                if ((c = lexchar())=='\n')
                        yyline++;
                if (c!=*cp++)
                        cp = mat;
        }
        if (c < 0) {
                yyline = lno;
                lexerror("End of file in comment");
        }
}
