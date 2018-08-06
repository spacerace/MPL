/* SDB - definition file */
#define Lattice
/* useful definitions */
#define TRUE            1
#define FALSE           0
#ifndef NULL
#define NULL            0
#endif

/* program limits */
#define LINEMAX         132     /* maximum input line length */
#define TABLEMAX        132     /* maximum table output line */
#define KEYWORDMAX      10      /* maximum keyword length */
#define NUMBERMAX       20      /* maximum number length */
#define STRINGMAX       132     /* maximum string length */
#define CODEMAX         100     /* maximum length of code array */
#define STACKMAX        20      /* maximum interpreter stack size */

/* token definitions */
#define EOS             0
#define LSS             -1
#define LEQ             -2
#define EQL             -3
#define NEQ             -4
#define GEQ             -5
#define GTR             -6
#define SELECT          -7
#define FROM            -8
#define WHERE           -9
#define CREATE          -10
#define DELETE          -11
#define INSERT          -12
#define EXIT            -13
#define CHAR            -14
#define NUM             -15
#define ID              -16
#define STRING          -17
#define NUMBER          -18
#define UPDATE          -19
#define PRINT           -20
#define IMPORT          -21
#define EXPORT          -22
#define INTO            -23
#define HELP            -24
#define COMPRESS        -25
#define EXTRACT         -26
#define DEFINE          -27
#define SHOW            -28
#define USING           -29
#define SORT            -30
#define BY              -31
#define ASCENDING       -32
#define DESCENDING      -33
#define SET             -34

/* operand types */
#define LITERAL 1
#define ATTR    2
#define TEMP    3

/* attribute data types */
#define TCHAR   1
#define TNUM    2

/* tuple status codes */
#define UNUSED  0
#define ACTIVE  1
#define DELETED 2

/* relation header page format definitions */
#define RNSIZE          10      /* size of a relation name */
#define HSIZE           16      /* size of a relation entry */
#define ASIZE           16      /* size of a attribute entry */
#define ANSIZE          10      /* size of a attribute name */
#define NATTRS          31      /* number of attributes in header block */

/* error code definitions */
#define END             0       /* end of retrieval set */
#define INSMEM          1       /* insufficient memory */
#define RELFNF          2       /* relation file not found */
#define BADHDR          3       /* bad relation file header */
#define TUPINP          4       /* tuple input error */
#define TUPOUT          5       /* tuple output error */
#define RELFUL          6       /* relation file full */
#define RELCRE          7       /* error creating relation file */
#define DUPATT          8       /* duplicate attribute on relation create */
#define MAXATT          9       /* too many attributes on relation create */
#define INSBLK          10      /* insufficient disk blocks */
#define SYNTAX          11      /* command syntax error */
#define ATUNDF          12      /* attribute name undefined */
#define ATAMBG          13      /* attribute name ambiguous */
#define RLUNDF          14      /* relation name undefined */
#define CDSIZE          15      /* boolean expression code too big */
#define INPFNF          16      /* input file not found */
#define OUTCRE          17      /* output file creation error */
#define INDFNF          18      /* indirect command file not found */
#define BADSET          19      /* bad set parameter */

struct attribute {
    char at_name[ANSIZE];       /* attribute name */
    char at_type;               /* attribute type */
    char at_size;               /* attribute size in bytes */
    char at_scale;              /* attribute scale factor (for numeric only) */
    char at_unused[ASIZE-ANSIZE-3];     /* unused space */
};

struct header {         /* sizeof(struct header) must be 512 bytes */
    char hd_tcnt[2];            /* # of tuples in relation */
    char hd_tmax[2];            /* maximum # of tuples */
    char hd_data[2];            /* offset to first data byte */
    char hd_size[2];            /* size of each tuple in bytes */
    char hd_unused[HSIZE-8];    /* unused space */
    struct attribute hd_attrs[NATTRS];  /* table of attributes */
};

struct relation {
    char rl_name[RNSIZE];       /* relation name */
    unsigned int rl_tcnt;       /* # of tuples in relation (from hd_tcnt) */
    unsigned int rl_tmax;       /* maximum # of tuples (from hd_tmax) */
    int rl_data;                /* offset to first data byte (from hd_data) */
    int rl_size;                /* size of eachtuple in bytes (from hd_size) */
    int rl_store;               /* flag indicating a store happened */
    int rl_fd;                  /* file descriptor for relation file */
    int rl_scnref;              /* number of scans referencing this relation */
    struct header rl_header;    /* the relation file header block */
    struct relation *rl_next;   /* pointer to next relation */
};

struct scan {
    struct relation *sc_relation;       /* pointer to relation definition */
    unsigned int sc_dtnum;              /* desired tuple number */
    unsigned int sc_atnum;              /* actual tuple number */
    int sc_store;                       /* flag indicating a store happened */
    char *sc_tuple;                     /* tuple buffer */
};

struct srel {
    char *sr_name;                      /* alternate relation name */
    struct scan *sr_scan;               /* relation scan structure ptr */
    int sr_ctuple;                      /* current tuple flag */
    int sr_update;                      /* updated tuple flag */
    struct srel *sr_next;               /* next selected relation in list */
};

struct sattr {
    char *sa_rname;                     /* relation name */
    char *sa_aname;                     /* attribute name */
    char *sa_name;                      /* alternate attribute name */
    char *sa_aptr;                      /* pointer to attr in tuple buffer */
    struct srel *sa_srel;               /* pointer to the selected relation */
    struct attribute *sa_attr;          /* attribute structure ptr */
    struct sattr *sa_next;              /* next selected attribute in list */
};

struct operand {
    int o_type;
    union  {
        struct {
            int ovc_type;
            char *ovc_string;
            int ovc_length;
        } ov_char;
        int ov_boolean;
    } o_value;
};

union codecell {
    int (*c_operator)();
    struct operand *c_operand;
};

struct binding {
    struct attribute *bd_attr;          /* bound attribute */
    char *bd_vtuple;                    /* pointer to value in tuple */
    char *bd_vuser;                     /* pointer to user buffer */
    struct binding *bd_next;            /* next binding */
};

struct sel {
    struct srel *sl_rels;               /* selected relations */
    struct sattr *sl_attrs;             /* selected attributes */
    union codecell *sl_where;           /* where clause */
    struct binding *sl_bindings;        /* user variable bindings */
};

struct mtext {
    char *mt_text;
    struct mtext *mt_next;
};

struct macro {
    char *mc_name;
    struct mtext *mc_mtext;
    struct macro *mc_next;
};

struct ifile {
    char *if_fp;
    struct mtext *if_mtext;
    char *if_cmdline;
    int if_savech;
    char *if_lptr;
    struct ifile *if_next;
};

struct skey {
    int sk_type;
    struct attribute *sk_aptr;
    int sk_start;
    struct skey *sk_next;
};
