/*
 * Copyright (c) 1978 Charles H. Forsyth
 */

/*
 * lex -- initialisation, allocation, set creation
 *
 *     Revised for PDP-11 (Decus) C by Martin Minow
 */

/* Modified 02-Dec-80 Bob Denny -- Conditionalized debug code for smaller size
 *                           01 -- Moved calls to dfa build, min, print, write
 *                                  and to stat, and code for ending() into
 *                                  this module so that 'ytab' could be put
 *                                  into overlay region.
 *          29-May-81 Bob Denny -- More extern hacking for RSX overlaying.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 *          28-Aug-82 Bob Denny -- Add "-s" switch to supress references to
 *                                  "stdio.h" in generated code.  Add switch
 *                                  comments in code. Add -e for "easy" com-
 *                                  mand line. "lex -e file" is the short way
 *                                  of saying:
 *                                      "lex -i file.lxi -o file.c -t file"
 * More(!)  30-Oct-82 Bob Denny -- Fix RSX ODL to put lots of FCS junk into
 *                                  overlay, pick up (badly needed) 3KW for
 *                                  NFA nodes, etc.  Change static allocations
 *                                  in LEXLEX.H for RSX so can do non-trivial
 *                                  things.  Task is now big on RSX and grows
 *                                  from big to huge as it runs.
 *                                 Fix "-s" support so it is again possible
 *                                  to do a lexswitch() (dumb!).
 *          14-Apr-83 Bob Denny    VAX-11 C workarounds.
 *                                 Fix definition of toupper().
 *			20-Nov-83 Scott Guthery  Adapt for IBM PC & DeSmet C
 */

#ifdef  DOCUMENTATION

title   lex     A Lexical Analyser Generator
index           A Lexical Analyser Generator

synopsis

        lex [-options] [-i grammar] [-o outfile] [-t table]

description

        Lex compiles a lexical analyser from a grammar and description of
        actions.  It is described more fully in lex.doc: only usage is
        described.  The following options are available:
        .lm +16
        .s.i-16;-a              Disable recognition of non-ASCII characters
        (codes > 177 octal) for exception character classes (form [^ ...]).
        .s.i-16;-d              Enable debugging code within lex.  Normally
        needed only for debugging lex.
        .s.i-16;-e              "Easy" command line. Saying "lex#-e#name" is the
        same as saying:
        .s.i 4;"lex -i name.lxi -o name.c -t name"
        .s
        Do not include devices or an extension on "name" or make it longer
        than 8 characters, or you'll get several error messages.
        .s.i-16;-i file         Read the grammar from the file.  If "-i" is not
        specified, input will be read from the standard input.
        .s.i-16;-m              Enable state minimization. Currently not
        implemented, switch is a no-op.
        .s.i-16;-o file         Write the output to the file.  If "-o" is not
        specified, output will be written to file "lextab.c".
        .s.i-16;-s              "Stand-alone" switch.  Supresses the line
        "#include <stdio.h>" normally generated in the lex output.  Use this
        if LEX is generating a module to be used in a program which does not
        use the "standard I/O" package.
        .s.i-16;-t table        Name the recognizer "table" instead of the
        default "lextab".  If -o is not given, output will be written to file
        "table.c".
        .s.i-16;-v [file]       Verify -- write internal tables to the
        indicated file.  If "-v" is given without a file name argument,
        tables will be written to "lex.out".
        .lm -16

diagnostics

        The following error messages may occur on invocation.  See lex
        documentation for information on compilation errors.
        .lm +8
        .s.i -8;Can't create ...
        .s.i -8;Cannot open ...
        .s.i -8;Illegal option.
        .s.i -8;Illegal switch combination.
        .s
        "-i", "-o" or "-t" given with "-e" or vice-versa
        .s.i -8;Table name too long.
        .s
        The table name (argument to "-t") must not be longer than 8 bytes.
        .s.i -8;Missing table name.
        .s.i -8;Missing input file.
        .s.i -8;Missing output file.
        .s.i -8;Missing name.
        .lm -8

author
        Charles Forsyth
        Modified by Martin Minnow, Bob Denny & Scott Guthery

bugs

#endif

#include <stdio.h>
#include "system.h"		/* includes system configuration constants */

extern char *lalloc();
extern char tolower();

struct  nfa     nfa[MAXNFA];
struct  nfa     *nfap = &nfa[1];

struct  xset    sets[NCHARS];
char    insets[NCHARS];

struct  trans   trans[NTRANS];
struct  trans   *transp = &trans[0];

char    ccls[NCCLS][(NCHARS+1)/NBPC];
int     nccls;

int     ndfa;
struct  dfa     dfa[MAXDFA];
struct  move    move[NNEXT];

char    *tabname = "lextab";
char    tabfile[15];
char    *infile         = NULL;
char    *outfile        = NULL;

#ifdef DEBUG
char    *dumpfile       = "lex.out";
int     lldebug = 0;
#endif

int  llnxtmax = 0;

FILE llout;
FILE lexin;
FILE lexlog;

/*
 * Flags.  Allow globals only for those requiring same. Some only
 * used for checking for bad combos.
 */
        int     aflag = 0;      /* Ignore non-ASCII in [^ ...] */
static  int     eflag = 0;      /* Easy command line */
static  int     iflag = 0;      /* "-i" given */
        int     mflag = 0;      /* Enable state minimization (not imp.) */
static  int     oflag = 0;      /* "-o" given */
        int     sflag = 0;      /* Supress "#include <stdio.h>" in output */
static  int     tflag = 0;      /* "-t" given */

struct  set     *setlist = 0;

main(argc, argv)
int argc;
char *argv[];
{
        register char *cp, *cp2;

#ifdef DEBUG
        int vflag;
        vflag = 0;
#endif

        for (; argc>1 && *argv[1]=='-'; argv++, argc--)
        switch (tolower(argv[1][1])) {

#ifdef DEBUG
        /*
         * Create "verification" file, describing the scanner.
         */
        case 'v':                               /* -v => lex.out        */
                vflag++;                        /* -v x.out => x.out    */
                if (argc > 2 && argv[2][1] != '1') {
                        --argc;
                        dumpfile = (++argv)[1];
                }
                break;
        /*
         * Enable debug displays
         */
        case 'd':
                lldebug++;
                break;
#endif
        /*
         * Enable state minimization. Currently not implemented.
         */
        case 'm':
                mflag++;
                break;

        /*
         * Disable matching of non-ASCII characters (codes > 177(8))
         * for exception character classes (form "[^ ...]").
         */
        case 'a':
                aflag++;
                break;

        /*
         * Supress "#include <stdio.h>" in generated
         * code for programs not using standard I/O.
         */
        case 's':
                sflag++;
                break;

        /*
         * "Easy" command line
         */
        case 'e':
                if(iflag || oflag || tflag) {
                        error("Illegal switch combination\n");
                        exit(1);
                }
                if (--argc <= 1) {
                        error("Missing name\n");
                        exit(1);
                }
                if (strlen(tabname = (++argv)[1]) > 8) {
                        error("Name too long\n");
                        exit(1);
                }
                infile = malloc(14);
                outfile = malloc(12);
				strcpy(infile, tabname); strcat(infile, ".lxi");
				printf("Input read from %s\n", infile);
                if ((lexin = fopen(infile, "r")) == NULL) {
                        error("Cannot open input \"%s\"\n", infile);
                        exit(1);
                }
				strcpy(outfile, tabname); strcat(outfile, ".c");
                break;

        /*
         * Specify input file name.
         */
        case 'i':
                if (eflag) {
                        error("Illegal switch combination\n");
                        exit(1);
                }
                iflag++;
                if (--argc <= 1) {
                        error("Missing input file\n");
                        exit(1);
                }
                infile = (++argv)[1];
				printf("Input read from %s\n", infile);
                if ((lexin = fopen(infile, "r")) == NULL) {
                        error("Cannot open input \"%s\"\n", infile);
                        exit(1);
                }
                break;

        /*
         * Specify output file name. Default = "lextab.c"
         */
        case 'o':
                if (eflag) {
                        error("Illegal switch combination\n");
                        exit(1);
                }
                oflag++;
                if (--argc <= 1) {
                        error("Missing output file");
                        exit(1);
                }
                outfile = (++argv)[1];
                break;

        /*
         * Specify table name.  Default = "lextab.c".  If "-o"
         * not given, output will go to "tabname.c".
         */
        case 't':
                if (eflag) {
                        error("Illegal switch combination\n");
                        exit(1);
                }
                tflag++;
                if (--argc <= 1) {
                        error("Missing table name");
                        exit(1);
                }
                if (strlen(tabname = (++argv)[1]) > 8) {
                        error("Table name too long\n");
                        exit(1);
                }
                break;

        default:
                error("Illegal option: %s\n", argv[1]);
                exit(1);
        }

#ifdef DEBUG

        cp = (vflag) ? dumpfile : "NUL";
		printf("Log written to %s\n", cp);
        if ((lexlog = fopen(cp, "w")) == NULL) {
                error("Cannot open \"%s\"", cp);
                exit(1);
        }
#endif
        if (infile == NULL) {
                infile = malloc(31);
                strcpy(infile, "lex.lxi");
        }
        cp = infile;                    /* Fold infile to lower case */
/*
 * The following 2 loops cannot use the form "*cp++ = tolower(*cp)" 
 * due to a bug in VAX-11 C V1.0-09 where the pointer increment
 * is done too soon (!).
 */
        while(*cp)
                {
                *cp = tolower(*cp);
                cp++;
                }
        cp = tabname;                   /* Fold tabname to lower case */
        while(*cp)
                {
                *cp = tolower(*cp);
                cp++;
                }
        if (outfile == NULL) {
                /*
                 * Typical hacker's idiom!
                 */
                for (cp = tabname, cp2 = tabfile; *cp2 = *cp++;)
                        cp2++;
                for (cp = ".c"; *cp2++ = *cp++;)
                        ;
                outfile = tabfile;
        }
		printf("Analyzer written to %s\n", outfile);
        if ((llout = fopen(outfile, "w"))==NULL) {
                error("Can't create %s\n", outfile);
                exit(1);
        }

        heading();
        fprintf(stderr, "Parse LEX source ...\n");
        if (yyparse())
                error("Parse failed\n");
        fprintf(stderr, "Build NFA then DFA ...\n");
        dfabuild();                                             /* 01+ */
        fprintf(stderr, "Minimize DFA ...\n");
        dfamin();
        fprintf(stderr, "Create C source ...\n");
        dfaprint();
        dfawrite();
#ifdef DEBUG
        stats();
		fclose(lexlog);
#endif                                                          /* 01- */
        fprintf(stderr, "\07LEX done.\n");
		fclose(llout);
} /** END OF MAIN **/

/*
 * This module was moved here from out.c so it could be called from
 * ytab.c residing in same overlay region as out.c.
 * 02-Dec-80  Bob Denny.
 */
                                                                /* 01+ */
ending()
{
        static int ended;

        if (ended++)
                return;
        fprintf(llout, "\t}\n\treturn(LEXSKIP);\n}\n");
        setline();
}

#ifdef DEBUG
stats()
{
        fprintf(lexlog, "\n");
        fprintf(lexlog, "%d/%d NFA states, %d/%d DFA states\n",
                nfap-nfa, MAXNFA, ndfa, MAXDFA);
        fprintf(lexlog, "%d/%d entries in move vectors\n", llnxtmax, NNEXT);
}

/*
 * Print a state set on { ... } form on lexlog.
 */
pset(t, nf)
register struct set *t;
{
        register i;

        fprintf(lexlog, "{");
        for (i = 0; i < t->s_len; i++)
                if (nf)
                        fprintf(lexlog, " %d", t->s_els[i]-nfa); else
                        fprintf(lexlog, " %d", t->s_els[i]);
        fprintf(lexlog, "}");
}

/*
 * Print a character to lexlog in readable form.
 * Returns the number of characters generated.
 */
chprint(ch)
{
        register char *s;

        ch &= 0377;
        switch (ch) {
        case '\t':
                s = "\\t";
                break;
        case '\n':
                s = "\\n";
                break;
        case '\b':
                s = "\\b";
                break;
        case '\r':
                s = "\\r";
                break;
        default:
                if(ch<040 || ch>=0177)
                        {
                        fprintf(lexlog, "\\%03o", ch);
                        return(4);
                        }
                else
                        {
                        putc(ch, lexlog);
                        return(1);
                        }
        }
        fprintf(lexlog, s);
        return(2);
}
#endif

/*
 * The following functions simply
 * allocate various kinds of
 * structures.
 */
struct nfa *
newnfa(ch, nf1, nf2)
struct nfa *nf1, *nf2;
{
        register struct nfa *nf;

        if ((nf = nfap++) >= &nfa[MAXNFA]) {
                error("Too many NFA states");
                exit(1);
        }
        nf->n_char = ch;
        nf->n_succ[0] = nf1;
        nf->n_succ[1] = nf2;
        nf->n_trans = 0;
        nf->n_flag = 0;
        nf->n_look = 0;
        return(nf);
}

newdfa()
{
        register struct dfa *df;

        if ((df = &dfa[ndfa++]) >= &dfa[MAXDFA]) {
                error("Out of dfa states");
                exit(1);
        }
        return(df);
}

char *
newccl(ccl)
char *ccl;
{
        register char *p, *q;
        register i;
        int j;

        for (j = 0; j < nccls; j++) {
                p = ccl;
                q = ccls[j];
                for (i = sizeof(ccls[j]); i--;)
                        if (*p++ != *q++)
                                goto cont;
                return(ccls[j]);
        cont:;
        }
        if (nccls >= NCCLS) {
                error("Too many character classes");
                exit(1);
        }
        p = ccl;
        q = ccls[j = nccls++];
        for (i = sizeof(ccls[j]); i--;)
                *q++ = *p++;
        return(ccls[j]);
}

struct trans *
newtrans(st, en)
struct nfa *st, *en;
{
        register struct trans *tp;

        if ((tp = transp++) >= &trans[NTRANS]) {
                error("Too many translations");
                exit(1);
        }
        tp->t_start = st;
        tp->t_final = en;
        en->n_trans = tp;
        return(tp);
}

/*
 * Create a new set. `sf', if set, indicates that the elements of the
 * set are states of an NFA). If `sf' is not set, the elements are state
 * numbers of a DFA.
 */
struct set *
newset(v, i, sf)
register struct nfa **v;
register i;
{
        register struct set *t;
        register k;
        int setcomp();

        qsort(v, i, sizeof(*v), setcomp);
        for (t = setlist; t; t = t->s_next)
                if (t->s_len==i && eqvec(t->s_els, v, i))
                        return(t);
        t = lalloc(1, sizeof(*t)+i*sizeof(t->s_els[0]), "set nodes");
        t->s_next = setlist;
        setlist = t;
        t->s_final = 0;
        t->s_state =  0;
        t->s_flag = 0;
        t->s_len = i;
        t->s_group = 0;
        t->s_look = 0;
        for (v += i; i;) {
                --v;
                if (sf) {
                        if ((*v)->n_char==FIN)
                                t->s_final =  (*v)-nfa;
                        if ((*v)->n_flag&LOOK)
                                t->s_look |= 1<<(*v)->n_look;
                } else {
                        k = *v;
                        dfa[k].df_name->s_group = t;
                }
                t->s_els[--i] = *v;
        }
        return(t);
}

setcomp(n1p, n2p)
struct nfa **n1p, **n2p;
{
        register struct nfa *n1, *n2;

        n1 = *n1p;
        n2 = *n2p;
        if (n1 > n2)
                return(1);
        if (n1==n2)
                return(0);
        return(-1);
}

eqvec(a, b, i)
register *a, *b, i;
{
        if (i)
                do {
                        if (*a++ != *b++)
                                return(0);
                } while (--i);
        return(1);
}

/*
 * Ask for core, and complain if there is no more.
 */
char *
lalloc(n, s, w)
char *w;
{
        register char *cp;

        if ((cp = calloc(n, s)) == NULL) {
                fprintf(stderr, "No space for %s", w);
#ifdef DEBUG
                if (lldebug)
                        dfaprint();
#endif
                exit(1);
        }
        return(cp);
}

error(format, argument)
char *format, *argument;
{
	fprintf(stderr, format, argument);
}