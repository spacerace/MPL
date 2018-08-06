/*
 *  SetCapture() & ReleaseCapture()
 *  setcapt.c
 *
 *  This program demonstrates the use of the functions SetCapture()
 *  and ReleaseCapture(). It causes all subsequent mouse input to be
 *  sent to the specified window, regardless of the position of the
 *  mouse cursor. Capture can be released by pressing the right
 *  mouse button.
 *
 */

#include "windows.h"
#include "stdio.h"
#include "string.h"

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
    wcClass.lpszClassName  = (LPSTR)"Setcapt";
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
    HWND   hWnd;                /* Handle to the parent window    */
    MSG    msg;                 /* Window messages                */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setcapt",
                        (LPSTR)"Setting Mouse Capture",
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

    /* All mouse input will be sent to this window */
    MessageBox (hWnd, (LPSTR)"Press OK to capture the mouse", (LPSTR)"Start", MB_OK);
    SetCapture (hWnd);
          
    /* The window message loop */
    while (GetMessage ((LPMSG)&msg, NULL, 0, 0))
      { 
         TranslateMessage ((LPMSG)&msg);
         DispatchMessage  ((LPMSG)&msg);
      }
  
    return (int)msg.wParam;
}


/* The window procedure */
long FAR PASCAL SampleWindowProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
{
    POINT ptMousePos;      /* The location of the current mouse position */
    HDC   hDC;             /* Handle to the display context              */
    char  szBuffer[40];    /* String buffer used for output              */
    short nBytes;          /* Number of character in the string buffer   */

    switch (message)
      {
        case WM_PAINT:
          hDC = GetDC (hWnd);
          TextOut (hDC, 10, 10, (LPSTR)"Even when the mouse is out of this window,", 42);
          TextOut (hDC, 10, 20, (LPSTR)"the mouse input is still sent to this window.", 45);
          TextOut (hDC, 10, 30, (LPSTR)"Press the right mouse button to release the mouse capture.", 58);
          ValidateRect (hWnd, (LPRECT)NULL);
          ReleaseDC (hWnd, hDC);
          break; 

        /* All the mouse messages will come through this window procedure
         * after the mouse is captured, even if the mouse is outside of 
         * this window.
         */
        case WM_MOUSEMOVE:
          hDC = GetDC (hWnd);
          ptMousePos = MAKEPOINT (lParam);
          nBytes = sprintf (szBuffer, "The Mouse is now at: X = %d, Y = %d   ", 
                            ptMousePos.x, ptMousePos.y);
          TextOut (hDC, 50, 50, (LPSTR)szBuffer, nBytes);
          ReleaseDC (hWnd, hDC);
          break;
 
        /* The mouse must be released before you can exit this program */
        case WM_RBUTTONDOWN:
          ReleaseCapture();
          MessageBox (hWnd, (LPSTR)"Mouse is released",
                      (LPSTR)"ReleaseCapture()", MB_OK);
          break;

        default:
          return (DefWindowProc (hWnd, message, wParam, lParam));
          break;
      }
    return 0L;
}
