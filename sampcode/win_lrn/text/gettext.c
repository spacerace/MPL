/*
 *   GetWindowText
 *
 *   This program demonstrates the use of the GetWindowText function. The
 *   GetWindowText function copies the given window's caption bar (if one
 *   exists) into the second parameter. GetWindowText returns an integer
 *   specifying the length of the caption. If GetWindowText returns 0, then
 *   there is no caption for the given window.
 *
 */

#include "windows.h"
#include <stdio.h>     /* required for sprintf */

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"Sample Application";
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
    int x;		       /* return value from GetWindowText	*/
    char buffer[40];	       /* buffer that GetWindowText places text */

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
			(LPSTR)"Sample Application",
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

    /* get caption bar text if it exists */
    x=GetWindowText(hWnd,buffer,40);
    /* if there is a caption */
    if (x>0)
      /* output the caption    */
      MessageBox(hWnd,(LPSTR)buffer,
		     (LPSTR)"THE CAPTION BAR SAYS:",MB_OK);
    else
      /* output "There is no Title Bar */
      MessageBox(hWnd,(LPSTR)NULL,
		     (LPSTR)"There is no Title Bar",MB_OK);

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
        switch (wParam)
        {
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
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
