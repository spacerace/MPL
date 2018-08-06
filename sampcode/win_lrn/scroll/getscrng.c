/*
 *
 *  GetScrollRange
 *
 *  This program demonstrates the use of the function GetScrollRange.
 *  It retrieves the current minimum and maximum scroll bar range
 *  for the given scroll bar. 
 *
 *  Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"
#include "stdio.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Getscrng";
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
    HWND  hWnd;                /* Handle to the parent window    */
    short nMinPos, nMaxPos;    /* The range of the scroll bar    */
    char  szBuffer[50];        /* string buffer for the message  
                                * box                            */
 
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Getscrng",
                        (LPSTR)"GetScrollRange",
                        WS_OVERLAPPEDWINDOW|WS_VSCROLL,
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

    /* Setting up the vertical scroll bar range and use GETSCROLLRANGE
     * to retrieve the range
     */
    SetScrollRange (hWnd, SB_VERT, 1, 10, (BOOL)TRUE);
    GetScrollRange (hWnd, SB_VERT, (LPINT) &nMinPos, (LPINT) &nMaxPos);
    sprintf (szBuffer, "The range of the scroll bar is between %d and %d", 
             nMinPos, nMaxPos);
    MessageBox (hWnd, (LPSTR)szBuffer, (LPSTR)"GetScrollRange", MB_OK);

    return 0;
}

