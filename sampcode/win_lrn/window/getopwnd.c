/*
 *
 *   This program demonstrates the use of the GetTopWindow() function.
 *   GetTopWindow() searches for the top level child window that belongs
 *   to the parent window specified by the "hWnd" parameter. GetTopWindow()
 *   is called twice in WinMain(). It is first called when no child windows
 *   exist, and then after two child windows have been created.
 *
 *   Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ChildAProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ChildBProc(HWND, unsigned, WORD, LONG);

HWND hChildAWnd = NULL;
HWND hChildBWnd = NULL;

/* Procedure called when the application is loaded for the first time */

BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"GetTopWindow";
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

    HWND  hTopWindow;	  /*** return value from GetTopWindow() ***/

    HelloInit( hInstance );

    hWnd = CreateWindow((LPSTR)"GetTopWindow",
			(LPSTR)"GetTopWindow()",
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
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    MessageBox(NULL,
	       (LPSTR)"Will destroy window if handle returned.",
	       (LPSTR)"First call to GetTopWindow().",
	       MB_OK);

/*** get handle to top level child to "hWnd" (if one exists) ***/
    hTopWindow = GetTopWindow(hWnd);
    if (hTopWindow)
	DestroyWindow(hTopWindow);
    else
	MessageBox(NULL,
		   (LPSTR)"No top level child window exists",
		   (LPSTR)"GetTopWindow() info:",
		   MB_OK);

    hChildAWnd = CreateWindow((LPSTR)"CHILD A",
			(LPSTR)"Child A",
			WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION |
			WS_CLIPSIBLINGS,
			5,
			5,
			250,
			250,
			(HWND)hWnd,
			(HMENU)1,	   /* child: use constant */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    hChildBWnd = CreateWindow((LPSTR)"CHILD B",
			(LPSTR)"Child B",
			WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION |
			WS_CLIPSIBLINGS,
			5,
			5,
			150,
			150,
			(HWND)hChildAWnd,
			(HMENU)2,	   /* child: use constant */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    MessageBox(NULL,
	       (LPSTR)"Will destroy window if handle returned.",
	       (LPSTR)"Second call to GetTopWindow().",
	       MB_OK);

/*** get handle to top level child to "hWnd" (if one exists) ***/
    hTopWindow = GetTopWindow(hWnd);
    if (hTopWindow)
	DestroyWindow(hTopWindow);
    else
	MessageBox(NULL,
		   (LPSTR)"No top level child window exists",
		   (LPSTR)"GetTopWindow() info:",
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