/*Tcpg - c program source listing formatter */
/*F cpg.c **********************************************************
 *
 *                           cpg.c
 *
 *    DESCRIPTION OF FILE CONTENTS:
 *      C source program listing formatter source.
 *
 *  Cpg provides the facility to print out a C language source file
 *  with headers, nesting level indicators, and table of contents.
 *  It makes use of "triggers" for page headings, titles and
 *  subtitles, and pagination.  It also recognizes function
 *  declarations and form feeds and treats them appropriately.
 *
 *******************************************************************/
/*E*/
/*S includes, defines, and globals */
/*P*/
#include <stdio.h>
#include    <ctype.h>
#include    <time.h>

#define EQ ==
#define NE !=
#define GT >
#define GE >=
#define LT <
#define LE <=
#define OR ||
#define AND &&

#define TRUE 1
#define FALSE 0
#define YES 1
#define NO 0

#define SPACE ' '
#define NUL '\0'

typedef short   BOOL;

#define INULL -32768
#define LNULL -2147483648

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))

#define LINESINHEAD 6
#define LPP 60
#define MAXWIDTH    132

#define notend(ll) ((ll[0] EQ SLASH AND ll[1] EQ STAR AND ll[2] EQ 'E') ? FALSE : TRUE)
#define SLASH   '/'
#define STAR    '*'
#define DQUOTE '"'
#define SQUOTE '\''
#define BSLASH '\\'

#ifdef BSD
#define strrchr rindex
#define strchr index
#endif

extern char *strrchr ();
extern char *strchr ();

char    *basename ();

char    tim_lin[40];
char    *file_name;
char    fh_name[50] = "";
char    fnc_name[40] = "";
char    subttl[70] = "";
char    title[70] = "";
char    tocname[] = "/tmp/toc_XXXXXX";

int     nlvl = 0;

int     page_line = LPP+1;
int     pageno = 1;

int     tabstop = 8;

int     infunc = FALSE;
int     logging = 0;
int     BASENAMES = FALSE;

int     incomment = FALSE;
int     insquote = FALSE;
int     indquote = FALSE;

char    specline = FALSE;

FILE    *tocfile;
FILE    *fd;

char    *pgm;

char    *ReservedWord[]  = { 
     "auto", "bool", "break", "case", "char", "continue",
     "default", "do", "double", "else", "entry", "enum",
     "extern", "float", "for", "goto", "if",
     "int", "long", "register", "return", "short",
     "sizeof", "static", "struct", "switch",
     "typedef", "union", "unsigned", "void", "while",
     NULL };

/*S main function */
/*Hmain */
/*E*/

main (ac, av)
int     ac;
char    **av;
{
    char    *std_input = "standard input";  /* input file name      */

    long    cur_time;               /* place for current raw time   */

    register int i;                 /* temporary for indexes, etc.  */

    struct tm *tim;                 /* return from localtime        */
    struct tm *localtime ();

    char    cmdbuf[40];             /* place to format sort command */

    extern char *optarg;            /* option argument pointer      */
    extern int   optind;            /* option index                 */

    pgm = basename (av[0]);

    while ((i = getopt (ac, av, "bl:t:")) NE EOF)
    {
        switch (i)
        {
            case    'b':
                BASENAMES = TRUE;
                break;
            case    'l':
                logging = atoi (optarg);
                break;
            case    't':
                tabstop = atoi (optarg);
                break;
            default:
                fprintf (stderr,
        "usage: %s [ -b<asename> ] [ -t <tabstop> ] [ files... ]\n",
                        pgm);
        }
    }

    /* ------------------------------------------------------------ */
    /* set up the date/time portion of page headings                */
    /* ------------------------------------------------------------ */

    time(&cur_time);

    tim = localtime (&cur_time);
    sprintf (tim_lin, "Printed: %02d/%02d/%02d at %2d:%02d %s",
        tim->tm_mon + 1, tim->tm_mday, tim->tm_year,
        tim->tm_hour GT 12 ? tim->tm_hour - 12 : tim->tm_hour,
        tim->tm_min,
        tim->tm_hour GE 12 ? "PM" : "AM" );

    /* ------------------------------------------------------------ */
    /* create the temporary file for the table of contents          */
    /*   don't bother if output is to a terminal                    */
    /* ------------------------------------------------------------ */

    mktemp (tocname);
    if (!isatty (1))
    {
        tocfile = fopen (tocname, "w");
        if (!tocfile)
        {
            fprintf (stderr, "%s: unable to create tocfile %s\n",
                pgm, tocname);
            exit (2);
        }
    }

    /* ------------------------------------------------------------ */
    /* if no file names, read standard input                        */
    /* ------------------------------------------------------------ */

    if (optind EQ ac)
    {
        fd = stdin;
        file_name = std_input;
        dofile (fd);
    }
    else
    {
    /* ------------------------------------------------------------ */
    /* process each file named on the command line                  */
    /* ------------------------------------------------------------ */

        for (i = optind; i LT ac; i++)
        {
    /* ------------------------------------------------------------ */
    /* special file name `-' is standard input                      */
    /* ------------------------------------------------------------ */

            if (strcmp (av[i], "-") EQ 0)
            {
                fd = stdin;
                file_name = std_input;
            }
            else
            {
                fd = fopen (av[i], "r");
                if (fd EQ NULL)
                {
                    fprintf (stderr,
                        "cpg: unable to open %s\n", av[i]);
                }
            }
            if (fd NE NULL)
            {
                if (BASENAMES) strcpy (fh_name, basename (av[i]));
                else strcpy (fh_name, av[i]);
                file_name = av[i];
                dofile (fd);
                fclose (fd);
            }
        }
    }

    fflush (stdout);

    /* ------------------------------------------------------------ */
    /* sort and print the table of contents - straight alpha order  */
    /* on function and file name                                    */
    /* ------------------------------------------------------------ */

    if (!isatty (1))
    {
        fclose (tocfile);
        sprintf (cmdbuf, "sort +1 -2 +0 -1 -u -o %s %s", tocname, tocname);
        system (cmdbuf);
        tocfile = fopen (tocname, "r");
        if (!tocfile)
        {
            fprintf (stderr, "%s: unable to read tocfile\n", pgm);
            exit (2);
        }
        else
        {
            tocout (tocfile);
            fclose (tocfile);
            if (!logging) unlink (tocname);
        }
    }

    fprintf (stdout, "\f");

    exit (0);
}
/*Sdofile - process an input file */
/*Hdofile*/
/*E*/
dofile (fd)
FILE    *fd;
{
    register int i;                 /* temporary                    */

    int     lineno = 1;             /* line number in current file  */

    register char *line;            /* current line pointer         */

    char    ibuf[MAXWIDTH];         /* original input line          */
    char    ebuf[MAXWIDTH];         /* line with tabs expanded      */

    register char *p;               /* temporary char pointer       */

    /* ------------------------------------------------------------ */
    /* initialize the function name to `.' - unknown                */
    /* retrieve the basename portion of the file name               */
    /* ------------------------------------------------------------ */

    strcpy (fnc_name, ".");

    /* ------------------------------------------------------------ */
    /* if building TOC, add this entry                              */
    /* ------------------------------------------------------------ */

    if (!isatty (1))
        fprintf (tocfile,
            "%s %s %d %d\n", fh_name, fnc_name, pageno, lineno);

    /* ------------------------------------------------------------ */
    /* if tabs are to be expanded, use the expansion buffer         */
    /* ------------------------------------------------------------ */

    if (tabstop) line = ebuf;
    else         line = ibuf;

    /* ------------------------------------------------------------ */
    /* process each line in the file, looking for triggers          */
    /* ------------------------------------------------------------ */

    while (fgets (ibuf, MAXWIDTH, fd) NE NULL)
    {
        if (logging GE 9) fprintf (stderr, "%s: LOG: %s", pgm, line);
    /* ------------------------------------------------------------ */
    /* expand the input line                                        */
    /* ------------------------------------------------------------ */

        expand (ebuf, ibuf);

        if (line[0] EQ SLASH AND line[1] EQ STAR)
        {
    /* ------------------------------------------------------------ */
    /* comment found - could be a trigger                           */
    /* ------------------------------------------------------------ */

            switch (line[2])
            {
                case 'F':
                case 'H':
                {
                    if (logging GE 9) fprintf (stderr, "F/H header\n");
                    header (&lineno, line, fd);
                    break;
                }
                case 'P':
                {
                    if (logging GE 9) fprintf (stderr, "page break\n");
                    print_head ();
                    lineno++;
                    break;
                }
                case 'S':
                {
                    if (logging GE 9) fprintf (stderr, "subtitle\n");
                    getname (line, subttl);
                    lineno++;
                    break;
                }
                case 'T':
                {
                    if (logging GE 9) fprintf (stderr, "title\n");
                    getname (line, title);
                    /* print_head (); */
                    lineno++;
                    break;
                }
                default:
                {
                    if (logging GE 9) fprintf (stderr, "other comment\n");
                    print (&lineno, line);
                    break;
                }
            }
        }
        else
        {
    /* ------------------------------------------------------------ */
    /* not a comment - check for function declaration               */
    /* if a form feed is found, start a new page with header        */
    /* ------------------------------------------------------------ */

            if (logging GE 9) fprintf (stderr, "not a comment\n");
            if (!nlvl AND !isatty (1)) infunc = ckfunc (lineno, line);
            if (*line EQ '\f') print_head ();
            else print (&lineno, line);
        }
    }

    page_line = LPP+1;      /* force new page after file            */
    title[0] = NUL;         /* clear title and subtitle             */
    subttl[0] = NUL;

    return;
}
/*Sheader - construct and print header box */
/*Hheader*/
/*E*/
header  (lineno, line, fd)
register int     *lineno;
register char    *line;
register FILE    *fd;
{
    register char *p;

    if (line[2] EQ 'F')
    {
        getname (line, fh_name);
        if (BASENAMES) strcpy (fh_name, basename (fh_name));
        strcpy (fnc_name, ".");
    }
    else if (line[2] EQ 'H')
    {
        getname (line, fnc_name);
    }

    if (!isatty (1))
        fprintf (tocfile,
            "%s %s %d %d\n", fh_name, fnc_name, pageno, *lineno);

    print_head ();

    print (lineno, line);

    while (fgets (line, MAXWIDTH, fd) NE NULL AND
            notend (line))
    {
        if (line[0] EQ SLASH AND line[1] EQ STAR AND line[2] EQ 'P')
        {
            print_head ();
            (*lineno)++;
        }
        else
        {
            print (lineno, line);
        }
    }

    print (lineno, line);

    return;
}
/*Sgetname - get a string from a signal line */
/*Hgetname */
/*E*/
getname (line, name)
register char    *line;
register char    *name;
{
    register int     i;
    register int     j;

    /* ------------------------------------------------------------ */
    /* skip leading spaces in the trigger line                      */
    /* copy up to trailing asterisk or end-of-line                  */
    /* strip trailing spaces                                        */
    /* ------------------------------------------------------------ */

    for (i = 3; isspace(line[i]) AND i LT MAXWIDTH; i++);

    for (j = 0; line[i] AND line[i] NE '*'; i++, j++)
    {
        name[j] = line[i];
    }

    while (j-- GT 0 AND isspace (name[j]));

    name[++j] = NUL;

    return;
}
/*Sprint - print a line with line number */
/*Hprint */
/*E*/
print (lineno, line)
register int     *lineno;
register char    *line;
{
    register int llen = strlen (line);
    register int i;

    register char sc = specline ? '*' : ' ';

    int     j = 0;

    register char    dc = NUL;

    /* ------------------------------------------------------------ */
    /* new page with header if page length is exceeded              */
    /* ------------------------------------------------------------ */

    if (page_line GT LPP)
    {
        print_head ();
    }

    /* ------------------------------------------------------------ */
    /* if brace(s) found,                                           */
    /*   modify the nesting level by the net nesting delta          */
    /*   select the indicator according to the net delta            */
    /*   if nexting is back to zero (none), clear function name     */
    /* ------------------------------------------------------------ */

    if (fnd (line, &j))
    {
        nlvl += j;

        if (j LT 0) dc = '<';
        else if (j EQ 0) dc = '*';
        else dc = '>';

        i = nlvl;
        if (j LT 0) i++;
        fprintf (stdout, "%4d%c%2d%c ",
            (*lineno)++, sc, i, dc);
        if (nlvl EQ 0) strcpy (fnc_name, ".");
    }
    else
    {
        fprintf (stdout, "%4d%c    ", (*lineno)++, sc);
    }

    /* ------------------------------------------------------------ */
    /* break up long lines by finding the first space form the end  */
    /* ------------------------------------------------------------ */

    if (llen GT 132)
    {
        for (i = 132; i GE 0; i--)
        {
            if (line[i] EQ SPACE)
            {
                fprintf (stdout, "%*.*s \\\n", i, i, line);
                page_line++;
                break;
            }
        }

        j = 79 - (llen - i);

        for (j; j GE 0; j--) putc (SPACE, stdout);

        fprintf (stdout, "%s", &line[i+1]);
    }
    else
    {
        fprintf (stdout, "%s", line);
    }

    page_line++;

    specline = FALSE;       /* true if function declaration     */

    return;
}
/*Sprint_head - print the page heading with page number */
/*Hprint_head */
/*E*/
print_head ()
{
    char    headbuf[80];
    register int len;

    sprintf (headbuf, "[ %s | %s <- %s",
        tim_lin, fh_name, fnc_name);

    for (len = strlen (headbuf); len LT 68; len++) headbuf[len] = SPACE;

    sprintf (&headbuf[68], "Page %-4d ]", pageno++);
    fprintf (stdout, "\f\n");
    if (!isatty (1))
        fprintf (stdout, "_______________________________________\
________________________________________");
    fprintf (stdout, "\n%s\n", headbuf);
    fprintf (stdout, "[-------------------------------+------\
---------------------------------------]\n");

    if (*title)
    {
        sprintf (headbuf, "[    %s", title);
    }
    else
    {
        sprintf (headbuf, "[    %s", fh_name);
    }
    for (len = strlen (headbuf); len LT 78; len++) headbuf[len] = SPACE;
    headbuf[78] = ']';
    fprintf (stdout, "%s\n", headbuf);

    if (*subttl)
    {
        sprintf (headbuf, "[    %s", subttl);
    }
    else
    {
        sprintf (headbuf, "[    %s", fnc_name);
    }
    for (len = strlen (headbuf); len LT 78; len++) headbuf[len] = SPACE;
    headbuf[78] = ']';
    fprintf (stdout, "%s", headbuf);

    if (!isatty (1))
        fprintf (stdout, "\r_______________________________________\
________________________________________");
    fprintf (stdout, "\n\n");

    page_line = LINESINHEAD;

    return;
}
/*S fnd - return true if a brace is found */
/*H fnd */
/*E*/
fnd (in, nchg)
register char *in;
register int    *nchg;
{
#   define LBRACE   '{'
#   define RBRACE   '}'
#   define SHARP    '#'
#   define COLON    ':'

    register found = FALSE;         /* true if keyword found        */

    register char blank = TRUE;     /* used to check for shell/make */
                                    /* comments beginning with #/:  */
    register int inshcomment = FALSE;   /* true if in shell comment */

    *nchg = 0;              /* initialize net change to zero        */

    /* ------------------------------------------------------------ */
    /* check each character of the line                             */
    /* ------------------------------------------------------------ */

    for (in; *in; in++)
    {
        if (!incomment AND !inshcomment AND !indquote AND !insquote)
        {
            if (logging GE 9) fprintf (stderr, "not in comment or quote\n");
            if (*in EQ SLASH AND *(in+1) EQ STAR)
            {
                incomment = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "new comment\n");
            }
            else if (blank AND
                     ((*in EQ SHARP OR *in EQ COLON) AND
                     (*(in+1) NE LBRACE AND *(in+1) NE RBRACE))
                    )
            {
                inshcomment = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "new shell comment\n");
            }
            else if (*in EQ DQUOTE AND
                    (*(in-1) NE BSLASH OR *(in-2) EQ BSLASH))
            {
                indquote = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "new dquote\n");
            }
            else if (*in EQ SQUOTE AND
                    (*(in-1) NE BSLASH OR *(in-2) EQ BSLASH))
            {
                insquote = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "new squote\n");
            }
            else if (*in EQ LBRACE)
            {
                (*nchg)++;
                found = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "nest in\n");
            }
            else if (*in EQ RBRACE)
            {
                (*nchg)--;
                found = TRUE;
                blank = FALSE;
                if (logging GE 9) fprintf (stderr, "nest out\n");
            }
            else if (!isspace (*in))
            {
                blank = FALSE;
            }
        }
        else if (incomment AND *in EQ STAR AND *(in+1) EQ SLASH)
        {
            incomment = FALSE;
            if (logging GE 9) fprintf (stderr, "end comment\n");
        }
        else if (indquote AND *in EQ DQUOTE AND
                (*(in-1) NE BSLASH OR *(in-2) EQ BSLASH))
        {
            indquote = FALSE;
            if (logging GE 9) fprintf (stderr, "end dquote\n");
        }
        else if (insquote AND *in EQ SQUOTE AND
                (*(in-1) NE BSLASH OR *(in-2) EQ BSLASH))
        {
            insquote = FALSE;
            if (logging GE 9) fprintf (stderr, "end squote\n");
        }
    }
    
    return found;
}
/*Stocout - print out the table of contents */
/*Htocout */
/*E*/
tocout (toc)
FILE    *toc;
{
    char    buf[80];
    char    filenam[80];
    char    fncnam[80];
    int     page;
    int     line;

    char    outline[80];

    register int toclines = 99;

    while (fscanf (toc, "%s%s%d%d", filenam, fncnam, &page, &line) EQ 4)
    {
        if (toclines GT 54)
        {
            printf ("\f\n\
                             _____________________\n\
                             [ TABLE OF CONTENTS ]\r\
                             _____________________\n\n\
                File -> Function                     Page    Line\r\
________________________________________\
________________________________________\n\n");
            toclines = 0;
        }

        toclines++;

        printf ("\
    %16s -> %-16.16s ............ %3d   %5d\n",
            filenam, *fncnam EQ '.' ? "START" : fncnam, page, line);
    }
    return;
}
/*S expand - expand tabs to tabstop */
/*H expand */
/*E*/
expand (to, from)
register char *to;
register char *from;
{
    register int i;
    register int tofill;

#   define BACKSPACE '\b'
#   define FORMFEED '\f'
#   define NEWLINE '\n'
#   define RETURN '\r'
#   define TAB '\t'
    
    i = 0;

    while (*from)
    {
        switch (*from)
        {
            case    TAB:
                tofill = tabstop - (i % tabstop);
                i += tofill;
                while (tofill--) *(to++) = SPACE;
                break;
            case    NEWLINE:
            case    RETURN:
                i = 0;
            case    FORMFEED:
                *(to++) = *from;
                break;
            case    BACKSPACE:
                i--;
                *(to++) = *from;
                break;
            default:
                i++;
                *(to++) = *from;
                break;
        }

        from++;
    }

    *to = NUL;

    return;
}
/*S ckfunc - check line for function declaration */
/*H ckfunc */
/*E*/

#define isidchr(c) (isalnum(c) || (c == '_'))

ckfunc (lineno, s)
register int lineno;
register char   *s;
{
    register char *p;
    register int  Cnt;
    register int  i;
    register int  result;
    register char found = FALSE;

    static char *_fnm = "ckfunc";

    char FunctionName[40];

    if (logging GE 3)
    {
        fprintf (stderr,
            "%s<%s>: LOG: ckfunc called - line = %s",
            pgm, _fnm, s);
    }

    if(!strcmp (fnc_name, ".") AND !incomment && !indquote && !insquote)
    {
        found = TRUE;

        while (found)
        {
            found = FALSE;
            p = FunctionName;
            for (s; isascii (*s) && isspace (*s) && *s; s++);
            if( *s == '*' )
            {
                for (++s; isascii (*s) && isspace (*s) && *s; s++);
            }

            if ((*s == '_') || isalpha(*s))
            {
                while (isidchr (*s)) *p++ = *s++;

                *p = '\0';

                for (found = FALSE, i = 0;
                     !found AND ReservedWord[i]; i++)
                {
                    if (!(result = strcmp (FunctionName, ReservedWord[i])))
                        found = TRUE;

                    if  (result < 0) break;
                }

                if (logging GE 3 AND found)
                {
                    fprintf (stderr,
                        "%s<%s>: LOG: reserved word = %s\n",
                        pgm, _fnm, FunctionName);
                }
            }
        }

        if (logging GE 3)
        {
            fprintf (stderr,
                "%s<%s>: LOG: last word = %s\n",
                pgm, _fnm, FunctionName);
        }
        
        for (s; isascii (*s) && isspace (*s) && *s; s++);

        if (*s EQ '(')
        {
            for (found = FALSE; *s AND !found; s++)
                found = *s EQ ')';
            
            if (found)
            {
                for (; *s AND isspace (*s); s++);

                found = *s NE ';';
                
                if (found)
                {
                    strcpy (fnc_name, FunctionName);
                    fprintf (tocfile,
                        "%s %s %d %d\n",
                        fh_name, fnc_name, pageno-1, lineno);
                    specline = TRUE;
                }
            }
        }
    }

    if (logging GE 3)
    {
        fprintf (stderr,
    "%s<%s>: LOG: this line does%s contain a function declaration\n",
            pgm, _fnm, found ? "" : " not");
    }

    return found;
}
/*S basename - return the basename part of a pathname */
/*H basename *********************************************************
*
*                                   basename
*
*  given a (presumed) pathname, return the part after the last slash
*
*********************************************************************/
/*E*/
char *
basename (str)
register char *str;
{
    register char *ret;

    if (ret = strrchr (str, '/')) ret++;
    else ret = str;

    return ret;
}
