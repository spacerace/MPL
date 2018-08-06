#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>

/* Boolean type and values. */
typedef int     BOOLEAN;
#define TRUE    1
#define FALSE   0

/* Exit values. */
#define SUCCESS   0
#define FAILURE   1

/* Structure for option values. */
typedef struct options
{
    int   lngval;
} OPTIONS;

/* Default option values. */
#define DEF_LNGVAL   25

/* Function declarations. */
void    cmpfiles (char *, char *, OPTIONS *);
BOOLEAN readin   (FILE *, char ***, int *);
void    compare  (char **, int, char **, int, int);

/* Program name. */
static char   Progname[] = "test";

main (argc, argv)

int    argc;
char   **argv;
{
    OPTIONS   opt;        /* Structure for option values. */
    char      *thisopt;   /* Current option. */
    char      *thisval;   /* Current option value. */

    /* Move past program name in arg list. */
    argv++;
    argc--;

    /* Initialize options. */
    opt.lngval = DEF_LNGVAL;

    /* Process options. */
    for ( ; argc > 0 && **argv == '-'; argc--, argv++)
    {
        /* Get option flag. */
        thisopt = *argv + 1;

        /* Look for 'l' option. */
        if (tolower (*thisopt) == 'l')
        {
            /* Get option value. */
            thisval = thisopt + 1;

            /* Option value must be a positive integer. */
            if (isdigit (*thisval) && (opt.lngval = atoi (thisval)))
            {
                ;
            }
            else
            {
                fprintf (stderr,
                        "%s:  invalid value, \"%s\", for option \"%c\"\n",
                        Progname, thisval, *thisopt);
                exit (FAILURE);
            }
        }
        else
        {
            fprintf (stderr, "%s:  invalid option, \"%c\"\n",
                    Progname, *thisopt);
            exit (FAILURE);
        }
    }

    /* Must be exactly two arguments left. */
    if (argc == 2)
    {
        cmpfiles (*argv, *(argv + 1), &opt);
        exit (SUCCESS);
    }
    else
    {
        fprintf (stderr, "usage:  %s [-ln] file1 file2\n", Progname);
        exit (FAILURE);
    }
}

static void cmpfiles (file1, file2, opt)

char      *file1;   /* (I) File name #1. */
char      *file2;   /* (I) File name #2. */
OPTIONS   *opt;     /* (O) Options. */
{
    FILE   *fp1;   /* File pointer #1. */
    FILE   *fp2;   /* File pointer #2. */
    char   **a1;   /* Array of lines #1. */
    int    n1;     /* Number of lines in a1 (not including 0th element). */
    char   **a2;   /* Array of lines #2. */
    int    n2;     /* Number of lines in a2 (not including 0th element). */

    /* Open the files. */
    if ((fp1 = fopen (file1, "rt")) && (fp2 = fopen (file2, "rt")))
    {
        /* Read each file and place the lines of text into an array. */
        if (readin (fp1, &a1, &n1) && readin (fp2, &a2, &n2))
        {
            compare (a1, n1, a2, n2, opt->lngval);
        }
        else
        {
            fprintf (stderr,
                    "%s:  could not read files into memory\n", Progname);
        }
    }
    else
    {
        fprintf (stderr, "%s:  could not open files\n", Progname);
    }
}

static BOOLEAN readin (fp, a, n)

FILE   *fp;    /* (I) File pointer. */
char   ***a;   /* (O) Array of lines. */
int    *n;     /* (O) Number of lines in a (not including 0th element). */
{
    #define NLINES   1000   /* Maximum number of lines that will be handled. */
    #define NBUF     512    /* Maximum line length that will be handled. */

    char      **array;         /* Array of lines. */
    int       i;               /* Counter. */
    int       len;             /* Length of line. */
    char      buf[NBUF + 1];   /* Text buffer. */

    /* Allocate an array for a maximum number of lines of text. */
    if (array = (char **) malloc ((NLINES + 1) * sizeof (char *)))
    {
        ;
    }
    else
    {
        return (FALSE);
    }

    /* Read the lines of text. */
    for (i = 1; fgets (buf, NBUF + 1, fp) && i < NLINES; i++)
    {
        /* Allocate an array. */
        len = strlen (buf);
        if (array[i] = malloc ((len + 1) * sizeof (char)))
        {
            /* Copy the text. */
            memcpy (array[i], buf, len + 1);
        }
        else
        {
            return (FALSE);
        }
    }

    /* Return the array and the number of lines. */
    *a = array;
    *n = i - 1;

    return (TRUE);
}
