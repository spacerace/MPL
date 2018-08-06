/*
 * Wildcard expansion for within C programs
 */

#include <stdio.h>              /* Standard I/O definitions */
#include <ctype.h>              /* Character type macros */
#include <dos.h>                /* Msdos definitions */
#include <errno.h>              /* error codes */
#include <string.h>
#include <stdlib.h>
#include <search.h>
#include <malloc.h>

#define SIZE(x)         (sizeof(x)/sizeof(x[0]))
#define A_RONLY         0x01    /* Read only file */
#define A_HIDDEN        0x02    /* Hidden file */
#define A_SYSTEM        0x04    /* System file */
#define A_DIRECTORY     0x10    /* Directory file */
#define A_ARCHIVE       0x20    /* Archive bit */
#define FNLEN           80      /* Maximum filename length */
#define MAXFILES        512     /* Max number of files/directory */

static char ID[]="## wild.c 1.0 K.van Houten 230987 ##";
struct date {
unsigned    d_sec:      5;      /* Time, 2 second intervals */
unsigned    d_min:      6;      /* Time, minutes */
unsigned    d_hour:     5;      /* Time, hours */
unsigned    d_day:      5;      /* Date, day of month */
unsigned    d_month:    4;      /* Date, month of year */
unsigned    d_year:     7;      /* Date, year since 1980 */
};
struct find {
    char    fnd_dosinfo[21];    /* Reserved for dos */
    char    fnd_attr;           /* File attribute */
    struct date fnd_date;       /* Date structure */
    long    fnd_size;           /* File size */
    char    fnd_name[13];       /* File name less path */
};
char   *strrchr ();
int     qscmp ();
void findf();
void findn();
char **wild();

static int tmp;
static struct find f;              /* Used to return data from msdos */
static union REGS r, o;            /* Contains register values for intcall() */

char **
wild(file)
char *file;
{
    int i,j,count;
    char **fnd;

    fnd = (char **)malloc(MAXFILES * sizeof(char *));
    if (fnd == NULL) {
        printf("wildcard expansion out of mem\n");
        exit(2);
    }

    r.h.ah = 0x1a;              /* SET DISK TRANSFER ADDRESS FUNCTION */
    r.x.dx = (int) & f;
    tmp = intdos (&r, &o);
    count = 0;

    findf (file);
    if (o.x.cflag) {
        return(NULL);
    }
    for (;;) {
        if (o.x.cflag) {
            break;
        }
        if (f.fnd_attr & A_DIRECTORY) {
            if (f.fnd_name[0] == '.')
               goto next;
        }
        if ((fnd[count] = malloc(15)) == NULL) {
              printf("wildcard expansion out of memory\n");
              exit(2);
        }
        strcpy(fnd[count],strlwr(f.fnd_name));
        count++;
next:   findn ();
     }
     if (count == 0) {
        return(NULL);
    }
    qsort ((char *)fnd, count, sizeof (char *), qscmp);

    fnd[count] = NULL;
    return(fnd);
}


/*
 * Find first entry, return info in find structure.
 */
void
findf (file)
char *file;
{
    r.h.ah = 0x4e;              /* FINDFIRST function */
    r.x.cx = A_HIDDEN | A_SYSTEM | A_DIRECTORY |
        A_RONLY | A_ARCHIVE;
    r.x.dx = (int) file;
    tmp = intdos (&r, &o);      /* Find first */
}

/*
 * Find next entry
 */
void
findn ()
{
    r.h.ah = 0x4f;              /* Now use FINDNEXT function */
    tmp = intdos (&r, &o);      /* Find next */
}


/*
 * Qsort compare routine
 * files sort alphabetically.
 * This routine is called by qsort and passed two pointers to
 * find structure pointers.
 */
qscmp (p1, p2)
char **p1;
char **p2;
{
    register char   *f1 = *p1;
    register char   *f2 = *p2;
    register int    cmp;

    return (strcmp (f1, f2));
}
