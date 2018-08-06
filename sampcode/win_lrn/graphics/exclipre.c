/*
 *  Function Name:   ExcludeClipRect
 *  Program Name:    exclipre.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   This function will exclude an area from a region that would otherwise be
 *   accessible.  The program below creates a rectangular region, excludes
 *   part of the rectangle, then inverts the region, showing the excluded
 *   rectangle is unaffected by the inversion.
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_ExcludeClipRect(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  HRGN hNewRgn;
  short nRgnType;

  hNewRgn = CreateRectRgn(0, 0, 200, 100);

  nRgnType = ExcludeClipRect(hDC, 50, 20, 100, 50);  /* rectangle clipped   */
                                                      /* from region         */
  if (nRgnType == 1)                     /* Check for error */
    {
    MessageBox(hWnd,(LPSTR)"SelectClipRgn failed.",(LPSTR)"ERROR",MB_ICONHAND);
      return;
    }
  InvertRgn(hDC, hNewRgn);
  DeleteObject( hNewRgn );

  return;
}

/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     =0;
    wcClass.cbWndExtra     =0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"ExcludeClipRect";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance)
        {
        /* Call initialization procedure if this is the first instance */
        if (!WinInit( hInstance ))
            return FALSE;
        }

    hWnd = CreateWindow((LPSTR)"ExcludeClipRect",
                        (LPSTR)"ExcludeClipRect()",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
        {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* Procedures which make up the window class. */
long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        CALL_ExcludeClipRect(hWnd, ps.hdc);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}