/*
 *
 *  SetCursor
 *  setcurso.c
 *
 *  This program demonstrates the use of the function SetCursor. 
 *  It sets the cursor shape to the cursor specified by hCursor. HCursor
 *  is a handle to a cursor resource, which must have been loaded 
 *  previously using the LoadCursor function. The cursor is set only if the 
 *  new shape is different from the existing shape. If hCursor is NULL, the 
 *  cursor is removed from the screen. To change the cursor while it is in
 *  a window, the class cursor for the given window's class must be set to 
 *  NULL. If the class cursor is not null, Windows restores the old shape
 *  each time the mouse moves.
 *
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = (HCURSOR)NULL;
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Setcurso";
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
    HWND    hWnd;              /* Handle to the parent window    */
    HCURSOR hCursor;           /* Handle to the cursor           */
    MSG     msg;               /* Window messages                */
    
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setcurso",
                        (LPSTR)"Changing Cursor",
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

    /* Setting the new cursor */
    MessageBox (hWnd, (LPSTR)"This is the old cursor", (LPSTR)"Before", MB_OK);
    hCursor = LoadCursor (NULL, IDC_CROSS);
    MessageBox (hWnd, (LPSTR)"Press OK to see the new cursor", 
                (LPSTR)"After", MB_OK);
    SetCursor (hCursor);

    /* Window message loop */
    while (GetMessage ((LPMSG) &msg, NULL, 0, 0))
      { 
         TranslateMessage ((LPMSG) &msg);
         DispatchMessage ((LPMSG) &msg);
      }
    
    return (int) msg.wParam;
}
