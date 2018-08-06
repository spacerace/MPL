/***	ds.c - Display Subdirectories
*
*	Display subdirectories in tree form.  This example paints a directory
*	tree on the screen using VIO calls.  USER=drive:path must be in the
*	enviornment. It determines where the initialization file is kept. The
*	first time this program is called, it creates a new initialization file
*	and builds the display tree from the current directory.
*
*/

#include <stdio.h>

#include "ds.h"
#include "vars.h"

extern buildTree (Directory *, char *);
extern decorateTree (Directory *);
extern clearBottomLine ();
extern clearScreen();
extern fileInit ();
extern getState ();
extern handleDisplay (Directory *);
extern helpInit ();
extern optionInit ();
extern refreshDisplay (int, Directory *);
extern saveState ();
extern screenInit ();

/***	main - entry point to DS program
*
*
*/
main (argc,argv,envp)
int argc;
char *argv[];
char *envp[];
{
    initialize ();
    if (root->d_child) {		/* There is a tree to display */
	decorateTree (root);
	handleDisplay (root);
    }
    else {				/* No tree */
	VIOSETCURPOS (WINDOW_TOP+2,0, VioHandle); /* Avoid scrolling error */
	fprintf (stderr, "\nNo subdirectories in this directory.\n");
	exitError (2);
    }
    if (stateModified)
	saveState ();			/* Save colors */
    cleanUp ();
}   /* main */


/***	initialize - initialize components of DS program
*
*
*/
initialize ()
{
    int rCode;

    fileInit ();			/* Init path and file stuff */
    screenInit ();			/* Init the physical screen */
    helpInit ();			/* Initialize help screen */
    optionInit ();			/* Initialize option screen */
    rCode = getState ();		/* Read initialization file */
    clearScreen ();			/* Clear screen */
    refreshDisplay (-1, NULL);		/* Display screen header */
    if (!rCode) {			/* Init file bad */
	newDir (&root,"\0",0);		/* Make root node */
	buildTree (root,IntRootPath);	/* Build directory tree */
	stateModified = TRUE;
    }
    else
	stateModified = FALSE;
    return;

}   /* initialize */



/***	exitError - clean up and exit with error code
*
*
*/
exitError (code)
int code;
{
    cleanUp ();
    exit (code);
}   /* exitError */


/***	cleanUp - clean up before exit
*
*
*/
cleanUp ()
{
    clearBottomLine (); 	    /* Don't leave garbage if using vt52.sys */
}   /* cleanUpUp */
