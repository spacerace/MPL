/*
 *  SetCaretPos
 *  setcapos.c
 *
 *  This program demonstrates the use of the function SetCaretPos.
 *  It moves the caret to the position specified by the parameters. The
 *  movement applies to the caret whether it is visible or hidden.
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
    wcClass.lpszClassName  = (LPSTR)"Setcapos";
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

    hWnd = CreateWindow((LPSTR)"Setcapos",
                        (LPSTR)"Setting Caret Position",
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
   
    /* Create a caret before showing */
    CreateCaret (hWnd, NULL, 8, 12);
    SetCaretPos (50,50);    
    ShowCaret (hWnd);
    /* Message Box will make the parent window inactive */
    MessageBox (hWnd, (LPSTR)"This is the current caret position", 
                (LPSTR)"Before", MB_OK);

    /* move the caret to a new location */
    SetCaretPos (50,125);    
    MessageBox (hWnd, (LPSTR)"Now the caret is moved to a new position", 
                (LPSTR)"After", MB_OK);

    return 0;
}
