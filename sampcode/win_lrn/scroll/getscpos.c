/*
 *
 *  GetScrollPos
 *
 *  This program demonstrates the use of the function GetScrollPos.
 *  It retrieves the current position of a scroll bar elevator.
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
    wcClass.lpszClassName  = (LPSTR)"Getscpos";
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
    WORD  nPos;                /* The position of the elevator   */
    char  szBuffer[35];        /* string buffer for the message  
                                * box                            */
 
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Getscpos",
                        (LPSTR)"Getting Scroll Position",
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

    /* Setting up the vertical and horizontal scroll bars range */
    SetScrollRange (hWnd, SB_VERT, 1, 10, (BOOL)TRUE);

    /* initial position on a vertical scroll bar */
    SetScrollPos (hWnd, SB_VERT, 2, (BOOL)TRUE);
    nPos = GetScrollPos (hWnd, SB_VERT);  
    sprintf (szBuffer, "The elevator is currently at %d", nPos);
    MessageBox (hWnd, (LPSTR)szBuffer, (LPSTR)"Vertical", MB_OK);

    /* moved to a new location */
    SetScrollPos (hWnd, SB_VERT, 9, (BOOL)TRUE);
    nPos = GetScrollPos (hWnd, SB_VERT);  
    sprintf (szBuffer, "The elevator is currently at %d", nPos);
    MessageBox (hWnd, (LPSTR)szBuffer, (LPSTR)"Vertical", MB_OK);

    return 0;
}
