/***************************************************************************\

Module Name: SYMBOL.H

This header file contains the definitions necessary to look up symbols
in the .SYM files.

Created by Microsoft Corporation, 1989

\***************************************************************************/


/* Debug Symbol Table Structures
   -----------------------------
For each symbol table (map): (MAPDEF)
-------------------------------------------------------------------------------------------------
| map_ptr | lsa | pgm_ent | abs_cnt | abs_ptr | seg_cnt | seg_ptr | nam_max | nam_len | name... |
------------------------------------------------------------------------------------------------- */
typedef struct _MAPDEF {    /* mpdf */
    unsigned    map_ptr;    /* 16 bit ptr to next map (0 if end)    */
    unsigned    lsa    ;    /* 16 bit Load Segment address          */
    unsigned    pgm_ent;    /* 16 bit entry point segment value     */
    int         abs_cnt;    /* 16 bit count of constants in map     */
    unsigned    abs_ptr;    /* 16 bit ptr to   constant chain       */
    int         seg_cnt;    /* 16 bit count of segments in map      */
    unsigned    seg_ptr;    /* 16 bit ptr to   segment chain        */
    char        nam_max;    /*  8 bit Maximum Symbol name length    */
    char        nam_len;    /*  8 bit Symbol table name length      */
} MAPDEF;

typedef struct _MAPEND {    /* mped */
    unsigned        chnend;         /* end of map chain (0) */
    char            rel;            /* release              */
    char            ver;            /* version              */
} MAPEND;

/* For each segment/group within a symbol table: (SEGDEF)
--------------------------------------------------------------
| nxt_seg | sym_cnt | sym_ptr | seg_lsa | name_len | name... |
-------------------------------------------------------------- */
typedef struct _SEGDEF {    /* sgdf */
    unsigned    nxt_seg;    /* 16 bit ptr to next segment(0 if end) */
    int         sym_cnt;    /* 16 bit count of symbols in sym list  */
    unsigned    sym_ptr;    /* 16 bit ptr to symbol list            */
    unsigned    seg_lsa;    /* 16 bit Load Segment address          */
    unsigned    seg_in0;    /* 16 bit instance 0 physical address   */
    unsigned    seg_in1;    /* 16 bit instance 1 physical address   */
    unsigned    seg_in2;    /* 16 bit instance 2 physical address   */
    unsigned    seg_in3;    /* 16 bit instance 3 physical address   */
    unsigned    seg_lin;    /* 16 bit ptr to line number record     */
    char        seg_ldd;    /*  8 bit boolean 0 if seg not loaded   */
    char        seg_cin;    /*  8 bit current instance              */
    char        nam_len;    /*  8 bit Segment name length           */
} SEGDEF;

/*  Followed by a list of SYMDEF's..
    for each symbol within a segment/group: (SYMDEF)
-------------------------------
| sym_val | nam_len | name... |
------------------------------- */
typedef struct _SYMDEF {    /* sydf */
    unsigned    sym_val;    /* 16 bit symbol addr or const          */
    char        nam_len;    /*  8 bit symbol name length            */
} SYMDEF;

typedef struct _SEGINFO {   /* sinfo */
    PID         pid;        /* Which process did we get selector for */
    USHORT      selector;
    char far   *lpModName;
    char far   *lpPathName;
    char far   *lpSegName;
    char far   *lpSymName;
    int         symfh;
    int         segcnt;     /* 16 bit count of segment in seg list  */
    int         segptr;     /* 16 bit file offset of segment  list  */
    int         symcnt;     /* 16 bit count of symbols in sym list  */
    long        symFPos;
    WHOISINFO	whoIs;
} SEGINFO;
typedef SEGINFO     *PSEGINFO;
typedef SEGINFO far *LPSEGINFO;

void    pascal near DebugBreakReturn( void );

long    pascal near DebugFileSeek( int, long, int );
USHORT  pascal near DebugFileRead( int, PSZ, USHORT );

int     pascal near IdentifyCodeSegment( PSEGINFO, PID, USHORT );
USHORT  pascal near FindSymbol( PSEGINFO, USHORT );
