/*
    QFN.C   Qualify Filename, OS/2 version
    Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>
#include <string.h>

#define API unsigned extern far pascal  /* OS/2 API functions */

API DosChDir(char far *, unsigned long);
API DosQCurDisk(int far *, unsigned long far *);
API DosQCurDir(int, void far *, int far *);
API DosSelectDisk(int);

static char qbuff[80];                  /* receives qualified filename */

char *qfn(char *p)
{   
    char tbuff[80];                     /* target directory */
    char cpath[80];                     /* current path at entry */
    int cdrive;                         /* current drive at entry */
    int cpsiz = sizeof(cpath);          /* size of path buffer */
    int qbsiz = sizeof(qbuff);          /* size of qual. name buffer */
    unsigned long drvmap;               /* bitmap for valid drives */
    char *q;                            /* scratch pointer */
    int i;                              /* scratch variable */

    DosQCurDisk(&cdrive, &drvmap);      /* get current drive */

    DosQCurDir(0, &cpath[1], &cpsiz);   /* get current directory */
    cpath[0] = '\\';                    /* and prepend backslash */

                                        /* any drive specified? */
    if((strlen(p) >= 2) && (p[1] == ':'))
    {   
        i = (p[0] | 0x20)-'a'+1;        /* get binary drive code */

        if(DosSelectDisk(i))            /* switch to new drive */
            goto errexit;               /* return if bad drive */

                                        /* get current directory again */
        DosQCurDir(0, &cpath[1], &cpsiz);

        p += 2;                         /* bump ptr past drive */
    }   

    strcpy(tbuff, p);                   /* copy target pathname 
                                           to local buffer */

    q = strrchr(tbuff, '\\');           /* look for last backslash */

    if (q != NULL)                      /* any path specified? */
    {   
        *q = 0;                         /* yes, make path ASCIIZ */

        if(q == tbuff)                  /* select directory */
        {   
            if(DosChDir("\\", 0L))      /* target is root */
                goto errexit;
        }
        else 
        {   
            if(DosChDir(tbuff, 0L))     /* target is not root */
            goto errexit;
        }
        q += 1;                         /* point to filename */
    }
    else q = tbuff;                     /* if no path specified,
                                           point to filename */

    if(q[0] == '.') goto errexit;       /* filename may not be 
                                           directory alias */

    /* drive and/or path are selected, build qualified filename */

    DosQCurDisk(&i, &drvmap);           /* get target drive */
    qbuff[0] = i+'a'-1;                 /* and convert to ASCII */

    qbuff[1] = ':';                     /* add drive delimiter */
    qbuff[2] = '\\';                    /* and root backslash */

    DosQCurDir(0, &qbuff[3], &qbsiz);   /* get target directory */
    
    i = strlen(qbuff);                  /* length of drive+path */

    if(i != 3) qbuff[i++] = '\\';       /* if not root, add
                                           trailing backslash */

    strcpy(qbuff+i, q);                 /* copy in filename */  

    DosChDir(cpath, 0L);                /* restore original path */
    DosSelectDisk(cdrive);              /* restore original drive */

    return(strlwr(qbuff));              /* fold pathname to lower 
                                           case, return pointer */

errexit:                                /* common error exit point */
    DosChDir(cpath, 0L);                /* restore original path */
    DosSelectDisk(cdrive);              /* restore original drive */
    return(NULL);                       /* return null pointer */
}

