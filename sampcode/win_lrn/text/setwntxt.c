/*
 *  SetWindowText
 *  setwntxt.c
 *
 *  This program demonstrates the use of the function SetWindowText.
 *  First a message box is shown which says that the window text caption
 *  is about to change.  After the caption changes then a message box pops
 *  up which gives the user a chance to look at the change.
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
    HWND  hWnd; 	      /* Handle to the parent window  */
    HDC   hDC;		      /* Handle to the display context of client area */
    char szBuffer[80];	      /* Contains the new caption title */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
			(LPSTR)"SetWindowText",  /* Window caption title */
                        WS_TILEDWINDOW,
                        20,20,400,200,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow (hWnd, cmdShow);
    UpdateWindow (hWnd);

    MessageBox (hWnd, (LPSTR)"Press ENTER to change the caption title",
		(LPSTR)"Done", MB_OK);
    sprintf (szBuffer,"%s","New Caption Title to demonstrate SetWindowText");
/**************************************************************************/
/* This function will set the window caption title to the string pointed
 * to by szBuffer.  SetWindowText has no return value. */

    SetWindowText (hWnd, (LPSTR)szBuffer);

/**************************************************************************/

    MessageBox (hWnd, (LPSTR)"The caption title has been changed",
		(LPSTR)"Done", MB_OK);

    return 0;
}
