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
#include <malloc.h>

#define A_RONLY         0x01    /* Read only file */
#define A_HIDDEN        0x02    /* Hidden file */
#define A_SYSTEM        0x04    /* System file */
#define A_DIRECTORY     0x10    /* Directory file */
#define A_ARCHIVE       0x20    /* Archive bit */

static char ID[]="## du.c 1.0 K.van Houten 092387 ##";
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
long addir();
struct find f;                  /* Used to return data from msdos */
union REGS r, o;                /* Contains register values for intcall() */
int   tmp, i;
int bsize=8*1024;               /* size of disk allocation */
int kpb;                        /* Kbytes per block */
int     sflg = 0;               /* summarize */
long total;
extern int clsize();            /* get cluster size */

main (argc, argv)
int     argc;                   /* Number of command line arguments */
char   *argv[];                 /* Array of pointers to arguments */
{
    char    buf[70];
    bsize=clsize(defdrv());
    kpb=bsize/1024;

    for (argc--,argv++; argc>0; argc--,argv++) {
        if (**argv == '-') {
            while (*++(*argv)) {
                switch (**argv) {
                case 's':
                    sflg++;
                    break;
                default:
                    printf("Usage: du directory\n");
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
    if (argc == 0) {
        total=addir(".");
    }
    else {
        findf(*argv);
        if (o.x.cflag) {
            printf("du: entry not found\n");
            exit(1);
        }
        if (!(f.fnd_attr & A_DIRECTORY)) {
            printf("du: %s: not a directory\n",f.fnd_name);
            exit(1);
        }
        total=addir(*argv);
    }
    if (sflg) {
        if (argc) {
            printf("%10ld %s\n",kpb*total,strlwr(*argv));
        }
        else {
            printf("%10ld .\n",kpb*total);
        }
    }
}

long
addir(dir)
char *dir;
{
    char *dirlist[100];  /* for subdirs */
    char buf[100];       /* for filename */
    int dircnt=0;        /* nr of subdirs */
    int i;
    long dtot=0;         /* total of this level */

    strcpy(buf,dir);
    strcat(buf,"/*.*");
/*    printf("Scanning %s\n",dir); */
    findf(buf);
    while (o.x.cflag == 0) {
        if (f.fnd_attr & A_DIRECTORY) {
            if (!(*f.fnd_name == '.')) {
                dirlist[dircnt]=malloc(sizeof(f.fnd_name)+1);
                if (dirlist[dircnt] == NULL) {
                    perror("du ");
                    exit(1);
                }
                strcpy(dirlist[dircnt++],f.fnd_name);
            }
        }
        else {
          dtot+=((f.fnd_size%bsize)?(f.fnd_size/bsize+1):(f.fnd_size/bsize));
        }
        findn();
    }
    /* all entrys done,
     * now scan subdirs.
     */
    for(i=0;i<dircnt;i++) {
        strcpy(buf,dir);
        strcat(buf,"/");
        strcat(buf,dirlist[i]);
/*        printf("nesting: %s\n",buf); */
        dtot += addir(buf);
    }
    if (sflg ==0)
        printf("%10ld %s\n",kpb*dtot,strlwr(dir));
    return(dtot);
}

/*
 * Find first entry, return info in find structure.
 */
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
findn ()
{
    r.h.ah = 0x4f;              /* Now use FINDNEXT function */
    tmp = intdos (&r, &o);      /* Find next */
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
