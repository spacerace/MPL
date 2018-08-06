/*  vars.c - global variables */

#include "ds.h"


cellType *DisplayRow[MAX_IMAGE_ROWS];	/* Display rows pointers */
int LastRow=0;				/* Index of last row in image */
int LastTopRow; 			/* Index of last top row */
char helpMsg[] = "Press F1 for Help";

char *RootPath; 			/* Path to root */
char IntRootPath[MAX_PATH_LEN]; 	/* Path to root without path separator */
Directory *root;			/* Root of directory tree */

int N_of_Cols;				/* Number of columns on screen */
int N_of_Rows;				/* Number of rows on screen */

int WINDOW_RIGHT;			/* Right-most column (zero based) */
int WINDOW_BOTTOM;			/* Bottom-most row (zero based) */
int WINDOW_SIZE;			/* Number of rows in window */

int stateModified;			/* !0 if colors/structure has changed */
