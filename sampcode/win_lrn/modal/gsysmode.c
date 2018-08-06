/*
 *
 *
 *   This program demonstrates the use of the GetSysModalWindow() function.
 *   GetSysModalWindow(). GetSysModalWindow() returns a handle to a system
 *   modal window if one exists.
 *
 *   Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"

HWND hSysModalWnd;

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName	= (LPSTR)"min";
    pHelloClass->lpszClassName	= (LPSTR)"SetSysModalWindow";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc    = HelloWndProc;

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

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"SetSysModalWindow",
			(LPSTR)"SetSysModalWindow()",
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
	       (LPSTR)"I'm a system modal message box",
	       (LPSTR)"HI!",
	       MB_SYSTEMMODAL);

    hSysModalWnd = GetSysModalWindow();

    if (hSysModalWnd)
	DestroyWindow(hSysModalWnd);
    else
	MessageBox(NULL,
		   (LPSTR)"not found",
		   (LPSTR)"system modal window...",
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

    case WM_COMMAND:

/*** make "hWnd" system modal ***/
	SetSysModalWindow(hWnd);

	break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
	BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
	TextOut(ps.hdc,
		5,
		5,
		(LPSTR)"To make this window system modal, click on the menu",
		(long)51);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
