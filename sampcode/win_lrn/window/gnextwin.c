/*
 *
 *   gnextwin.c
 *
 *   This program demonstrates the use of the GetNextWindow() function.
 *   GetNextWindow() searches for a handle that identifies the next (or
 *   previous window, depending on the second parameter) in the window-
 *   manager's list. GetNextWindow() is called from WinMain() in this
 *   application, and if a non-null handle is returned, CloseWindow() is
 *   is called to make the window whose handle was returned iconic. If
 *   this application's window is not the active window, but no other
 *   window is over it, then that may mean the previous window is the
 *   blank window that you see in the background.
 *
 */

#include "windows.h"

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon          = LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName  = (LPSTR)"GetNextWindow";
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
    HWND  hNextWindow;   /*** return value from GetNextWindow() ***/

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"GetNextWindow",
                        (LPSTR)"GetNextWindow()",
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
               (LPSTR)"     If a non-null handle is returned, the window\n\
corresponding to this handle is brought to the\n\
top.",
               (LPSTR)"This application calls GetNextWindow().",
               MB_OK);

/***   get the handle to the next window in window-manager's list  ***/

    hNextWindow = GetNextWindow(hWnd,GW_HWNDNEXT);

    if (!hNextWindow)
        MessageBox(NULL,
                   (LPSTR)"There is no next window",
                   (LPSTR)"GetNextWindow() info:",
                   MB_OK);
    else if (hNextWindow == hWnd)
        MessageBox(NULL,
                   (LPSTR)"which is already the top window",
                   (LPSTR)"GetNextWindow returned this window...",
                   MB_OK);
    else if ( !IsIconic(hNextWindow) )
        BringWindowToTop(hNextWindow);
    else
        ShowWindow(hNextWindow,SW_SHOWMAXIMIZED);

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

