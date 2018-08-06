/*  tree.c - build and maintain directory tree */

#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#include "ds.h"
#include "vars.h"

/*  Local variables */

static int ImageExists=FALSE;		/* TRUE if image has been created */


/***	decorateTree - Decorate directory tree and build screen image
*
*
*/
decorateTree (root)
Directory *root;
{
    LastRow = -1;
    newLine ();
    decorateSubTree (root, 0);
    ImageExists = TRUE; 		/* Set flag for newLine() */
}   /* decorateTree */


/***	decorateSubTree - Recursive portion called by decorateTree
*
*
*/
decorateSubTree (par, level)
Directory *par;
int level;
{
    Directory *p;
    int col,first;

    first = TRUE;
    col = level*D_INDENT;
    p = par->d_child;
    while (p) {
	if (first)		    /* First subdir at this level */
	    first = FALSE;	    /* show on same row as parent */
	else			    /* Not first at this level */
	    newLine();		    /* Show underneath previous one */
	p->d_row = LastRow;
	p->d_col = col;
	addImage (p,level);	    /* Add image for this directory */
	decorateSubTree (p, level+1);  /* Process subtree */
	p = p->d_next;
    }	/* while */
}   /* decorateSubTree */


/***	addImage - add image of one directory into screen image
*
*
*/
addImage (p,level)
Directory *p;
int level;
{
    Directory *par;
    int pLen,bLen,bCol;
    char buf[MAX_COLS+1];
    int parRow;
    int row;
    int col;
    int distance;
    cellType *c;

    par = p->d_parent;			    /* Get parent pointer */
    modLine (p->d_col, p->d_name, color[nameC]); /* Put directory name in image */
    if (level > 0)  {			    /* Draw line to parent */
	distance = p->d_row - par->d_row;
	if (distance == 0) {		    /* Parent on same row */
	    pLen = strlen (par->d_name);
	    bLen = D_INDENT - pLen;	    /* Length of bar */
	    bCol = par->d_col+pLen;
	    buf[bLen] = '\0';		    /* Store string terminator */
	    memset (buf,LR_CHAR,bLen);	    /* Store bar in buffer */
	    modLine (bCol, buf, color[barC]);	   /* Put bar in image */
	}
	else {				    /* Parent is above */
	    bLen = D_INDENT;
	    bCol = par->d_col;
	    buf[bLen] = '\0';		    /* Store string terminator */
	    memset (buf,LR_CHAR,bLen);	    /* Store bar in buffer */
	    buf[0] = RT_CHAR;		    /* Store angle char */
	    modLine (bCol, buf, color[barC]);	   /* Put bar in image */
	    row = LastRow-1;
	    col = par->d_col;
	    parRow = par->d_row;
	    while (row > parRow) {	    /* Finish path */
		c = &DisplayRow[row][col];
		switch (c->theChar) {
		    case ' ':
			c->theChar = BT_CHAR;
			c->theAttr = color[barC];
			row--;
			break;

		    case RT_CHAR:
			c->theChar = BRT_CHAR;
			row = 0;
			break;

		    default:
			row = 0;
			break;
		}   /* switch */
	    }	/* while */
	}   /* else (distance != 0) */
    }	/* if (level > 0) */
}   /* addImage */


/***	newLine - add a new line to the scree image
*
*
*/
newLine ()
{
    cellType *p;
    int i;

    if (++LastRow >= MAX_IMAGE_ROWS) {	    /* Too many image rows */
	fprintf (stderr, "\nDirectory image too large - Internal error.\n");
	exitError (2);
    }
    if (ImageExists)			    /* Get image row pointer */
	p = DisplayRow[LastRow];
    else				    /* Make new image row */
	p = 0;
    if (!p) {				    /* Have to make new image row */
	p = (cellType *) calloc(N_of_Cols,sizeof(cellType));
	if (!p) {
	    fprintf (stderr, "\nOut of memory - Allocating image line\n");
	    exitError (2);
	}
	DisplayRow[LastRow] = p;	    /* Store pointer to image row */
    }	/* if */
    for (i=0; i<N_of_Cols; i++) {	    /* Blank out row */
	p->theChar = ' ';
	p->theAttr = color[blankC];
	p++;
    }
}   /* newLine */


/***	modLine - store string with color in current line
*
*
*/
modLine (col, str, color)
int col;
char *str;
int color;
{
    cellType *p;
    char *s;

    s = str;
    p = &DisplayRow[LastRow][col];
    while (*s) {
	p->theChar = *s++;
	p->theAttr = color;
	p++;
    }	/* while */
}   /* modLine */


Canonicalize (fullPath, p, headPath)
char *fullPath;
Directory *p;
char *headPath;
{
    if (p->d_parent)
	Canonicalize (fullPath, p->d_parent, headPath);
    else
	strcat (fullPath, headPath);
    if (fullPath[strlen(fullPath)-1] != '\\')
	strcat (fullPath, "\\");
    strcat (fullPath, p->d_name);
}


/***	buildTree - scan directories breadth-first to build tree
*
*
*/
buildTree (par,parPath)
Directory *par;
char *parPath;
{
    char	 sName[MAX_PATH_LEN];	    /* Search path name */
    Directory	*child;
    int 	 errCode;
    int 	 sLen;
    Attr	 a;
    Cell	 c;
    Directory	*bfsHead;
    Directory	*bfsTail;
    FileSearch	 sBuf;		      /* Find First/Next buffer */
    unsigned	 sCount;	      /* Number of entries to search for */
    unsigned	 sHandle;	      /* Search Handle */

    bfsHead = par;
    bfsTail = par;
    c.ch = ' ';
    c.at = color[blankC];
    a = color[statusC];

    while (bfsHead) {
	sName[0] = '\0';
	Canonicalize (sName, bfsHead, parPath);

	sLen = strlen(sName);
	VIOWRTCHARSTRATT (chfs(sName), sLen, WINDOW_TOP,0, afs(&a), VioHandle);
	VIOWRTNCELL (cefs(&c), N_of_Cols-sLen, WINDOW_TOP,sLen, VioHandle);

	if (sName[sLen-1] == '\\')
	    strcat (sName, "*.*");
	else
	    strcat (sName, "\\*.*");
	sHandle = -1;			/* Family API only has one handle */
	sCount = 1;			/* Search for one at a time */
	errCode = DOSFINDFIRST (chfs(sName),
				ufs(&sHandle),
				DosAttrSubDir,
				(FileSearch far *) &sBuf,
				sizeof(sBuf),
				ufs(&sCount),
				0L);
	while (!errCode) {
	    if (sBuf.file_name[0] != '.') { /* Not a back link */
		if (sBuf.attributes & DosAttrSubDir) {	/* A subdirectory */
		    newDir (&child, sBuf.file_name, bfsHead);
		    insDir (bfsHead, child, &bfsTail);
		    bfsTail->d_link = child;
		    bfsTail = child;
		}
	    }
	    errCode = DOSFINDNEXT (sHandle,
				   (FileSearch far *) &sBuf,
				   sizeof(sBuf),
				   ufs(&sCount));
	}
	DOSFINDCLOSE (sHandle);
	bfsHead = bfsHead->d_link;
    }
}   /* buildTree */


/***	insDir - insert child directory into tree in alphabetical order
*
*
*/
insDir (par,child)
Directory  *par;
Directory  *child;
{
    Directory *p,*q;

    p = par->d_child;			/* p = first child */
    q = 0;				/* q = previous pointer */
    while (p && (strcmp(child->d_name,p->d_name) > 0)) {
	q = p;				/* q = previous node */
	p = p->d_next;			/* p = next node */
    }
    if (!q)				/* Insert at front of list */
	par->d_child = child;
    else				/* Insert at middle or end of list */
	q->d_next = child;

    child->d_prev = q;
    child->d_next = p;

    if (p)				/* Not inserting at end of list */
	p->d_prev = child;
}   /* insDir */


/***	newDir - create a new directory entry for tree
*
*
*/
newDir (dir,name,par)
Directory **dir;
char *name;
Directory *par;
{
    Directory *p;

    p = (Directory *) malloc (sizeof(Directory));
    if (!p) {
	fprintf (stderr, "\nOut of memory - Allocating directory node\n");
       exitError (2);
    }
    p->d_name[0]   = '\0';
    strcpy (p->d_name,name);
    p->d_parent    = par;
    p->d_prev	   = 0;
    p->d_next	   = 0;
    p->d_child	   = 0;
    p->d_link	   = 0;

    *dir = p;			    /* Return address of new dir node */
}   /* newDir */


/***	pruneTree - prune subtree that has been deleted
*
*	Called when we discover that a directory no longer exists.
*
*	ENTRY:	root = address of pointer to directory node that has evaporated.
*
*	EXIT:	root = address of pointer to some other node that is "close"
*		       to the evaporated node.	Choose previous, next, or
*		       parent, in that order.
*/
pruneTree (curDir)
Directory **curDir;
{
    Directory *p;			/* Used to traverse tree */
    Directory *q;			/* Used to traverse tree */
    Directory *n;			/* Node to return */

    p = *curDir;			/* p = node to delete */
    if (n = p->d_prev) {		/* n = previous sibling */
	q = p->d_next;			/* q = next for n */
	n->d_next = q;			/* Point new node to new next */
	if (q)				/* Not last sibling */
	    q->d_prev = n;		/* Point next back to new node */
    }
    else				/* No previous sibling */
	if (n = p->d_next) {		/* n = next sibling */
	    n->d_prev = 0;		/* Next sibling is now first */
	    q = n->d_parent;		/* q = parent */
	    q->d_child = n;		/* Point parent to new first child */
	}
	else
	    if (n = p->d_parent)	/* n = parent */
		if (n->d_parent)	/* n is not root */
		    n->d_child = 0;	/* Deleted only child of parent */
		else {
		    fprintf (stderr, "directory tree is empty\n");
		    exit (2);
		}
	    else {
		fprintf (stderr, "directory tree is empty\n");
		exit (2);
	    };

    delDir (p); 			/* Delete subtree */
    decorateTree (root);		/* Re-build tree image */
    stateModified = TRUE;		/* Tree is modified */
    *curDir = n;			/* Return new node */

}   /* pruneTree */


/***	delDir - Delete directory subtree
*
*
*/
delDir (root)
Directory *root;
{
    Directory *p;
    Directory *q;

    p = root->d_child;			/* p = first child */
    while (p) { 			/* Still children to delete */
	q = p;
	p = p->d_next;
	delDir (q);
    }

    free ( (char *) root);

}   /* delDir */
