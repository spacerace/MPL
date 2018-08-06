/*
 *  mv.c move (rename a file or files)
 */

#include <stdio.h>              /* Standard I/O definitions */
#include <dos.h>                /* Msdos definitions */
#include <errno.h>              /* error codes */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

static char ID[]="## mv.c 1.0 K.van Houten 230987 ##";
struct stat statbuf;
int iflg=0;
int dflg=0;
int i;
int tmp=0;
char errbuf[100];
char nmbuf[100];
char *dp;                       /* points to dest. dir argv[argc-1] */
char *tp;

union REGS r;
struct SREGS s;

main (argc, argv)
int     argc;                   /* Number of command line arguments */
char   *argv[];                 /* Array of pointers to arguments */
{
    for (argc--,argv++; argc>0; argc--,argv++) {
        if (**argv == '-') {
            while (*++(*argv)) {
                switch (**argv) {
                case 'i':
                    iflg++;
                    printf("mv: -i not yet supported.\n");
                    exit(1);
                    break;
                default:
usage:              printf("Usage: mv [-i] file [file | [file...] directory]\n");
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
    if (argc < 2)
        goto usage;
    dp = argv[argc-1];

    if (strcmp(dp,"\\")==0 || strcmp(dp,"..")==0 ||
     (dp[strlen(dp)]=='.' && dp[strlen(dp)-1]=='.' )) {
        dflg++;
    }
    else {
        if (stat(dp,&statbuf) == -1) {
            if (argc>2) {
                sprintf(errbuf,"mv: %s ",dp);
                perror(errbuf);
                exit(1);
            }
        }
        else {
            if ((statbuf.st_mode & S_IFDIR) == 0) {
                if (argc > 2) {
                    fprintf(stderr,"mv: destination must be directory\n");
                    exit(1);
                }
            }
            else
                dflg++;
        }
    }
    for (i=0; i< argc-1 ;i++) {
        if (dflg) {
            if ((tp=strrchr(argv[i],'\\'))==NULL)
                tp=argv[i];
            else
                tp++;
            strcpy(nmbuf,dp);
            strcat(nmbuf,"/");
            strcat(nmbuf,tp);
        }
        else {
            strcpy(nmbuf,argv[1]);
        }
        rename(argv[i],nmbuf);
        if (r.x.cflag) {
            sprintf(errbuf,"mv: Error %d ",errno);
            switch(errno) {
            case 2:
                fprintf(stderr,"mv: File not found.\n");
                exit(1);
            case 3:
                fprintf(stderr,"mv: Path not found.\n");
                exit(1);
            case 5:
                if (unlink(nmbuf) == -1) {
                    fprintf(stderr,"mv: %s write protect.\n",nmbuf);
                    exit(1);
                }
                rename(argv[i],nmbuf);
                if (r.x.cflag) {
                    fprintf(stderr,"mv: Unexpected Error %d\n",errno);
                    exit(1);
                }
                break;
            case 17:
                fprintf(stderr,"mv: No cross device move\n");
                exit(1);
            default:
                fprintf(stderr,"mv: Unexpected ERROR.\n");
                exit(1);
            }
        }
    }
    exit(0);
}

rename(c1,c2)
char *c1, *c2;
{
        segread(&s);
        s.es = s.ds;
        r.h.ah = 0x56;   /* Rename function */
        r.x.dx = (int)c1;
        r.x.di = (int)c2;
        errno = intdosx(&r, &r, &s);
}

    
