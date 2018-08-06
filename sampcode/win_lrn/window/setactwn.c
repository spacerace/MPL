/*
 *  SetActiveWindow
 *  setactwn.c,
 *
 *  This program demonstrates the use of the function SetActiveWindow.
 *  It makes a tiled window or popup style window the active window. 
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
    wcClass.lpszClassName  = (LPSTR)"Setactwn";
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
    MSG          msg;                 /* Window messages                */
    HWND         hWnd;                /* Handle to the parent window    */
    HDC          hDC;                 /* Display context of client area */
 
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setactwn",
                        (LPSTR)"Active Window",
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

    /* Message Box will make the parent window inactive */
    MessageBox (hWnd, (LPSTR)"The window is not active\n(Look at the caption bar)", 
                (LPSTR)"Before", MB_OK);

    /* make the window becomes the active window */
    SetActiveWindow (hWnd);
    hDC = GetDC (hWnd);
    TextOut (hDC, 10, 10, (LPSTR)" Now this window is active.", 26);
    ReleaseDC (hWnd, hDC);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
      { 
        TranslateMessage ((LPMSG)&msg);
        DispatchMessage  ((LPMSG)&msg);
      }

    return (int)msg.wParam;
}
