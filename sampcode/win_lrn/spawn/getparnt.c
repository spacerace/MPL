/*
 *
 *   This program demonstrates use of the GetParent() function.
 *   GetParent() returns the parent (if one exists) of the given window.
 *   GetParent() is called three times in WinMain() in this application.
 *
 *   Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ChildAProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ChildBProc(HWND, unsigned, WORD, LONG);

HWND hChildAWnd = NULL;     /*	handle to child window	     */
HWND hChildBWnd = NULL;     /*	handle to grandchild window  */
HWND hMainWnd = NULL;       /*   handle to main window        */

/* Procedure called when the application is loaded for the first time */

BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"GetParent";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc	= HelloWndProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"CHILD A";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc	= ChildAProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"CHILD B";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc	= ChildBProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pHelloClass );
    return TRUE;        /* Initialization succeeded */
}

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    HWND  hParent;  /* return value from GetParent() */

    HelloInit( hInstance );

    hMainWnd = CreateWindow((LPSTR)"GetParent",
			(LPSTR)"GetParent() [hMainWnd]",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hMainWnd, cmdShow );
    UpdateWindow( hMainWnd );

    hChildAWnd = CreateWindow((LPSTR)"CHILD A",
			(LPSTR)"Child A [hChildAWnd]",
			WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION |
			WS_CLIPSIBLINGS,
			5,
			5,
			400,
			300,
			(HWND)hMainWnd,
			(HMENU)1,	   /* child: use constant */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    hChildBWnd = CreateWindow((LPSTR)"CHILD B",
			(LPSTR)"Child B [hChildBWnd]",
			WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION |
			WS_CLIPSIBLINGS,
			5,
			5,
			300,
			250,
			(HWND)hChildAWnd,
			(HMENU)2,	   /* child: use constant */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

/*
 *     hWnd is the parent of hChildAWnd and hChildAWnd is the parent of
 *     hChildBWnd. The following calls to GetParent() should confirm this
 */

hParent = GetParent(hMainWnd);
if (!hParent)
    MessageBox(NULL,
	       (LPSTR)"because hMainWnd has no parent",
	       (LPSTR)"GetParent(hMainWnd) returned NULL, which is correct...",
	       MB_OK);
else
    MessageBox(NULL,
	       (LPSTR)"which is an ERROR",
	       (LPSTR)"GetParent(hWnd) did not return NULL...",
	       MB_OK);

hParent = GetParent(hChildAWnd);
if (hParent == hMainWnd)
    MessageBox(NULL,
	       (LPSTR)"because hMainWnd is hChildAWnd's parent",
	       (LPSTR)"GetParent(hChildAWnd) returned hMainWnd, which is correct...",
	       MB_OK);
else
    MessageBox(NULL,
	       (LPSTR)"which is an ERROR",
	       (LPSTR)"GetParent(hChildAWnd) did not return hMainWnd,",
	       MB_OK);

hParent = GetParent(hChildBWnd);
if (hParent == hChildAWnd)
    MessageBox(NULL,
	       (LPSTR)"because hChildAWnd is hChildBWnd's parent",
	       (LPSTR)"GetParent(hChildBWnd) returned hChildAWnd, which is correct...",
	       MB_OK);
else
    MessageBox(NULL,
	       (LPSTR)"which is an ERROR",
	       (LPSTR)"GetParent(hChildBWnd) did not return hChildAWnd",
	       MB_OK);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* Procedures which make up the window class. */
long FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
	return DefWindowProc( hWnd, message, wParam, lParam );
	break;

    case WM_LBUTTONDOWN:
	MessageBox(hChildBWnd,(LPSTR)"Left Button Click",
			      (LPSTR)"PARENT WINDOW",
			      MB_OK);
	break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}

long FAR PASCAL ChildAProc( hChildAWnd, message, wParam, lParam )
HWND hChildAWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_LBUTTONDOWN:
	MessageBox(hChildBWnd,(LPSTR)"Left Button Click",
			      (LPSTR)"CHILD A",
			      MB_OK);
	break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
	BeginPaint( hChildAWnd, (LPPAINTSTRUCT)&ps );
	EndPaint( hChildAWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
	return DefWindowProc( hChildAWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}

long FAR PASCAL ChildBProc( hChildBWnd, message, wParam, lParam )
HWND hChildBWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_LBUTTONDOWN:
	MessageBox(hChildBWnd,(LPSTR)"Left Button Click",
			      (LPSTR)"CHILD B",
			      MB_OK);
	break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
	BeginPaint( hChildBWnd, (LPPAINTSTRUCT)&ps );
	EndPaint( hChildBWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
	return DefWindowProc( hChildBWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
