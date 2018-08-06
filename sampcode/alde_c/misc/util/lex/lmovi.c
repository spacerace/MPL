/*
 * Bob Denny	 28-Aug-82  Remove reference to stdio.h
 * Scott Guthery 20-Nov-83	Adapt for IBM PC & DeSmet C
 */

#include <lex.h>

_lmovi(lp, c, st)
register int    c;
register int    st;
register struct lextab *lp;
{
        int base;

        while ((base = lp->llbase[st]+c) > lp->llnxtmax ||
                        (int *)(lp->llcheck)[base]!=st)
                if (st != lp->llendst) {
/*
 * This miscompiled on Decus C many years ago
 *                      st = ((int *)lp->lldefault)[st];
 */
                        base = ((int *)lp->lldefault)[st];
                        st = base;
                }
                else
                        return(-1);
        return(((int *)lp->llnext)[base]);
}
