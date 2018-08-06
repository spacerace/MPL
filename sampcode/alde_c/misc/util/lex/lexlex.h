/*
 * lex -- header file for lex.c
 *
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for smaller size
 *                                 Turn on debug code when overlaid.
 *          03-Dec-80 Bob Denny -- Change allocations for moves, nfa's and
 *                                  dfa's for RT-11. Try these for RSX if
 *                                  you overlay the same way.
 *          28-May-81 Bob Denny -- Change allocations for both RT and RSX.
 *                                  Overlay is same. Debug code adds little
 *                                  task space. Delete externs which caused
 *                                  TKB to barf.
 *          28-Aug-81 Bob Denny -- Add extern int sflag for "-s" support.
 *          30-Oct-82 Bob Denny -- Change allocations for RSX.
 *          15-Apr-83 Bob Denny -- Add big allocations for VAX-11 C
 *			20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 */

#define DEBUG	YES

/*
 * Original allocations.
 */
#define NCHARS  0400    /* Size of character set */
#define NCPW    2       /* # characters per word */
#define NBPC    8       /* # bits per character */
#define NBPW    (NCPW*NBPC)     /* # bits per word */

#define MAXNFA  600     /* Number of NFA states */
#define MAXDFA  800     /* Number of DFA states */
#define NTRANS  128     /* Number of translations */
#define NCCLS    50     /* Number of character classes */
#define NNEXT  2400     /* Size of dfa move vectors (later: allocate) */

/*
 * Special node characters.
 */
#define CCL     NCHARS          /* One of a character class */
#define EPSILON NCHARS+1        /* Transition on epsilon */
#define FIN     NCHARS+2        /* Final state; NFA */

/*
 * Set of state numbers (dfa state name).
 */
struct  set {
        struct set *s_next;
        struct  dfa     *s_state;       /* pointer into dfa array  */
        struct  set     *s_group;       /* pointer to owning group (dfamin) */
        int     s_final;                /* nf state which matches  */
        char    s_flag;                 /* see below */
        int     s_look;                 /* look-ahead bits */
        int     s_len;                  /* number of elements in set */
        struct nfa *s_els[1];
};

/*
 * State entry
 */
struct  nfa {
        int     n_char;
        char    *n_ccl;
        char    n_flag;
        char    n_look;         /* lookahead index */
        struct  nfa     *n_succ[2];
        struct  trans   *n_trans;
};

/*
 * DFA transition entry.
 */
struct  move {
        struct  set     *m_next;
        struct  dfa     *m_check;
};

/*
 * Structure of DFA vector.
 */
struct  dfa {
        struct  set     *df_name;
        struct  move    *df_base;
        struct  move    *df_max;
        struct  dfa     *df_default;
        int     df_ntrans;
};

/*
 * s_flag values for DFA node
 */
#define LOOK    01      /* Lookahead mark */
#define ADDED   02      /* DFA construction mark */
#define FLOOK   04      /* Mark on final state of lookahead translation */

/*
 * Flag used to print node
 */
#define NPRT    010     /* NFA node printed */

/*
 * Transition set.
 */
struct  xset {
        struct  set     *x_set;
        char    x_char;
        char    x_defsame;
};

/*
 * Translations
 */
struct  trans {
        struct  nfa     *t_start;
        struct  nfa     *t_final;
};

/*
 * External definitions.
 */
extern  struct  trans   trans[];
extern  struct  trans   *transp;
extern  struct  nfa     nfa[];
extern  struct  nfa     *nfap;
extern  struct  dfa     dfa[];
extern  int     ndfa;
extern  struct  move    move[];
extern  struct  xset    sets[];
extern  char    insets[];
extern  struct  set     *setlist;
extern  char    ccls[][(NCHARS+1)/NBPC];
extern  int     nccls;
extern  int     llnxtmax;
extern  char    *tabname;
extern  FILE    llout;
extern  FILE    lexin;
extern  int     aflag;
extern  int     sflag;
extern  char    *infile;

#ifdef DEBUG
extern  FILE    lexlog;
extern  int     lldebug;
extern  int     mflag;
#endif
