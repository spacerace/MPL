/*
 *  SetROP2
 *  setrop2.c
 *
 *  This program demonstrates the use of the function SetROP2.
 *  It sets the current drawing mode of the display context. GDI uses the
 *  drawing mode to combine pens and interior of filled objects with color
 *  already on the display surface. The mode specifies how the color of 
 *  the pen or interior and the color already on the display surface are
 *  combined to yield a new color. In this program, a rectangle is drawn on 
 *  the client area with different drawing mode. It will show how the pen
 *  (which is used to draw the border) and the brush (which is used to fill
 *  the rectangle) combine with the color already on the screen. 
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
    wcClass.lpszClassName  = (LPSTR)"Setrop2";
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
    HWND  hWnd;                 /* Handle to the parent window    */
    HDC   hDC;                  /* Display context of client area */
    MSG   msg;                  /* Window messages                */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setrop2",
                        (LPSTR)"Setting Drawing Mode",
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

    /* Using the default drawing mode of R2_COPYPEN from the display 
     * context, where the pixel is the color of the pen and brush, 
     * regardless of what is on the screen
     */
    Rectangle (hDC, 50, 50, 150, 100);
    MessageBox (hWnd, (LPSTR)"The ROP2 is R2_COPYPEN", (LPSTR)"R2_COPYPEN", MB_OK);

    /* The pixel is always white with R2_WHITE */
    SetROP2 (hDC, R2_WHITE);
    Rectangle (hDC, 60, 60, 160, 110);
    MessageBox (hWnd, (LPSTR)"The ROP2 is R2_WHITE", (LPSTR)"R2_WHITE", MB_OK);

    /* The pixel is a combination of colors in the pen/brush and display,
     * but not in both witj R2_XORPEN
     */    
    SetROP2 (hDC, R2_XORPEN);
    Rectangle (hDC, 70, 70, 170, 120);
    MessageBox (hWnd, (LPSTR)"The ROP2 is R2_XORPEN", (LPSTR)"R2_XORPEN", MB_OK);

    /* The pixel is the inverse of the display color with R2_NOT */
    SetROP2 (hDC, R2_NOT);
    Rectangle (hDC, 80, 80, 180, 130);
    MessageBox (hWnd, (LPSTR)"The ROP2 is R2_NOT", (LPSTR)"R2_NOT", MB_OK);

    /* return the display context when everything is done */
    ReleaseDC (hWnd, hDC);
    
    /* Window message loop */
    while (GetMessage ((LPMSG)&msg, NULL, 0, 0))
      {
         TranslateMessage ((LPMSG)&msg);
         DispatchMessage  ((LPMSG)&msg);
      }
 
    return (int)msg.wParam;
}
