/*  vars.h - extern definitions for global variables */

extern cellType *DisplayRow[MAX_IMAGE_ROWS]; /* Display rows pointers */
extern char IntRootPath[MAX_PATH_LEN];	/* Path to root without path separator */
extern char *RootPath;			/* Path to root */
extern Directory *root; 		/* Root of directory tree */

extern int LastRow;			/* Index of last row in image */
extern int LastTopRow;			/* Index of last top row */


extern int N_of_Cols;			/* Number of columns on screen */
extern int N_of_Rows;			/* Number of rows on screen */

extern int WINDOW_RIGHT;		/* Right-most column (zero based) */
extern int WINDOW_BOTTOM;		/* Bottom-most row (zero based) */
extern int WINDOW_SIZE; 		/* Number of rows in window */

extern int color[N_COLORS];		/* Colors for screen output */

extern int stateModified;		/* !0 if colors/structure has changed */
