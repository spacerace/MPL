/*
Microsoft Systems Journal
Volume 3; Issue 3; May, 1988

Code Listings For:

	DEBUG & BUGTEST
	pp. 64-72


Author(s): Kevin P. Welch
Title:     Debug Microsoft Windows Programs More Effectively with a 
           Simple Utility



Code Listings NOT INCLUDED in Article (referred to on page 66, middle column)

*/


/*
 * WINDOWS 2.00 DEBUG UTILITY - HEADER FILE (DEBUG.H)
 *
 * LANGUAGE : Microsoft C 5.0
 * MODEL    : small or medium
 * STATUS   : operational
 *
 * 12/11/87 1.00 - Kevin P. Welch - initial creation.
 *
 */
 
/* debug control panel constants */
#define	BUG_OFF		100	/* debug inactive */
#define	BUG_ON		101	/* debug active */
#define	BUG_COUNT	102	/* count debug events */
#define	BUG_DISPLAY	103	/* display debug events */
#define	BUG_FILTER	104	/* filter debug events */
#define	BUG_FILTERLIST	105	/* debug filter list */
#define	BUG_LOG		106	/* log debug events */
#define	BUG_LOGFILE	107	/* debug log file */
#define	BUG_OK		108	/* OK button */
#define	BUG_CANCEL	109	/* CANCEL button */

/* default dialog box definitions */
#define	DEF_BUTTON	(BS_DEFPUSHBUTTON|WS_TABSTOP|WS_CHILD)

/* standard dialog box definitions */
#define	STD_FRAME	(SS_BLACKFRAME|WS_CHILD)
#define	STD_CHECKBOX	(BS_CHECKBOX|WS_TABSTOP|WS_CHILD)
#define	STD_BUTTON	(BS_PUSHBUTTON|WS_TABSTOP|WS_CHILD)
#define	STD_RADIO	(BS_RADIOBUTTON|WS_TABSTOP|WS_CHILD)
#define	STD_EDITFIELD	(WS_BORDER|WS_TABSTOP|WS_CHILD|
				ES_AUTOHSCROLL)

/* debug function definitions */
BOOL FAR 		 Debug();
BOOL FAR PASCAL DebugControl( HWND );
BOOL FAR PASCAL DebugSetup( HWND, WORD, WORD );
BOOL FAR PASCAL DebugControlDlgFn( HWND, WORD, WORD, LONG );
