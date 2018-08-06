/* SDB - command parser */

#include "stdio.h"
#include "sdbio.h"
#include "ctype.h"

extern int dbv_token;
extern char dbv_tstring[];
extern int dbv_tvalue;
extern struct ifile *dbv_ifp;
extern struct macro *dbv_macros;
extern int dbv_fold;

#ifdef Lattice
int _fmode = 0;  /*dns*/
#endif

/* db_parse - parse a command */
int db_parse(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    int sts;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* determine the statement type */
    switch (db_ntoken()) {
    case ';':   sts = TRUE;
                break;
    case COMPRESS:
                sts = db_squeeze(NULL);
                break;
    case CREATE:
                sts = create();
                break;
    case DEFINE:
                sts = mc_define();
                break;
    case DELETE:
                sts = delete();
                break;
    case EXIT:
                exit();
    case EXPORT:
                sts = db_export(NULL);
                break;
    case EXTRACT:
                sts = db_extract(NULL);
                break;
    case HELP:
                sts = help();
                break;
    case IMPORT:
                sts = db_import(NULL);
                break;
    case INSERT:
                sts = insert();
                break;
    case PRINT:
                sts = print();
                break;
    case SELECT:
                sts = select();
                break;
    case SET:
                sts = set();
                break;
    case SHOW:
                sts = mc_show();
                break;
    case SORT:
                sts = db_sort(NULL);
                break;
    case UPDATE:
                sts = update();
                break ;
    default:
                return (db_ferror(SYNTAX));
    }

    return (sts);
}

/* help - print a short help message */
static int help()
{
    FILE *fp;
    int ch;

    if ((fp = fopen("sdb.hlp","r")) != NULL) {
     /* while ((ch = agetc(fp)) != EOF)    dns */
        while ((ch =  getc(fp)) != EOF)
            putchar(ch);
        fclose(fp);
    }
    else
        printf("No online help available.  Read the manual\n");

    /* return successfully */
    return (TRUE);
}

/* create - create a new relation */
static int create()
{
    struct relation *rptr;
    char aname[STRINGMAX+1];
    int atype;

    /* get relation name */
    if (db_ntoken() != ID)
        return (db_ferror(SYNTAX));

    /* start relation creation */
    if ((rptr = db_rcreate(dbv_tstring)) == NULL)
        return (FALSE);

    /* check for attribute list */
    if (db_ntoken() != '(') {
        free(rptr);
        return (db_ferror(SYNTAX));
    }

    /* parse the attributes */
    while (TRUE) {

        /* get the attribute name */
        if (db_ntoken() != ID) {
            free(rptr);
            return (db_ferror(SYNTAX));
        }
        strcpy(aname,dbv_tstring);

        /* get the attribute type */
        db_ntoken();
        if (dbv_token == CHAR)
            atype = TCHAR;
        else if (dbv_token == NUM)
            atype = TNUM;
        else {
            free(rptr);
            return (db_ferror(SYNTAX));
        }

        /* get the attribute size */
        if (db_ntoken() != NUMBER) {
            free(rptr);
            return (db_ferror(SYNTAX));
        }

        /* add the attribute */
        if (!db_rcattr(rptr,aname,atype,dbv_tvalue)) {
            free(rptr);
            return (FALSE);
        }

        /* check for end of attributes */
        if (db_token() != ID)
            break;
    }

    /* check for attribute list end */
    if (db_ntoken() != ')') {
        free(rptr);
        return (db_ferror(SYNTAX));
    }

    /* check for relation size */
    if (db_ntoken() != NUMBER) {
        free(rptr);
        return (db_ferror(SYNTAX));
    }

    /* finish relation creation */
    if (!db_rcheader(rptr))
        return (FALSE);
    if (!db_rctuples(rptr,dbv_tvalue))
        return (FALSE);
    if (!db_rcdone(rptr))
        return (FALSE);

    /* return successfully */
    return (TRUE);
}

/* insert - insert a tuple into a relation */
static int insert()
{
    struct scan *sptr;
    struct attribute *aptr;
    char aname[ANSIZE+1],avalue[STRINGMAX+1];
    int tcnt,astart,i;

    /* get relation name */
    if (db_token() == ID)
        db_ntoken();
    else
        strcpy(dbv_tstring,"sdbcur");

    /* make sure that the rest of the line is blank */
    if (!db_flush())
        return (FALSE);

    /* open the relation */
    if ((sptr = db_ropen(dbv_tstring)) == NULL)
        return (FALSE);

    /* insert tuples */
    for (tcnt = 0; ; tcnt++) {

        /* print separator if not the first tuple */
        if (tcnt != 0)
            printf("----\n");

        /* get attribute values */
        astart = 1;
        for (i = 0; i < NATTRS; i++) {

            /* get a pointer to the current attribute */
            aptr = &sptr->sc_relation->rl_header.hd_attrs[i];

            /* check for the last attribute */
            if (aptr->at_name[0] == 0)
                break;

            /* get the attribute name */
            stccpy(aname,aptr->at_name,ANSIZE); aname[ANSIZE] = 0;

            /* setup null prompt strings */
            db_prompt(NULL,NULL);

            /* prompt and input attribute value */
            while (TRUE) {
                if (dbv_ifp == NULL)
                    if (strlen(aname) < 8)
                        printf("%s\t\t: ",aname);
                    else
                        printf("%s\t: ",aname);
                if (db_gline(avalue) != NULL)
                    break;
            }

            /* check for last insert */
            if (i == 0 && avalue[0] == EOS)
                break;

            /* store the attribute value */
            db_aput(aptr,&sptr->sc_tuple[astart],avalue);

            /* update the attribute start */
            astart += aptr->at_size;
        }

        /* check for last insert */
        if (avalue[0] == EOS)
            break;

        /* store the new tuple */
        if (!db_rstore(sptr)) {
            db_rclose(sptr);
            return (FALSE);
        }
    }

    /* close the relation */
    db_rclose(sptr);

    /* check number of tuples inserted */
    if (tcnt != 0) {

        /* print tuple count */
        printf("[ %d inserted ]\n",tcnt);
    }
    else
        printf("[ none inserted ]\n");

    /* return successfully */
    return (TRUE);
}

/* delete - delete tuples from a relation */
static int delete()
{
    struct sel *slptr;
    struct srel *srptr;
    int tcnt;

    /* parse the retrieval clause */
    if ((slptr = db_retrieve(NULL)) == NULL)
        return (FALSE);

    /* loop through the retrieved tuples */
    for (tcnt = 0; db_fetch(slptr); tcnt++)

        /* delete the retrieved tuples */
        for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next)
            if (!db_rdelete(srptr->sr_scan)) {
                db_done(slptr);
                return (FALSE);
            }

    /* finish the retrieval */
    db_done(slptr);

    /* check number of tuples deleted */
    if (tcnt != 0) {

        /* print tuple count */
        printf("[ %d deleted ]\n",tcnt);
    }
    else
        printf("[ none deleted ]\n");

    /* return successfully */
    return (TRUE);
}

/* update - update tuples from a relation */
static int update()
{
    struct sel *slptr;
    struct sattr *saptr;
    struct attribute *aptr;
    char aname[ANSIZE+1],avalue[STRINGMAX+1],*ap;
    int tcnt;

    /* parse the selection clause */
    if ((slptr = db_select(NULL)) == NULL)
        return (FALSE);

    /* make sure that the rest of the line is blank */
    if (!db_flush()) {
        db_done(slptr);
        return (FALSE);
    }

    /* loop through the selected tuples */
    for (tcnt = 0; db_fetch(slptr); tcnt++) {

        /* print separator if not the first tuple */
        if (tcnt != 0)
            printf("----\n");

        /* loop through the selected attributes */
        for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {

            /* get the attribute pointer */
            aptr = saptr->sa_attr;

            /* get the attribute name */
            stccpy(aname,aptr->at_name,ANSIZE); aname[ANSIZE] = 0;

            /* get the attribute value */
            db_aget(aptr,saptr->sa_aptr,avalue);
            for (ap = avalue; isspace(*ap); ap++)
                ;

            /* print it */
            if (strlen(aname) < 8)
                printf("%s\t\t: %s\n",aname,ap);
            else
                printf("%s\t: %s\n",aname,ap);

            /* setup null prompt strings */
            db_prompt(NULL,NULL);

            /* prompt and input attribute value */
            while (TRUE) {
                if (strlen(aname) < 8)
                    printf("%s\t\t: ",aname);
                else
                    printf("%s\t: ",aname);
                if (db_gline(avalue) != NULL)
                    break;
            }

            /* store the attribute value */
            if (avalue[0] != EOS) {
                db_aput(aptr,saptr->sa_aptr,avalue);
                saptr->sa_srel->sr_update = TRUE;
            }
        }

        /* update the tuples */
        db_update(slptr);
    }

    /* finish the selection */
    db_done(slptr);

    /* check number of tuples updated */
    if (tcnt != 0) {

        /* print tuple count */
        printf("[ %d updated ]\n",tcnt);
    }
    else
        printf("[ none updated ]\n");

    /* return successfully */
    return (TRUE);
}

/* print - print tuples from a set of relations */
static int print()
{
    struct sel *slptr;
    FILE *ffp,*ofp;
    int tcnt;

    /* parse the using clause */
    if (!using(&ffp,".frm"))
        return (FALSE);

    /* parse the select clause */
    if ((slptr = db_select(NULL)) == NULL)
        return (FALSE);

    /* parse the into clause */
    if (!db_to(&ofp,".txt")) {
        db_done(slptr);
        return (FALSE);
    }

    /* check for normal or formated output */
    if (ffp == NULL)
        tcnt = table(ofp,slptr);
    else
        tcnt = form(ofp,slptr,ffp);

    /* finish the selection */
    db_done(slptr);

    /* close the form definition file */
    if (ffp != NULL)
        fclose(ffp);

    /* close the output file */
    if (ofp != stdout)
        fclose(ofp);

    /* check number of tuples selected */
    if (tcnt != 0)
        printf("[ %d found ]\n",tcnt);
    else
        printf("[ none found ]\n");

    /* return successfully */
    return (TRUE);
}

/* select - select tuples from a set of relations */
static int select()
{
    struct sel *slptr;
    struct relation *rptr;
    struct sattr *saptr;
    char *aname,*tbuf;
    int tcnt,abase,i;

    /* parse the select clause */
    if ((slptr = db_select(NULL)) == NULL)
        return (FALSE);

    /* create a new relation */
    if ((rptr = db_rcreate("sdbcur")) == NULL) {
        db_done(slptr);
        return (FALSE);
    }

    /* create the selected attributes */
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {

        /* decide which attribute name to use */
        if ((aname = saptr->sa_name) == NULL)
            aname = saptr->sa_aname;

        /* add the attribute */
        if (!db_rcattr(rptr,aname,saptr->sa_attr->at_type,
                                  saptr->sa_attr->at_size)) {
            free(rptr);
            db_done(slptr);
            return (FALSE);
        }
    }

    /* create the relation header */
    if (!db_rcheader(rptr)) {
        db_done(slptr);
        return (FALSE);
    }

    /* allocate and initialize a tuple buffer */
    if ((tbuf = calloc(1,rptr->rl_size)) == NULL) {
        db_rcdone(rptr);
        return (db_ferror(INSMEM));
    }
    tbuf[0] = ACTIVE;

    /* loop through the selected tuples */
    for (tcnt = 0; db_fetch(slptr); tcnt++) {

        /* create the tuple from the selected attributes */
        abase = 1;
        for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {
            for (i = 0; i < saptr->sa_attr->at_size; i++)
                tbuf[abase + i] = saptr->sa_aptr[i];
            abase += i;
        }

        /* write the tuple */
        if (write(rptr->rl_fd,tbuf,rptr->rl_size) != rptr->rl_size) {
            db_rcdone(rptr);
            free(tbuf);
            return (db_ferror(INSBLK));
        }
        rptr->rl_tcnt++;
        rptr->rl_tmax++;
    }

    /* finish the selection */
    db_done(slptr);

    /* finish relation creation */
    if (!db_rcdone(rptr))
        return (FALSE);

    /* check number of tuples selected */
    if (tcnt != 0)
        printf("[ %d found ]\n",tcnt);
    else
        printf("[ none found ]\n");

    /* return successfully */
    return (TRUE);
}

/* mc_define - define a macro */
static int mc_define()
{
    struct macro *mptr,*mlast;
    struct mtext *tptr,*tlast;
    char textline[LINEMAX+1];

    /* get macro name */
    if (db_xntoken() != ID)
        return (db_ferror(SYNTAX));

    /* make sure that the rest of the line is blank */
    if (!db_flush())
        return (FALSE);

    /* find the macro in the macro table and free it */
    for (mptr = dbv_macros, mlast = NULL; mptr != NULL; mlast = mptr, mptr = mptr->mc_next)
        if (db_scmp(mptr->mc_name,dbv_tstring) == 0) {
            if (mlast == NULL)
                dbv_macros = mptr->mc_next;
            else
                mlast->mc_next = mptr->mc_next;
            mc_free(mptr);
        }

    /* allocate and initialize a macro structure */
    if ((mptr = malloc(sizeof(struct macro))) == NULL)
        return (db_ferror(INSMEM));
    if ((mptr->mc_name = malloc(strlen(dbv_tstring)+1)) == NULL) {
        free(mptr);
        return (db_ferror(INSMEM));
    }
    strcpy(mptr->mc_name,dbv_tstring);
    mptr->mc_mtext = NULL;

    /* setup null prompt strings */
    db_prompt(NULL,"SDB-DEF> ");

    /* get definition text */
    for (tlast = NULL; ; tlast = tptr) {

        /* get a line */
        db_gline(textline);
        if (textline[0] == EOS || textline[0] == '\n')
            break;

        /* allocate a macro text structure */
        if ((tptr = malloc(sizeof(struct mtext))) == NULL) {
            mc_free(mptr);
            return (db_ferror(INSMEM));
        }
        if ((tptr->mt_text = malloc(strlen(textline)+1)) == NULL) {
            mc_free(mptr);
            return (db_ferror(INSMEM));
        }
        strcpy(tptr->mt_text,textline);
        tptr->mt_next = NULL;

        /* link it into the macro list */
        if (tlast == NULL)
            mptr->mc_mtext = tptr;
        else
            tlast->mt_next = tptr;
    }

    /* link the new macro into the macro list */
    if (tlast == NULL)
        mc_free(mptr);
    else {
        mptr->mc_next = dbv_macros;
        dbv_macros = mptr;
    }

    /* return successfully */
    return (TRUE);
}

/* mc_show - show a macro */
static int mc_show()
{
    struct macro *mptr;
    struct mtext *tptr;

    /* get macro name */
    if (db_xntoken() != ID)
        return (db_ferror(SYNTAX));

    /* find the macro in the macro table */
    for (mptr = dbv_macros; mptr != NULL; mptr = mptr->mc_next)
        if (db_scmp(mptr->mc_name,dbv_tstring) == 0) {
            for (tptr = mptr->mc_mtext; tptr != NULL; tptr = tptr->mt_next)
                printf("\t%s\n",tptr->mt_text);
            break;
        }

    /* check for successful search */
    if (mptr == NULL)
        printf("*** no macro named: %s ***\n",dbv_tstring);

    /* return successfully */
    return (TRUE);
}

/* mc_free - free a macro definition */
static mc_free(mptr)
  struct macro *mptr;
{
    struct mtext *tptr;

    while ((tptr = mptr->mc_mtext) != NULL) {
        mptr->mc_mtext = tptr->mt_next;
        free(tptr->mt_text);
        free(tptr);
    }
    free(mptr->mc_name);
    free(mptr);
}

/* db_to - redirect output into a file */
int db_to(pfp,ext)
  FILE **pfp; char *ext;
{
#ifdef vms
    int fd;
#endif

    /* assume no into clause */
    *pfp = stdout;

    /* check for "into <fname>" */
    if (db_token() != INTO)
        return (TRUE);
    db_ntoken();
    if (db_ntoken() == ID)
        strcat(dbv_tstring,ext);
    else if (dbv_token != STRING)
        return (db_ferror(SYNTAX));

    /* open the output file */
#ifdef vms
    if ((fd = creat(dbv_tstring,0,"rfm=var","rat=cr")) == -1)
        return (db_ferror(OUTCRE));
    *pfp = fdopen(fd,"w");
#else
#ifdef Lattice
    _fmode = 0x8000;  /*dns*/
#endif
    *pfp = fopen(dbv_tstring,"w");  /*dns*/
#ifdef Lattice
    _fmode = 0;       /*dns*/
#endif
    if (*pfp == NULL)               /*dns*/
        return (db_ferror(OUTCRE)); /*dns*/
#endif

    /* return successfully */
    return (TRUE);
}

/* using - get form definition file spec */
static int using(pfp,ext)
  FILE **pfp; char *ext;
{
    /* assume no using clause */
    *pfp = NULL;

    /* check for "using <fname>" */
    if (db_token() != USING)
        return (TRUE);
    db_ntoken();
    if (db_ntoken() == ID)
        strcat(dbv_tstring,ext);
    else if (dbv_token != STRING)
        return (db_ferror(SYNTAX));

    /* open the input file */
    if ((*pfp = fopen(dbv_tstring,"r")) == NULL)
        return (db_ferror(INPFNF));

    /* return successfully */
    return (TRUE);
}

/* table - output a relation table */
static int table(fp,slptr)
  FILE *fp; struct sel *slptr;
{
    int tcnt;

    /* loop through the selected tuples */
    for (tcnt = 0; db_fetch(slptr); tcnt++) {

        /* print table head on first tuple selected */
        if (tcnt == 0)
            db_thead(fp,slptr);

        /* print the tuple */
        db_tentry(fp,slptr);
    }

    /* print table foot */
    if (tcnt != 0)
        db_tfoot(fp,slptr);

    /* return the tuple count */
    return (tcnt);
}

/* form - process a form */
static int form(ofp,slptr,ffp)
  FILE *ofp; struct sel *slptr; FILE *ffp;
{
    char aname[ANSIZE+1];
    int ch,tcnt;

    /* loop through the selected tuples */
    for (tcnt = 0; db_fetch(slptr); tcnt++) {

        /* reposition the form definition file */
        fseek(ffp,0L,0);

        /* process the form */
        while ((ch = getc(ffp)) != -1)
            if (ch == '<') {
                get_aname(ffp,aname);
                put_avalue(ofp,slptr,aname);
            }
            else
                putc(ch,ofp);
    }

    /* return the tuple count */
    return (tcnt);
}

/* get_aname - get an attribute name */
static get_aname(fp,aname)
  FILE *fp; char *aname;
{
    int ch;

    while ((ch = getc(fp)) != '>')
        if (!isspace(ch))
            *aname++ = ch;
    *aname = 0;
}

/* put_avalue - output attribute value */
static put_avalue(fp,slptr,aname)
  FILE *fp; struct sel *slptr; char *aname;
{
    struct sattr *saptr;
    char *saname;
    int i;

    /* loop through the selected attributes */
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {

        /* check the selected attribute name */
        if ((saname = saptr->sa_name) == NULL)
            saname = saptr->sa_aname;
        if (db_scmp(saname,aname) == 0)
            break;
    }

    if (saptr == NULL) {
        fprintf(fp,"<error>");
        return;
    }

    /* get the attribute value */
    for (i = 0; i < saptr->sa_attr->at_size; i++)
        if (saptr->sa_aptr[i] != 0)
            putc(saptr->sa_aptr[i],fp);
        else
            putc(' ',fp);
}

/* set - set internal parameters */
static int set()
{
    int value;

    /* process each set request */
    while (db_token() == ID) {

        /* skip the identifier */
        db_ntoken();

        /* check for "no" */
        if (db_scmp(dbv_tstring,"no") == 0) {
            value = FALSE;
            if (db_token() != ID)
                return (db_ferror(BADSET));
            db_ntoken();
        }
        else
            value = TRUE;

        /* check for parameter to set */
        if (db_scmp(dbv_tstring,"fold") == 0)
            dbv_fold = value;
        else
            return (db_ferror(BADSET));
    }

    /* return successfully */
    return (TRUE);
}

