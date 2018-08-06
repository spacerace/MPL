/*
 *  RoundRect
 *
 *  This program demonstrates the use of the function RoundRect.
 *  It gets a DC and then draws a rectangle with rounded corners in the 
 *  client area.
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
    wcClass.lpszClassName  = (LPSTR)"Rrect";
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
    HWND  hWnd;             /* Handle to the parent window    */
    BOOL  bRect;            /* flag for RoundRect routine     */
    HDC   hDC;              /* display context of client area */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Rrect",
                        (LPSTR)"Drawing Round Rectangle",
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

    /* draw the round rectangle on the client area */
    hDC = GetDC (hWnd);
    bRect = RoundRect (hDC, 50, 50, 175, 150, 25, 20);
    ReleaseDC (hWnd, hDC);
    
    /* return code for RoundRect routine */
    if (bRect)
      {  MessageBox (hWnd, (LPSTR)"The rectangle is drawn",
                     (LPSTR)"Done", MB_OK);
      }
    else 
      {  MessageBox (hWnd, (LPSTR)"The rectangle is not drawn",
                     (LPSTR)NULL, MB_OK);
      }

    return 0;
}

