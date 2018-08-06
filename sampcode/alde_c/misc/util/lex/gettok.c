/*
 * Bob Denny	 28-Aug-82  Remove reference to stdio.h
 * Scott Guthery 20-Nov-83	Adapt for IBM PC & DeSmet C
 */

#include <lex.h>

extern char *llend, llbuf[];

gettoken(lltb, lltbsiz)
char *lltb;
{
        register char *lp, *tp, *ep;

        tp = lltb;
        ep = tp+lltbsiz-1;
        for (lp = llbuf; lp < llend && tp < ep;)
                *tp++ = *lp++;
        *tp = 0;
        return(tp-lltb);
}
