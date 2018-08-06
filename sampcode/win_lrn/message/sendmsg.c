/*
 *  SendMessage
 *  sendmsg.c,
 *
 *  This program demonstrates the use of the function SendMessage.
 *  It sends a message to the specified window immediately without
 *  putting the message in the application queue. SendMessage does not
 *  return until the message has been processed. The window function 
 *  is called immediately, as a subroutine.
 *
 */

#include "windows.h"

long FAR PASCAL SampleWindowProc (HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Sendmsg";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.hInstance      = hInstance;
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = SampleWindowProc;
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
    HWND   hWnd;                /* Handle to the parent window     */
    MSG    msg;                 /* Message received from the queue */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Sendmsg",
                        (LPSTR)"Sending Messages",
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

    /* Sending the window a WM_DESTROY message directly to the 
     * window procedure immediately
     */
    MessageBox (hWnd, (LPSTR)"Sending the window a destroy message", 
                (LPSTR)"SENDING", MB_OK);
    SendMessage (hWnd, WM_DESTROY, 0,0L);

    return 0;
}

/* The window procedure for this program */
long FAR PASCAL SampleWindowProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
      {  case WM_DESTROY:
           MessageBox (hWnd, (LPSTR)"Destroy Message Received", 
                       (LPSTR)"RECEIVED", MB_OK);
           PostQuitMessage (0);
           break;
 
         default:
           return (DefWindowProc (hWnd, message, wParam, lParam));
           break;
      }
    return 0L;
}
