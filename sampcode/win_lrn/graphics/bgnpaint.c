/*
 *
 *   bgnpaint.c, jeffst, v1.00, 6-Dec-1987
 *
 *   This program demonstrates the use of the BeginPaint() function.
 *   BeginPaint() prepares the given window for painting and fills the paint
 *   structure with information about the painting. BeginPaint() sets the
 *   clipping region to exclude any area outside the update region. This
 *   is demonstrated in this program by a call to ValidateRect() immediately
 *   before the BeginPaint() call. BeginPaint() is called in response to a
 *   WM_PAINT message in HelloWndProc. This is standard for most window
 *   procedures in Windows applications.
 *
 *   Microsoft Product Support Services
 *   Windows Version 2.0 function demonstration application
 *   Copyright (c) Microsoft 1987
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
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"BeginPaint";
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
    hWnd = CreateWindow((LPSTR)"BeginPaint",
			(LPSTR)"BeginPaint()",
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
    PAINTSTRUCT ps;	/* paint structure filled by BeginPaint() */
    RECT rExcludeRect;	/* area to validate */
    switch (message)
    {
    case WM_SYSCOMMAND:
            return DefWindowProc( hWnd, message, wParam, lParam );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
	rExcludeRect.top    = 0;
	rExcludeRect.left   = 50;
	rExcludeRect.bottom = 100;
	rExcludeRect.right  = 80;
   /* remove the area described by rExcludeRect from update region */
	ValidateRect(hWnd,(LPRECT)&rExcludeRect);

   /****	prepare window for painting	     ****/
	BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
	TextOut(ps.hdc,
		5,
		5,
		(LPSTR)"This line of text messed up by ValidateRect()",
		45);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
