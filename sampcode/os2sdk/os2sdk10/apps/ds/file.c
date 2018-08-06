/*  file.c - DS.INI file and path stuff */

#include <direct.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds.h"
#include "vars.h"


/* External procedures */

extern newDir (Directory **, char *, Directory *);


/*  Local variables */

static char initFile[MAX_PATH_LEN];	/* Initialization file */
static FILE *iF;			/* Init file handle */

static int  i_level;			/* Directory level */
static char i_name[FILE_NAME_LEN];	/* Directory name */


/***	fileInit - initialize file stuff
*
*
*/
fileInit ()
{
    char pLastChar;
    int pLast;
    char *iPath;

    RootPath = getcwd (NULL,MAX_PATH_LEN); /* Get current working directory */
    if (!RootPath) {
	perror ("\nCouldn't get current working directory");
	exitError (2);
    }
    strcpy (IntRootPath,RootPath);
    pLast =  strlen(IntRootPath) - 1;	/* Index of last character */
    pLastChar = IntRootPath[pLast];	/* Last character of root path */
    if ((pLastChar == '\\') || (pLastChar == '/'))
	IntRootPath[pLast] = '\0';	/* Get rid of path separator */

    iPath = getenv (INIT_SYMBOL);
    if (!iPath) {
	fprintf (stderr, "\n%s symbol not set in environment.\n",INIT_SYMBOL);
	fprintf (stderr,
		 "Put SET %s=drive:path in your AUTOEXEC.BAT.\n",INIT_SYMBOL);
	exitError (2);
    }
    strcpy (initFile,iPath);		/* Copy path */
    pLast =  strlen(initFile) - 1;	/* Index of last character */
    pLastChar = initFile[pLast];	/* Last character of root path */
    if ((pLastChar != '\\') && (pLastChar != '/'))
	strcat (initFile,"\\"); 	/* Append path separator */
    strcat (initFile,INIT_FILE);	/* Append file name and extension */
}   /* fileInit */


/***	saveState - save options to init file
*
*
*/
saveState ()
{
    int i;

    iF = fopen (initFile, "w");
    if (!iF) {
	fprintf (stderr, "\n%s", initFile);
	perror ("\nCannot write to initialization file");
	exitError (2);
    }

    fprintf (iF,INIT_FILE_HEADER);
    for (i=0; i<N_COLORS; i++)
	fprintf (iF, " %x", color[i]);
    fprintf (iF, "\n");

    fprintf (iF, "%s\n", IntRootPath);	/* Write out root directory */
    writeTree (root->d_child, 1);	/* Write out tree */
    fprintf (iF, "0 The_End\n");	/* Terminate tree */

    if (fclose (iF) == EOF) {
	fprintf (stderr,"\nCannot close initialization file");
	exitError (2);
    }

    /* Return to original directory */

    if (chdir(RootPath) == -1) {
	fprintf (stderr, "\nDirectory %s", RootPath);
	perror ("\nCannot change directory");
	exitError (2);
    }
}   /* saveState */


/***	writeTree - write out directory tree
*
*
*/
writeTree (root, level)
Directory *root;		 /* Root of some subtree */
int level;				/* Distance of this root to top root */
{
    Directory *p;		 /* Used to traverse tree */

    p = root;
    while (p)  {			/* Process directories at this level */
	writeNode (p,level);		/* Write out node information */
	if (p->d_child) 		/* Process subtree */
	    writeTree (p->d_child, level+1);
	p = p->d_next;
    } /* while */
}   /* writeTree */


/***	writeNode - write out a directory node
*
*
*/
writeNode (p, level)
Directory *p;			 /* Directory node to write out */
int level;				/* Distance of this node to top root */
{
    fprintf (iF, "%d %s\n", level, p->d_name);

}   /* writeNode */


/***	getState - get options from init file
*
*	EXIT:	rCode = true, tree WAS read from init file
*		rCode = false, tree NOT read from init file
*
*/
getState ()
{
    int i;
    int rCode;

    rCode = TRUE;
    iF = fopen (initFile, "r");
    if (!iF)				/* File not opened */
	return (FALSE);

    if (fscanf(iF,INIT_FILE_HEADER) == EOF)
	return (FALSE);

    for (i=0; i<N_COLORS; i++) {
	rCode = fscanf(iF, " %x", &color[i]);
	if (rCode != 1) 		/* Bad data */
	    i = N_COLORS;		/* Exit loop */
    }

    if (fscanf(iF,"%s\n", IntRootPath) == EOF)
	return (FALSE);

    newDir (&root,IntRootPath,0);	/* Make root node */
    rCode = readTree (root);

    if (fclose (iF) == EOF) {
	fprintf (stderr,"\nCannot close initialization file\n");
	exitError (2);
    }

    return (rCode);

}   /* getState */


/***	readTree - read in directory tree
*
*	EXIT:	rCode = true, tree WAS read from init file
*		rCode = false, tree NOT read from init file
*
*/
readTree (root)
Directory *root;		 /* Root of some subtree */
{
    int level;				/* Current level */
    Directory *p;		 /* Used to build tree */
    Directory *n;		 /* Used to build tree */
    int rCode;				/* Return code */

    p = root;				/* Root is parent node */
    level = 0;				/* Level of root */
    if (!readNode())			/* Failed to read first node */
	return (FALSE);

    while (i_level) {
	if (i_level > level ) { 	/* Subtree */
	    level++;			/* Increase level */
	    newDir (&n, i_name, p);	/* Create new node */
	    p->d_child = n;		/* Point parent to child */
	}
	else {				/* Sibling or higher up tree */
	    while (i_level < level) {	/* Backup, if necessary */
		level--;		/* Decrease level */
		p = p->d_parent;	/* Backup to parent */
	    }
	    newDir (&n, i_name, p->d_parent); /* Create new node */
	    p->d_next = n;		/* Point previous to new */
	    n->d_prev = p;		/* Point new to previous */
	}
	p = n;				/* New parent node */
	if (!readNode())		/* Failed to read next node */
	    return (FALSE);
    } /* while */
    return (TRUE);			/* Read entire tree successfully */
}   /* readTree */


/***	readNode - read in a directory node
*
*
*/
readNode ()
{
    int rCode;

    rCode = fscanf (iF, "%d %s\n", &i_level, i_name);
    if (rCode == 2)			/* Read succeeded */
	rCode = TRUE;
    else				/* Read failed */
	rCode = FALSE;

    return (rCode);			/* Return status */
}   /* readNode */
