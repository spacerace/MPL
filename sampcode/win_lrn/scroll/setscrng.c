/*
 *  SetScrollRange
 *  setscpos.c
 *
 *  This program demonstrates the use of the function SetScrollRange.
 *  It sets the minimum and maximum position values for the scroll bar
 *  of a window. If the minimum position equals the maximum position, 
 *  then the scroll bar is removed.
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
    wcClass.lpszClassName  = (LPSTR)"Setscrng";
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

    hWnd = CreateWindow((LPSTR)"Setscrng",
                        (LPSTR)"Setting Scroll Range",
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
    MessageBox (hWnd, (LPSTR)"The new range is set for the vertical scroll bar",
                (LPSTR)"Vertical", MB_OK);
    
    SetScrollRange (hWnd, SB_HORZ, 1, 20, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"The new range is set for the horizontal scroll bar",
                (LPSTR)"Horizontal", MB_OK);

    /* removing the vertical scroll bar */
    SetScrollRange (hWnd, SB_VERT, 0, 0, (BOOL)TRUE);
    MessageBox (hWnd, (LPSTR)"The vertical scroll bar is now removed",
                (LPSTR)"Vertical", MB_OK);
    
    return 0;
}
