/*
 *  SetBkMode
 *  setbkmd.c
 *
 *  This program demonstrates the use of the function SetBkMode.
 *  It sets the background mode used with text, hatched brushes, and 
 *  line styles. This program only illustrates with text in the client
 *  area. 
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
    wcClass.lpszClassName  = (LPSTR)"Setbkmd";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
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
    HWND  hWnd;                        /* Handle to the parent window    */
    HDC   hDC;                         /* Display context of client area */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setbkmd",
                        (LPSTR)"Setting Background Mode",
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

    /* Get a DC before writing to the client area */
    hDC = GetDC (hWnd);

    /* The default background mode is Opaque */
    TextOut(hDC, 20, 10, (LPSTR) "This is Opaque Background Mode", 30);

    /* Change the background mode to Transparent */
    SetBkMode (hDC, TRANSPARENT);
    TextOut(hDC, 20, 30, (LPSTR) "This is Transparent Background mode", 35);

    ReleaseDC (hWnd, hDC);
    
    MessageBox (hWnd, (LPSTR)"The background mode has changed", (LPSTR)"Done", MB_OK);

    return 0;
}
