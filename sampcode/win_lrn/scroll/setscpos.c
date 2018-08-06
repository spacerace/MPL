/*
 *  SetScrollPos
 *  setscpos.c
 *
 *  This program demonstrates the use of the function SetScrollPos.
 *  It sets the current position of a scroll bar elevator to a specified 
 *  position and redraws the scroll bar to reflect the new position, if 
 *  requested.
 *
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Setscpos";
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
    HWND         hWnd;                /* Handle to the parent window    */
 
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setscpos",
                        (LPSTR)"Setting Scroll Position",
                        WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL,
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
    SetScrollRange (hWnd, SB_HORZ, 1, 10, (BOOL)TRUE);

    /* initial position on a vertical scroll bar */
    SetScrollPos   (hWnd, SB_VERT, 2, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"The scroll bar elevator is near the top", 
                (LPSTR)"Vertical", MB_OK);

    /* moved to a new location */
    SetScrollPos (hWnd, SB_VERT, 9, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"Now the scroll bar elevator is set to another location",
                (LPSTR)"Vertical", MB_OK);

    /* initial position on a vertical scroll bar */
    SetScrollPos   (hWnd, SB_HORZ, 2, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"The scroll bar elevator is near the left", 
                (LPSTR)"Horizontal", MB_OK);

    /* moved to a new location */
    SetScrollPos (hWnd, SB_HORZ, 9, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"Now the scroll bar elevator is set to another location",
                (LPSTR)"Horizontal", MB_OK);

    return 0;
}
