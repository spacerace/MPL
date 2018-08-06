/*
 * Copyright (c) 1978 Charles H. Forsyth
 *
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for smaller size
 *                           01 -- Removed ending() function code from here
 *                                  to lex.c, so ytab.c code could share the
 *                                  same overlay region as this module.
 *                           02 -- Removed nfaprint(), llactr(), newcase(),
 *                                  cclprint(), chprint() and setline(),
 *                                  the rest of this can share an overlay.
 *                                  They're in 'out2.c'. This is now 'out1.c'.
 *          29-May-81 Bob Denny -- More extern hacking for RSX overlaying.
 *          19-Mar-82 Bob Denny -- New compiler and library
 *          03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 *          28-Aug-82 Bob Denny -- Put "=" into table initializers to make
 *                                  new compiler happy. Add "-s" code to
 *                                  supress "#include <stdio.h>" in output.
 *                                  Tables output 8 values/line instead of
 *                                  16.  Overran R.H. edge on 3 digit octals.
 *                                  Change output format for readability.
 *          31-Aug-82 Bob Denny -- Add lexswitch( ...) to llstin so table
 *                                  name selected by -t switch is automatically
 *                                  switched-to at yylex() startup time.  Removed
 *                                  hard reference to "lextab" from yylex();
 *                                  This module generates extern declaration
 *                                  for forward reference.
 *          14-Apr-83 Bob Denny -- Add VAX11C support.  Remove usage of remote
 *                                  formats (damn!) not supported on VAX-11 C.
 *                                 Use "short int" for 16-bit table items under
 *                                  VAX-11 C to save size.
 *                                 Contitional out flaky debug code.  Probably
 *                                  related to non-functional minimization.
 *			20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *			26-Dec-83 Scott Guthery -- Removed "extern FILE lexin" from llstin
 *									   code and put it lex.h.
 */

/*
 * lex -- output human- and machine-readable tables
 */

#include <stdio.h>
#include "lexlex.h"

extern char *ignore;
extern char *illeg;
extern char *breakc;
extern int nlook;

char strdec1[] = "\nstruct lextab %s =\t{\n";
char strdec2[] = "\t\t\t%d,\t\t/* Highest state */\n";
char strdec3[] = "\t\t\t_D%s\t/* --> \"Default state\" table */\n";
char strdec4[] = "\t\t\t_N%s\t/* --> \"Next state\" table */\n";
char strdec5[] = "\t\t\t_C%s\t/* --> \"Check value\" table */\n";
char strdec6[] = "\t\t\t_B%s\t/* --> \"Base\" table */\n";
char strdec7[] = "\t\t\t%d,\t\t/* Index of last entry in \"next\" */\n";
char strdec8[] = "\t\t\t%s,\t\t/* --> Byte-int move routine */\n";
char strdec9[] = "\t\t\t_F%s\t/* --> \"Final state\" table */\n";
char strdec10[] = "\t\t\t_A%s\t/* --> Action routine */\n";
char strdec11[] = "\n\t\t\t%s%s\t/* Look-ahead vector */\n";

char ptabnam[] = { "         " };

/*
 * Print the minimised DFA, and at the same time, construct the vector which
 * indicates final states by associating them with their translation index.
 * (DFA printout supressed ifndef DEBUG.)
 */
dfaprint()
{
        register struct move *dp;
        register struct dfa *st;
        register i;
        int fi, k, l;

        vstart("LL16BIT _F%s", tabname);
#ifdef DEBUG
        fprintf(lexlog, "\nMinimised DFA for complete syntax\n");
#endif
        for (i = 0; i < ndfa; i++) {
#ifdef DEBUG
                fprintf(lexlog, "\nstate %d", i);
#endif
                st = &dfa[i];
                k = -1;
                if (fi = st->df_name->s_final) {
                        k = nfa[fi].n_trans - trans;
#ifdef DEBUG
                        fprintf(lexlog, " (final %d[%d])", fi, k);
#endif
                        if (nfa[fi].n_flag&FLOOK) {
                                k |= (nfa[fi].n_look+1)<<11;
#ifdef DEBUG
                                fprintf(lexlog, " restore %d", nfa[fi].n_look);
#endif
                        }
                }
                if (l = st->df_name->s_look)
#ifdef DEBUG
                        fprintf(lexlog, " look-ahead %o", l);
#else
                        ;
#endif
                vel(" %d,", k);
#ifdef MINIM_OK
                k = st->df_name->s_group->s_els[0];
                if (k!=i) {
                        fprintf(lexlog, " deleted\n");
                        continue;
                }

                fprintf(lexlog, "\n");
                for (dp = st->df_base; dp < st->df_max; dp = range(st, dp))
                if (st->df_default)
                      fprintf(lexlog, "\t.\tsame as %d\n", st->df_default-dfa);
#endif
        }
        vel(" %d,", -1);        /* blocking state */
        vend();
}

#ifdef MINIM_OK

range(st, dp)
register struct dfa *st;
register struct move *dp;
{
        int low, high, last;
        struct set *s;
        register a;

        while (dp < st->df_max && dp->m_check!=st)
                dp++;
/***************************************************
 * This always returns given the above statement ! *
 ***************************************************/
        if (dp >= st->df_max)
                return(dp);

        low = dp - st->df_base;
/*
        s = dp->m_next->s_group->s_els[0];
*/
        s = dp->m_next;
        for (last = low-1; dp < st->df_max &&
                           dp->m_check==st &&
                           (a = dp - st->df_base)==last+1 &&
/*
                           dp->m_next->s_state->s_els[0]==s; dp++)
*/
                           dp->m_next==s; dp++)
                last = a;
        high = last;
        fprintf(lexlog, "\t");
        if (high==low)
                chprint(low);
        else {
                fprintf(lexlog, "[");
                if (high-low > 4) {
                        chprint(low);
                        fprintf(lexlog, "-");
                        chprint(high);
                } else {
                        while (low<=high)
                                chprint(low++);
                }
                fprintf(lexlog, "]");
        }
        if (s->s_state==NULL)
                fprintf(lexlog, "\tNULL\n"); else
                fprintf(lexlog, "\t%d\n", s->s_state-dfa);
        return(dp);
}
#endif

heading()
{
        char *ver;
        fprintf(llout,
         "/*\n * Created by IBM PC LEX from file \"%s\"\n", infile);
        if(sflag == 0)                  /* If "standalone" switch off */
                {
                fprintf(llout,
                        " *\t- for use with standard I/O\n */\n");
                fprintf(llout, "\n#include <stdio.h>\n");
                }
        else
                fprintf(llout, " *\t- for use with stand-alone I/O\n */\n");

        fprintf(llout, "#include <lex.h>\n");
        fprintf(llout, "#define LL16BIT int\n");
        fprintf(llout, "extern int _lmov%c();\n",
                        (ndfa <= 255) ? 'b' : 'i');
		fprintf(llout, "extern struct lextab *_tabp;\n");
		fprintf(llout, "extern int lexval, yyline;\n");
		fprintf(llout, "extern char lbuf[], *llend;\n");
}

dfawrite()
{
        register struct move *dp;
        register i, a;
        struct dfa *st, *def;
        struct set *xp;
        char *xcp;

        setline();
        fprintf(llout,
                "\n#define\tLLTYPE1\t%s\n", ndfa<=255? "char": "LL16BIT");
        vstart("LLTYPE1 _N%s", tabname);
        for (i = 0; i <= llnxtmax; i++)
                if (xp = move[i].m_next)
                        vel(" %d,", xp->s_state-dfa); else
                        vel(" %d,", ndfa);
        vend();
        vstart("LLTYPE1 _C%s", tabname);
        for (i = 0; i <= llnxtmax; i++)
                if (st = move[i].m_check)
                        vel(" %d,", st-dfa); else
                        vel(" %d,", -1);
        vend();
        vstart("LLTYPE1 _D%s", tabname);
        for (i = 0; i < ndfa; i++)
                if (def = dfa[i].df_default)
                        vel(" %d,", def-dfa); else
                        vel(" %d,", ndfa); /* refer to blocking state */
        vend();
        vstart("LL16BIT _B%s", tabname);
        for (i = 0; i < ndfa; i++)
                if (dp = dfa[i].df_base)
                        vel(" %d,", dp-move); else
                        vel(" %d,", 0);
        vel(" %d,", 0); /* for blocking state */
        vend();
        if (nlook) {
                fprintf(llout, "char    *llsave[%d];\n", nlook);
                vstart("LL16BIT _L%s", tabname);
                a = nlook<=NBPC? NCHARS-1: -1;
                for (i = 0; i < ndfa; i++)
                        vel(" 0%o,", dfa[i].df_name->s_look&a);
                vel(" %d,", 0);
                vend();
        }
        dospccl(ignore, "LLIGN", "X");
        dospccl(breakc, "LLBRK", "Y");
        dospccl(illeg, "LLILL", "Z");

        strcpy(ptabnam, tabname); strcat(ptabnam,",");
        fprintf(llout, strdec1, tabname);
        fprintf(llout, strdec2, ndfa);
        fprintf(llout, strdec3, ptabnam);
        fprintf(llout, strdec4, ptabnam);
        fprintf(llout, strdec5, ptabnam);
        fprintf(llout, strdec6, ptabnam);
        fprintf(llout, strdec7, llnxtmax);
        fprintf(llout, strdec8, ndfa<=255?"_lmovb":"_lmovi");
        fprintf(llout, strdec9, ptabnam);
        fprintf(llout, strdec10, ptabnam);
        fprintf(llout, strdec11, nlook?"_L":"", nlook?ptabnam:"NULL,   ");
        refccl(ignore, "Ignore", "X");
        refccl(breakc, "Break", "Y");
        refccl(illeg, "Illegal", "Z");
        fprintf(llout, "\t\t\t};\n");
        if(sflag == 0)                  /* If stdio flavor */
                {
                fprintf(llout, "\n/* Standard I/O selected */\n");
/*
                fprintf(llout, "extern FILE lexin;\n\n");
*/
                fprintf(llout, "llstin()\n   {\n   if(lexin == NULL)\n");
                fprintf(llout, "      lexin = stdin;\n");
                }
        else                            /* Stand-alone flavor */
                {
                fprintf(llout, "\n/* Stand-alone selected */\n");
                fprintf(llout, "\llstin()\n   {\n");
                }
        fprintf(llout, "   if(_tabp == NULL)\n");
        fprintf(llout, "      lexswitch(&%s);\n   }\n\n", tabname);
        fclose(llout);
}

dospccl(cp, s, tag)
register char *cp;
char *s, *tag;
{
        register n;
        char cclnam[16];

        if (cp==0)
                return;
        fprintf(llout, "#define\t%s\t%s\n", s, tag);
        strcpy(cclnam, tag); strcat(cclnam, tabname);
        vstart("char _%s", cclnam);
        for (n = sizeof(ccls[0]); n--;)
                vel(" 0%o,", *cp++&0377);
        vend();
}

refccl(cp, nm, tag)
char *cp, *nm, *tag;
{
        if (cp==0)
                fprintf(llout, "\t\t\t0,\t\t/* No %s class */\n", nm);
        else
                fprintf(llout, "\t_%s%s,\t/* %s class */\n", tag, tabname, nm);
}

int     vnl;

vstart(fmt, str)
char *fmt;
char *str;              /*** WATCH IT ***/
{
        vnl = 0;
        putc('\n', llout);
        fprintf(llout, fmt, str);
        fprintf(llout, "[] =\n   {\n  ");
}

vend()
{
        fprintf(llout, "\n   };\n");
}

vel(fmt, val)
char *fmt;
int val;                /*** WATCH IT ***/
{
        fprintf(llout, fmt, val);
        if ((++vnl&07)==0)
                fprintf(llout, "\n  ");
}
