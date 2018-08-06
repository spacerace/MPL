/*
 * OS/2 PM File Finder Utility - HEADER FILE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft OS/2 PM SDK
 * STATUS        : operational
 *
 * 11/01/88 1.00 - Jerry Weldon - initial creation.
 *
 */

/* resource ids */
#define ID_FINDER         1

/* dialog ids */
#define IDD_PATTERN       100
#define IDD_DRIVES        101
#define IDD_FILES         102
#define IDD_SEARCH        103
#define IDD_RUN           104
#define IDD_QUIT          105
#define IDD_ABOUTDLG      200

/* menu ids */
#define IDM_ABOUT         101

/* macro definitions */
#define ABS(x)            ((x)<0?(-(x)):(x))
#define HWNDOWNER(h)      WinQueryWindow( h, QW_OWNER, FALSE )
#define HWNDPARENT(h)     WinQueryWindow( h, QW_PARENT, FALSE )
#define MAPWINDOWRECTS(hwndFrom,hwndTo,prcl,crcl) \
          WinMapWindowPoints( hwndFrom, hwndTo, (PPOINTL)prcl, 2*crcl )

/* function prototypes */
MRESULT EXPENTRY FinderDlgProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY AboutDlgProc( HWND, USHORT, MPARAM, MPARAM );
static BOOL Directory( NPSZ, USHORT, HWND );
static VOID CenterPopup( HWND, HWND );
static VOID MakeDateString( FDATE, NPSZ );
static VOID MakeTimeString( FTIME, NPSZ );
