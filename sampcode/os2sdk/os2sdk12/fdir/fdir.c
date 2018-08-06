/*************************************************************

 This sample code demonstrates conversion from longnames to 8.3 and how
 to determine the longname from the EA of a FAT file.
 Should be compiled with the Large model.

 Written by Jeff Johnson 6/20/89

 This code was written to demonstrate two new features of OS/2 V1.2:
 1) How to convert long filenames to the appropriate 8.3 names.
 2) How to find a file's longname on a FAT system by checking its EA.

 Procedures in this file:
   main()             Does the main directory program calling support modules
   Convert_to_8dot3() Converts a longname to 8.3 name
   ParsePathName()    Breaks a full path into its 3 components
   QueryLongname()    Gets the .LONGNAME EA for a given file
   QueryIFS()         Determines the IFS type of a drive

**************************************************************/

#define INCL_DOSFILEMGR
#define INCL_BASE
#include <os2.h>

#include <string.h>
#include <malloc.h>
#include <stdio.h>

#define FAT  0
#define HPFS 1
#define GetInfoLevel1 0x0001
#define GetInfoLevel3 0x0003

VOID Convert_to_8dot3(CHAR *,CHAR *);
VOID ParsePathName(CHAR *,CHAR *,CHAR *,CHAR *);
VOID QueryLongname(CHAR *,CHAR *);
VOID QueryIFS(CHAR *,PUSHORT,PUSHORT);


/*
 * Function name: main()
 *
 * Parameters:  argc, argv.  If the user places a file spec on the command
 *              line it is used to select/filter the directory listing.
 *              Otherwise, the default directory is listed.
 *
 * Returns: Always exits with 0.
 *
 * Purpose: main() coordinates the directory process by calling the
 *          appropriate setup routines, then handling the DosFindNext
 *          loop for each file.
 *
 * Usage/Warnings:  Very little error checking is done as the code is written
 *                  to demonstrate longname conversion/EA reading, not as
 *                  a finished app.
 *
 * Calls: ParsePathName(), QueryIFS(), Convert_to_8dot3(), QueryLongname()
 */

main (int argc, char *argv[])
{
    USHORT uRetval,hdir=0xffff,SearchCount=1;
    PFILEFINDBUF pfbuf;
    char szDrive[3],szPath[260],szFilename[CCHMAXPATH],szFullPath[CCHMAXPATH];
    USHORT ifsloc,ifsname;
    char *szFilePtr,szLongName[260];

    if(argc<2)
        ParsePathName("",szDrive,szPath,szFilename);
    else
        ParsePathName(argv[1],szDrive,szPath,szFilename);

    if(strlen(szFilename) == 0)
        strcpy(szFilename,"*");

    strcpy(szFullPath,szDrive);
    strcat(szFullPath,szPath);
    szFilePtr = szFullPath + strlen(szFullPath);
    strcat(szFullPath,szFilename);

    QueryIFS(szDrive,&ifsloc,&ifsname);

    if(ifsname != FAT && ifsname != HPFS)
    {
        printf("Unrecognized file system.\n");
        return 0;
    }

    if(ifsname == FAT)
        printf("FAT -> HPFS directory listing\n");
    else
        printf("HPFS -> FAT directory listing\n");


    pfbuf = (PFILEFINDBUF) malloc(sizeof(FILEFINDBUF));
    uRetval=DosFindFirst(szFullPath,(PHDIR) &hdir,FILE_DIRECTORY,
                         pfbuf,sizeof(FILEFINDBUF),&SearchCount,0L);

    if(uRetval)
    {
        printf("No files found.\n");
        return 0;
    }

    do
    {
        if(ifsname == FAT)
        {
            strcpy(szFilePtr,pfbuf->achName);  /* Drop in name after path */

            QueryLongname(szFullPath,szLongName);

            if(strlen(szLongName) == 0)
                printf("%s\n",pfbuf->achName);
            else
                printf("%s\n",szLongName);
        }
        else   /* It's HPFS */
        {
            Convert_to_8dot3(pfbuf->achName,szFilename);
            printf("%s\n",szFilename);
        }
    } while(!(uRetval=DosFindNext(hdir,pfbuf,
                                  sizeof(FILEFINDBUF),&SearchCount)));
}


/*
 * Function name: Convert_to_8dot3()
 *
 * Parameters:  szLong points to the input long file name.
 *              szFat points to a return buffer for the FAT compatible name.
 *
 * Returns: VOID. The converted string is placed in szFat buffer.
 *
 * Purpose: Converts a HPFS longname to the standard 8.3 name.  This is
 *          done as follows:  The extension is the first 3 characters after
 *          the last dot, no extension if there are no dots.  The file stem
 *          is at most 8 character and is taken from the beginning of the
 *          longname string, replacing all dots with underscores.
 *
 * Usage/Warnings:  Should be bulletproof.  Exception code included to allow
 *                  the special file name '..' through.
 *
 * Calls:
 */

VOID Convert_to_8dot3(CHAR *szLong,CHAR *szFat)
{
    USHORT usStemMaxLen; /* Holds the max size the 8 char base can be */
    USHORT cnt;
    CHAR   szStem[9],szExt[4];  /* Holds the Stem and Extension */
    CHAR   *szLastDot;          /* Pointer to the last dot */

    if(!strcmp(szLong,"..")) /* Allow the predecessor file to pass thru */
    {
        strcpy(szFat,"..");
        return;
    }

    szLastDot = strrchr(szLong,'.'); /* Find the last period */

    if(szLastDot)  /* There is at least one period */
    {
        strncpy(szExt,szLastDot+1,3);       /* 1st 3 chars after . are ext */
        szExt[3]=0;
        usStemMaxLen = szLastDot - szLong;  /* Max stem is everything b4 . */
    }
    else
    {
        *szExt = 0;                         /* No extension */
        usStemMaxLen = strlen(szLong);      /* Stem can be whole string */
    }

    if(usStemMaxLen>8)                      /* Limit stem to 8 chars */
        usStemMaxLen = 8;

    for(cnt=0;cnt<usStemMaxLen;cnt++)       /* Copy in chars to form stem */
    {
        switch(szLong[cnt])
        {
            case '.':                       /* Convert .'s to _'s */
                szStem[cnt] = '_';
                break;
            default:                        /* Copy all other chars */
                szStem[cnt] = szLong[cnt];
                break;
        }
    }
    szStem[cnt] = 0;

    /* Put it all together */
    strcpy(szFat,szStem);
    strcat(szFat,".");
    strcat(szFat,szExt);
}


/*
 * Function name: ParsePathName()
 *
 * Parameters:  szFullPath points to the input full path name.
 *              szDrive points to the return buffer for the drive letter.
 *              szPath points to the return buffer for the path.
 *              szFilename points to the return buffer for the Filename.
 *
 * Returns: VOID. The converted string is placed in last 3 passed params.
 *
 * Purpose: Break a full path string and break it into its three components.
 *          If the passed string doesn't have a drive, the current letter is
 *          fetched an placed in the return buffer.  The same is true for
 *          the path buffer.
 *
 * Usage/Warnings:  Error checking should be done on the DOS calls.
 *
 * Calls:
 */

VOID ParsePathName(CHAR *szFullPath,CHAR *szDrive,CHAR *szPath,CHAR *szFilename)
{
    CHAR *szBack;          /* Used to find last backslach */
    USHORT usPathLen;      /* Holds the length of the path part of string */

    *szPath = *szFilename = 0;

    /* Do the Drive letter */
    if(*(szFullPath+1)==':')           /* If there is a drive letter */
    {
        szDrive[0] = *szFullPath;

        szFullPath += 2;
    }
    else                               /* We take the default */
    {
        USHORT dno;  /* Drive number */
        ULONG  dmap; /* Map of available drives */

        DosQCurDisk((PUSHORT) &dno,(PULONG) &dmap);
        *szDrive = (CHAR)( dno + 'A'-1);
    }
    szDrive[1] = ':';          /* Add the colon */
    szDrive[2] = (CHAR) 0;

    /* Now do the path */
    szBack = strrchr(szFullPath,'\\');
    if(szBack)                         /* There is at least 1 backslash */
    {
       usPathLen = szBack - szFullPath + 1;
       strncpy(szPath,szFullPath,usPathLen);   /* Copy path */
       szPath[usPathLen] = (CHAR) 0;
    }
    else
    {
       *szPath = (CHAR) 0;
       szBack  = szFullPath-1;  /* Points 1 char before the file name */
    }

    /* Finally do the file name */
    strcpy(szFilename,szBack+1);
}


/*
 * Function name: QueryLongname()
 *
 * Parameters:  szfile points to the file to be queried.
 *              szLong points to the return buffer for the long filename.
 *
 * Returns: VOID. The converted string is placed in last 3 passed params.
 *
 * Purpose: Looks for an EA named .LONGNAME attached to szfile.  If found,
 *          it places the EA value in the return buffer.
 *
 * Usage/Warnings:  Routine assumes that the EA format is LP ASCII which
 *                  is what the specs required, but probably the exception
 *                  handling should be a bit tighter.  Return buf should be
 *                  at least CCHMAXPATH long to accomodate max length names.
 *                  Note also that no check is made to prevent overwriting
 *                  the end of the return buffer.
 *
 * Calls:
 */

VOID QueryLongname(CHAR *szfile,CHAR *szLong)
{
    CHAR    *szEAName;  /* Points to the .LONGNAME string */
    SHORT   cbEAName;   /* Length of the .LONGNAME string */

    SHORT   cbFEAList;  /* The length of the FEA buf to be used */
    SHORT   cbGEAList;  /* "                 GEA                */

    CHAR   cvFdump[300],cvGdump[50];     /* FEA and GEA buffers */
    FEALIST *pFEAList;                   /* Pointers to the buffers */
    GEALIST *pGEAList;
    EAOP    eaop;
                                         /* Pass struct for Dos call */
    USHORT  usRetval;
    SHORT   cbRet;
    CHAR    *szT;

    *szLong = (CHAR) 0;                /* Default if we can't get the EA */

    szEAName = ".LONGNAME";        /* The particular EA we are interested in */
    cbEAName = strlen(szEAName);

    cbGEAList = sizeof(GEALIST) + cbEAName;  /* Set buf lengths */
    cbFEAList = sizeof(FEALIST) + cbEAName+1 + 256 + 2;

    pFEAList = (FEALIST *) cvFdump;   /* Set pointers to 2 buffers */
    pGEAList = (GEALIST *) cvGdump;

    eaop.fpGEAList = (PGEALIST) pGEAList; /* Build the EAOP struct */
    eaop.fpFEAList = (PFEALIST) pFEAList;

    pGEAList->cbList = cbGEAList;       /* Fill in GEA buf length */
    pGEAList->list[0].cbName = (CHAR) cbEAName; /* Set .longname length */
    strcpy((char *) pGEAList->list[0].szName, (char *) szEAName);

    pFEAList -> cbList = cbFEAList; /* Set length of receiving buffer */

    usRetval = DosQPathInfo(szfile,      /* Get the .LONGNAME EA */
                            GetInfoLevel3,
                            (PVOID) &eaop,
                            sizeof(EAOP),
                            0L);

    if(usRetval != 0) /* There was an error */
       return;

    if(pFEAList->list[0].cbValue <=0)  /* It couldn't return EA value */
       return;

    szT = (CHAR *) pFEAList + sizeof(FEALIST) + cbEAName + 1;
    if (*((USHORT *) szT) == 0xfffd) /* length preceeded ASCII */
    {
        szT += 2;
        cbRet = *((USHORT *) szT);

        szT += 2;
        strncpy(szLong,szT,cbRet);
        szLong[cbRet]=0;
    }
}


/*
 * Function name: QueryIFS()
 *
 * Parameters:  szDrive points to drive letter to be queried.
 *              pusLocale will contain the location of the drive:
 *                        3=local, 4=remote.
 *              pusFSDName will contain the IFS type: 0=FAT, 1=HPFS, 2=Other.
 *
 * Returns: VOID.  All returns are in the last 2 params.
 *
 * Purpose: Mainly used to determine whether the file system is FAT or HPFS.
 *          also returns info on whether the drive is local or remote.
 *
 * Usage/Warnings:  Error checking on the DOS call should be implemented.
 *                  The buffer filled by DosQFSAttach is structured as follows:
 *                         USHORT iType;
 *                         USHORT cbName;
 *                         UCHAR  szName[];
 *                         USHORT cbFSDName;
 *                         UCHAR  szFSDName[];
 *                         ...
 *
 * Calls:
 */

VOID QueryIFS(CHAR *szDrive,PUSHORT pusLocale,PUSHORT pusFSDName)
{
    CHAR vChunk[100];             /* Buffer for data from DosQFSAttach */
    USHORT usChunkLen=100;
    USHORT usTemp;                /* Holds offset for FSDName */

    DosQFSAttach(szDrive,0,GetInfoLevel1, vChunk,&usChunkLen,0L);

    *pusLocale = *((PUSHORT) vChunk);  /* Set local from 1st USHORT *);*/

    /* Skip over iType, cbName, szName, and cbFSDName fields to szFSDName */
    usTemp = *((PUSHORT) &vChunk[sizeof(USHORT)]) + sizeof(USHORT)*3 + 1;

    if(!strcmp("FAT",&vChunk[usTemp]))
    {
        *pusFSDName = 0;
        return;
    }
    if(!strcmp("HPFS",&vChunk[usTemp]))
    {
        *pusFSDName = 1;
        return;
    }

    *pusFSDName = 2;
    return;
}


