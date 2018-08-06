/*
 *  SetClassWord
 *  setclswd.c
 *
 *  This program demonstrates the use of the function SetClassWord.
 *  It replaces the word at the specified index in the WNDCLASS structure
 *  of the window. It is used to change the attributes of the window class
 *  after the window class has been registered.  
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
    wcClass.lpszClassName  = (LPSTR)"Setclswd";
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
    HCURSOR hCursor;           /* Handle to the brushes          */
    MSG     msg;               /* Window messages                */
    
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setclswd",
                        (LPSTR)"Changing Window Class Information",
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

    /* SetClassWord is used here to change the cursor in the window class
     * structure. It can be used also to change the icon, window style, and
     * other attributes of the window class.
     */
    MessageBox (hWnd, (LPSTR)"This is the old cursor", (LPSTR)"Before", MB_OK);
    hCursor = LoadCursor (NULL, IDC_CROSS);
    SetClassWord (hWnd, GCW_HCURSOR, hCursor);
    MessageBox (hWnd, (LPSTR)"Press OK to see the new cursor", (LPSTR)"After", MB_OK);

    /* the message loop */
    while (GetMessage ((LPMSG) &msg, NULL, 0, 0))
      { 
         TranslateMessage ((LPMSG) &msg);
         DispatchMessage ((LPMSG) &msg);
      }
    
    return (int) msg.wParam;
}
