/*
	WHEREIS.C

	A file finder utility that searches the current drive, starting
	with the root directory (\), for the specified pathname.
	Wildcard characters can be included.

	Compile with:  C> cl whereis.c

	Usage is:  C> whereis pathname

	Copyright (C) 1988 Ray Duncan
*/


#include <stdio.h>
#include <string.h>

#define API unsigned extern far pascal

API DosChDir(char far *, unsigned long);
API DosFindClose(unsigned);
API DosFindFirst(char far *, unsigned far *, unsigned, void far *,
                 int, int far *, unsigned long);
API DosFindNext(unsigned, void far *, int, int far *);
API DosQCurDisk(int far *, unsigned long far *);
API DosQCurDir(int, void far *, int far *);
API DosSelectDisk(int);
API DosWrite(unsigned, void far *, unsigned, unsigned far *);

#define NORM      0x00                  /* file attribute bits */
#define RD_ONLY   0x01
#define HIDDEN    0x02
#define SYSTEM    0x04
#define DIR       0x10
#define ARCHIVE   0x20

struct _srec {                          /* used by DosFindFirst */
                    unsigned cdate;     /* and DosFindNext */
                    unsigned ctime;
                    unsigned adate;
                    unsigned atime;
                    unsigned wdate;
                    unsigned wtime; 
                    long fsize;
                    long falloc;
                    unsigned fattr;
                    char fcount;
                    char fname[13];
             } 
                sbuf;                   /* receives search results */

int drvno;                              /* current drive */
int count = 0;                          /* total files matched */
char sname[80];                         /* target pathname */


main(int argc, char *argv[])
{
    unsigned long drvmap;               /* logical drive map */

    if(argc < 2)                        /* exit if no parameters */
    {
        printf("\nwhereis: missing filename\n");
        exit(1);
    }

    DosQCurDisk(&drvno, &drvmap);       /* get current drive */

                                        /* any drive specified? */
    if(((strlen(argv[1])) >= 2) && ((argv[1])[1] == ':'))
    {
                                        /* get binary drive code */
        drvno = ((argv[1]) [0] | 0x20) - ('a'-1);

        if(DosSelectDisk(drvno))        /* select drive or exit */
        {
            printf("\nwhereis: bad drive\n");
            exit(1);
        }

        argv[1] += 2;                   /* advance past drive */
    }

    strcpy(sname,argv[1]);              /* save search target */

    schdir("\\");                       /* start search with root */

                                        /* advise if no matches */
    if(count == 0) printf("\nwhereis: no files\n");
}


/*
    SCHDIR: search directory for matching files and
            any other directories
*/

schdir(char *dirname)
{
    unsigned shan = -1;                 /* search handle */
    int scnt = 1;                       /* max search matches */

    DosChDir(dirname, 0L);              /* select new directory */

    schfile();                          /* find and list files */

                                        /* search for directories */
    if(!DosFindFirst("*.*", &shan, NORM|DIR, &sbuf, sizeof(sbuf), &scnt, 0L))
    {
        do                              /* if found directory other */
        {                               /* than . and .. aliases */
            if((sbuf.fattr & DIR) && (sbuf.fname[0] != '.'))
            {
                schdir(sbuf.fname);     /* search the directory */
                DosChDir("..", 0L);     /* restore old directory */                         
            }
                                        /* look for more directories */
        } while(DosFindNext(shan, &sbuf, sizeof(sbuf), &scnt) == 0);
    }

    DosFindClose(shan);                 /* close search handle */
}


/*
    SCHFILE: search current directory for files
             matching string in 'sname'
*/

schfile()
{
    unsigned shan = -1;                 /* search handle */
    int scnt = 1;                       /* max search matches */

    if(!DosFindFirst(sname, &shan, NORM, &sbuf, sizeof(sbuf), &scnt, 0L))
    {
        do pfile();                     /* list matching files */
        while(DosFindNext(shan, &sbuf, sizeof(sbuf), &scnt) == 0);
    }

    DosFindClose(shan);                 /* close search handle */
}


/*
    PFILE: display current drive and directory, 
           followed by filename from 'sbuf.fname'
*/

pfile()
{
    count++;                            /* count matched files */
    pdir();				/* list drive and path */
    printf("%s\n", strlwr(sbuf.fname)); /* list filename */ 
}


/*
    PDIR: display current drive and directory
*/

pdir()
{
    char dbuf[80];			/* receives current dir */
    int dlen = sizeof(dbuf);            /* length of buffer */

    DosQCurDir(0, dbuf, &dlen);         /* get current directory */

    if(strlen(dbuf) != 0)               /* add backslash to */
        strcat(dbuf,"\\");              /* directory if not root */
    
					/* display drive and path */
    printf("%c:\\%s", drvno+'a'-1, strlwr(dbuf));
}


