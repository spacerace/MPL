/*  display.c - screen display stuff for tree */

#include <conio.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds.h"
#include "vars.h"


/*  External routines */

extern pruneTree (Directory **);


/*  Local variables */

static char helpMsg[] = "Press F1 for Help";


/***	getKey - return two-byte key stroke; includes IBM extended ASCII codes
*
*
*/
unsigned int getKey ()
{
    char c;
    unsigned int key;

    if (c = getch())		    /* Get character */
	key = c << 8;		    /* Make scan code zero */
    else
	key = getch();		    /* Get scan code */
}   /* getKey */


/***	screenInit - Initialize the screen
*
*
*/
screenInit ()
{
    struct ModeData TheModeData;    /* Mode return data from VIOGETMODE */

    VIOGETMODE ((struct ModeData far *) &TheModeData, VioHandle);
    N_of_Cols = TheModeData.col;
    N_of_Rows = TheModeData.row;
    WINDOW_RIGHT = N_of_Cols - 1;
    WINDOW_BOTTOM = N_of_Rows - 1;
    WINDOW_SIZE = WINDOW_BOTTOM - WINDOW_TOP + 1;
}   /* screenInit */


/***	handleDisplay - display tree and handel user keystrokes
*
*
*/
handleDisplay (root)
Directory   *root;
{
    int topRow;
    unsigned int key;
    char tmp[MAX_PATH_LEN];
    char path[MAX_PATH_LEN];
    int imageRow;
    Directory *curDir,*lastDir,*p;
    int rc;
    char *commandPath;

    LastTopRow = LastRow-WINDOW_SIZE+1; /* Top row when last row displayed */
					/*   at the bottom of the window */
    if (LastTopRow < 0)
	LastTopRow = 0;

    topRow = 0;
    displayWindow (WINDOW_TOP,topRow,WINDOW_SIZE);
    curDir = root->d_child;
    highlightDir (topRow, curDir, color[cursorC]);
    key = 0;
    while (key != exitKey) {
	lastDir = curDir;
	key = getKey();
	switch (key) {
	    case optKey:
		showOption ();
		refreshDisplay (topRow, curDir);
		break;
	    case helpKey:
		showHelp ();
		refreshDisplay (topRow, curDir);
		break;
	    case enterKey:		      /* Exec shell */
		p = curDir;
		path[0] = '\0'; 	      /* Empty path */
		while (p->d_parent) {	      /* Process all but root */
		    strcpy (tmp, path);       /* Save path so far */
		    strcpy (path,"\\");       /* Put in separator */
		    strcat (path, p->d_name); /* Append directory name */
		    strcat (path, tmp);       /* Append rest of path so far */
		    p = p->d_parent;	      /* Go up to parent */
		}
		strcpy (tmp, path);	      /* Save tail of path */
		strcpy (path, IntRootPath);   /* Get root path */
		strcat (path, tmp);	      /* Append tail of path */

		if (chdir(path) == -1)	      /* Change directory failed */
		    pruneTree (&curDir);      /* Node is gone, update tree */
		else {
		    commandPath = getenv ("COMSPEC");	/* Get shell path */
		    clearBottomLine ();
		    if (commandPath)
			rc = spawnl(P_WAIT,commandPath,"COMMAND",NULL);
		    else
			rc = spawnlp(P_WAIT,"COMMAND","COMMAND",NULL);
		    if (rc == -1) {		  /* Exec failed */
			perror ("\nCouldn't spawn shell");
			exitError (2);
		    }
		    clearScreen ();
		}
		refreshDisplay (topRow, curDir);
		break;
	    case upKey: 		      /* Go to previous sibling */
		if (curDir->d_prev)	      /* Previous sibling exists */
		    curDir = curDir->d_prev;  /* Go to previous sibling */
		break;
	    case downKey:		      /* Go to next sibling */
		if (curDir->d_next)	      /* Next sibling exists */
		    curDir = curDir->d_next;  /* Go to next sibling */
		break;
	    case leftKey:		/* Go to parent */
		if (curDir->d_parent->d_parent) /* Not top level directory */
		    curDir = curDir->d_parent;	/* Go to parent */
		break;
	    case rightKey:		/* Go to first child */
		if (curDir->d_child)	/* Child exists */
		    curDir = curDir->d_child;  /* Go to child */
		break;
	    default:
		break;
	}   /* switch */
	if (curDir != lastDir)
	    adjustWindow (&topRow, curDir, lastDir);
    }	/* while */
}   /* handleDisplay */


/***	clearScreen - blank out the entire screen
*
*
*/
clearScreen ()
{
    Cell c;

    c.ch = ' ';
    c.at = color[blankC];
    VIOWRTNCELL(cefs(&c), N_of_Rows*N_of_Cols, 0, 0, VioHandle);

}   /* clearScreen */


/***	clearBottomLine - Clear the bottom line on the screen
*
*
*/
clearBottomLine ()
{
    Cell c;

    c.ch = ' ';
    c.at = color[blankC];
    VIOWRTNCELL(cefs(&c), N_of_Cols, N_of_Rows-1,0, VioHandle);

}   /* clearBottomLine */


/***	refreshDisplay - Repaint the main screen
*
*
*/
refreshDisplay (topRow, curDir)
int topRow;
Directory *curDir;
{
    int col;
    char buf[MAX_PATH_LEN];
    int lStr;
    Cell c;
    Attr a;

    c.ch = '[';
    c.at = color[statusC];
    VIOWRTNCELL(cefs(&c), 1, 0,0, VioHandle);

    lStr = strlen(RootPath);
    a = color[nameC];
    VIOWRTCHARSTRATT (chfs(RootPath), lStr, 0,1, afs(&a), VioHandle);

    c.ch = ']';
    c.at = color[statusC];
    VIOWRTNCELL(cefs(&c), 1, 0,lStr+1, VioHandle);

    lStr = strlen(helpMsg);
    col = N_of_Cols - lStr;
    a = color[statusC];
    VIOWRTCHARSTRATT (chfs(helpMsg), lStr, 0,col, afs(&a), VioHandle);

    if (topRow >= 0) {
	displayWindow (WINDOW_TOP,topRow,WINDOW_SIZE);
	highlightDir (topRow, curDir, color[cursorC]);
    }
    VIOSETCURPOS (N_of_Rows-1, N_of_Cols-1, VioHandle); /* Hide cursor */
}


/***	adjustWindow - Repaint tree with minimum screen writing
*
*
*/
adjustWindow (topRow, curDir, lastDir)
int *topRow;
Directory *curDir;
Directory *lastDir;
{
    int botRow, cRow, lRow;

    botRow = *topRow + WINDOW_SIZE - 1;
    cRow = curDir->d_row;
    lRow = lastDir->d_row;
    if ((*topRow <= cRow) && (cRow <= botRow)) { /* Window doesn't move */
	highlightDir (*topRow, lastDir, color[nameC]);	/* Turn off old cursor */
	highlightDir (*topRow, curDir, color[cursorC]); /* Turn on new cursor */
    }
    else if ((cRow <= *topRow - WINDOW_SIZE) ||
	    (botRow + WINDOW_SIZE <= cRow)) {	/* Repaint entire window */
	*topRow = (cRow < LastTopRow) ? cRow : LastTopRow;
	displayWindow (WINDOW_TOP,*topRow,WINDOW_SIZE);
	highlightDir (*topRow, curDir, color[cursorC]);
    }
    else {					/* Can do a scroll */
	if (cRow < lRow)	/* Pretend scroll up */
	    *topRow = (cRow < LastTopRow) ? cRow : LastTopRow;
	else {			/* Pretend scroll down */
	    cRow = cRow - WINDOW_SIZE + 1;
	    *topRow = cRow ? cRow : 0;
	}
	displayWindow (WINDOW_TOP,*topRow,WINDOW_SIZE);
	highlightDir (*topRow, curDir, color[cursorC]);
    }
}   /* adjustWindow */


/***	highlightDir - Highlight the currently selected directory
*
*
*/
highlightDir (topRow, p, color)
int topRow;
Directory *p;
int color;
{
    int dLen,cCol,cRow;
    Attr a;

    cRow = WINDOW_TOP + p->d_row - topRow;
    cCol = p->d_col;

    dLen = strlen(p->d_name);
    a = color;
    VIOWRTNATTR (afs(&a), dLen, cRow, cCol, VioHandle);

}   /* highlightDir */


/***	displayWindow - Displaya a portion of the tree in the tree window
*
*
*/
displayWindow (screenRow,imageRow,count)
int screenRow,imageRow,count;
{
    int row;
    Cell c;

    while (count-- && (imageRow <= LastRow)) {
	VIOWRTCELLSTR (cefs(DisplayRow[imageRow]), N_of_Cols*sizeof(Cell),
		       screenRow,0, VioHandle);
	screenRow++;
	imageRow++;
    }
    c.ch = ' ';
    c.at = color[blankC];
    for (row=screenRow; row<N_of_Rows; row++)	/* Clear remaining lines */
	VIOWRTNCELL (cefs(&c), N_of_Cols, row,0, VioHandle);
}   /* displayWindow */
