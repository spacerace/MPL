/*
    TOUCH.C     Force time and date stamps on file(s)

    Compile:    C> CL touch.c

    Usage:      C> TOUCH pathname [ pathname... ]

    Pathnames may include wildcard characters

    Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>
#include <string.h>

#define API unsigned extern far pascal

API DosClose(unsigned);                 /* OS/2 function prototypes */
API DosFindClose(unsigned);         
API DosFindFirst(char far *, unsigned far *, unsigned, char far *, 
                 unsigned, int far *, unsigned long);
API DosFindNext(unsigned, char far *, unsigned, unsigned far *);
API DosGetDateTime(void far *);
API DosOpen(char far *, unsigned far *, unsigned far *, unsigned long,
            unsigned, unsigned, unsigned, unsigned long);           
API DosSetFileInfo(unsigned, int, void far *, int);

#define READ_ONLY 0x01                  /* file attribute bits */
#define HIDDEN    0x02
#define SYSTEM    0x04
#define DIRECTORY 0x10
#define ARCHIVE   0x20

#define ATTR      0                     /* attributes to use 
                                           during file search */

struct _finfo {                         /* used by DosSetFileInfo */
                    unsigned cdate;
                    unsigned ctime;
                    unsigned adate;
                    unsigned atime;
                    unsigned wdate;
                    unsigned wtime; 
              }     finfo;

struct _dinfo {                         /* used by DosGetDateTime */
                    char hour;
                    char min;
                    char sec;
                    char csec;
                    char day;
                    char mon;
                    int  year;
                    int  zone;
                    char dow;       
              }     dinfo;              


main(int argc, char *argv[])
{
    int i;

    if(argc < 2)
    {
        printf("\ntouch: missing filename\n");
        exit(1);
    }

    DosGetDateTime(&dinfo);             /* get current date&time */

                                        /* set up date & time of
                                           last write in directory 
                                           format for DosSetFileInfo */
    finfo.wdate = ((dinfo.year-1980)<<9) + (dinfo.mon<<5) + dinfo.day;
    finfo.wtime = (dinfo.hour<<11) + (dinfo.min<<5);

                                        /* file creation and last 
                                           access fields useless 
                                           in FAT file systems */
    finfo.cdate = finfo.ctime = finfo.adate = finfo.atime = 0;

    for(i = 1; i < argc; i++)           /* process all pathnames */
        findfiles(argv[i]);             /* in the command line */

    puts("");                           /* final blank line */
}

/*
    Search for all files matching a command line argument
*/
findfiles(char *cname)
{
    char resbuf[36];                    /* receives search results */
    unsigned status;                    /* receives function status */
    unsigned handle = -1;               /* directory search handle */
    unsigned attr = 0;                  /* attribute for search */
    int matches = 1;                    /* no. of matches requested/found */

                                        /* is there any match? */
    if(DosFindFirst(cname, &handle, ATTR, resbuf, 36, &matches, 0L) == 0)
    {
        stampfile(cname, &resbuf[23]);  /* initial match found */

                                        /* any additional matches? */
        while(DosFindNext(handle, resbuf, 36, &matches) == 0)
            stampfile(cname, &resbuf[23]);
    }
    else printf("\nno matches:  %s", strlwr(cname));
    DosFindClose(handle);               /* release search handle */
}


/*
    Set the time and date stamp on a file
*/

stampfile(char *cname, char *sname)
{
    unsigned status, handle, action;    /* scratch variables */
    char *p;                            /* scratch pointer */
    char qbuff[80];                     /* qualified filename */

    memset(qbuff,0,80);                 /* initialize buffer */

    p = strrchr(cname, '\\');           /* look for backslash */

    if(p != NULL)                       /* any path present? */
        memcpy(qbuff,cname,p-cname+1);  /* yes, copy it */
    else                                /* no, is drive present? */
        if((strlen(cname) >= 2) && (cname[1] == ':'))
        {
            qbuff[0] = cname[0];        /* yes, copy drive */
            qbuff[1] = cname[1];
        }
    strcat(qbuff,sname);                /* add filename from search
                                           to drive +/or path */

                                        /* try to open the file */
    if(DosOpen(qbuff, &handle, &action, 0L, 0, 1, 0x22, 0L))
        printf("\ncan't open:  %s", strlwr(qbuff));
    else
    {                                   /* set new time & date stamp */
        if(DosSetFileInfo(handle, 1, &finfo, sizeof(finfo)))
            printf("\ncan't touch: %s", strlwr(qbuff));
        else                            /* audit touched files */
            printf("\ntouched:     %s", strlwr(qbuff));

        DosClose(handle);               /* release file handle */
    }       
}

