/*
 *  SetCaretBlinkTime
 *  setcarbt.c
 *
 *  This program demonstrates the use of the function SetCaretBlinkTime.
 *  It establishes the caret flash rate for the client area.
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
    wcClass.lpszClassName  = (LPSTR)"Setcarbt";
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

    hWnd = CreateWindow((LPSTR)"Setcarbt",
                        (LPSTR)"Setting Caret Blink Time",
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
   
    /* create a caret before showing it */
    CreateCaret (hWnd, NULL, 8, 12);
    SetCaretPos (50,50);    
    ShowCaret (hWnd);
    /* Message Box will make the parent window inactive */
    MessageBox (hWnd, (LPSTR)"This is a slower caret blink time", 
                (LPSTR)"Before", MB_OK);

    /* make the caret blink faster by decreasing the delay time */
    SetCaretBlinkTime (GetCaretBlinkTime()/2);
    MessageBox (hWnd, (LPSTR)"Now the caret is twice as fast", 
                (LPSTR)"After", MB_OK);

    return 0;
}
