/*
 *  SaveDC
 *  savedc.c,
 *
 *  This program demonstrates the use of the function SaveDC.
 *  It saves the current state of the display context by copying state 
 *  information to a context stack. The saved display context can later
 *  be restored by using the RestoreDC function.
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
    wcClass.lpszClassName  = (LPSTR)"Savedc";
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
    HDC    hDC;                 /* Display context of client area */
    HBRUSH hNewBrush, hOldBrush;/* Handle to the brushes          */
    short  nDCIdentity;         /* identifier to the saved DC     */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Savedc",
                        (LPSTR)"Saving Display Context",
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

    /* Get the DC of the client area */
    hDC = GetDC (hWnd);

    /* Create a new brush and change the DC */
    hNewBrush = CreateSolidBrush (GetSysColor (COLOR_WINDOW));
    hOldBrush = (HBRUSH) SelectObject (hDC, (HANDLE) hNewBrush);

    /* Save the DC and get an unique identity of the saved DC */
    nDCIdentity = SaveDC (hDC);

    /* Release the DC */
    ReleaseDC (hWnd, hDC);
    
    /* return code for SaveDC routine. It is 0 if the DC is not saved */
    if (nDCIdentity)
      {  MessageBox (hWnd, (LPSTR)"The DC is saved",
                     (LPSTR)"Done", MB_OK);
      }
    else 
      {  MessageBox (hWnd, (LPSTR)"The DC is not saved",
                     (LPSTR)NULL, MB_OK);
      }

    return 0;
}
