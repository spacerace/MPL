/*
 *  Attribute bits:
 *      d - directory
 *      r - read only file
 *      h - hidden file
 *      s - system file
 *      a - archive bit
 *
 *  Files within each directory are sorted alphabetically.
 *  Directories are listed separate from other files.
 */

#include <stdio.h>              /* Standard I/O definitions */
#include <ctype.h>              /* Character type macros */
#include <dos.h>                /* Msdos definitions */
#include <errno.h>              /* error codes */
#include <string.h>

#define SIZE(x)         (sizeof(x)/sizeof(x[0]))
#define A_RONLY         0x01    /* Read only file */
#define A_HIDDEN        0x02    /* Hidden file */
#define A_SYSTEM        0x04    /* System file */
#define A_DIRECTORY     0x10    /* Directory file */
#define A_ARCHIVE       0x20    /* Archive bit */
#define FNLEN           80      /* Maximum filename length */
#define MAXFILES        512     /* Max number of files/directory */

static char ID[]="## ls.c 2.0 K.van Houten 230987 ##";
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
char   *nameptr ();
char   *date ();
char   *malloc ();
char   *strrchr ();
int     qscmp ();
extern int clsize();            /* get cluster size */
long clust();
short   tmp;
struct find f;                  /* Used to return data from msdos */
union REGS r, o;                /* Contains register values for intcall() */
int     count;                  /* Number of files/directory */
char   *p;                      /* Pointer to filename */
int     i;
int     j;
int     lflg = 0;               /* long */
int     tflg = 0;               /* time sort */
int     sflg = 0;               /* size in bloks */
int     rflg = 0;               /* reverse sort */
int     fflg = 0;               /* identify executables and subdirs */
long    dirlen;                 /* Total length of one dir */
char    line[FNLEN];            /* Filename buffer */
char    dirs[20][50];           /* Subdir buffer */
int     dirc;                   /* Subdir count */
int drive;                      /* drive nr */
char   *file;                   /* Command line argument */
struct find *fnd[MAXFILES];     /* Pointers to file structures */
int bsize;                      /* size of clusters */
int spc;                        /* number of K's per cluster */

static struct atr {             /* Attribute structure */
    int     a_mask;
    int     a_name;
} atr[] = {
    A_DIRECTORY, 'd',
    A_RONLY,     'r',
    A_HIDDEN,    'h',
    A_SYSTEM,    's',
    A_ARCHIVE,   'a'
};

main (argc, argv)
int     argc;                   /* Number of command line arguments */
char   *argv[];                 /* Array of pointers to arguments */
{
    char    buf[70];

    drive = defdrv();
    bsize=clsize(drive);
    spc=bsize/1024;
    for (argc--,argv++; argc>0; argc--,argv++) {
        if (**argv == '-') {
            while (*++(*argv)) {
                switch (**argv) {
                case 'l':
                    lflg++;
                    break;
                case 't':
                    tflg++;
                    break;
                case 's':
                    sflg++;
                    break;
                case 'r':
                    rflg++;
                    break;
                case 'F':
                    fflg++;
                    break;
                default:
                    printf("Usage: ls [-Flrst] [file [file ...]]\n");
                    exit(1);
                }
            }
        }
        else
            break;
    }

    r.h.ah = 0x1a;              /* SET DISK TRANSFER ADDRESS FUNCTION */
    r.x.dx = (int) & f;
    tmp = intdos (&r, &o);
/*  Produces error 0 ???
    if (o.x.cflag) {
        perror ("SETTING DTA ");
        exit (1);
    }
*/
    dirc = 0;
    count = 0;

    if (argc == 0) {
        strcpy(dirs[dirc++],".");
    }
    else {
        while (file = *argv++, argc--) {
            /* handle relative directorys */
            if (strcmp(file,"..") == 0) {
                strcpy(dirs[dirc++],"..\\.");
                continue;
            }
            if (file[strlen(file)-1] == '.') {
                strcpy(dirs[dirc++],file);
                continue;
            }
            /* handle drive letters */
            if (strlen(file) == 2 && file[1] == ':') {
                strcpy(dirs[dirc],file);
                strcat(dirs[dirc++],".");
                continue;
            }
            /* handle abs. dirs */
            if (file[strlen(file)-1] == '\\') {
                strcpy(dirs[dirc], file);
                strcat(dirs[dirc++],".");
                continue;
            }
            findf ();
            if (o.x.cflag) {
                continue;
            }
            for (;;) {
                if (o.x.cflag) {
                    break;
                }
                if (f.fnd_attr & A_DIRECTORY) {
                    if (f.fnd_name[0] == '.')
                       goto next;
                    *dirs[dirc]='\0';
                    if (strrchr(file,'\\')!=NULL) {
                       strncpy(dirs[dirc],file,((strrchr(file,'\\')-file)+1));
                    }
                    strcat (dirs[dirc++], f.fnd_name);/* save subdir name */
                }
                else {
                    if ((fnd[count] =
                       (struct find *)malloc(sizeof(struct find))) == NULL)
                          fatal ("out of memory");
                    *fnd[count] = f;
                    /* Increment total size */
                    dirlen += clust(fnd[count]->fnd_size);
                    count++;
                }
next:           findn ();
            }
        }
        if (count == 0 && dirc == 0) {
            printf("File not found\n");
            exit(1);
        }
    }
    /* All arguments done, now print simple files */
    qsort (fnd, count, sizeof (struct find *), qscmp);

    for (i = 0; i < count; i++) {
        printentry(i);
        if ((lflg==0) && (((i+1) % (sflg?4:5)) == 0) && (i > 1))
            printf("\n");
    }
    for (i = 0; i < dirc; i++)
        printdir(i);
    return (1);
}

/*
 * Print one entry, format depends on options selected.
 */
printentry(x)
int x;
{
    long sz;
    char pbuf[50];    /* print buffer */

    sz = fnd[x] -> fnd_size;

    /* block size */
    if (sflg)
    {
        if ((fnd[x] -> fnd_attr & A_DIRECTORY) == 0)
        {
           printf("%3d ",clust(sz));
        }
        else
           printf("  0 ");
    }
    if (lflg) {
        /* mode bits */
        for (j = 0; j < SIZE (atr); j++)
            if (atr[j].a_mask & fnd[x] -> fnd_attr)
                putchar (atr[j].a_name);
            else
                putchar ('-');

        /* file size */
        printf (" %10ld  ", sz);

        /* file date */
        printf ("%s  ", date (&fnd[x] -> fnd_date));
    }

    /* file name */
    if (fflg == 0)
        printf ("%-12s  " , strlwr (fnd[x] -> fnd_name));

    /* executable / subdir info */
    if (fflg)
    {
        if (fnd[x] -> fnd_attr & A_DIRECTORY)
            sprintf (pbuf,"%s/" , strlwr (fnd[x] -> fnd_name));
        else
        {
            if (isexe(fnd[x]->fnd_name))
                sprintf (pbuf,"%s*" , strlwr (fnd[x] -> fnd_name));
            else
                sprintf (pbuf,"%s " , strlwr (fnd[x] -> fnd_name));
        }
        printf ("%-12s  " , pbuf);
    }

    if (lflg)
        printf("\n");

    free (fnd[x]);          /* Free find structure */
}


printdir (x)
int x;
{
    int i;
    char fbuf[80]; /* to assemble filename */

    /* Complex, but needed for DOS 2.11 compatibility */
    if (dirs[x][strlen(dirs[x])-1] == '.') {
        dirs[x][strlen(dirs[x])-1] = '\0';
        strcpy(fbuf,dirs[x]);
        strcat(fbuf,"*.*");
    }
    else {
        strcpy(fbuf,dirs[x]);
        strcat(fbuf,"\\*.*");
    }
    if (dirs[x][1] == ':') { /* other drive */
        drive=tolower(*dirs[x])-'a';
    }
    else {
        drive=defdrv();
    }
    bsize=clsize(drive);
    spc=bsize/1024;
    file = fbuf;
    p = nameptr (file);
    count = p - file;
    if (count + sizeof (f.fnd_name) > FNLEN)
        fatal ("filename too long: %s", file);
    strncpy (line, file, count);
    p = line + count;
    count = 0;                  /* Init file count */
    dirlen = 0;

    findf ();
    for (;;) {
        if (o.x.cflag)
            break;
        if (count >= MAXFILES)
            fatal ("Too many files in one directory");
        if ((fnd[count] =
           (struct find *) malloc (sizeof (struct find))) == NULL)
              fatal ("out of memory");
        if (!((f.fnd_attr & A_DIRECTORY) &&
                    ((strcmp (f.fnd_name, ".") == 0) ||
                        (strcmp (f.fnd_name, "..") == 0)))) {
            memcpy (fnd[count], &f, sizeof f);
            /* Increment total size */
            dirlen += clust(fnd[count] -> fnd_size);
            count++;
        }
        findn ();
    }
    if (count)
        qsort (fnd, count, sizeof (struct find *), qscmp);

    if (*dirs[x] != '\0')
        printf("\n%s:\n", strlwr(dirs[x]));
    if (lflg) {
        printf ("Total %d\n",dirlen);
    }
    for (i = 0; i < count; i++) {
        printentry(i);
        if ((lflg==0) && (((i+1) % (sflg?4:5)) == 0) && (i > 1))
            printf("\n");
    }
}

/*
 * Find first entry, return info in find structure.
 */
findf ()
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
findn ()
{
    r.h.ah = 0x4f;              /* Now use FINDNEXT function */
    tmp = intdos (&r, &o);      /* Find next */
}

/*
 * Return a pointer to an ascii date, given a pointer
 * to a date structure
 */
char    ad[40];

char   *
        date (dp)
register struct date   *dp;
{
    static char *month[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

    sprintf (ad, "%2d %s %4d  %02d:%02d:%02d",
            dp -> d_day,
            month[dp -> d_month - 1],
            dp -> d_year + 1980,
            dp -> d_hour,
            dp -> d_min,
            dp -> d_sec * 2);
    return (ad);
}

/*
 * Qsort compare routine
 * Directories first, other files second, sort alphabetically.
 * This routine is called by qsort and passed two pointers to
 * find structure pointers.
 */
qscmp (p1, p2)
struct find **p1;
struct find **p2;
{
    register struct find   *f1 = *p1;
    register struct find   *f2 = *p2;
    register int    cmp;
    int inv = 1;

    if (rflg)
        inv=-1;
/*
    if (cmp = (f2 -> fnd_attr & A_DIRECTORY) - (f1 -> fnd_attr & A_DIRECTORY))
        return (cmp);
 */
    if (tflg) {
        if (cmp = (f2->fnd_date.d_year - f1->fnd_date.d_year))
            return(inv * cmp);
        if (cmp = (f2->fnd_date.d_month - f1->fnd_date.d_month))
            return(inv * cmp);
        if (cmp = (f2->fnd_date.d_day - f1->fnd_date.d_day))
            return(inv * cmp);
        if (cmp = (f2->fnd_date.d_hour - f1->fnd_date.d_hour))
            return(inv * cmp);
        if (cmp = (f2->fnd_date.d_min - f1->fnd_date.d_min))
            return(inv * cmp);
        if (cmp = (f2->fnd_date.d_sec - f1->fnd_date.d_sec))
            return(inv * cmp);
    }
    return (inv * strcmp (f1 -> fnd_name, f2 -> fnd_name));
}

/*
 * Return a pointer to the beginning of the filename (past pathname if any)
 */
char   *
        nameptr (name)
char   *name;
{
    register char  *p1;

    if ((p1 = strrchr (name, '\\')) != NULL)
        return (++p1);
    else
        if ((p1 = strrchr (name, ':')) != NULL)
            return (++p1);
    return (name);
}

/*
 * Fatal error
 * The %r (recursive) printf format specifier is non-portable.
 */
fatal (p)
char   *p;
{
    printf ("ls fatal error\n");
    exit (0);
}

long
clust(sz)
long sz;
{
return(spc*((sz % bsize) ? (sz / bsize + 1) : (sz / bsize)));
}

int
defdrv()
{
    int tmp;
    union REGS r;

    r.h.ah = 0x19;  /* get default drive */
    tmp = intdos(&r,&r);
    if (r.x.cflag != 0) {
        printf("Error reading def. drive: %d\n",tmp);
        exit(1);
    }
    return(r.h.al);
}

int
isexe(n)
char *n;
{
    char *p,*t;

    if ((p=strchr(n,'.')) == NULL)
        return(0);
    p++;
    if (strncmp(p,"EXE",3) == 0)
        return(1);
    if (strncmp(p,"COM",3) == 0)
        return(1);
    if (strncmp(p,"BAT",3) == 0)
        return(1);
    return(0);
}
