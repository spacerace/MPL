/*
 *  ScreenToClient
 *  scntocli.c,
 *
 *  This program demonstrates the use of the function ScreenToClient.
 *  It converts the screen coordinates of the point into equivalent client
 *  coordinates. The new coordinates replace the existing point. 
 *
 */

#include "windows.h"
#include <stdio.h>

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Scntocli";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.hInstance      = hInstance;
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = DefWindowProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;

    RegisterClass( (LPWNDCLASS)&wcClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND   hWnd;                /* Handle to the parent window    */
    POINT  ptCurrentPoint;      /* POINT data type                */
    char   szBuffer[60];        /* string buffer for output       */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Scntocli",
                        (LPSTR)"Screen/Client Coordinates",
                        WS_OVERLAPPEDWINDOW,
                        50,                /* x         */
                        50,                /* y         */
                        600,               /* width     */
                        250,               /* height    */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Initializing the point to some screen coordinate and display the
     * value in a message box
     */
    ptCurrentPoint.x = 234;
    ptCurrentPoint.y = 150;
    sprintf (szBuffer, "The screen coordinates of the point is %d and %d",
             ptCurrentPoint.x, ptCurrentPoint.y);
    MessageBox (hWnd, (LPSTR)szBuffer, (LPSTR)"Screen", MB_OK);

    /* convert the point to client coordinate and display the new value
     * in a message box
     */
    ScreenToClient (hWnd, (POINT FAR *) &ptCurrentPoint);
    sprintf (szBuffer, "The client coordinates of the point is %d and %d",
             ptCurrentPoint.x, ptCurrentPoint.y);
    MessageBox (hWnd, (LPSTR)szBuffer, (LPSTR)"Client", MB_OK);

    return 0;
}
