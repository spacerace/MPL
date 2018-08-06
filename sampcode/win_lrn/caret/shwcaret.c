/*
 *  ShowCaret
 *  shwcaret.c
 *
 *  This program demonstrates the use of the function ShowCaret.
 *  This function shows the system caret on the display at the
 *  caret's current position.  Once shown, the caret begins flashing
 *  automatically.
 *
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pClass;

    pClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    /* registering the parent window class */
    pClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pClass->lpszMenuName   = (LPSTR)NULL;
    pClass->lpszClassName  = (LPSTR)"Window";
    pClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pClass->hInstance      = hInstance;
    pClass->style          = CS_HREDRAW | CS_VREDRAW;
    pClass->lpfnWndProc    = DefWindowProc;

    if (!RegisterClass( (LPWNDCLASS)pClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE) pClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND         hWnd;                /* Handle to the parent window    */
 
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
			(LPSTR)"ShowCaret",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,     /* x         */
                        CW_USEDEFAULT,     /* y         */
                        CW_USEDEFAULT,     /* width     */
                        CW_USEDEFAULT,     /* height    */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

/*************************************************************************/

    CreateCaret (hWnd, NULL, 8, 12); /* Create a caret before showing */
    SetCaretPos (50,50);    
    ShowCaret (hWnd);		     /* ShowCaret doesn't return a value */

/*************************************************************************/

    /* Message Box will make the parent window inactive */
    MessageBox (hWnd, (LPSTR)"The flashing black box is the caret",
		(LPSTR)"ShowCaret", MB_OK);


    return 0;
}
