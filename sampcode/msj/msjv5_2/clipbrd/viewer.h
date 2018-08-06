/*
 * WINDOWS CLIPBOARD VIEWER - GENERAL HEADER FILE
 *
 * LANGUAGE      : Microsoft C 5.1
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * 1.01-Kevin P. Welch-add param to GetClipboardFmtName.
 *
 */

/* main menu definitions */
#define IDM_STATUS        0x0100
#define IDM_ADDFMT        0x0101
#define IDM_REMFMT        0x0102
#define IDM_EXIT          0x0103
#define IDM_ABOUT         0x0104
#define IDM_ERASE         0x0105
#define IDM_FORMATS       0x0106

/* private message definitions */
#define WM_UPDATE         (WM_USER+1)
#define WM_ADDFMT         (WM_USER+2)
#define WM_GETFMT         (WM_USER+3)
#define WM_REMFMT         (WM_USER+4)

/* dialog box definitions */
#define IDADD             0x0100
#define IDFORMAT          0x0101
#define IDLIBRARY         0x0102

#define IDREMOVE          0x0100
#define IDLIBLIST         0x0101

/* library function addresses */
#define LIB_INIT          MAKEINTRESOURCE(1)
#define LIB_CREATE        MAKEINTRESOURCE(2)
#define LIB_SIZE          MAKEINTRESOURCE(3)
#define LIB_HSCROLL       MAKEINTRESOURCE(4)
#define LIB_VSCROLL       MAKEINTRESOURCE(5)
#define LIB_PAINT         MAKEINTRESOURCE(6)
#define LIB_DESTROY       MAKEINTRESOURCE(7)

/* general programming extensions */
#define ID(x)         GetWindowWord(x,GWW_ID)
#define PARENT(x)     GetWindowWord(x,GWW_HWNDPARENT)
#define INSTANCE(x)   GetWindowWord(x,GWW_HINSTANCE)
#define WARNING(x,y)  MessageBox(x,y,"Clipboard Viewer",\
                                 MB_OK|MB_ICONEXCLAMATION)

/* library module data structure */
#define MAX_MODULE   16

typedef struct {
  WORD        hModule;
  WORD        wFormat;
} MODULE;

typedef struct {
  WORD        wModules;
  MODULE      Module[MAX_MODULE];
} LIBRARY;

typedef LIBRARY FAR * LPLIBRARY;

/* viewer function definitions (viewer1.c) */
LONG FAR PASCAL     ViewerWndFn( HWND, WORD, WORD, LONG );

/* client and status function definitions (viewer2.c) */
LONG FAR PASCAL     ClientWndFn( HWND, WORD, WORD, LONG );
LONG FAR PASCAL     StatusWndFn( HWND, WORD, WORD, LONG );

/* dialog function definitions (viewer3.c) */
BOOL FAR PASCAL     Dialog( HWND, LPSTR, FARPROC );
BOOL FAR PASCAL     AboutDlgFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL     RemFormatDlgFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL     AddFormatDlgFn( HWND, WORD, WORD, LONG );

/* utility function definitions (viewer4.c) */
BOOL FAR PASCAL     CenterPopup( HWND, HWND );
WORD FAR PASCAL     GetClipboardFmtNumber( LPSTR );
HANDLE FAR PASCAL   GetClipboardModule( WORD, HANDLE );
WORD FAR PASCAL     GetClipboardFmtName( WORD, LPSTR, WORD, BOOL );

/* undocumented internal function definitions */
int FAR PASCAL      lstrlen( LPSTR );
int FAR PASCAL      lstrcmp( LPSTR, LPSTR );
LPSTR FAR PASCAL    lstrcpy( LPSTR, LPSTR );
LPSTR FAR PASCAL    lstrcat( LPSTR, LPSTR );
