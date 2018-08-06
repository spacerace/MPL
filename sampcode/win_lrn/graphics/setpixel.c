/*
 *  SetPixel
 *  setpixel.c
 *
 *  This program demonstrates the use of the function SetPixel.
 *  It sets the pixel at the point specified by the X and Y coordinates 
 *  to the closest approxmiation to the color specified by the rgbcolor.
 *  The point must be in the clipping region. If the point is not in the
 *  clipping region, the function is ignored.
 *
 */

#include "windows.h"

/* Registering the parent window class */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Setpixel";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.hInstance      = hInstance;
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = DefWindowProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;

    RegisterClass( (LPWNDCLASS) &wcClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND  hWnd;                   /* Handles to the windows        */
    HDC   hDC;                    /* Display context of the window */
    short nPos;                   /* X-coordinate of the pixel     */
    LONG  nResult;                /* return value of SetPixel      */

    WinInit (hInstance);

    /* creating the window */
    hWnd = CreateWindow((LPSTR)"Setpixel",
                        (LPSTR)"Setting Pixel",
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

    /* Getting the display context and draw a line by setting a series of 
     * pixels in the client area. The rgbcolor is set by using RGB macro.
     */
    hDC = GetDC (hWnd);
    
    for (nPos = 50; nPos < 550; nPos++)
      {
        nResult = SetPixel (hDC, nPos, 50, RGB (0,0,0));
        if (nResult == -1)
          MessageBox (hWnd, (LPSTR)"SetPixel Function Failed", 
                      (LPSTR)"ERROR!", MB_OK);
      }

    ReleaseDC (hWnd, hDC);

    MessageBox (hWnd, (LPSTR)"Using SetPixel to draw a line",
                (LPSTR)"Done", MB_OK);

    return 0;
}
