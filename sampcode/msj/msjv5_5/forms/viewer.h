/*
 * FORM VIEWER - HEADER FILE
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* default window characteristics */
#define    VIEWER_STYLE      WS_OVERLAPPEDWINDOW
#define    VIEWER_XPOS       CW_USEDEFAULT
#define    VIEWER_YPOS       CW_USEDEFAULT
#define    VIEWER_WIDTH      ((2*GetSystemMetrics(SM_CXSCREEN))/3)
#define    VIEWER_HEIGHT     ((2*GetSystemMetrics(SM_CYSCREEN))/3)

#define    VIEWER_HWNDEDIT MAKEINTRESOURCE(100)
#define    VIEWER_HFORMLIB MAKEINTRESOURCE(101)

/* menu definitions */
#define    IDM_OPEN      0x0100
#define    IDM_EDIT      0x0101
#define    IDM_PRINT     0x0102
#define    IDM_EXIT      0x0103
#define    IDM_ABOUT     0x0104

/* dialog box definitions */
#define    ID_OK         IDOK
#define    ID_CANCEL     IDCANCEL
#define    ID_FORM       0x0100

/* form interface definitions */
#define    FORM_INIT     MAKEINTRESOURCE(2)
#define    FORM_EDIT     MAKEINTRESOURCE(3)
#define    FORM_PRINT    MAKEINTRESOURCE(4)

/* general programming definitions */
#define    ID(x)         GetWindowWord(x,GWW_ID)
#define    PARENT(x)     GetWindowWord(x,GWW_HWNDPARENT)
#define    INSTANCE(x)   GetWindowWord(x,GWW_HINSTANCE)
#define    WARNING(x,y)  MessageBox(x,y,"Form Viewer",
                                    MB_OK|MB_ICONASTERISK)

/* utility function definitions */
BOOL FAR PASCAL    Dialog( HWND, LPSTR, FARPROC );
BOOL FAR PASCAL    CenterPopup( HWND, HWND );

/* window function definitions *application specific definitions */
BOOL FAR PASCAL    OpenDlgFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL    AboutDlgFn( HWND, WORD, WORD, LONG );
LONG FAR PASCAL    ViewerWndFn( HWND, WORD, WORD, LONG );

/* form library function definitions */
typedef HANDLE (FAR PASCAL * LPEDITFN)( HWND, HANDLE );
typedef HANDLE (FAR PASCAL * LPPRINTFN)( HWND, HANDLE );
