/*
 * WINDOWS FILE FINDER UTILITY - HEADER FILE
 *
 * LANGUAGE : Microsoft C 5.1
 * TOOLKIT  : Windows 2.10 SDK
 * MODEL    : Small or Medium
 * STATUS   : Operational
 *
 * 07/21/88 - Kevin P. Welch - initial creation.
 *
 */

/* general programming extensions */
#define  ID(x)             GetWindowWord(x,GWW_ID)
#define PARENT(x)       GetWindowWord(x,GWW_HWNDPARENT)
#define INSTANCE(x)     GetWindowWord(x,GWW_HINSTANCE)

/* system menu definitions */
#define  SC_ABOUT          0x0001   /* about command */

/* dialog box definitions */
#define  ID_PATTERN        100      /* file search string */
#define  ID_DRIVES         101      /* disk drive list */
#define  ID_FILES          102      /* filename list */
#define  ID_SEARCH         103      /* search button */
#define  ID_RUN            104      /* run button */
#define  ID_QUIT           105      /* quit button */

/* function definitions */
BOOL FAR PASCAL   Present( PSTR, PSTR );
BOOL FAR PASCAL   CenterPopup( HWND, HWND );
BOOL FAR PASCAL   Directory( PSTR, WORD, HWND );
BOOL FAR PASCAL   Dialog( HWND, LPSTR, FARPROC );
BOOL FAR PASCAL   AboutDlgFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL   FinderDlgFn( HWND, WORD, WORD, LONG );
