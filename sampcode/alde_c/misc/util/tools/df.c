#include <stdio.h>              /* Standard I/O definitions */
#include <ctype.h>              /* Character type macros */
#include <dos.h>                /* Msdos definitions */
#include <errno.h>              /* error codes */
#include <string.h>

static char ID[]="## df.c 1.0 K.van Houten 230987 ##";
union REGS r, o;
char *getvol();

main (argc, argv)
int     argc;
char   *argv[];
{
    int i=3;

    printf("\
       ------- b y t e s ------   - c l u s t e r s -\n\
drive   total     used     free   total   used   free  usage\n");

    if (argc > 1) {
        while (--argc > 0) {
            ++argv;
            if ((argv[0][1] == ':')&&(driveinfo(tolower(**argv) - 'a' + 1) != 0))
                exit(0);
        }
    }

    while (driveinfo(i) == 0)
        i++;
}

/* get info of drive i ( 1=A, 2=B, etc...) */
driveinfo(i)
int i;
{
    int tmp;
    union REGS r;
    unsigned long total, free;
    float clsize;

    r.h.ah = 0x36;   /* get free space */
    r.h.dl = i;
    tmp = intdos(&r,&r);
    if (r.x.ax == 0xffff)
        return(1);
    clsize =(float)((double)(r.x.ax * r.x.cx) / (double)1024);
    total  = r.x.dx;
    free   = r.x.bx;
    printf("  %c:  ",i+64);
    /*
    printf(" %-10s",getvol(i));
    */
    printf("%5ld K  ",(long)(total * clsize));
    printf("%5ld K  ",(long)((total - free) * clsize));
    printf("%5ld K   ",(long)(free * clsize));

    printf("%5ld  ",total);
    printf("%5ld  ",(total-free));
    printf("%5ld   ",free);

    printf("%3d%%",(int)(100 * (float)((double)(total - free) / (double)total)));

    printf("\n");
    return(0);
}

char *
getvol(d)
int d;                          /* drive number */
{
#define A_VOLUME        0x08    /* Volume label */

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
    struct find f;              /* Used to return data from msdos */
    union REGS r;               /* Contains register values for intcall() */
    char file[20];
    char *t;


    r.h.ah = 0x1a;              /* SET DISK TRANSFER ADDRESS FUNCTION */
    r.x.dx = (int) & f;
    intdos (&r, &r);

    sprintf("%c:\\*.*",d+64);
    r.h.ah = 0x4e;              /* FINDFIRST function */
    r.x.cx = A_VOLUME;
    r.x.dx = (int) file;
    intdos (&r, &r);      /* Find first */
    if (r.x.cflag) {
        return(NULL);
    }
    t = (char *)malloc(13);
    if (t == NULL) {
        perror("volname");
        exit(2);
    }
    strcpy(t,f.fnd_name);
    return(t);
}
