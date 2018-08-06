/*
 * Page Selection Control - HEADER FILE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * Eikon Systems, Inc.
 * 989 East Hillsdale Blvd, Suite 260
 * Foster City, California 94404
 * 415-349-4664
 *
 * 11/30/89 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* Page Control Commands */
#define  PM_SETRANGE       (WM_USER+1)
#define  PM_GETRANGE       (WM_USER+2)
#define  PM_SETSELECT      (WM_USER+3)
#define  PM_GETSELECT      (WM_USER+4)

/* Page Control Notifications */
#define  PN_SELCHANGE      1

/* Page Control Functions */
WORD FAR PASCAL   WEP( WORD );
HANDLE FAR PASCAL PageInfo( VOID );
BOOL FAR PASCAL   PageInit( HANDLE );
WORD FAR PASCAL   PageFlags( WORD, LPSTR, WORD );
LONG FAR PASCAL   PageWndFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL   PageStyle( HWND, HANDLE, LPFNSTRTOID, LPFNIDTOSTR );
