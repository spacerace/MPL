/*
 * SCREEN CAPTURE UTILITY - HEADER FILE
 *
 * LANGUAGE : Microsoft C 5.0
 * SDK      : Windows 2.03 SDK
 * MODEL    : small
 * STATUS   : operational
 *
 * This file contains all the definitions (except function definitions)
 * used by the screen capture program.  This file should be included
 * at compile time for each module linked into the system.
 *
 * 06/13/88 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* supplemental session menu definitions */
#define  CMD_CAPTURE          0x0050      /* Screen Capture... */

/* screen capture definitions */
#define  CAPTURE_OFF          0           /* capture not active */
#define  CAPTURE_CLIENT       1           /* capture client area */
#define  CAPTURE_WINDOW       2           /* capture active window */
#define  CAPTURE_SCREEN       3           /* capture entire screen */

/* screen capture dialog box messages */
#define  DLGSC_OFF            101         /* turn auto-capture off */
#define  DLGSC_CLIENT         102         /* capture client area */
#define  DLGSC_WINDOW         103         /* capture active window */
#define  DLGSC_SCREEN         104         /* capture entire screen */
#define  DLGSC_MONOCHROME     105         /* capture in monochrome */

/* screen capture macro definitions */
#define  SCREEN_WIDTH         GetSystemMetrics(SM_CXSCREEN)
#define  SCREEN_HEIGHT        GetSystemMetrics(SM_CYSCREEN)

#define  MSGBOX(x,y)          MessageBox(x,y,"Screen Capture",MB_OK)

/* undocumented Windows API calls */
int FAR PASCAL lstrlen( LPSTR );
int FAR PASCAL lstrcmp( LPSTR, LPSTR );
LPSTR FAR PASCAL lstrcpy( LPSTR, LPSTR );
LPSTR FAR PASCAL lstrcat( LPSTR, LPSTR );

/* general function declarations */
WORD FAR PASCAL PrtScHook( int, WORD, LONG );
BOOL FAR PASCAL PrtScDlgFn( HWND, WORD, WORD, LONG );
LONG FAR PASCAL PrtScFilterFn( HWND, WORD, WORD, LONG );
