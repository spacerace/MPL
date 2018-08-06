/*
 * rm.c remove a file or files
 */

#include <stdio.h>              /* Standard I/O definitions */
#include <dos.h>                /* Msdos definitions */
#include <errno.h>              /* error codes */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <conio.h>

static char ID[]="## rm.c 1.0 K.van Houten 230987 ##";
int isdir(char *);
extern char **wild(char *);
int rmfile(char *);
int rmdirect(char *);

int fflg=0;                     /* FORCE flag (no errors) */
int rflg=0;                     /* RECURSIVE flag */
int iflg=0;                     /* Interactive flag */
int tmp;                        /* for return values */
struct stat statbuf;            /* for file status structure */
char ibuf[10];

main (argc, argv)
int     argc;                   /* Number of command line arguments */
char   *argv[];                 /* Array of pointers to arguments */
{
    for (argc--,argv++; argc>0; argc--,argv++) {
        if (**argv == '-') {
            while (*++(*argv)) {
                switch (**argv) {
                case 'f':
                    fflg++;
                    break;
                case 'r':
                    rflg++;
                    break;
                case 'i':
                    iflg++;
                    break;
                default:
usage:              printf("Usage: rm [-fir] <pathname> [<pathname> ..]\n");
                    exit(1);
                }
            }
        }

        else
            break;
    }
    /*
     * NOW THE ARGV[0] CONTAINS THE FIRST NON-OPTION ARGUMENT,
     * WHILE ARGC CONTAINS THE NUMBER OF ARGUMENTS LEFT.
     * SO WE HAVE AVAILABLE ARGV[0] -- ARGV[ARGC-1].
     */

    *ibuf = (char)7;

    if (argc < 1)
        goto usage;

    if (rflg && !fflg) {
        printf("rm: Are you sure you want recursive remove?");
        cgets(ibuf);
        putch('\n');
        if (!((ibuf[2] == 'y') || (ibuf[2] == 'Y'))) {
            exit(0);
        }

    }

    while (argc > 0) {
        /* check arg is dir ? */
        if (stat(*argv,&statbuf) == -1) {
            if (fflg == 0) {
                printf("rm: %s no such file or directory\n",*argv);
            }
            argc--;
            argv++;
            continue;
        }
        if (statbuf.st_mode & S_IFDIR) {
            if (rflg) {
                (void)rmdirect(*argv);
            }
            else {
                if (fflg == 0) {
                    printf("rm: %s is directory\n",*argv);
                }
            }
        }
        else {
            (void)rmfile(*argv);
        }

        /* Next argument */
        argc--;
        argv++;
    }
    return(0);
}

int
rmfile(p)
char *p;
{
    if (iflg) {
        printf("rm: remove %s ?",p);
        cgets(ibuf);
        putch('\n');
        if ((ibuf[2] == 'q') || (ibuf[2] == 'Q'))
            exit(2);
        if (!((ibuf[2] == 'y') || (ibuf[2] == 'Y'))) {
            return(0);
        }
    }
    if (fflg) {
#ifdef DEBUG
        tmp = 0;
        printf("Changing mode %s\n",p);
#else
        tmp = chmod(p, S_IREAD | S_IWRITE);
#endif
        if (tmp == -1)
            perror("chmod");
    }
#ifdef DEBUG
    tmp = 0;
    printf("Unlinking %s\n",p);
#else
    tmp = unlink(p);
#endif
    if (tmp == -1) {
        perror("unlink");
        return(0);
    }
    return(1);
}

int
rmdirect(p)
char *p;
{
    char buf[100];
    char base[100];
    char **w;
    int i;
    int error = 0; /* counts errors */

    strcpy(base,p);
    strcpy(buf,p);
    if (buf[strlen(buf)-1] != '\\')
       strcat(buf,"\\");
    strcat(buf,"*.*\0");
    w = wild(buf);
    if (w != NULL) {
        for (i=0;(w[i] != NULL);i++) {
            strcpy(buf,base);
            if (buf[strlen(buf)-1] != '\\')
                strcat(buf,"\\");
            strcat(buf,w[i]);
            if (isdir(buf) == 1) {
                if (rflg == 1) {
                    if (rmdirect(buf) == 0) {
                        error++;
                    }
                }
                else {
                    if (fflg == 0) {
                        printf("rm: %s is directory\n",buf);
                    }
                    error++;
                }
            }
            else {
                if (rmfile(buf) == 0)
                    error++;
            }
        }
    }
    /* all entrys done , now we can rmdir */
    if (error == 0) {
#ifdef DEBUG
        tmp = 0;
        printf("Removing Directory %s\n",base);
#else
        tmp = rmdir(base);
#endif
        if (tmp == -1) {
            perror("rmdir");
            return(0);
        }
        return(1);
    }
    return(0);
}

int
isdir(p)
char *p;
{
    struct stat st;
    if (stat(p,&st) == -1) {
        if (fflg == 0) {
            printf("rm: %s no such file or directory\n",p);
        }
    }
    return((st.st_mode & S_IFDIR) ? 1 : 0);
}
