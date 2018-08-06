/* SDB - token scanning routines */

#include "stdio.h"
#include "sdbio.h"
#include "ctype.h"

int dbv_token;                          /* current token */
int dbv_tvalue;                         /* integer token value */
char dbv_tstring[STRINGMAX+1];          /* string token value */
struct ifile *dbv_ifp;                  /* indirect file context */
struct macro *dbv_macros;               /* macro definitions */
int dbv_fold;                           /* case fold alpha comparisons */

static char *iprompt,*cprompt;          /* input prompts */
static char cmdline[LINEMAX+2],*lptr;   /* current line and pointer */
static int atbol;                       /* flag indicating at bol */
static int savech;                      /* lookahead character */
static int savetkn;                     /* lookahead token */
static char *keywords[] = {             /* keyword table */
    "ascending",
    "by",
    "char",
    "compress",
    "create",
    "define",
    "delete",
    "descending",
    "exit",
    "export",
    "extract",
    "from",
    "help",
    "insert",
    "import",
    "into",
    "num",
    "print",
    "select",
    "set",
    "show",
    "sort",
    "update",
    "using",
    "where",
    NULL
};
static int keytokens[] = {              /* token values for each keyword */
    ASCENDING,
    BY,
    CHAR,
    COMPRESS,
    CREATE,
    DEFINE,
    DELETE,
    DESCENDING,
    EXIT,
    EXPORT,
    EXTRACT,
    FROM,
    HELP,
    INSERT,
    IMPORT,
    INTO,
    NUM,
    PRINT,
    SELECT,
    SET,
    SHOW,
    SORT,
    UPDATE,
    USING,
    WHERE,
    NULL
};

/* db_sinit - initialize the scanner */
db_sinit()
{
    /* at beginning of line */
    atbol = TRUE;

    /* make the command line null */
    lptr = NULL;

    /* no lookahead yet */
    savech = EOS;
    savetkn = NULL;

    /* no indirect command files */
    dbv_ifp = NULL;

    /* no macros defined */
    dbv_macros = NULL;

    /* fold alpha comparisons */
    dbv_fold = TRUE;
}

/* db_prompt(ip,cp) - initialize prompt strings */
db_prompt(ip,cp)
  char *ip,*cp;
{
    /* save initial and continuation prompt strings */
    iprompt = ip;
    cprompt = cp;
}

/* db_scan(fmt,args) - initiate line scan command parsing */
db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)
{
    /* convert the command line and arguments */
    sprintf(cmdline,fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);

    /* start at the beginning of the command line */
    lptr = cmdline;
    iprompt = NULL;
    dbv_ifp = NULL;

    /* no lookahead yet */
    savech = EOS;
    savetkn = NULL;

    /* fold alpha comparisons */
    dbv_fold = TRUE;
}

/* db_flush - flush the current input line */
int db_flush()
{
    while (savech != '\n')
        if (savech > ' ')
            return (db_ferror(SYNTAX));
        else
            savech = getchx();

    savech = EOS;
    atbol = TRUE;
    return (TRUE);
}

/* db_gline - get a line from the current input */
char *db_gline(buf)
  char *buf;
{
    int ch,i;

    for (i = 0; (ch = getch()) != '\n' && ch != -1; )
        if (i < LINEMAX)
            buf[i++] = ch;
        else {
            printf("*** line too long ***\nRetype> ");
            fflush(stdout) ;
            i = 0;
        }
    buf[i] = EOS;

    return (buf);
}

/* db_ifile - setup an indirect command file */
int db_ifile(fname)
  char *fname;
{
    struct ifile *new_ifp;

    if ((new_ifp = malloc(sizeof(struct ifile))) == NULL)
        return (db_ferror(INSMEM));
    else if ((new_ifp->if_fp = fopen(fname,"r")) == NULL) {
        free(new_ifp);
        return (db_ferror(INDFNF));
    }
    new_ifp->if_mtext = NULL;
    new_ifp->if_savech = savech;
    new_ifp->if_lptr = lptr;
    new_ifp->if_next = dbv_ifp;
    dbv_ifp = new_ifp;

    /* return successfully */
    return (TRUE);
}

/* db_kill - kill indirect command file input */
db_kill()
{
    struct ifile *old_ifp;

    while ((old_ifp = dbv_ifp) != NULL) {
        dbv_ifp = old_ifp->if_next;
        if (old_ifp->if_fp != NULL)
            fclose(old_ifp->if_fp);
        savech = old_ifp->if_savech;
        lptr = old_ifp->if_lptr;
        free(old_ifp);
    }

    while (savech != '\n')
        savech = getchx();

    savech = EOS;
    savetkn = NULL;
    atbol = TRUE;
}

/* db_token - return the current input token */
int db_token()
{
    struct macro *mptr;
    struct ifile *new_ifp;

    /* find a token that's not a macro call */
    while (db_xtoken() == ID) {

        /* check for a macro call */
        for (mptr = dbv_macros; mptr != NULL; mptr = mptr->mc_next)
            if (db_scmp(dbv_tstring,mptr->mc_name) == 0) {
                if ((new_ifp = malloc(sizeof(struct ifile))) == NULL)
                    printf("*** error expanding macro: %s ***\n",dbv_tstring);
                else {
                    new_ifp->if_fp = NULL;
                    new_ifp->if_mtext = mptr->mc_mtext->mt_next;
                    new_ifp->if_lptr = lptr; lptr = mptr->mc_mtext->mt_text;
                    new_ifp->if_savech = savech; savech = EOS;
                    new_ifp->if_next = dbv_ifp;
                    dbv_ifp = new_ifp;
                }
                savetkn = NULL;
                break;
            }

        if (mptr == NULL)
            break;
    }

    return (dbv_token);
}

/* db_xtoken - return the current input token */
int db_xtoken()
{
    int ch;

    /* check for a saved token */
    if ((dbv_token = savetkn) != NULL)
        return (dbv_token);

    /* get the next non-blank character */
    ch = nextch();

    /* check type of character */
    if (isalpha(ch))                    /* identifier or keyword */
        get_id();
    else if (isdigit(ch))               /* number */
        get_number();
    else if (ch == '"')                 /* string */
        get_string();
    else if (get_rel())                 /* relational operator */
        ;
    else                                /* single character token */
        dbv_token = getch();

    /* save the lookahead token */
    savetkn = dbv_token;

    /* return the token */
    return (dbv_token);
}

/* db_ntoken - get next token (after skipping the current one) */
int db_ntoken()
{
    /* get the current token */
    db_token();

    /* make sure another is read on next call */
    savetkn = NULL;

    /* return the current token */
    return (dbv_token);
}

/* db_xntoken - get next token (after skipping the current one) */
int db_xntoken()
{
    /* get the current token */
    db_xtoken();

    /* make sure another is read on next call */
    savetkn = NULL;

    /* return the current token */
    return (dbv_token);
}

/* db_scmp - compare two strings */
int db_scmp(str1,str2)
  char *str1,*str2;
{
    if (dbv_fold)
        return (scmp(str1,str2));
    else
        return (strcmp(str1,str2));
}

/* db_sncmp - compare two strings with a maximum length */
int db_sncmp(str1,str2,len)
  char *str1,*str2; int len;
{
    if (dbv_fold)
        return (sncmp(str1,str2,len));
    else
        return (strncmp(str1,str2,len));
}

/* scmp - compare two strings with alpha case folding */
static int scmp(str1,str2)
  char *str1,*str2;
{
    int ch1,ch2;

    /* compare each character */
    while (*str1 && *str2) {

        /* fold the character from the first string */
        if (isupper(*str1))
            ch1 = tolower(*str1++);
        else
            ch1 = *str1++;

        /* fold the character from the second string */
        if (isupper(*str2))
            ch2 = tolower(*str2++);
        else
            ch2 = *str2++;

        /* compare the characters */
        if (ch1 != ch2)
            if (ch1 < ch2)
                return (-1);
            else
                return (1);
    }

    /* check for strings of different lengths */
    if (*str1 == *str2)
        return (0);
    else if (*str1 == 0)
        return (-1);
    else
        return (1);
}

/* sncmp - compare two strings with alpha case folding and a maximum length */
static int sncmp(str1,str2,len)
  char *str1,*str2; int len;
{
    int ch1,ch2;

    /* compare each character */
    while (*str1 && *str2 && len > 0) {

        /* fold the character from the first string */
        if (isupper(*str1))
            ch1 = tolower(*str1++);
        else
            ch1 = *str1++;

        /* fold the character from the second string */
        if (isupper(*str2))
            ch2 = tolower(*str2++);
        else
            ch2 = *str2++;

        /* compare the characters */
        if (ch1 != ch2)
            if (ch1 < ch2)
                return (-1);
            else
                return (1);

        /* decrement the string length */
        len--;
    }

    /* check for strings of different lengths */
    if (len == 0 || *str1 == *str2)
        return (0);
    else if (*str1 == 0)
        return (-1);
    else
        return (1);
}

/* get_id - get a keyword or a user identifier */
static get_id()
{
    int ch,nchars,i;

    /* input letters and digits */
    ch = nextch();
    nchars = 0;
    while (isalpha(ch) || isdigit(ch)) {
        if (nchars < KEYWORDMAX)
            dbv_tstring[nchars++] = ch;
        getch(); ch = thisch();
    }

    /* terminate the keyword */
    dbv_tstring[nchars] = EOS;

    /* assume its an identifier */
    dbv_token = ID;

    /* check for keywords */
    for (i = 0; keywords[i] != NULL; i++)
        if (db_scmp(dbv_tstring,keywords[i]) == 0)
            dbv_token = keytokens[i];
}

/* get_number - get a number */
static get_number()
{
    int ch,ndigits,nodot;

    /* read digits and at most one decimal point */
    ch = nextch();
    ndigits = 0; nodot = TRUE;
    while (isdigit(ch) || (nodot && ch == '.')) {
        if (ch == '.')
            nodot = FALSE;
        if (ndigits < NUMBERMAX)
            dbv_tstring[ndigits++] = ch;
        getch(); ch = thisch();
    }

    /* terminate the number */
    dbv_tstring[ndigits] = EOS;

    /* get the value of the number */
    sscanf(dbv_tstring,"%d",&dbv_tvalue);

    /* token is a number */
    dbv_token = NUMBER;
}

/* get_string - get a string */
static get_string()
{
    int ch,nchars;

    /* skip the opening quote */
    getch();

    /* read characters until a closing quote is found */
    ch = thisch();
    nchars = 0;
    while (ch && ch != '"') {
        if (nchars < STRINGMAX)
            dbv_tstring[nchars++] = ch;
        getch(); ch = thisch();
    }

    /* terminate the string */
    dbv_tstring[nchars] = EOS;

    /* skip the closing quote */
    getch();

    /* token is a string */
    dbv_token = STRING;
}

/* get_rel - get a relational operator */
static int get_rel()
{
    int ch;

    switch (ch = nextch()) {
    case '=':
            getch();
            dbv_token = EQL;
            return (TRUE);;
    case '<':
            getch(); ch = nextch();
            if (ch == '>') {
                getch();
                dbv_token = NEQ;
            }
            else if (ch == '=') {
                getch();
                dbv_token = LEQ;
            }
            else
                dbv_token = LSS;
            return (TRUE);;
    case '>':
            getch(); ch = nextch();
            if (ch == '=') {
                getch();
                dbv_token = GEQ;
            }
            else
                dbv_token = GTR;
            return (TRUE);;
    default:
            return (FALSE);
    }
}

/* getch - get the next character */
static int getch()
{
    char fname[STRINGMAX+1];
    int ch,i;

    /* return the lookahead character if there is one */
    if (savech != EOS) {
        ch = savech;
        savech = EOS;
        return (ch);
    }

    /* get a character */
    ch = getchx();

    /* skip spaces at the beginning of a command */
    if (atbol && iprompt != NULL)
        while (ch <= ' ')
            ch = getchx();

    /* use continuation prompt next time */
    iprompt = NULL;

    /* check for indirect command file */
    while (ch == '@') {
        for (i = 0; (savech = getchx()) > ' '; )
            if (i < STRINGMAX)
                fname[i++] = savech;
        fname[i] = 0;
        if (db_ifile(fname) != TRUE)
            printf("*** error opening command file: %s ***\n",fname);
        ch = getchx();
    }

    /* return the character */
    return (ch);
}

/* getchx - get the next character */
static int getchx()
{
    struct ifile *old_ifp;
    int ch;

    /* check for input from buffer */
    if (lptr != NULL) {
        while (*lptr == EOS)
            if (dbv_ifp != NULL)
                if (dbv_ifp->if_mtext == NULL) {
                    old_ifp = dbv_ifp;
                    ch = dbv_ifp->if_savech; savech = EOS;
                    lptr = dbv_ifp->if_lptr;
                    dbv_ifp = dbv_ifp->if_next;
                    free(old_ifp);
                    if (ch != EOS)
                        return (ch);
                    if (lptr == NULL)
                        break;
                }
                else {
                    lptr = dbv_ifp->if_mtext->mt_text;
                    dbv_ifp->if_mtext = dbv_ifp->if_mtext->mt_next;
                }
            else
                return (EOS);

        if (lptr != NULL)
            return (*lptr++);
    }

    /* print prompt if necessary */
    if (atbol && dbv_ifp == NULL)  {  /*dns*/
        if (iprompt != NULL)
            printf("%s",iprompt);
        else if (cprompt != NULL)
            printf("%s",cprompt);
#ifdef Lattice
        fflush(stdout); /*dns*/
#endif
        } /*dns*/

    if (dbv_ifp == NULL)
        if ((ch = getcx(stdin)) == '\n')
            atbol = TRUE;
        else
            atbol = FALSE;
    else {
        if ((ch = getcx(dbv_ifp->if_fp)) == -1) {
            old_ifp = dbv_ifp;
            ch = dbv_ifp->if_savech; savech = EOS;
            lptr = dbv_ifp->if_lptr;
            dbv_ifp = dbv_ifp->if_next;
            fclose(old_ifp->if_fp);
            free(old_ifp);
        }
    }

    /* return the character */
    return (ch);
}

/* thisch - get the current character */
static int thisch()
{
    /* get a lookahead character */
    if (savech == EOS)
        savech = getch();

    /* return lookahead character */
    return (savech);
}

/* nextch - get the next non-blank character */
static int nextch()
{
    int ch;

    /* skip blank characters */
    while ((ch = thisch()) <= ' ' && ch != EOS)
        getch();

    /* return the first non-blank */
    return (ch);
}

